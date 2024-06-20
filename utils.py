import time
import os
import shutil
import cv2
import numpy as np
import pandas as pd
import json
from functools import wraps

def timingdecorator(func):
    @wraps(func)
    def wrapper(*args, **kwargs):
        start_time = time.time()
        result = func(*args, **kwargs)
        end_time = time.time()
        print(f"Function {func.__name__}: Elapsed time: {end_time - start_time:.3f} seconds.")
        return result
    return wrapper


def image2video(folder_dir: str, output_dir: str, fps: int = 30):
    img = cv2.imread(os.path.join(folder_dir, os.listdir(folder_dir)[0]))
    images = [img for img in os.listdir(folder_dir) if img.endswith(".png")]
    video = cv2.VideoWriter(output_dir, cv2.VideoWriter_fourcc(*'DIVX'), fps, (img.shape[1], img.shape[0]))
    for image in images:
        video.write(cv2.imread(os.path.join(folder_dir, image)))
    cv2.destroyAllWindows()
    video.release()
    print(f"Video saved at {output_dir}")


def video2image(video_dir: str, output_dir: str):
    if os.path.exists(output_dir):
        shutil.rmtree(output_dir)
    os.makedirs(output_dir, exist_ok=False)

    vidcap = cv2.VideoCapture(video_dir)
    next, image = vidcap.read()
    frame_num = 0
    while next:
        cv2.imwrite(os.path.join(output_dir, f"frame#{frame_num:04d}.png"), image)
        next, image = vidcap.read()
        frame_num += 1
    print(f"Images saved at {output_dir}")


def write_json(info: dict, output_dir: str):
    data = []
    if os.path.exists(output_dir):
        data = read_json(output_dir)
    data.append(info)
    with open(output_dir, "w") as f:
        json.dump(data, f, indent=2)

def read_json(input_dir: str) -> dict:
    with open(input_dir, "r") as f:
        data = json.load(f)
    return data


def load_pipeline_src(src_file: str) -> tuple[str, list[str]]:
    """
    Load the source file (image_folder or video). Only accepts images or video under `~/data/` directory.
    If it is a video, convert it to images under `~/data/{video_name}`.
    return a tuple of (image_files directory basename, [list of image files]).
    """
    if os.path.isdir(src_file): # already an image folder directory
        image_basename = os.path.basename(src_file)
        image_folder = src_file
    elif os.path.isfile(src_file) and src_file.endswith(('.mp4', '.avi', '.mov')):
        image_basename = f"{os.path.splitext(os.path.basename(src_file))[0]}_images" # create a new folder
        image_folder = os.path.join("data", image_basename)
        os.makedirs(image_folder, exist_ok=True)
        video2image(src_file, image_folder)
    else:
        print("Invalid source input. Please try another file or directory.")
        return None, []

    image_files = [f for f in sorted(os.listdir(image_folder)) if f.endswith(('.png', '.jpg', '.jpeg'))]
    return (image_basename, image_files)

def calculate_perc_error(true_pos: tuple[float, float], detected_pos: tuple[float, float]) -> float:
    """Calculate the percent error between true and detected positions."""
    try:
        error = np.sqrt((true_pos[0] - detected_pos[0]) ** 2 + (true_pos[1] - detected_pos[1]) ** 2)
        true_magnitude = np.sqrt(true_pos[0] ** 2 + true_pos[1] ** 2)
        percent_error = (error / true_magnitude) * 100
        percent_error = np.round(percent_error, 4)
    except TypeError as e:
        # print(f"Error: Detected a frame of blink or no centroid found.")
        percent_error = None
    return percent_error

def perc_error(true_pos_file :str, pred_pos_file: str, output_name: str):
    """
    Calculate the percent error between true and detected centroids.
    frame_info = {"frame#": i, "P1": pos_P1, "P4": pos_P4, "dx": dx, "dy": dy}
    """
    true_pos = pd.read_json(true_pos_file)
    pred_pos = pd.read_json(pred_pos_file)
    full_table = true_pos.merge(pred_pos, on="frame#", suffixes=("_true", "_pred"))

    full_table["perc_error_P1"] = full_table.apply(lambda x: calculate_perc_error(x["P1_true"], x["P1_pred"]), axis=1)
    full_table["perc_error_P4"] = full_table.apply(lambda x: calculate_perc_error(x["P4_true"], x["P4_pred"]), axis=1)

    full_table["error_rate"] = full_table.apply(lambda x: np.mean([x["perc_error_P1"], x["perc_error_P4"]]), axis=1)
    error_rate = full_table.pop('error_rate')
    full_table.insert(1, 'error_rate', error_rate)

    # print(full_table)
    full_table.to_csv(f"data/result_{output_name}.csv", index=False, na_rep=None)


if __name__ == "__main__":
    perc_error("data/gen_image.json", "data/pred_gen_image.json")