#pragma once

#include <vector>
#include <numeric>
#include <random>
#include <algorithm>

inline int fast_floor(float x) {
    int xi = (int)x;
    return x < xi ? xi - 1 : xi;
}

enum class FadeMode { NONE, CUBIC, QUINTIC };

struct PerlinNoiseCore {

    static constexpr float G2D[8][2] = {
        { 1.0f,       0.0f      }, {-1.0f,       0.0f      },
        { 0.0f,       1.0f      }, { 0.0f,      -1.0f      },
        { 0.707106f,  0.707106f }, {-0.707106f,  0.707106f },
        { 0.707106f, -0.707106f }, {-0.707106f, -0.707106f }
    };

    std::vector<int> p;
    FadeMode fade_mode;

    void set_seed(unsigned int seed) {
        p.clear();
        p.resize(256);
        std::iota(p.begin(), p.end(), 0);
        std::default_random_engine engine(seed);
        std::shuffle(p.begin(), p.end(), engine);
        p.insert(p.end(), p.begin(), p.end()); // Duplica para 512
    }

    PerlinNoiseCore(unsigned int seed, FadeMode mode = FadeMode::QUINTIC) : fade_mode(mode) {
        set_seed(seed);
    }

    float fade_none(float t) const {
        return t;
    }

    float fade_cubic(float t) const {
        return t * t * (3.0f - 2.0f * t);
    }

    float fade_quintic(float t) const {
        return t * t * t * (t * (6.0f * t - 15.0f) + 10.0f);
    }

    float fade(float t) const {
        switch (fade_mode) {
            case FadeMode::NONE:    return fade_none(t);
            case FadeMode::CUBIC:   return fade_cubic(t);
            case FadeMode::QUINTIC: return fade_quintic(t);
        }
        return t;
    }

    float lerp(float t, float a, float b) const {
        return a + t * (b - a);
    }


    float grad(int hash, float x, float y) const {
        int h = hash & 7;
        return G2D[h][0] * x + G2D[h][1] * y;
    }

    float grad(int hash, float x, float y, float z) const {
        int h = hash & 15;
        float u = h < 8 ? x : y;
        float v = h < 4 ? y : (h == 12 || h == 14 ? v : -v);
        return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
    }

    float noise(float x, float y) const {
        int xi = fast_floor(x);
        int yi = fast_floor(y);
        int i = xi & 255;
        int j = yi & 255;

        float xf = x - xi;
        float yf = y - yi;

        float u = fade(xf);
        float v = fade(yf);

        int h00 = p[p[i] + j];
        int h01 = p[p[i] + j + 1];
        int h10 = p[p[i + 1] + j];
        int h11 = p[p[i + 1] + j + 1];

        float x1 = lerp(u, grad(h00, xf, yf),
            grad(h10, xf - 1.0f, yf));
        float x2 = lerp(u, grad(h01, xf, yf - 1.0f),
            grad(h11, xf - 1.0f, yf - 1.0f));

        return lerp(v, x1, x2);
    }

    float noise(float x, float y, float z) const {
        int xi = fast_floor(x);
        int yi = fast_floor(y);
        int zi = fast_floor(z);

        int i = xi & 255;
        int j = yi & 255;
        int k = zi & 255;

        float xf = x - xi;
        float yf = y - yi;
        float zf = z - zi;

        float u = fade(xf);
        float v = fade(yf);
        float w = fade(zf);

        int a = p[i] + j;
        int aa = p[a] + k;
        int ab = p[a + 1] + k;
        int b = p[i + 1] + j;
        int ba = p[b] + k;
        int bb = p[b + 1] + k;

        return lerp(w,
            lerp(v,
                lerp(u, grad(p[aa], xf, yf, zf),
                    grad(p[ba], xf - 1.0f, yf, zf)),
                lerp(u, grad(p[ab], xf, yf - 1.0f, zf),
                    grad(p[bb], xf - 1.0f, yf - 1.0f, zf))
            ),
            lerp(v,
                lerp(u, grad(p[aa + 1], xf, yf, zf - 1.0f),
                    grad(p[ba + 1], xf - 1.0f, yf, zf - 1.0f)),
                lerp(u, grad(p[ab + 1], xf, yf - 1.0f, zf - 1.0f),
                    grad(p[bb + 1], xf - 1.0f, yf - 1.0f, zf - 1.0f))
            )
        );
    }

    float fBm(float x, float y, int octaves, float persistence, float lacunarity) const {
        float total = 0.0f;
        float amplitude = 1.0f;
        float frequency = 1.0f;
        float max_value = 0.0f;

        for (int i = 0; i < octaves; ++i) {
            total += noise(x * frequency, y * frequency) * amplitude;
            max_value += amplitude;
            amplitude *= persistence;
            frequency *= lacunarity;
        }

        return total / max_value;
    }

    float fBm(float x, float y, float z, int octaves, float persistence, float lacunarity) const {
        float total = 0.0f;
        float amplitude = 1.0f;
        float frequency = 1.0f;
        float max_value = 0.0f;

        for (int i = 0; i < octaves; ++i) {
            total += noise(x * frequency, y * frequency, z * frequency) * amplitude;
            max_value += amplitude;
            amplitude *= persistence;
            frequency *= lacunarity;
        }

        return total / max_value;
    }

    // TODO: Exposto exclusivamente para a ferramenta de debug visual.
    // Permite a renderização da tabela de permutação e dos vetores de gradiente na interface.
    int get_hash(int i, int j) const {
        return p[p[i & 255] + (j & 255)];
    }
};
