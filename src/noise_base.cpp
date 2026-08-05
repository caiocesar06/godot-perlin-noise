#include "noise_base.hpp"
#include <godot_cpp/variant/utility_functions.hpp>
#include <algorithm>

namespace godot {
    NoiseBase::~NoiseBase() = default;

    void NoiseBase::_bind_methods() {
        ClassDB::bind_method(
            D_METHOD("set_octaves", "octaves"),
            &NoiseBase::set_octaves
        );
        ClassDB::bind_method(
            D_METHOD("get_octaves"),
            &NoiseBase::get_octaves
        );
        ClassDB::bind_method(
            D_METHOD("set_persistence", "persistence"),
            &NoiseBase::set_persistence
        );
        ClassDB::bind_method(
            D_METHOD("get_persistence"),
            &NoiseBase::get_persistence
        );
        ClassDB::bind_method(
            D_METHOD("set_lacunarity", "lacunarity"),
            &NoiseBase::set_lacunarity
        );
        ClassDB::bind_method(
            D_METHOD("get_lacunarity"),
            &NoiseBase::get_lacunarity
        );
        ClassDB::bind_method(
            D_METHOD("set_fractal_type", "type"),
            &NoiseBase::set_fractal_type
        );
        ClassDB::bind_method(
            D_METHOD("get_fractal_type"),
            &NoiseBase::get_fractal_type
        );

        ClassDB::bind_method(
            D_METHOD("get_fractal_noise_2d", "x", "y"),
            &NoiseBase::get_fractal_noise_2d
        );
        ClassDB::bind_method(
            D_METHOD("get_fractal_noise_3d", "x", "y", "z"),
            &NoiseBase::get_fractal_noise_3d
        );
        ClassDB::bind_method(
            D_METHOD(
                "get_fbm_image_data",
                "width",
                "height",
                "scale",
                "offset_x",
                "offset_y"
            ),
            &NoiseBase::get_fbm_image_data
        );
        ClassDB::bind_method(
            D_METHOD(
                "get_fbm_volume_data",
                "width",
                "height",
                "depth",
                "scale",
                "offset_x",
                "offset_y",
                "offset_z"
            ),
            &NoiseBase::get_fbm_volume_data
        );

        ADD_PROPERTY(
            PropertyInfo(Variant::INT, "octaves", PROPERTY_HINT_RANGE, "1,16,1"),
            "set_octaves",
            "get_octaves"
        );
        ADD_PROPERTY(
            PropertyInfo(Variant::FLOAT, "persistence", PROPERTY_HINT_RANGE, "0.0,1.0,0.05"),
            "set_persistence",
            "get_persistence"
        );
        ADD_PROPERTY(
            PropertyInfo(Variant::FLOAT, "lacunarity", PROPERTY_HINT_RANGE, "1.0,4.0,0.1"),
            "set_lacunarity",
            "get_lacunarity"
        );
        ADD_PROPERTY(
            PropertyInfo(Variant::INT, "fractal_type", PROPERTY_HINT_ENUM, "FBM,Ridged,Billow"),
            "set_fractal_type",
            "get_fractal_type"
        );
    }

    void NoiseBase::set_octaves(int32_t p_octaves) {
        if (p_octaves < 1 || p_octaves > 16)
            WARN_PRINT("NoiseBase: 'octaves' deve estar entre 1 e 16. Usando o limite mais proximo.");
        _octaves = std::clamp(p_octaves, 1, 16);
    }
    int32_t NoiseBase::get_octaves() const { return _octaves; }

    void NoiseBase::set_persistence(double p_persistence) {
        if (p_persistence < 0.0 || p_persistence > 1.0)
            WARN_PRINT("NoiseBase: 'persistence' deve estar entre 0.0 e 1.0. Usando o limite mais proximo.");
        _persistence = std::clamp(p_persistence, 0.0, 1.0);
    }
    double NoiseBase::get_persistence() const { return _persistence; }

    void NoiseBase::set_lacunarity(double p_lacunarity) {
        if (p_lacunarity < 1.0 || p_lacunarity > 4.0)
            WARN_PRINT("NoiseBase: 'lacunarity' deve estar entre 1.0 e 4.0. Usando o limite mais proximo.");
        _lacunarity = std::clamp(p_lacunarity, 1.0, 4.0);
    }
    double NoiseBase::get_lacunarity() const { return _lacunarity; }

    void NoiseBase::set_fractal_type(int32_t p_type) {
        if (p_type < 0 || p_type > 2) {
            WARN_PRINT("NoiseBase: 'fractal_type' invalido. Usando FBM por padrao.");
            p_type = 0;
        }
        _fractal_type = static_cast<FractalType>(p_type);
    }
    int32_t NoiseBase::get_fractal_type() const { return static_cast<int32_t>(_fractal_type); }

