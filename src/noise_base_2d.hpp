#pragma once

#include "noise_base.hpp"
#include <godot_cpp/variant/packed_byte_array.hpp>

namespace godot {
    class NoiseBase2D : public NoiseBase {
        GDCLASS(NoiseBase2D, NoiseBase)

    protected:
        static void _bind_methods();

    public:
        NoiseBase2D() = default;
        virtual ~NoiseBase2D() = default;

        virtual double sample(double x, double y) const = 0;

        double fbm(double x, double y) const;
        PackedByteArray get_fbm_buffer(int64_t width, int64_t height, double scale, double offset_x, double offset_y) const;
    };
}
