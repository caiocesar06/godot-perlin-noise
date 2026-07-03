#include "perlin_noise_gd.hpp"
#include <algorithm>

namespace godot {

    void PerlinNoise::_bind_methods() {
        ClassDB::bind_method(D_METHOD("set_octaves", "octaves"), &PerlinNoise::set_octaves);
        ClassDB::bind_method(D_METHOD("get_octaves"), &PerlinNoise::get_octaves);
        ClassDB::bind_method(D_METHOD("set_persistence", "persistence"), &PerlinNoise::set_persistence);
        ClassDB::bind_method(D_METHOD("get_persistence"), &PerlinNoise::get_persistence);
        ClassDB::bind_method(D_METHOD("set_lacunarity", "lacunarity"), &PerlinNoise::set_lacunarity);
        ClassDB::bind_method(D_METHOD("get_lacunarity"), &PerlinNoise::get_lacunarity);
        ClassDB::bind_method(D_METHOD("set_seed", "seed"), &PerlinNoise::set_seed);

        ClassDB::bind_method(D_METHOD("sample", "x", "y"), &PerlinNoise::sample);
        ClassDB::bind_method(D_METHOD("get_fbm", "x", "y"), &PerlinNoise::get_fbm);

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

    double PerlinNoise::sample(double x, double y) const {
        return core.noise(static_cast<float>(x), static_cast<float>(y));
    }

    double PerlinNoise::get_fbm(double x, double y) const {
        return core.fBm(static_cast<float>(x), static_cast<float>(y), octaves, static_cast<float>(persistence), static_cast<float>(lacunarity));
    }

} // namespace godot
