import numpy as np
import cv2
import os
from abc import ABC, abstractmethod
from sklearn.cluster import DBSCAN
import matplotlib.pyplot as plt

# Abstract class; Base class
class BaseTracker(ABC):
    def __init__(self):
        pass

    @abstractmethod
    def track(self, image_BGR):
        """
        Abstract function for tracking P1 and P4 in the given image.
        inputs: [image (ndarray)] The input image in which P1 and P4 need to be tracked.
        output: [dict] A dictionary containing the coordinates of P1 and P4.
        """
        pass

        
# Inherited class (subclass);
class MyTracker(BaseTracker):
    def __init__(self):
        super().__init__()
        self.kalman = self._init_kalman_filter()

    def track(self, image: np.ndarray) -> dict:
        """
        Track P1 and P4 in the given image.
        inputs: [image_GRAYSCALE (ndarray)] The input image
        output: [dict] A dictionary containing the coordinates of P1 and P4, and difference dx and dy.
        """
        # Step 1: Convert to grayscale
        if len(image.shape) == 3: image = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)
        
        # Step 2: Detect approximate locations of P1 and P4
        rois = self._detect(image)
        
        # Step 3: Refine the locations by centroid
        centroids = self._localize(image, rois)
        
        # Step 4: Kalman Filter
        for centroid in centroids:
            measurement = np.array([[centroid[0]], [centroid[1]]], dtype=np.float32)
            self.kalman.correct(measurement)
        # Predict the next state
        prediction = self.kalman.predict()
        dx = np.abs(prediction[0] - centroids[0][0])
        dy = np.abs(prediction[1] - centroids[0][1])

        return {"P1": centroids[0], "P4": centroids[1], "dx": dx, "dy": dy}


    def _init_kalman_filter(self) -> cv2.KalmanFilter:
        """ Initialize Kalman Filter """
        kalman = cv2.KalmanFilter(dynamParams=4, measureParams=2)
        # measure position x, y
        kalman.measurementMatrix = np.array([
            [1, 0, 0, 0], 
            [0, 1, 0, 0]
        ], dtype=np.float32)
        # transition matrix: x, y / dx, dy (velocity)
        kalman.transitionMatrix = np.array([
            [1, 0, 1, 0], 
            [0, 1, 0, 1], 
            [0, 0, 1, 0], 
            [0, 0, 0, 1]
        ], dtype=np.float32)
        kalman.processNoiseCov = np.array([
            [1, 0, 0, 0], 
            [0, 1, 0, 0], 
            [0, 0, 1, 0], 
            [0, 0, 0, 1]
        ], dtype=np.float32) * 0.03 # predictable
        kalman.measurementNoiseCov = np.array([
            [1, 0], 
            [0, 1]
        ], dtype=np.float32) * 1e-1
        kalman.errorCovPost = np.eye(4, dtype=np.float32)
        kalman.statePost = np.zeros(4, dtype=np.float32)
        return kalman

    
    def _detect(self, image: np.ndarray, factor: int = 2) -> list:
        """
        (Low-resolution) detection: Detect approximate locations of P1 and P4
        inputs: image
        output: rois(x, y, w, h)
        # Qeustion TODO: In Paper, P1: [256, 256] ROIs, P4: [64, 64] ROIs, do we need to follow this rule?
        """
        # First, Downsample the image by factor of 8 (here 2 since 800x600 only)
        kernel_size = (factor * 2) + 1
        # Step 1: Reduce Noise with Gaussian Blur; sigma can be parameter
        blur = cv2.GaussianBlur(src=image, ksize=(kernel_size, kernel_size), sigmaX=0)
        downsampled = cv2.resize(blur, (image.shape[1] // factor, image.shape[0] // factor), interpolation=cv2.INTER_LINEAR)
        # Step 2: Otsu's Thresholding
        ret, dst = cv2.threshold(downsampled, 0, 255, cv2.THRESH_BINARY+cv2.THRESH_OTSU)
        # Morphological Operationss with closing. Performed more like a circle if use gaussian filter
        closing = cv2.morphologyEx(src=dst, op=cv2.MORPH_CLOSE, kernel=np.ones((5, 5), np.uint8))
        # Step 3: Find extreme outer contours from morphology closing binaries
        contours, hierarchy = cv2.findContours(image=closing, mode=cv2.RETR_EXTERNAL, method=cv2.CHAIN_APPROX_SIMPLE)
        
        # print(f"Found {len(contours)} contours")
        # Sort contours by area in descending order
        contours = sorted(contours, key=cv2.contourArea, reverse=True)

        if len(contours) < 2:
            raise ValueError("Detection Error: Blink, or Not enough contours found")
        rois = [(x*factor, y*factor, w*factor, h*factor) for x, y, w, h in [cv2.boundingRect(contour) for contour in contours[:2]]]
        print(f"ROIs (x, y, w, h): {rois}")
        # cv2.imshow("Cropped image 1", image[rois[0][1]:rois[0][1]+rois[0][3], rois[0][0]:rois[0][0]+rois[0][2]])
        # cv2.imshow("Cropped image 2", image[rois[1][1]:rois[1][1]+rois[1][3], rois[1][0]:rois[1][0]+rois[1][2]])
        return rois


    def _localize(self, image: np.ndarray, rois: list) -> list:
        """
        (High-resolution) localization: Refine the locations by centroid.
        inputs: image, rois(x, y, w, h)
        output: centroids(cX, cY)
        """
        centroids = []
        for (x, y, w, h) in rois:
            # Step 1: crop the image
            croppred_img = image[y:y+h, x:x+w]
            # TODO: Question: if we need to upscale resolution?

            # Step 2: Calculate Centroids
            blur_cropped = cv2.GaussianBlur(src=croppred_img, ksize=(3, 3), sigmaX=1)
            ret, dst_cropped = cv2.threshold(blur_cropped, 0, 255, cv2.THRESH_BINARY+cv2.THRESH_OTSU)
            cv2.imshow("Cropped image", dst_cropped)

            points = np.column_stack(np.where(dst_cropped.transpose() > 0))
            if len(points) == 0: continue
            # Implementing and using Clustering here
            clustering = DBSCAN(eps=3, min_samples=2, metric='euclidean')
            clustering.fit(points)
            labels = clustering.labels_
            unique_labels, indices = np.unique(labels, return_index=True)
            # Sort the unique labels and remove noise label -1
            unique_labels = labels[np.sort(indices)]
            unique_labels = unique_labels[unique_labels != -1]
            
            if len(unique_labels) == 0:
                print(unique_labels, len(unique_labels))
                raise ValueError("Localization Failed: No centroids found")
            
            label_best = max(unique_labels, key=lambda x: np.sum(labels == x))
            points_best = points[labels == label_best]

            cX = np.mean(points_best[:, 0])
            cY = np.mean(points_best[:, 1])
            centroids.append((cX + x, cY + y))
        
        if len(centroids) < 2: 
            print(centroids, len(centroids))
            raise ValueError("Localization Failed: Not enough centroids found")
        
        print(f"Centroids (cP1, cP4): {centroids}")
        return centroids
    
    
if __name__ == "__main__":
    # Samples
    output_dir = "./images"
    image = cv2.imread(os.path.join(output_dir, "frame#0000.png"), cv2.IMREAD_GRAYSCALE)

    tracker = MyTracker()
    track_dic = tracker.track(image)
    print(f"P1: {track_dic['P1']}, P4: {track_dic['P4']}, dx: {track_dic['dx']}, dy: {track_dic['dy']}")
