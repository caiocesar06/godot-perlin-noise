#pragma once

#include <array>
#include <numeric>
#include <random>
#include <algorithm>

enum class FadeMode { NONE, CUBIC, QUINTIC };

class PerlinHash {
    public:
        FadeMode fade_mode = FadeMode::QUINTIC;

        PerlinHash() {
            set_seed(std::random_device{}());
        }

        explicit PerlinHash(unsigned int seed) {
            set_seed(seed);
        }

        void set_seed(unsigned int seed) {
            std::vector<int> temp(256);
            std::iota(temp.begin(), temp.end(), 0);
            std::default_random_engine engine(seed);
            std::shuffle(temp.begin(), temp.end(), engine);

            for (int i = 0; i < 256; ++i) {
                _permutation[i] = temp[i];
                _permutation[i + 256] = temp[i];
            }
        }

        inline float fade(float t) const {
            if (fade_mode == FadeMode::NONE)
                return t;
            if (fade_mode == FadeMode::CUBIC)
                return t * t * (3.0f - 2.0f * t);

            return t * t * t * (t * (t * 6.0f - 15.0f) + 10.0f);
        }

        inline float lerp(float t, float a, float b) const {
            return a + t * (b - a);
        }

        inline int hash(int x) const {
            return _permutation[x];
        }

    private:
        std::array<int, 512> _permutation;
};
