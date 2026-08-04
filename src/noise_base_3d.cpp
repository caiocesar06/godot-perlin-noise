#include "noise_base_3d.hpp"
#include <godot_cpp/variant/utility_functions.hpp>
#include <algorithm>

namespace godot {
    void NoiseBase3D::_bind_methods() {
        ClassDB::bind_method(D_METHOD("set_octaves", "octaves"), &NoiseBase3D::set_octaves);
        ClassDB::bind_method(D_METHOD("get_octaves"), &NoiseBase3D::get_octaves);
        ClassDB::bind_method(D_METHOD("set_persistence", "persistence"), &NoiseBase3D::set_persistence);
        ClassDB::bind_method(D_METHOD("get_persistence"), &NoiseBase3D::get_persistence);
        ClassDB::bind_method(D_METHOD("set_lacunarity", "lacunarity"), &NoiseBase3D::set_lacunarity);
        ClassDB::bind_method(D_METHOD("get_lacunarity"), &NoiseBase3D::get_lacunarity);
        ClassDB::bind_method(D_METHOD("fbm", "x", "y", "z"), &NoiseBase3D::fbm);
        ClassDB::bind_method(D_METHOD("get_fbm_buffer_3d", "width", "height", "depth", "scale", "offset_x", "offset_y", "offset_z"), &NoiseBase3D::get_fbm_buffer_3d);

        ADD_PROPERTY(PropertyInfo(Variant::INT, "octaves", PROPERTY_HINT_RANGE, "1,16,1"), "set_octaves", "get_octaves");
        ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "persistence", PROPERTY_HINT_RANGE, "0.0,1.0,0.05"), "set_persistence", "get_persistence");
        ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "lacunarity", PROPERTY_HINT_RANGE, "1.0,4.0,0.1"), "set_lacunarity", "get_lacunarity");
    }

    void NoiseBase3D::set_octaves(int32_t p_octaves) {
        if (p_octaves < 1 || p_octaves > 16) WARN_PRINT("Perlin 3D: Octaves deve estar entre 1 e 16.");
        _octaves = std::clamp(p_octaves, 1, 16);
    }
    int32_t NoiseBase3D::get_octaves() const { return _octaves; }

    void NoiseBase3D::set_persistence(double p_persistence) {
        if (p_persistence < 0.0 || p_persistence > 1.0) WARN_PRINT("Perlin 3D: Persistence deve estar entre 0.0 e 1.0.");
        _persistence = std::clamp(p_persistence, 0.0, 1.0);
    }
    double NoiseBase3D::get_persistence() const { return _persistence; }

    void NoiseBase3D::set_lacunarity(double p_lacunarity) {
        if (p_lacunarity < 1.0 || p_lacunarity > 4.0) WARN_PRINT("Perlin 3D: Lacunarity deve estar entre 1.0 e 4.0.");
        _lacunarity = std::clamp(p_lacunarity, 1.0, 4.0);
    }
    double NoiseBase3D::get_lacunarity() const { return _lacunarity; }

    double NoiseBase3D::fbm(double x, double y, double z) const {
        double total = 0.0;
        double amplitude = 1.0;
        double frequency = 1.0;
        double max_value = 0.0;

        for (int i = 0; i < _octaves; ++i) {
            total += sample(x * frequency, y * frequency, z * frequency) * amplitude;
            max_value += amplitude;
            amplitude *= _persistence;
            frequency *= _lacunarity;
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
