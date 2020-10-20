# Point cloud cropper

Efficient point cloud bounding-box cropping. Parallelized implementation based
on [TBB](https://github.com/oneapi-src/oneTBB) `parallel_scan`.

### Installation

```bash
# Dependency.
sudo apt install cmake

# Get code.
# --recursive is required, or `git submodule update --init --recursive`.
git clone --recursive https://github.com/yxlao/pointcloud_cropper
cd pointcloud_cropper

# Build (activate virtualenv fist).
python setup.py install

# Test.
python benchmark.py
```

### Usage

```python
import pointcloud_cropper as pc
import numpy as np

# Prepare (N, 3) points of np.float64 or np.float32.
np.random.seed(0)
points = np.random.rand(int(1e8), 3)
print(points.shape)

# get_crop_index returns the selected index.
index_np = pc.get_crop_index(points=points,
                             x_min=0.25,
                             y_min=0.25,
                             z_min=0.25,
                             x_max=0.75,
                             y_max=0.75,
                             z_max=0.75)
cropped_points = points[index_np]
print(cropped_points.shape)
```

### Performance

1e8 points, 50% selection ratio. Measured with Intel i9-9980XE.

```txt
Time with numpy: 1.428987979888916 sec
Time with TBB  : 0.103589773178100 sec
```
