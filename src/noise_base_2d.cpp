#include "noise_base_2d.hpp"
#include <algorithm>

namespace godot {
    void NoiseBase2D::_bind_methods() {
        ClassDB::bind_method(D_METHOD("fbm", "x", "y"), &NoiseBase2D::fbm);
        ClassDB::bind_method(D_METHOD("get_fbm_buffer", "width", "height", "scale", "offset_x", "offset_y"), &NoiseBase2D::get_fbm_buffer);
    }

    double NoiseBase2D::fbm(double x, double y) const {
        double total = 0.0;
        double amplitude = 1.0;
        double frequency = 1.0;
        double max_value = 0.0;

        int32_t octs = get_octaves();
        double  pers = get_persistence();
        double  lac  = get_lacunarity();

        for (int i = 0; i < octs; ++i) {
            total += sample(x * frequency, y * frequency) * amplitude;
            max_value += amplitude;
            amplitude *= pers;
            frequency *= lac;
        }
        return total / max_value;
    }

    PackedByteArray NoiseBase2D::get_fbm_buffer(int64_t width, int64_t height, double scale, double offset_x, double offset_y) const {
        ERR_FAIL_COND_V_MSG(width <= 0 || height <= 0, PackedByteArray(), "Width e Height devem ser maiores que zero.");

        PackedByteArray buffer;
        buffer.resize(width * height * 4);
        uint8_t* ptr = buffer.ptrw();

#pragma omp parallel for
        for (int64_t y = 0; y < height; ++y) {
            for (int64_t x = 0; x < width; ++x) {
                double nx = (x + offset_x) * scale;
                double ny = (y + offset_y) * scale;

                double val = fbm(nx, ny);
                int64_t color = std::clamp(static_cast<int64_t>((val + 1.0) * 127.5), int64_t(0), int64_t(255));

                int64_t idx = (y * width + x) * 4;
                ptr[idx + 0] = static_cast<uint8_t>(color);
                ptr[idx + 1] = static_cast<uint8_t>(color);
                ptr[idx + 2] = static_cast<uint8_t>(color);
                ptr[idx + 3] = 255;
            }
        }
        return buffer;
    }
}
