#include "noise_base_3d.hpp"
#include <algorithm>

namespace godot {
    void NoiseBase3D::_bind_methods() {
        ClassDB::bind_method(D_METHOD("fbm", "x", "y", "z"), &NoiseBase3D::fbm);
        ClassDB::bind_method(D_METHOD("get_fbm_buffer_3d", "width", "height", "depth", "scale", "offset_x", "offset_y", "offset_z"), &NoiseBase3D::get_fbm_buffer_3d);

    }

    double NoiseBase3D::fbm(double x, double y, double z) const {
        double total = 0.0;
        double amplitude = 1.0;
        double frequency = 1.0;
        double max_value = 0.0;

        int32_t octs = get_octaves();
        double  pers = get_persistence();
        double  lac = get_lacunarity();

        for (int i = 0; i < octs; ++i) {
            total += sample(x * frequency, y * frequency, z * frequency) * amplitude;
            max_value += amplitude;
            amplitude *= pers;
            frequency *= lac;
        }
        return total / max_value;
    }

    PackedByteArray NoiseBase3D::get_fbm_buffer_3d(int64_t width, int64_t height, int64_t depth, double scale, double offset_x, double offset_y, double offset_z) const {
        ERR_FAIL_COND_V_MSG(width <= 0 || height <= 0 || depth <= 0, PackedByteArray(), "As dimensões devem ser maiores que zero.");

        PackedByteArray buffer;
        buffer.resize(width * height * depth);
        uint8_t* ptr = buffer.ptrw();

#pragma omp parallel for collapse(2)
        for (int64_t z = 0; z < depth; ++z) {
            for (int64_t y = 0; y < height; ++y) {
                for (int64_t x = 0; x < width; ++x) {

                    double nx = (x + offset_x) * scale;
                    double ny = (y + offset_y) * scale;
                    double nz = (z + offset_z) * scale;

                    double val = fbm(nx, ny, nz);

                    int64_t density = std::clamp(static_cast<int64_t>((val + 1.0) * 127.5), int64_t(0), int64_t(255));

                    int64_t idx = z * (width * height) + y * width + x;
                    ptr[idx] = static_cast<uint8_t>(density);
                }
            }
        }
        return buffer;
    }
}
