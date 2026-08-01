#include "perlin_noise_2d.hpp"
#include <algorithm>
#include <cmath>

inline int fast_floor(float x) {
    int xi = static_cast<int>(x);
    return x < xi ? xi - 1 : xi;
}

namespace godot {

    void PerlinNoise2D::_bind_methods() {
        ClassDB::bind_method(D_METHOD("set_seed", "seed"), &PerlinNoise2D::set_seed);

        ClassDB::bind_method(D_METHOD("set_octaves", "octaves"), &PerlinNoise2D::set_octaves);
        ClassDB::bind_method(D_METHOD("get_octaves"), &PerlinNoise2D::get_octaves);
        ClassDB::bind_method(D_METHOD("set_persistence", "persistence"), &PerlinNoise2D::set_persistence);
        ClassDB::bind_method(D_METHOD("get_persistence"), &PerlinNoise2D::get_persistence);
        ClassDB::bind_method(D_METHOD("set_lacunarity", "lacunarity"), &PerlinNoise2D::set_lacunarity);
        ClassDB::bind_method(D_METHOD("get_lacunarity"), &PerlinNoise2D::get_lacunarity);
        ClassDB::bind_method(D_METHOD("set_fade_mode", "mode"), &PerlinNoise2D::set_fade_mode);
        ClassDB::bind_method(D_METHOD("get_fade_mode"), &PerlinNoise2D::get_fade_mode);

        ClassDB::bind_method(D_METHOD("sample", "x", "y"), &PerlinNoise2D::sample);
        ClassDB::bind_method(D_METHOD("fbm", "x", "y"), &PerlinNoise2D::fbm);

        ADD_PROPERTY(PropertyInfo(Variant::INT, "octaves", PROPERTY_HINT_RANGE, "1,16,1"), "set_octaves", "get_octaves");
        ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "persistence", PROPERTY_HINT_RANGE, "0.0,1.0,0.05"), "set_persistence", "get_persistence");
        ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "lacunarity", PROPERTY_HINT_RANGE, "1.0,4.0,0.1"), "set_lacunarity", "get_lacunarity");
        ADD_PROPERTY(PropertyInfo(Variant::INT, "fade_mode", PROPERTY_HINT_ENUM, "None,Cubic,Quintic"), "set_fade_mode", "get_fade_mode");
    }

    PerlinNoise2D::PerlinNoise2D() {}

    void PerlinNoise2D::set_seed(int64_t p_seed) { _hash.set_seed(static_cast<unsigned int>(p_seed)); }
    void PerlinNoise2D::set_octaves(int32_t p_octaves) { octaves = std::clamp(p_octaves, 1, 16); }
    int32_t PerlinNoise2D::get_octaves() const { return octaves; }
    void PerlinNoise2D::set_persistence(double p_persistence) { persistence = std::clamp(p_persistence, 0.0, 1.0); }
    double PerlinNoise2D::get_persistence() const { return persistence; }
    void PerlinNoise2D::set_lacunarity(double p_lacunarity) { lacunarity = std::clamp(p_lacunarity, 1.0, 4.0); }
    double PerlinNoise2D::get_lacunarity() const { return lacunarity; }

    void PerlinNoise2D::set_fade_mode(int32_t p_mode) { _hash.fade_mode = static_cast<FadeMode>(std::clamp(p_mode, 0, 2)); }
    int32_t PerlinNoise2D::get_fade_mode() const { return static_cast<int32_t>(_hash.fade_mode); }

    float PerlinNoise2D::grad(int hash, float x, float y) const {
        static constexpr float G2D[8][2] = {
            { 1.0f, 0.0f }, {-1.0f, 0.0f }, { 0.0f, 1.0f }, { 0.0f, -1.0f },
            { 0.707106f, 0.707106f }, {-0.707106f, 0.707106f },
            { 0.707106f, -0.707106f }, {-0.707106f, -0.707106f }
        };
        int h = hash & 7;
        return G2D[h][0] * x + G2D[h][1] * y;
    }

    double PerlinNoise2D::sample(double x, double y) const {
        float fx = static_cast<float>(x);
        float fy = static_cast<float>(y);

        int xi = fast_floor(fx);
        int yi = fast_floor(fy);
        float xf = fx - xi;
        float yf = fy - yi;

        float u = _hash.fade(xf);
        float v = _hash.fade(yf);

        int i = xi & 255;
        int j = yi & 255;

        int aa = _hash.hash(_hash.hash(i) + j);
        int ab = _hash.hash(_hash.hash(i) + j + 1);
        int ba = _hash.hash(_hash.hash(i + 1) + j);
        int bb = _hash.hash(_hash.hash(i + 1) + j + 1);

        float x1 = _hash.lerp(u, grad(aa, xf, yf), grad(ba, xf - 1.0f, yf));
        float x2 = _hash.lerp(u, grad(ab, xf, yf - 1.0f), grad(bb, xf - 1.0f, yf - 1.0f));

        return static_cast<double>(_hash.lerp(v, x1, x2));
    }

    double PerlinNoise2D::fbm(double x, double y) const {
        double total = 0.0;
        double amplitude = 1.0;
        double frequency = 1.0;
        double max_value = 0.0;

        for (int i = 0; i < octaves; ++i) {
            total += sample(x * frequency, y * frequency) * amplitude;
            max_value += amplitude;
            amplitude *= persistence;
            frequency *= lacunarity;
        }
        return total / max_value;
    }
}
