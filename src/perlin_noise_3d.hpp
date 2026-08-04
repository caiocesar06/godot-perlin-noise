#pragma once

#include "perlin_hash.hpp"
#include "noise_base_3d.hpp"

namespace godot {
    class PerlinNoise3D : public NoiseBase3D {
        GDCLASS(PerlinNoise3D, NoiseBase3D)

    private:
        PerlinHash _hash;

        float grad(int hash, float x, float y, float z) const;

    protected:
        static void _bind_methods();

    public:
        PerlinNoise3D();
        ~PerlinNoise3D() override = default;

        void set_seed(int64_t p_seed);

        void set_fade_mode(int32_t p_mode);
        int32_t get_fade_mode() const;

        double sample(double x, double y, double z) const override;
    };
}