    double NoiseBase::get_fractal_noise_2d(double x, double y) const {
        double total = 0.0;
        double amplitude = 1.0;
        double frequency = 1.0;
        double max_value = 0.0;
        for (int i = 0; i < _octaves; ++i) {
            double v = get_noise_2d(x * frequency, y * frequency);

            switch (_fractal_type) {
                case FractalType::FBM:
                    break;
                case FractalType::RIDGED:
                    v = 1.0 - (2.0 * std::abs(v));
                    break;
                case FractalType::BILLOW:
                    v = (2.0 * std::abs(v)) - 1.0;
                    break;
                default:
                    break;
            }

            total += v * amplitude;
            max_value += amplitude;
            amplitude *= _persistence;
            frequency *= _lacunarity;
        }
        return total / max_value;
    }

    double NoiseBase::get_fractal_noise_3d(double x, double y, double z) const {
        double total = 0.0;
        double amplitude = 1.0;
        double frequency = 1.0;
        double max_value = 0.0;
        for (int i = 0; i < _octaves; ++i) {
            double v = get_noise_3d(
                x * frequency,
                y * frequency,
                z * frequency
            );

            switch (_fractal_type) {
                case FractalType::FBM:
                    break;
                case FractalType::RIDGED:
                    v = 1.0 - (2.0 * std::abs(v));
                    break;
                case FractalType::BILLOW:
                    v = (2.0 * std::abs(v)) - 1.0;
                    break;
                default:
                    break;
            }

            total += v * amplitude;
            max_value += amplitude;
            amplitude *= _persistence;
            frequency *= _lacunarity;
        }
        return total / max_value;
    }

    PackedByteArray NoiseBase::get_fbm_image_data(
        int64_t width, int64_t height, double scale,
        double offset_x, double offset_y
    ) const {
        ERR_FAIL_COND_V_MSG(
            width <= 0 || height <= 0,
            PackedByteArray(),
            "Width e Height devem ser > 0."
        );
        constexpr int64_t MAX_BUFFER_BYTES = 512LL * 1024LL * 1024LL;
        ERR_FAIL_COND_V_MSG(
            width > (MAX_BUFFER_BYTES / 4) / height,
            PackedByteArray(),
            "Dimensoes excedem 512MB de RAM."
        );

        PackedByteArray buffer;
        buffer.resize(width * height * 4);
        uint8_t* ptr = buffer.ptrw();

#ifdef _OPENMP
#pragma omp parallel for
#endif
        for (int64_t y = 0; y < height; ++y) {
            for (int64_t x = 0; x < width; ++x) {
                double val = get_fractal_noise_2d(
                    (x + offset_x) * scale,
                    (y + offset_y) * scale
                );
                int64_t color = std::clamp(
                    static_cast<int64_t>((val + 1.0) * 127.5),
                    int64_t(0),
                    int64_t(255)
                );
                int64_t idx = (y * width + x) * 4;
                ptr[idx + 0] = ptr[idx + 1] = ptr[idx + 2] = static_cast<uint8_t>(color);
                ptr[idx + 3] = 255;
            }
        }
        return buffer;
    }

    PackedByteArray NoiseBase::get_fbm_volume_data(
        int64_t width, int64_t height, int64_t depth, double scale,
        double offset_x, double offset_y, double offset_z
    ) const {
        ERR_FAIL_COND_V_MSG(
            width <= 0 || height <= 0 || depth <= 0,
            PackedByteArray(),
            "Dimensoes devem ser > 0."
        );
        constexpr int64_t MAX_BUFFER_BYTES = 512LL * 1024LL * 1024LL;
        ERR_FAIL_COND_V_MSG(
            width > (MAX_BUFFER_BYTES / height) / depth,
            PackedByteArray(),
            "Volume 3D excede 512MB de RAM."
        );

        PackedByteArray buffer;
        buffer.resize(width * height * depth);
        uint8_t* ptr = buffer.ptrw();

#ifdef _OPENMP
#pragma omp parallel for collapse(2)
#endif
        for (int64_t z = 0; z < depth; ++z) {
            for (int64_t y = 0; y < height; ++y) {
                for (int64_t x = 0; x < width; ++x) {
                    double val = get_fractal_noise_3d(
                        (x + offset_x) * scale,
                        (y + offset_y) * scale,
                        (z + offset_z) * scale
                    );
                    int64_t density = std::clamp(
                        static_cast<int64_t>((val + 1.0) * 127.5),
                        int64_t(0),
                        int64_t(255)
                    );
                    int64_t idx = z * (width * height) + y * width + x;
                    ptr[idx] = static_cast<uint8_t>(density);
                }
            }
        }
        return buffer;
    }

}
