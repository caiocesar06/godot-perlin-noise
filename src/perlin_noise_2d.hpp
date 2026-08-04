#pragma once

#include "noise_base_2d.hpp"
#include "perlin_hash.hpp"

namespace godot {
    class PerlinNoise2D : public NoiseBase2D {
        GDCLASS(PerlinNoise2D, NoiseBase2D)

    private:
        PerlinHash _hash;

        float grad(int hash, float x, float y) const;

    protected:
        static void _bind_methods();

    public:
        PerlinNoise2D();
        ~PerlinNoise2D() override = default;

        void set_seed(int64_t p_seed);

        void set_fade_mode(int32_t p_mode);
        int32_t get_fade_mode() const;

        double sample(double x, double y) const override;
    };
}
