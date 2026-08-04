#include "perlin_noise.hpp"
#include <godot_cpp/variant/utility_functions.hpp>

namespace godot {
    void PerlinNoise::_bind_methods() {
        ClassDB::bind_method(
            D_METHOD("set_seed", "seed"),
            &PerlinNoise::set_seed
        );
        ClassDB::bind_method(
            D_METHOD("get_seed"),
            &PerlinNoise::get_seed
        );
        ClassDB::bind_method(
            D_METHOD("set_fade_mode", "mode"),
            &PerlinNoise::set_fade_mode
        );
        ClassDB::bind_method(
            D_METHOD("get_fade_mode"),
            &PerlinNoise::get_fade_mode
        );

        ClassDB::bind_method(
            D_METHOD("get_noise_2d", "x", "y"),
            &PerlinNoise::get_noise_2d
        );
        ClassDB::bind_method(
            D_METHOD("get_noise_3d", "x", "y", "z"),
            &PerlinNoise::get_noise_3d
        );

        ADD_PROPERTY(
            PropertyInfo(Variant::INT, "seed"),
            "set_seed",
            "get_seed"
        );
        ADD_PROPERTY(
            PropertyInfo(Variant::INT, "fade_mode", PROPERTY_HINT_ENUM, "None,Cubic,Quintic"),
            "set_fade_mode",
            "get_fade_mode"
        );
    }

    void PerlinNoise::set_seed(int64_t p_seed) {
        _core.set_seed(p_seed);
    }
    int64_t PerlinNoise::get_seed() const { return _core.get_seed(); }

    void PerlinNoise::set_fade_mode(int32_t p_mode) {
        if (p_mode < 0 || p_mode > 2) {
            WARN_PRINT("PerlinNoise: 'fade_mode' inválido. Usando Quintic por padrão.");
            p_mode = 2;
        }
        _core.set_fade_mode(static_cast<FadeMode>(p_mode));
    }
    int32_t PerlinNoise::get_fade_mode() const { return static_cast<int32_t>(_core.get_fade_mode()); }

    double PerlinNoise::get_noise_2d(double x, double y) const {
        float fx = static_cast<float>(x);
        float fy = static_cast<float>(y);

        int xi = PerlinCore::fast_floor(fx);
        int yi = PerlinCore::fast_floor(fy);
        float xf = fx - xi;
        float yf = fy - yi;

        float u = _core.fade(xf);
        float v = _core.fade(yf);

        int i = xi & 255;
        int j = yi & 255;

        int aa = _core.hash(_core.hash(i) + j);
        int ab = _core.hash(_core.hash(i) + j + 1);
        int ba = _core.hash(_core.hash(i + 1) + j);
        int bb = _core.hash(_core.hash(i + 1) + j + 1);

        float x1 = _core.lerp(
            u,
            _core.grad_2d(aa, xf, yf),
            _core.grad_2d(ba, xf - 1.0f, yf)
        );
        float x2 = _core.lerp(
            u,
            _core.grad_2d(ab, xf, yf - 1.0f),
            _core.grad_2d(bb, xf - 1.0f, yf - 1.0f)
        );

        return static_cast<double>(_core.lerp(v, x1, x2));
    }

    double PerlinNoise::get_noise_3d(double x, double y, double z) const {
        float fx = static_cast<float>(x);
        float fy = static_cast<float>(y);
        float fz = static_cast<float>(z);

        int xi = PerlinCore::fast_floor(fx);
        int yi = PerlinCore::fast_floor(fy);
        int zi = PerlinCore::fast_floor(fz);

        float xf = fx - xi;
        float yf = fy - yi;
        float zf = fz - zi;

        float u = _core.fade(xf);
        float v = _core.fade(yf);
        float w = _core.fade(zf);

        int i = xi & 255;
        int j = yi & 255;
        int k = zi & 255;

        int a  = _core.hash(i) + j;
        int aa = _core.hash(a) + k;
        int ab = _core.hash(a + 1) + k;
        int b  = _core.hash(i + 1) + j;
        int ba = _core.hash(b) + k;
        int bb = _core.hash(b + 1) + k;

        float x1 = _core.lerp(
            u,
            _core.grad_3d(_core.hash(aa), xf, yf, zf),
            _core.grad_3d(_core.hash(ba), xf - 1.0f, yf, zf)
        );
        float x2 = _core.lerp(
            u,
            _core.grad_3d(_core.hash(ab), xf, yf - 1.0f, zf),
            _core.grad_3d(_core.hash(bb), xf - 1.0f, yf - 1.0f, zf)
        );
        float y1 = _core.lerp(v, x1, x2);

        float x3 = _core.lerp(
            u,
            _core.grad_3d(_core.hash(aa + 1), xf, yf, zf - 1.0f),
            _core.grad_3d(_core.hash(ba + 1), xf - 1.0f, yf, zf - 1.0f)
        );
        float x4 = _core.lerp(
            u,
            _core.grad_3d(_core.hash(ab + 1), xf, yf - 1.0f, zf - 1.0f),
            _core.grad_3d(_core.hash(bb + 1), xf - 1.0f, yf - 1.0f, zf - 1.0f)
        );
        float y2 = _core.lerp(v, x3, x4);

        return static_cast<double>(_core.lerp(w, y1, y2));
    }
}
