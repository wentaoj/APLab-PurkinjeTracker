from base_tracker import IntermediateTracker
from utils import timingdecorator, load_pipeline_src, image2video, perc_error
import cv2
import numpy as np
import os
import json
from tqdm import tqdm
import matplotlib.pyplot as plt
import argparse

class ClusteringTracker(IntermediateTracker):
    def __init__(self):
        super().__init__()

    @timingdecorator
    def track(self, src_file: str):
        """Track P1 and P4 in the given image."""
        image_basename, image_files = load_pipeline_src(os.path.join("data", src_file))
        pred_info = []
        for i, image_file in enumerate(tqdm(image_files)):
            try:
                image = cv2.imread(os.path.join("data", image_basename, image_file), cv2.IMREAD_GRAYSCALE)
                preprocessed_image = self.prepreprocess_image(image, factor=2)
                rois = self._detect_rois(preprocessed_image, factor=2)
                centroids = self._localize_clustering(image, rois)
                pred_info.append(self.save_results(i, centroids))
            except ValueError:
                pred_info.append({"frame#": i, "P1": (np.nan, np.nan), "P4": (np.nan, np.nan), "dx": np.nan, "dy": np.nan})

        with open(os.path.join("data", f"pred_{image_basename}_cluster.json"), "w") as f:
            json.dump(pred_info, f, indent=2)
        print(f"Predicted resulting centroids saved at `data/pred_{image_basename}_cluster.json`")

    def _detect_rois(self, image: np.ndarray, factor: int) -> list[tuple[int, int, int, int]]:
        """(Low-resolution) Detect ROIs in the preprocessed image. """
        contours, _ = cv2.findContours(image=image, mode=cv2.RETR_EXTERNAL, method=cv2.CHAIN_APPROX_SIMPLE) # only return integer
        contours = sorted(contours, key=cv2.contourArea, reverse=True)
        if len(contours) < 2:
            raise ValueError("Detection Error: Not enough contours found")
        rois = [(x*factor, y*factor, w*factor, h*factor) for x, y, w, h in [cv2.boundingRect(contour) for contour in contours[:2]]]
        return rois

    def _localize_clustering(self, image: np.ndarray, rois: list[tuple[int, int, int, int]]) -> list[tuple[float, float]]:
        """(High-resolution) Localize centroids within the ROIs using DBSCAN."""
        from sklearn.cluster import DBSCAN
        
        centroids = []
        for (x, y, w, h) in rois:
            cropped_img = image[y:y+h, x:x+w]
            blur_cropped = cv2.GaussianBlur(src=cropped_img, ksize=(3, 3), sigmaX=1)
            ret, dst_cropped = cv2.threshold(blur_cropped, 0, 255, cv2.THRESH_BINARY+cv2.THRESH_OTSU)
            points = np.column_stack(np.where(dst_cropped.transpose() > 0))
            if len(points) == 0: 
                continue

            clustering = DBSCAN(eps=3, min_samples=2, metric="euclidean")
            clustering.fit(points)
            labels = clustering.labels_
            unique_labels, indices = np.unique(labels, return_index=True)
            unique_labels = labels[np.sort(indices)]
            unique_labels = unique_labels[unique_labels != -1]

            if len(unique_labels) == 0:
                raise ValueError("Localization Failed: No centroids found")

            label_best = max(unique_labels, key=lambda x: np.sum(labels == x))
            points_best = points[labels == label_best]

            cX = np.mean(points_best[:, 0])
            cY = np.mean(points_best[:, 1])
            centroids.append((cX + x, cY + y))

        if len(centroids) < 2:
            raise ValueError("Localization Failed: Not enough centroids found")
        
        return centroids


