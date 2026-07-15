#include "perlin_noise_gd.hpp"
#include <algorithm>

namespace godot {

    void PerlinNoise::_bind_methods() {
        ClassDB::bind_static_method("PerlinNoise", D_METHOD("create_with_seed", "seed"), &PerlinNoise::create_with_seed);

        ClassDB::bind_method(D_METHOD("set_octaves", "octaves"), &PerlinNoise::set_octaves);
        ClassDB::bind_method(D_METHOD("get_octaves"), &PerlinNoise::get_octaves);
        ClassDB::bind_method(D_METHOD("set_persistence", "persistence"), &PerlinNoise::set_persistence);
        ClassDB::bind_method(D_METHOD("get_persistence"), &PerlinNoise::get_persistence);
        ClassDB::bind_method(D_METHOD("set_lacunarity", "lacunarity"), &PerlinNoise::set_lacunarity);
        ClassDB::bind_method(D_METHOD("get_lacunarity"), &PerlinNoise::get_lacunarity);
        ClassDB::bind_method(D_METHOD("set_seed", "seed"), &PerlinNoise::set_seed);

        ClassDB::bind_method(D_METHOD("sample_2D", "x", "y"), &PerlinNoise::sample_2D);
        ClassDB::bind_method(D_METHOD("get_fbm_2D", "x", "y"), &PerlinNoise::get_fbm_2D);

        ClassDB::bind_method(D_METHOD("sample_3D", "x", "y", "z"), &PerlinNoise::sample_3D);
        ClassDB::bind_method(D_METHOD("get_fbm_3D", "x", "y", "z"), &PerlinNoise::get_fbm_3D);

        ClassDB::bind_method(D_METHOD("get_fbm_buffer", "width", "height", "scale", "offset_x", "offset_y"), &PerlinNoise::get_fbm_buffer);

        ADD_PROPERTY(PropertyInfo(Variant::INT, "octaves", PROPERTY_HINT_RANGE, "1,16,1"), "set_octaves", "get_octaves");
        ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "persistence", PROPERTY_HINT_RANGE, "0.0,1.0,0.05"), "set_persistence", "get_persistence");
        ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "lacunarity", PROPERTY_HINT_RANGE, "1.0,4.0,0.1"), "set_lacunarity", "get_lacunarity");
    }

    PerlinNoise::PerlinNoise() : core(std::random_device{}()) {
        octaves = 6;
        persistence = 0.5;
        lacunarity = 2.0;
    }

    PerlinNoise::~PerlinNoise() {}

    Ref<PerlinNoise> PerlinNoise::create_with_seed(int p_seed) {
        Ref<PerlinNoise> instance = memnew(PerlinNoise);
        instance->set_seed(p_seed);
        return instance;
    }

    void PerlinNoise::set_octaves(int32_t p_octaves) {
        octaves = std::clamp(p_octaves, 1, 16);
    }

    void PerlinNoise::set_persistence(double p_persistence) {
        persistence = std::clamp(p_persistence, 0.0, 1.0);
    }

    void PerlinNoise::set_lacunarity(double p_lacunarity) {
        lacunarity = std::clamp(p_lacunarity, 1.0, 4.0);
    }

    void PerlinNoise::set_seed(int64_t p_seed) {
        core.set_seed(static_cast<unsigned int>(p_seed));
    }

    int32_t PerlinNoise::get_octaves() const { return octaves; }
    double PerlinNoise::get_persistence() const { return persistence; }
    double PerlinNoise::get_lacunarity() const { return lacunarity; }

    double PerlinNoise::sample_2D(double x, double y) const {
        return core.noise(static_cast<float>(x), static_cast<float>(y));
    }

    double PerlinNoise::get_fbm_2D(double x, double y) const {
        return core.fBm(static_cast<float>(x), static_cast<float>(y), octaves, static_cast<float>(persistence), static_cast<float>(lacunarity));
    }

    double PerlinNoise::sample_3D(double x, double y, double z) const {
        return core.noise(static_cast<float>(x), static_cast<float>(y), static_cast<float>(z));
    }

    double PerlinNoise::get_fbm_3D(double x, double y, double z) const {
        return core.fBm(static_cast<float>(x), static_cast<float>(y), static_cast<float>(z), octaves, static_cast<float>(persistence), static_cast<float>(lacunarity));
    }

    PackedByteArray PerlinNoise::get_fbm_buffer(int64_t width, int64_t height,
        double scale, double offset_x, double offset_y) const
    {
        ERR_FAIL_COND_V_MSG(width <= 0 || height <= 0, PackedByteArray(), "Width e Height devem ser maiores que zero.");

        PackedByteArray buffer;
        buffer.resize(width * height * 4);
        uint8_t* ptr = buffer.ptrw();

#pragma omp parallel for
        for (int64_t y = 0; y < height; ++y) {
            for (int64_t x = 0; x < width; ++x) {
                double nx = (x + offset_x) * scale;
                double ny = (y + offset_y) * scale;

                float val = core.fBm(static_cast<float>(nx), static_cast<float>(ny),
                    octaves, static_cast<float>(persistence), static_cast<float>(lacunarity));

                int64_t cor = std::clamp(static_cast<int64_t>((val + 1.0) * 127.5),
                    int64_t(0), int64_t(255));

                int64_t idx = (y * width + x) * 4;
                ptr[idx + 0] = static_cast<uint8_t>(cor); // R
                ptr[idx + 1] = static_cast<uint8_t>(cor); // G
                ptr[idx + 2] = static_cast<uint8_t>(cor); // B
                ptr[idx + 3] = 255;                       // A (Opacidade máxima)
            }
        }
        return buffer;
    }

} // namespace godot
