import cv2
import numpy as np
from abc import ABC, abstractmethod

class BaseTracker(ABC):
    @abstractmethod
    def track(self, src: str):
        pass


class IntermediateTracker(BaseTracker):
    def prepreprocess_image(self, image: np.ndarray, factor: int) -> np.ndarray:
        """Preprocess the image by downsampling and thresholding."""
        kernel_size = (factor * 2) + 1
        blur = cv2.GaussianBlur(src=image, ksize=(kernel_size, kernel_size), sigmaX=0)
        downsampled = cv2.resize(blur, (image.shape[1] // factor, image.shape[0] // factor), interpolation=cv2.INTER_LINEAR)
        ret, dst = cv2.threshold(downsampled, 0, 255, cv2.THRESH_BINARY+cv2.THRESH_OTSU)
        closing = cv2.morphologyEx(src=dst, op=cv2.MORPH_CLOSE, kernel=np.ones((5, 5), dtype=np.float32))
        return closing

    def save_results(self, frame_num: int, centroids: list[tuple[float, float]]) -> dict:
        """Calculate dx, dy and save the centroids of two blobs."""
        dx = np.abs(centroids[0][0] - centroids[1][0])
        dy = np.abs(centroids[0][1] - centroids[1][1])
        return {"frame#": frame_num, "P1": centroids[0], "P4": centroids[1], "dx": dx, "dy": dy}