class DoHTracker_skimage(IntermediateTracker):
    def __init__(self):
        super().__init__()

    @timingdecorator
    def track(self, src_file: str):
        """Track P1 and P4 using the Difference of Hessians (DoH) method."""
        image_basename, image_files = load_pipeline_src(os.path.join("data", src_file))
        pred_info = []
        for i, image_file in enumerate(tqdm(image_files)):
            try:
                image = cv2.imread(os.path.join("data", image_basename, image_file), cv2.IMREAD_GRAYSCALE)
                preprocessed_image = self.prepreprocess_image(image, factor=1)
                centroids = self._detect_blobs(preprocessed_image)
                pred_info.append(self.save_results(i, centroids))
            except ValueError:
                pred_info.append({"frame#": i, "P1": (np.nan, np.nan), "P4": (np.nan, np.nan), "dx": np.nan, "dy": np.nan})

        with open(os.path.join("data", f"pred_{image_basename}_doh_skimage.json"), "w") as f:
            json.dump(pred_info, f, indent=2)
        print(f"Prediction results saved at `data/pred_{image_basename}_doh_skimage.json`")

    def _detect_blobs(self, image):
        """Detect blobs using the Difference of Hessian (DoH) method and return centroids."""
        from skimage.feature import blob_doh

        blobs_detected = blob_doh(image, max_sigma=30, threshold=0.01)
        if blobs_detected.size < 2:
            raise ValueError("DoH Detection Error: Not enough blobs found")
        blobs = sorted(blobs_detected, key=lambda b: b[2], reverse=True)[:2]
        centroids = [(blob[1], blob[0]) for blob in blobs]
        return centroids
    

class DoHTracker_cluster(IntermediateTracker):
    def __init__(self):
        super().__init__()

    @timingdecorator
    def track(self, src_file: str):
        """Track P1 and P4 using the Difference of Hessians (DoH) method."""
        image_basename, image_files = load_pipeline_src(os.path.join("data", src_file))
        pred_info = []
        for i, image_file in enumerate(tqdm(image_files)):
            try:
                image = cv2.imread(os.path.join("data", image_basename, image_file), cv2.IMREAD_GRAYSCALE)
                preprocessed_image = self.prepreprocess_image(image, factor=1)
                centroids = self._detect_blobs(preprocessed_image)
                if len(centroids) < 2:
                    raise ValueError("DoH Detection Error: Not enough blobs found")
                pred_info.append(self.save_results(i, centroids))
            except ValueError:
                pred_info.append({"frame#": i, "P1": (np.nan, np.nan), "P4": (np.nan, np.nan), "dx": np.nan, "dy": np.nan})

        with open(os.path.join("data", f"pred_{image_basename}_doh_cluster.json"), "w") as f:
            json.dump(pred_info, f, indent=2)
        print(f"Prediction results saved at `data/pred_{image_basename}_doh_cluster.json`")

    def _determinant_of_hessian(self, image):
        dxx = cv2.Sobel(image, cv2.CV_32F, 2, 0, ksize=3)
        dyy = cv2.Sobel(image, cv2.CV_32F, 0, 2, ksize=3)
        dxy = cv2.Sobel(image, cv2.CV_32F, 1, 1, ksize=3)
        return dxx * dyy - dxy * dxy

    def _detect_blobs(self, image):
        from sklearn.cluster import DBSCAN

        hessian_det = self._determinant_of_hessian(image)
        blobs = np.argwhere(hessian_det > 0)
        blobs = blobs[:, [1, 0]]  # (x, y) format
        clustering = DBSCAN(eps=15, min_samples=2).fit(blobs)
        labels = clustering.labels_
        grouped_blobs = {}
        for blob, label in zip(blobs, labels):
            if label == -1:
                continue  # Ignore noise
            if label not in grouped_blobs:
                grouped_blobs[label] = []
            grouped_blobs[label].append(blob)
        centroids = []
        for group in grouped_blobs.values():
            centroid = np.mean(group, axis=0)
            centroids.append(centroid.tolist())
        return centroids


class KalmanFilter(IntermediateTracker):
    def __init__(self):
        super().__init__()

    def initialize(self):
        pass

    def measure(self):
        pass

    def update(self):
        pass

    @timingdecorator
    def track(self, src_file: str) -> dict:
        """
        (predict) Kalman Filter Tracker logic
        """
        image_basename, image_files = load_pipeline_src(os.path.join("data", src_file))
