import numpy as np
import cv2
import os
import shutil
import json
from tqdm import tqdm
from utils import timingdecorator
import argparse

class DataGenerator:
    """A class representing a synthetic camera with Purkinje images outputs."""

    def __init__(self, frame_grid: tuple[int, int], noise_level: int, output_dir: str = "./data"):
        """Initialize the camera with a frame grid and a noise level."""
        self.frame_width: int = frame_grid[1]
        self.frame_height: int = frame_grid[0]
        self.noise_level: int = noise_level
        self.positions_P1: list[tuple[float, float]] = None
        self.positions_P4: list[tuple[float, float]] = None
        self.intensities_P1: list[float] = None
        self.intensities_P4: list[float] = None
        self.output_dir: str = output_dir

        if not os.path.exists(output_dir):
            os.makedirs(output_dir)

    def set_positions_P1(self, positions: list[tuple[float, float]]):
        """Set the position of P1 within the grid."""
        self.positions_P1 = positions

    def set_positions_P4(self, positions: list[tuple[float, float]]):
        """Set the position of P4 within the grid."""
        self.positions_P4 = positions

    def set_intensities_P1(self, intensity: list[float]):
        """Set the intensity factor of P1."""
        self.intensities_P1 = intensity

    def set_intensities_P4(self, intensity: list[float]):
        """Set the intensity factor of P4."""
        self.intensities_P4 = intensity

    @timingdecorator
    def produce_frame(self, num_images: int):
        """Produce a synthetic frame with pre-set positions and intensities."""
        assert self.positions_P1 is not None, "P1 positions (list of tuples) must be set."
        assert self.positions_P4 is not None, "P4 positions (list of tuples) must be set."
        assert self.intensities_P1 is not None, "P1 intensities (list of floats) must be set."
        assert self.intensities_P4 is not None, "P4 intensities (list of floats) must be set."

        image_output_dir = os.path.join(self.output_dir, "gen_image")
        if os.path.exists(image_output_dir):
            shutil.rmtree(image_output_dir)
        os.makedirs(image_output_dir, exist_ok=False)

        frame_info = []
        for i in tqdm(range(num_images)):
            pos_P1 = self.positions_P1[i]
            pos_P4 = self.positions_P4[i]
            int_P1 = self.intensities_P1[i]
            int_P4 = self.intensities_P4[i]
            # generate and save images
            synthetic = self._generate_image(pos_P1, pos_P4, int_P1, int_P4) # now in float32
            image_path = os.path.join(image_output_dir, f"frame#{i:04d}.png")
            cv2.imwrite(image_path, synthetic)
            # then generate frame info into json
            dx = np.abs(pos_P4[0] - pos_P1[0])
            dy = np.abs(pos_P4[1] - pos_P1[1])
            frame_info.append({"frame#": i, "P1": pos_P1, "P4": pos_P4, "dx": dx, "dy": dy})
        with open(os.path.join("data", "gen_image.json"), "w") as f:
            json.dump(frame_info, f, indent=2)
    
    def _generate_image(self, pos_P1: tuple[float, float], pos_P4: tuple[float, float], int_P1: float, int_P4: float) -> np.ndarray:
        """Generate a synthetic image with random positions of P1 and P4."""
        synthetic = np.zeros((self.frame_height, self.frame_width), dtype=np.float32)
        # Add noise in background
        noise_background = np.random.uniform(0, self.noise_level, (self.frame_height, self.frame_width)).astype(np.float32)
        # Draw Purkinje Images
        saturated = self._saturated_circle(pos_P1, int_P1, radius=25)
        gaussian = self._gaussian_circle(pos_P4, int_P4, radius=20)
        # Append all images
        synthetic = cv2.add(synthetic, noise_background)
        synthetic = cv2.add(synthetic, gaussian)
        synthetic = cv2.add(synthetic, saturated)
        return synthetic
    

    def _gaussian_circle(self, position: tuple[int, int], intensity: float, radius: int = 25) -> np.ndarray:
        """Generate a Gaussian circle."""
        x = np.arange(0, self.frame_width, 1, dtype=np.float32)
        y = np.arange(0, self.frame_height, 1, dtype=np.float32)
        x, y = np.meshgrid(x, y)
        x_0, y_0 = position
        distance = np.sqrt((x - x_0)**2 + (y - y_0)**2)
        sigma = radius / (2 * np.sqrt(2 * np.log(2))) # FWHM
        gaussian = intensity * np.exp(-(distance**2) / (2 * sigma**2))
        return gaussian.astype(np.float32)

    def _saturated_circle(self, position: tuple[int, int], intensity: float, radius: int = 20) -> np.ndarray:
        """Generate a saturated circle."""
        x = np.arange(0, self.frame_width, 1, dtype=np.float32)
        y = np.arange(0, self.frame_height, 1, dtype=np.float32)
        x, y = np.meshgrid(x, y)
        x_0, y_0 = position
        distance = np.sqrt((x - x_0)**2 + (y - y_0)**2)
        # Center uniform intensity at 255
        central_intensity = np.zeros_like(distance, dtype=np.float32)
        central_intensity[distance < radius] = intensity  # brightest intensity
        # Gaussian edges with FWHM = radius
        sigma = radius / (2 * np.sqrt(2 * np.log(2))) # FWHM = 2*sqrt(2*ln(2))*sigma
        gaussian_edge = intensity * np.exp(-((distance - radius)**2) / (2 * sigma**2))
        saturated = central_intensity + gaussian_edge
        saturated = np.clip(saturated, 0, intensity) # cutoff
        return saturated.astype(np.float32)
