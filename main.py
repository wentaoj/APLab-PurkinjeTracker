from camera import DataGenerator
from tracker import ClusteringTracker, DoHTracker_cluster, DoHTracker_skimage
from utils import perc_error
import numpy as np
import os

import argparse

def synthetic_random(num_images: int):
    """
    1. Generate synthetic images
    """
    # also can load these from a file (e.g. json, csv, etc.)
    positions_P1 = [(np.random.uniform(50, 750), np.random.uniform(50, 550)) for _ in range(num_images)]
    positions_P4 = [(np.random.uniform(50, 750), np.random.uniform(50, 550)) for _ in range(num_images)]
    intensities_P1 = [255.0 for _ in range(num_images)]
    intensities_P4 = [0.6 * 255.0 for _ in range(num_images)]

    camera_config = DataGenerator((600, 800), noise_level=50)
    camera_config.set_positions_P1(positions_P1)
    camera_config.set_positions_P4(positions_P4)
    camera_config.set_intensities_P1(intensities_P1)
    camera_config.set_intensities_P4(intensities_P4)
    camera_config.produce_frame(num_images)

    
def track_purkinje(src_file: str):
    """
    2. Track Purkinje Images (Polymorphism)
    Sample usage of Tracker class
    """
    # tracker = ClusteringTracker()
    # tracker = DoHTracker_cluster()
    tracker = DoHTracker_skimage()
    tracker.track(src_file)


if __name__ == "__main__":
    # python main.py -n 1000 -i gen_image
    parser = argparse.ArgumentParser(prog="AP Lab: Purkinje Image Synthetic & Tracker", description="Purkinje Image Synthetic and Tracker.")
    # ...
    parser.add_argument("-n" ,"--num_images", type=int, default=None, help="Number of images to generate")
    parser.add_argument("-i" ,"--input", type=str, default=None, help="Input image folder directory or a single video file. Please place the file under `data` directory")
    args = parser.parse_args()

    num_images = args.num_images
    input_dir = args.input

    # 1. Generate synthetic images: -n {num_images}
    if num_images:
        synthetic_random(num_images)
    
    # # 2. Track Purkinje Images (Polymorphism): -i {video_dir} | {image_dir}
    if input_dir:
        track_purkinje(input_dir)

        # 3. Calculate Percentage Error
        # perc_error("data/gen_image.json", "data/pred_gen_image_cluster.json", "cluster")
        # perc_error("data/gen_image.json", "data/pred_gen_image_doh_cluster.json", "doh_cluster")
        perc_error("data/gen_image.json", "data/pred_gen_image_doh_skimage.json", "doh_skimage")
