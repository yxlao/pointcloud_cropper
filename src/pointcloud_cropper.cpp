#include <pybind11/functional.h>
#include <pybind11/numpy.h>
#include <pybind11/operators.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/stl_bind.h>
#include <tbb/parallel_for.h>
#include <tbb/parallel_scan.h>

#include <cstring>

#include "stream_compaction.h"

namespace py = pybind11;
using namespace py::literals;

static py::array ToIntArray1D(const int *array_buf, int N) {
    py::dtype py_dtype(py::format_descriptor<int32_t>::format());
    py::array::ShapeContainer py_shape({N});
    py::array::StridesContainer py_strides({4});  // int32_t is 4 bytes

    auto py_destructor = [](PyObject *data) {
        // Deallocates buffer when python numpy variable goes out of scope.
        int *buf = reinterpret_cast<int *>(
                PyCapsule_GetPointer(data, "int32_t buffer"));
        if (buf) {
            // std::cout << "destructed" << std::endl;
            delete[] buf;
        } else {
            PyErr_Clear();
        }
    };
    py::capsule py_capsule(array_buf, "int32_t buffer", py_destructor);
    return py::array(py_dtype, py_shape, py_strides, array_buf, py_capsule);
}

static inline bool IsPointInRange(double x,
                                  double y,
                                  double z,
                                  double x_min,
                                  double y_min,
                                  double z_min,
                                  double x_max,
                                  double y_max,
                                  double z_max) {
    return x >= x_min && x <= x_max && y >= y_min && y <= y_max && z >= z_min &&
           z <= z_max;
}

template <typename T>
static py::array GetCropIndex(py::array_t<T> points,
                              double x_min,
                              double y_min,
                              double z_min,
                              double x_max,
                              double y_max,
                              double z_max) {
    py::buffer_info info = points.request();
    std::vector<int> shape(info.shape.begin(), info.shape.end());

    if (shape.size() != 2 || shape[1] != 3) {
        throw std::runtime_error("points must be (N, 3)");
    }
    int N = shape[0];
    const T *points_ptr = static_cast<const T *>(info.ptr);

    int *array_buf = new int[N];
    int total_sum = tbb::parallel_scan(
            tbb::blocked_range<int>(0, N), 0,
            [&](const tbb::blocked_range<int> &r, int sum,
                bool is_final_scan) -> int {
                int temp = sum;
                for (int i = r.begin(); i < r.end(); ++i) {
                    const T *point_ptr = points_ptr + i * 3;
                    bool selected = IsPointInRange(point_ptr[0], point_ptr[1],
                                                   point_ptr[2], x_min, y_min,
                                                   z_min, x_max, y_max, z_max);
                    if (selected) {
                        if (is_final_scan) {
                            array_buf[temp] = i;
                        }
                        temp += 1;
                    }
                }
                return temp;
            },
            [](int left, int right) { return left + right; });

    return ToIntArray1D(array_buf, total_sum);
}

template <typename T>
static py::array GetCropIndexSerial(py::array_t<T> points,
                                    double x_min,
                                    double y_min,
                                    double z_min,
                                    double x_max,
                                    double y_max,
                                    double z_max) {
    py::buffer_info info = points.request();
    std::vector<int> shape(info.shape.begin(), info.shape.end());

    if (shape.size() != 2 || shape[1] != 3) {
        throw std::runtime_error("points must be (N, 3)");
    }
    int N = shape[0];
    const T *points_ptr = static_cast<const T *>(info.ptr);

    int *array_buf = new int[N];
    int next_idx = 0;
    for (int i = 0; i < N; i++) {
        const T *point_ptr = points_ptr + i * 3;
        if (IsPointInRange(point_ptr[0], point_ptr[1], point_ptr[2], x_min,
                           y_min, z_min, x_max, y_max, z_max)) {
            array_buf[next_idx] = i;
            next_idx++;
        }
    }

    return ToIntArray1D(array_buf, next_idx);
}

PYBIND11_MODULE(pointcloud_cropper, m) {
    m.def("get_crop_index", &GetCropIndex<double>, "points"_a, "x_min"_a,
          "y_min"_a, "z_min"_a, "x_max"_a, "y_max"_a, "z_max"_a);
    m.def("get_crop_index", &GetCropIndex<float>, "points"_a, "x_min"_a,
          "y_min"_a, "z_min"_a, "x_max"_a, "y_max"_a, "z_max"_a);
    m.def("get_crop_index_serial", &GetCropIndexSerial<double>, "points"_a,
          "x_min"_a, "y_min"_a, "z_min"_a, "x_max"_a, "y_max"_a, "z_max"_a);
    m.def("get_crop_index_serial", &GetCropIndexSerial<float>, "points"_a,
          "x_min"_a, "y_min"_a, "z_min"_a, "x_max"_a, "y_max"_a, "z_max"_a);
}
