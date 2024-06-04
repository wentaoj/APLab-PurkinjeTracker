import numpy as np
import cv2
import os

# Abstract class; Base class
class BaseTracker:
    def __init__(self):
        pass

    def detect(self, image):
        """
        Low-resolution detection: Detect approximate locations of P1 and P4
        inputs: image
        output: rois(x, y, w, h)
        """
        # Step 1: Reduce Noise (cv2.GaussianBlur)
        # Step 2: Binarize the image (thresholding, separate P1, P4)
        # Step 3: Clustering: Identify largest cluster for each
        # Step 4: Obtained initial ROIs: Create bounding boxes to identify and extract
        return NotImplementedError("Return ROIs")

    def localize(self, image, rois):
        """
        High-resolution localization: Refine the locations by centroid.
        inputs: image, rois(x, y, w, h)
        output: centroids(cX, cY)
        """
        centroids = []
        for x, y, w, h in rois:
            # Step 1: crop the image
            roi = image[y:y+h, x:x+w]
            # Step 2: Calculate centroid
            # Implementing and using 1. Clustering here

            # or implement 2 in the derived class
        return centroids

    def track(self, image):
        """Main function to track"""
        rois = self.detect(image)
        return self.localize(image, rois)
        # Add Kalman filter above.
        
# Inherited class (subclass); Polymorphism
class MyTracker(BaseTracker):
    def __init__(self):
        super().__init__()
    
    def detect(self, image):
        return super().detect(image) # accept Base class method
        # or implementing other methods for replacement

    def localize(self, image, rois):
        return super().localize(image, rois) # Accept clustering method
        # or implementing and using 2. DoH (Determinant of Hessian)
    
    
if __name__ == "__main__":
    # Samples
    output_dir = "./images/"
    # Sample usage of Tracker class
    tracker = MyTracker()
    image = cv2.imread(os.path.join(output_dir, "frame#0000.png"), cv2.IMREAD_GRAYSCALE)
    result = tracker.track(image)
