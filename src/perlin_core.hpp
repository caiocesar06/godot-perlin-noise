#pragma once

#include <algorithm>
#include <array>
#include <cstdint>
#include <numeric>
#include <random>
#include <vector>

namespace godot {

    enum class FadeMode { NONE, CUBIC, QUINTIC };

    class PerlinCore {
    private:
        int64_t _seed = 0;
        FadeMode _fade_mode = FadeMode::QUINTIC;
        std::array<int, 512> _permutation;

    public:
        PerlinCore() {
            set_seed(std::random_device{}());
        }

        explicit PerlinCore(int64_t p_seed) {
            set_seed(p_seed);
        }

        void set_seed(int64_t p_seed) {
            _seed = p_seed;

            std::array<int, 256> temp;
            std::iota(temp.begin(), temp.end(), 0);

            std::mt19937_64 engine(
                static_cast<std::uint64_t>(_seed)
            );

            for (std::size_t i = temp.size() - 1; i > 0; --i) {
                std::uniform_int_distribution<std::size_t>
                    distribution(0, i);

                const std::size_t j = distribution(engine);
                std::swap(temp[i], temp[j]);
            }

            for (std::size_t i = 0; i < temp.size(); ++i) {
                _permutation[i] = temp[i];
                _permutation[i + 256] = temp[i];
            }
        }
        int64_t get_seed() const { return _seed; }

        void set_fade_mode(FadeMode mode) {
            _fade_mode = mode;
        }
        FadeMode get_fade_mode() const { return _fade_mode; }


        // --- Funções Matemáticas Utilitárias --- //

        static inline int fast_floor(float x) {
            int xi = static_cast<int>(x);
            return x < xi ? xi - 1 : xi;
        }

        inline float fade(float t) const {
            if (_fade_mode == FadeMode::NONE)
                return t;
            if (_fade_mode == FadeMode::CUBIC)
                return t * t * (3.0f - 2.0f * t);

            return t * t * t * (t * (t * 6.0f - 15.0f) + 10.0f);
        }

        static inline float lerp(float t, float a, float b) {
            return a + t * (b - a);
        }

        inline int hash(int x) const {
            return _permutation[x];
        }


        // --- Gradientes --- //

        static inline float grad_2d(int hash, float x, float y) {
            static constexpr float G2D[8][2] = {
                { 1.0f, 0.0f }, {-1.0f, 0.0f },
                { 0.0f, 1.0f }, { 0.0f, -1.0f },
                { 0.707106f, 0.707106f }, {-0.707106f, 0.707106f },
                { 0.707106f, -0.707106f }, {-0.707106f, -0.707106f }
            };
            int h = hash & 7;
            return G2D[h][0] * x + G2D[h][1] * y;
        }

        static inline float grad_3d(int hash, float x, float y, float z) {
            static constexpr float G3D[12][3] = {
                { 1.0f,  1.0f,  0.0f}, {-1.0f,  1.0f,  0.0f},
                { 1.0f, -1.0f,  0.0f}, {-1.0f, -1.0f,  0.0f},
                { 1.0f,  0.0f,  1.0f}, {-1.0f,  0.0f,  1.0f},
                { 1.0f,  0.0f, -1.0f}, {-1.0f,  0.0f, -1.0f},
                { 0.0f,  1.0f,  1.0f}, { 0.0f, -1.0f,  1.0f},
                { 0.0f,  1.0f, -1.0f}, { 0.0f, -1.0f, -1.0f}
            };
            int h = hash % 12;
            return G3D[h][0] * x + G3D[h][1] * y + G3D[h][2] * z;
        }
    };
}
