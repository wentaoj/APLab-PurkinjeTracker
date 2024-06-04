import numpy as np
import cv2
import os
from tqdm import tqdm

# Image Synthetic
def gaussian_circle(grid, radius, position, intensity_factor):
    """Gaussian circle with a central uniform intensity and Gaussian edges."""
    x = np.arange(0, grid[1], 1, dtype=np.float32)
    y = np.arange(0, grid[0], 1, dtype=np.float32)
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

    gaussian = central_intensity + gaussian_edge
    gaussian = np.clip(gaussian, 0, 255*intensity_factor) # center + edge > 255
    return gaussian.astype(np.uint8)

def saturated_circle(grid, radius, position, intensity_factor):
    """Saturated circle with a uniform intensity"""
    x = np.arange(0, grid[1], 1, dtype=np.float32)
    y = np.arange(0, grid[0], 1, dtype=np.float32)
    x, y = np.meshgrid(x, y)
    x_0, y_0 = position
    distance = np.sqrt((x - x_0)**2 + (y - y_0)**2)
    c = 255 * intensity_factor # .6
    saturated = np.zeros_like(distance, dtype=np.float32)
    saturated[distance < radius] = c
    return saturated.astype(np.uint8)

def randomize_images(output_dir, num_images, grid, radius_P1, radius_P4, intensity_factor_P1, intensity_factor_P4, noise_level):
    """
    Generate synthetic images with random positions of P1(right) and P4(left).
    inputs: grid (height, width): here we use (600, 800) for 800x600 image
    """
    os.makedirs(output_dir, exist_ok=True)
    frame_info = []
    for i in tqdm(range(num_images)):
        # Randomly position P1(left) and P4(right)
        position_P1 = (
            np.random.uniform(grid[1]//2 + 50, grid[1] - radius_P1), 
            np.random.uniform(50, grid[0] - radius_P1)
        ) # P1 saturated (x:[450, 750], y:[50, 550])
        position_P4 = (
            np.random.uniform(50, grid[1]//2 - 50),
            np.random.uniform(50, grid[0] - radius_P4)
        ) # P4 gaussian (x:[50, 350], y:[50, 550])

        synthetic = np.zeros(grid, dtype=np.uint8)
        # Add noise in background
        noise_background = np.random.randint(0, noise_level, grid, dtype=np.uint8)
        # Draw Purkinje Images
        saturated = saturated_circle(grid, radius_P1, position_P1, intensity_factor_P1)
        gaussian = gaussian_circle(grid, radius_P4, position_P4, intensity_factor_P4)
        # Append all images
        synthetic = cv2.add(synthetic, noise_background)
        synthetic = cv2.add(synthetic, gaussian) # P4
        synthetic = cv2.add(synthetic, saturated) # P1
        # Calculate dx, dy
        dx = np.abs(position_P4[0] - position_P1[0])
        dy = np.abs(position_P4[1] - position_P1[1])

        frame_info.append({"frame": i, "P1": position_P1, "P4": position_P4, "dx": dx, "dy": dy})
        # print the dx, dy on the image
        positions = (f"P1({frame_info[i]['P1']}), P4({frame_info[i]['P4']}),")
        dxdy = (f"dx={frame_info[i]['dx']:.2f}, dy={frame_info[i]['dy']:.2f}")
        cv2.putText(synthetic, positions, (10, 575), cv2.FONT_HERSHEY_SIMPLEX, 0.3, (255, 255, 255), 1, cv2.LINE_AA)
        cv2.putText(synthetic, dxdy, (10, 590), cv2.FONT_HERSHEY_SIMPLEX, 0.3, (255, 255, 255), 1, cv2.LINE_AA)
        cv2.imwrite(os.path.join(output_dir, f"frame#{i:04d}.png"), synthetic)
    np.save(os.path.join(output_dir, "frame_info.npy"), frame_info)


###     # ABOVE: END OF SYNTHETIC RANDOM IMAGES #      ###


# Synthetic Images with Eye Rotation
def synthetic_w_rotation(output_rot_dir, num_images, grid, radius_P1, radius_P4, intensity_factor_P1, intensity_factor_P4, noise_level):
    """
    (With Question whether the concept correct): 
    Views with Eye Rotations from -40 to 40 degrees (Figure 3C visualization).
    """
    def mm_to_px(mm, factor=20):
        """
        27px radius for P1, so is 27mm diameter in real world.
        So the real factor should be about 2.
        But here we use factor=20 for clearer visualization."""
        return mm * factor
    
    os.makedirs(output_rot_dir, exist_ok=True)
    frame_info = []
    # initializations
    # distance_mm = 120 # Assume P1 and P4 have 120mm distance
    # distance_px = mm_to_px(distance_mm) # 240px if factor=2
    distance_px = 240
    # Position P1(right) and P4(left), and 
    init_pos_P1 = (
        np.random.uniform(400, grid[1] - radius_P1),
        np.random.uniform(50, grid[0] - radius_P1)
    ) # assuming as eye rotate Y axis will change accordingly
    init_pos_P4 = (
        init_pos_P1[0] - distance_px, # P4 is left side of P1
        np.random.uniform(50, grid[0] - radius_P1) # same Y axis, or use init_pos_P1[1]
    )
    optical_center = (grid[1]//2, grid[0]//2) # z-axis: center of the image [400, 300]
    angle_x_values = np.linspace(-40, 40, num_images) # Eye rotation angles
    # Linear Functions [Eye Rotation(degrees) ~ Purkinje Locations(mm)]
    lin_func = lambda angle_x, m, c: angle_x * m + c
    m_x1 = (-5.0 - (5.0))/(40.0 - (-40.0)) # slope: -10.0/80.0 = -1/8
    m_x4 = (-10.0 - (10.0))/(40.0 - (-40.0)) # slope: -20.0/80.0 = -1/4

    for i in tqdm(range(num_images)):
        # Rotate P1 and P4 on X-axis
        angle_x = angle_x_values[i] # Eye rotation angles
        # Eye rotate from right to left, before passing the optical center
        position_P1 = (init_pos_P1[0] - mm_to_px(lin_func(angle_x, m_x1, 0)), init_pos_P1[1]) # c=0 since when eye rotate 0 deg, P1 is at init_pos_P1
        position_P4 = (init_pos_P4[0] - mm_to_px(lin_func(angle_x, m_x4, 0)), init_pos_P4[1]) # same for c=0 for P4
        synthetic = np.zeros(grid, dtype=np.uint8)
        # Add noise in background
        noise_background = np.random.randint(0, noise_level, grid, dtype=np.uint8)
        # Draw Purkinje Images
        saturated = saturated_circle(grid, radius_P1, position_P1, intensity_factor_P1)
        gaussian = gaussian_circle(grid, radius_P4, position_P4, intensity_factor_P4)
        # Append all images
        synthetic = cv2.add(synthetic, noise_background)
        synthetic = cv2.add(synthetic, saturated) # P1
        synthetic = cv2.add(synthetic, gaussian) # P4
        # mark the optical center
        cv2.circle(synthetic, optical_center, 10, (0, 255, 255), -1)
        # Calculate dx_0
        dx = np.abs(position_P1[0] - position_P4[0])
        rel_pos_x1 = init_pos_P1[0] - position_P1[0] # relative position to initial P1 (followed by linear relation m=-1/8)
        rel_pos_x4 = init_pos_P4[0] - position_P4[0] # relative position to initial P4 (followed by linear relation m=-1/4)
        frame_info.append({"frame": i, "P1": position_P1, "P4": position_P4, "dx": dx, "angle_x": angle_x, "rel_pos_x1": rel_pos_x1, "rel_pos_x4": rel_pos_x4})
        cv2.imwrite(os.path.join(output_rot_dir, f"frame#{i:04d}.png"), synthetic)
    np.save(os.path.join(output_rot_dir, "frame_info_w_rot.npy"), frame_info)

def images_to_video(image_folder, output_video, fps=30):
    images = [img for img in os.listdir(image_folder) if img.endswith(".png")]
    images.sort()
    frame = cv2.imread(os.path.join(image_folder, images[0]))
    height, width, _ = frame.shape
    fourcc = cv2.VideoWriter_fourcc(*"mp4v")
    video = cv2.VideoWriter(output_video, fourcc, fps, (width, height))

    frame_info = np.load(os.path.join(image_folder, "frame_info_w_rot.npy"), allow_pickle=True)

    for image, info in tqdm(zip(images, frame_info)):
        frame = cv2.imread(os.path.join(image_folder, image))
        text1 = (f"Frame {info['frame']}: P1({info['P1']}), P4({info['P4']}),")
        text2 = (f"dx={info['dx']:.2f}, angle_x={info['angle_x']:.2f}, rel_pos_x1={info['rel_pos_x1']:.2f}, rel_pos_x4={info['rel_pos_x4']:.2f}")
        cv2.putText(frame, text1, (10, height - 25), cv2.FONT_HERSHEY_SIMPLEX, 0.3, (255, 255, 255), 1, cv2.LINE_AA)
        cv2.putText(frame, text2, (10, height - 10), cv2.FONT_HERSHEY_SIMPLEX, 0.3, (255, 255, 255), 1, cv2.LINE_AA)
        video.write(frame)
    
    video.release()
    cv2.destroyAllWindows()

if __name__ == "__main__":
    # For Sample (Rotation Visualization) usage;
    # Random Synthetic Images could be run under main.py
    output_dir = "./images_rot/"
    num_images = 1000
    image_size = (600, 800)  # (height, width)
    P1_radius = 27  # P1 (Gaussian)
    P4_radius = 7.5  # P4 (Saturated)
    noise_level = 20
    intensity_factor_P1 = 1.0
    intensity_factor_P4 = 0.6 # P4 much lower intensity

    # generate images that position P1 and P4 with sample of rotation
    synthetic_w_rotation(output_dir, num_images, image_size, P1_radius, P4_radius, intensity_factor_P1, intensity_factor_P4, noise_level)
    # convert images to video
    images_to_video(output_dir, "image_rot_1000.mp4", fps=10)
    