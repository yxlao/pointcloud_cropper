import pointcloud_cropper as pc
import numpy as np
import time


def np_get_crop_indices(points, x_min, y_min, z_min, x_max, y_max, z_max):
    return np.nonzero((points[:, 0] >= x_min) & (points[:, 0] <= x_max)
                      & (points[:, 1] >= y_min) & (points[:, 1] <= y_max)
                      & (points[:, 2] >= z_min) & (points[:, 2] <= z_max))[0]


def run_benchmark():
    # Set up data.
    np.random.seed(0)
    points = np.random.rand(int(1e8), 3)
    x_min = 0.25
    y_min = 0.25
    z_min = 0.25
    x_max = 0.75
    y_max = 0.75
    z_max = 0.75

    print(f"Cropping point cloud of shape {points.shape}")

    # Crop with numpy.
    s = time.time()
    indices_np = np_get_crop_indices(points=points,
                                     x_min=x_min,
                                     y_min=y_min,
                                     z_min=z_min,
                                     x_max=x_max,
                                     y_max=y_max,
                                     z_max=z_max)
    print("Time with numpy", time.time() - s)

    # Crop impl with TBB.
    s = time.time()
    indices_tbb = pc.get_crop_indices(points=points,
                                      x_min=x_min,
                                      y_min=y_min,
                                      z_min=z_min,
                                      x_max=x_max,
                                      y_max=y_max,
                                      z_max=z_max)
    print("Time with TBB", time.time() - s)

    # Crop serial impl.
    s = time.time()
    indices_serial = pc.get_crop_indices_serial(points=points,
                                                x_min=x_min,
                                                y_min=y_min,
                                                z_min=z_min,
                                                x_max=x_max,
                                                y_max=y_max,
                                                z_max=z_max)
    print("Time with serial impl", time.time() - s)

    if np.allclose(indices_np, indices_tbb) and np.allclose(
            indices_np, indices_serial):
        print("Equal")


if __name__ == "__main__":
    run_benchmark()
