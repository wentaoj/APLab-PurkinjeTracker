import numpy as np
import cv2
import os
from tqdm import tqdm
from camera import DataGenerator
from tracker import MyTracker

def synthetic_random(output_dir: str, num_images :int = 1000, save_info: bool = True) -> None:
    """
    1. Generate synthetic images
    """
    # initialization
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
        output_frame = camera_config.produce_frame(frame_num=i, save_info=save_info)
        # save images at path
        cv2.imwrite(os.path.join(output_dir, f"frame#{i:04d}.png"), output_frame)
    
def track_purkinje(output_dir: str) -> dict:
    """
    2. Track Purkinje Images (Polymorphism)
    Sample usage of Tracker class
    """
    tracker = MyTracker() # Inherited class
    result_dic = {}
    for i, image in enumerate(os.listdir(output_dir)):
        image = cv2.imread(os.path.join(output_dir, f"frame#{i:04d}.png"), cv2.IMREAD_GRAYSCALE)
        result = tracker.track(image)
        result_dic[f"frame#{i:04d}"] = result["dx"], result["dy"]
    return result_dic

if __name__ == "__main__":
    output_dir = "./images"
    # 1. Generate synthetic images; dx, dy info is printed on the image
    synthetic_random(output_dir, num_images=1000)

    # # 2. Track Purkinje Images (Polymorphism)
    print(track_purkinje(output_dir))
    
