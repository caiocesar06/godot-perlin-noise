#include "perlin_noise_3d.hpp"
#include <algorithm>
#include <cmath>

inline int fast_floor_3d(float x) {
    int xi = static_cast<int>(x);
    return x < xi ? xi - 1 : xi;
}

namespace godot {

    void PerlinNoise3D::_bind_methods() {
        ClassDB::bind_method(D_METHOD("set_seed", "seed"), &PerlinNoise3D::set_seed);
        ClassDB::bind_method(D_METHOD("set_fade_mode", "mode"), &PerlinNoise3D::set_fade_mode);
        ClassDB::bind_method(D_METHOD("get_fade_mode"), &PerlinNoise3D::get_fade_mode);
        ClassDB::bind_method(D_METHOD("sample", "x", "y", "z"), &PerlinNoise3D::sample);

        ADD_PROPERTY(PropertyInfo(Variant::INT, "fade_mode", PROPERTY_HINT_ENUM, "None,Cubic,Quintic"), "set_fade_mode", "get_fade_mode");
    }

    PerlinNoise3D::PerlinNoise3D() {}

    void PerlinNoise3D::set_seed(int64_t p_seed) {
        _hash.set_seed(static_cast<unsigned int>(p_seed));
    }

    void PerlinNoise3D::set_fade_mode(int32_t p_mode) {
        _hash.fade_mode = static_cast<FadeMode>(std::clamp(p_mode, 0, 2));
    }
    int32_t PerlinNoise3D::get_fade_mode() const { return static_cast<int32_t>(_hash.fade_mode); }

    float PerlinNoise3D::grad(int hash, float x, float y, float z) const {
        static constexpr float G3D[12][3] = {
            { 1.0f,  1.0f,  0.0f}, {-1.0f,  1.0f,  0.0f}, { 1.0f, -1.0f,  0.0f}, {-1.0f, -1.0f,  0.0f},
            { 1.0f,  0.0f,  1.0f}, {-1.0f,  0.0f,  1.0f}, { 1.0f,  0.0f, -1.0f}, {-1.0f,  0.0f, -1.0f},
            { 0.0f,  1.0f,  1.0f}, { 0.0f, -1.0f,  1.0f}, { 0.0f,  1.0f, -1.0f}, { 0.0f, -1.0f, -1.0f}
        };

        int h = hash % 12;

        return G3D[h][0] * x + G3D[h][1] * y + G3D[h][2] * z;
    }

    double PerlinNoise3D::sample(double x, double y, double z) const {
        float fx = static_cast<float>(x);
        float fy = static_cast<float>(y);
        float fz = static_cast<float>(z);

        int xi = fast_floor_3d(fx);
        int yi = fast_floor_3d(fy);
        int zi = fast_floor_3d(fz);

        float xf = fx - xi;
        float yf = fy - yi;
        float zf = fz - zi;

        float u = _hash.fade(xf);
        float v = _hash.fade(yf);
        float w = _hash.fade(zf);

        int i = xi & 255;
        int j = yi & 255;
        int k = zi & 255;

        int a = _hash.hash(i) + j;
        int aa = _hash.hash(a) + k;
        int ab = _hash.hash(a + 1) + k;
        int b = _hash.hash(i + 1) + j;
        int ba = _hash.hash(b) + k;
        int bb = _hash.hash(b + 1) + k;

        float x1 = _hash.lerp(u, grad(_hash.hash(aa), xf, yf, zf), grad(_hash.hash(ba), xf - 1.0f, yf, zf));
        float x2 = _hash.lerp(u, grad(_hash.hash(ab), xf, yf - 1.0f, zf), grad(_hash.hash(bb), xf - 1.0f, yf - 1.0f, zf));
        float y1 = _hash.lerp(v, x1, x2);

        float x3 = _hash.lerp(u, grad(_hash.hash(aa + 1), xf, yf, zf - 1.0f), grad(_hash.hash(ba + 1), xf - 1.0f, yf, zf - 1.0f));
        float x4 = _hash.lerp(u, grad(_hash.hash(ab + 1), xf, yf - 1.0f, zf - 1.0f), grad(_hash.hash(bb + 1), xf - 1.0f, yf - 1.0f, zf - 1.0f));
        float y2 = _hash.lerp(v, x3, x4);

        return static_cast<double>(_hash.lerp(w, y1, y2));
    }
}
