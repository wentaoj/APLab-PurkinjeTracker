import numpy as np
import cv2
import os
import tqdm
from ver2_Synthetic import randomize_images
from tracker import MyTracker

from ver2_Synthetic import synthetic_w_rotation # with question

def track_purkinje(output_dir):
    """
    2. Track Purkinje Images (Polymorphism)
    Sample usage of Tracker class
    (Currently NOT in use)
    """
    tracker = MyTracker() # Inherited class
    image = cv2.imread(os.path.join(output_dir, "frame#0000.png"), cv2.IMREAD_GRAYSCALE)
    result = tracker.track(image)
    return result

def synthetic_random(output_dir):
    """
    1. Generate synthetic images
    """
    num_images = 1000
    image_size = (600, 800)  # (height, width)
    radius_P1 = 27  # P1 (Gaussian)
    radius_P4 = 10  # P4 (Saturated)
    noise_level = 20
    intensity_factor_P1 = 1.0
    intensity_factor_P4 = 0.6 # P4 much lower intensity
    # generate images that randomly position P1 and P4
    randomize_images(output_dir, num_images, image_size, radius_P1, radius_P4, intensity_factor_P1, intensity_factor_P4, noise_level) 
    # dx, dy info is printed on the image

def synthetic_w_angle(output_dir):
    pass

if __name__ == "__main__":
    output_dir = "./images"
    # 1. Generate synthetic images; dx, dy info is printed on the image
    synthetic_random(output_dir)

    # # 2. Track Purkinje Images (Polymorphism)
    # print(track_purkinje(output_dir))
    
