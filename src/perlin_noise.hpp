#pragma once

#include "noise_base.hpp"
#include "perlin_core.hpp"

namespace godot {
    class PerlinNoise final : public NoiseBase {
        GDCLASS(PerlinNoise, NoiseBase)

    private:
        PerlinCore _core;

    protected:
        static void _bind_methods();

    public:
        PerlinNoise() = default;
        ~PerlinNoise() = default;

        void set_seed(int64_t p_seed) override;
        int64_t get_seed() const override;

        void set_fade_mode(int32_t p_mode);
        int32_t get_fade_mode() const;

        double get_noise_2d(double x, double y) const override;
        double get_noise_3d(double x, double y, double z) const override;
    };
}
