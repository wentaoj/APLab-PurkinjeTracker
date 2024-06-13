import numpy as np
from typing import Tuple
import cv2
import os
from tqdm import tqdm

class DataGenerator:
    """A class representing a synthetic camera with Purkinje images outputs."""

    def __init__(self, frame_grid: Tuple[int, int], noise_level: int, output_dir: str = "./images"):
        """Initialize the camera with a frame grid and a noise level."""
        self.frame_width = frame_grid[1]
        self.frame_height = frame_grid[0]
        self.noise_level = noise_level
        self.position_P1 = None
        self.position_P4 = None
        self.intensity_factor_P1 = None
        self.intensity_factor_P4 = None
        self.output_dir = output_dir

    def set_position_P1(self, position: Tuple[int, int]):
        """Set the position of P1 within the grid."""
        self.position_P1 = position

    def set_position_P4(self, position: Tuple[int, int]):
        """Set the position of P4 within the grid."""
        self.position_P4 = position

    def set_intensity_P1(self, intensity_factor: float):
        """Set the intensity factor of P1."""
        self.intensity_factor_P1 = intensity_factor

    def set_intensity_P4(self, intensity_factor: float):
        """Set the intensity factor of P4."""
        self.intensity_factor_P4 = intensity_factor

    def produce_frame(self, frame_num: int, save_info: bool = True) -> np.ndarray:
        """Generate a synthetic image with random positions of P1 and P4."""
        synthetic = np.zeros((self.frame_height, self.frame_width), dtype=np.uint8)
        # Add noise in background
        noise_background = np.random.randint(0, self.noise_level, (self.frame_height, self.frame_width), dtype=np.uint8)
        # Draw Purkinje Images
        saturated = self._saturated_circle(self.position_P1, self.intensity_factor_P1, radius=25)
        gaussian = self._gaussian_circle(self.position_P4, self.intensity_factor_P4, radius=20)
        # Append all images
        synthetic = cv2.add(synthetic, noise_background)
        synthetic = cv2.add(synthetic, gaussian)
        synthetic = cv2.add(synthetic, saturated)
        # show info on the image, default is False (for easier clustering)
        return self._acquire_frame_info(frame_num, synthetic, save_info)
    
    
    def _acquire_frame_info(self, frame_num: int, synthetic: np.ndarray, save_info: bool) -> np.ndarray:
        """
        Acquire the frame info, and
        1. if save_info is False, print the dx, dy on the image
        2. if save_info is True, save the dx, dy as a .npy file
        """
        # Calculate dx, dy
        dx, dy = self._calculate_dx_dy()
        positions = (f"P1({self.position_P1}), P4({self.position_P4}),")
        dxdy = (f"dx={dx:.2f}, dy={dy:.2f}")
        if save_info:
            # save the dx, dy as a .npy file
            np_output_dir = os.path.join(self.output_dir, "frame_info")
            os.makedirs(np_output_dir, exist_ok=True)
            np.save(os.path.join(np_output_dir, f"frame#{frame_num:04d}.npy"), {"P1": self.position_P1, "P4": self.position_P4, "dx": dx, "dy": dy})
        else:
            # print the dx, dy on the image
            cv2.putText(synthetic, positions, (10, self.frame_height-25), cv2.FONT_HERSHEY_SIMPLEX, 0.3, (255, 255, 255), 1, cv2.LINE_AA)
            cv2.putText(synthetic, dxdy, (10, self.frame_height-10), cv2.FONT_HERSHEY_SIMPLEX, 0.3, (255, 255, 255), 1, cv2.LINE_AA)
        return synthetic
    
    def _calculate_dx_dy(self) -> Tuple[float, float]:
        """Calculate the difference in x and y coordinates between P1 and P4."""
        dx = np.abs(self.position_P4[0] - self.position_P1[0])
        dy = np.abs(self.position_P4[1] - self.position_P1[1])
        return dx, dy
    
    def _gaussian_circle(self, position: Tuple[int, int], intensity_factor: float, radius: int = 25) -> np.ndarray:
        """Generate a Gaussian circle."""
        x = np.arange(0, self.frame_width, 1, dtype=np.float32)
        y = np.arange(0, self.frame_height, 1, dtype=np.float32)
        x, y = np.meshgrid(x, y)
        x_0, y_0 = position
        distance = np.sqrt((x - x_0)**2 + (y - y_0)**2)
        c = 255 * intensity_factor  # c is the brightest intensity constant [0,255]
        # Gaussian edges with FWHM = radius
        sigma = radius / (2 * np.sqrt(2 * np.log(2))) # FWHM = 2*sqrt(2*ln(2))*sigma
        gaussian = c * np.exp(-(distance**2) / (2 * sigma**2)) # pixel
        return gaussian.astype(np.uint8)

    def _saturated_circle(self, position: Tuple[int, int], intensity_factor: float, radius: int = 20) -> np.ndarray:
        """Generate a saturated circle."""
        x = np.arange(0, self.frame_width, 1, dtype=np.float32)
        y = np.arange(0, self.frame_height, 1, dtype=np.float32)
        x, y = np.meshgrid(x, y)
        x_0, y_0 = position
        distance = np.sqrt((x - x_0)**2 + (y - y_0)**2)
        c = 255 * intensity_factor  # c is the brightest intensity constant [0,255]
        # Center uniform intensity at 1.0
        central_intensity = np.zeros_like(distance, dtype=np.float32)
        central_intensity[distance < radius] = c  # brightest 1.0 intensity
        # Gaussian edges with FWHM = radius
        sigma = radius / (2 * np.sqrt(2 * np.log(2))) # FWHM = 2*sqrt(2*ln(2))*sigma
        gaussian_edge = c * np.exp(-((distance - radius)**2) / (2 * sigma**2))
        saturated = central_intensity + gaussian_edge
        saturated = np.clip(saturated, 0, c) # cutoff
        return saturated.astype(np.uint8)


if __name__ == "__main__":
    output_dir = "./images"
    num_images = 1000

    image_size = (600, 800) # (height, width)
    noise_level = 20
    intensity_factor_P1 = 1.0
    intensity_factor_P4 = 0.6 # P4 much lower intensity
    os.makedirs(output_dir, exist_ok=True)

    camera_config = DataGenerator(image_size, noise_level, output_dir)
    camera_config.set_intensity_P1(intensity_factor_P1)
    camera_config.set_intensity_P4(intensity_factor_P4)

    for i in tqdm(range(num_images)):
        # Randomly position P1 and P4
        position_P1 = (
            np.random.uniform(50, image_size[1] - 50), 
            np.random.uniform(50, image_size[0] - 50)
        )
        position_P4 = (
            np.random.uniform(50, image_size[1] - 50),
            np.random.uniform(50, image_size[0] - 50)
        )
        camera_config.set_position_P1(position_P1)
        camera_config.set_position_P4(position_P4)
        output_frame = camera_config.produce_frame(frame_num=i, save_info=True)
        # save images at path
        cv2.imwrite(os.path.join(output_dir, f"frame#{i:04d}.png"), output_frame)