# Random Image Synthesis (Week May30)

### Environment
```bash
python3 -m venv venv
source venv/bin/activate
pip3 install numpy tqdm opencv-python
```

or using conda:
```bash
conda create -n venv python=3.11
conda activate venv
pip install numpy tqdm opencv-python
```

Directory:
```bash
.
├── README.md
├── requirements.txt
├── ver2_Synthetic.py
├── tracker.py
├── main.py
├── images/ # run `python main.py` 
│   └── *.png # to generate synthetic images (Random P1 and P4 placements with dx, dy)
├── images_rot/ # run `python ver2_Synthetic.py` 
# to generate synthetic images (Rotated P1 and P4 placements from [-40, 40] degrees)
│   └── *.png 
└── image_rot_1000.mp4 # run ver2_Synthetic.py to generate video of rotated images
```

### Structure

1. Implemented Random Image Synthesis ([`ver2_Synthetic.py`](./ver2_Synthetic.py))
    a. Functions:
      - gaussian_circle: Generates gaussian circle with a central uniform intensity and Gaussian edges.
      - saturated_circle: Generates saturated circle with a uniform intensity.
      - randomize_images: Generates synthetic images with randomized placements for P1 (restricted on the right) and P4 (restricted on the left) with noise in the background; or change `np.random.uniform` from (0, 800) to generate X1/X4 without restrictions.
    
    b. Tasks:
      - Generated synthetic images with Gaussian and saturated intensity.
      - Calculated the displacements (dx, dy) between P1 and P4 for each generated image, stored dx, dy in a numpy array and printed them on each image.
<br>
1. Polymorphism ([`tracker.py`](./tracker.py))
    a. Classes:
     - BaseTracker: Base class containing abstract functions.
     - MyTracker: Inherited class.
  
    b. Functions:
     - detect: Low-resolution detection to approximate locations of P1 and P4.
     - localize: High-resolution localization by refining the locations using centroid calculation.
     - track: Main function to track Purkinje images.
<br>
1. Main ([`main.py`](./main.py)): Interfaces
    - synthetic_random: Interface to generate synthetic images with random placements of P1 and P4.
    - track_purkinje: Interface to run the tracker on the generated synthetic images (in progress).

### Usage
Generating Synthetic Images (Random Placements)
```bash
python main.py
```

Generating Synthetic Images (Eye-Rotated Placements)
```bash
python ver2_Synthetic.py
```
The generated images will be stored in the images_rot directory and the video will be saved at [`image_rot_1000.mp4`](./image_rot_1000.mp4) under current directory.
