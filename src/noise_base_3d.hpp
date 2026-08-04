#pragma once

#include "noise_base.hpp"
#include <godot_cpp/variant/packed_byte_array.hpp>

namespace godot {
    class NoiseBase3D : public NoiseBase {
        GDCLASS(NoiseBase3D, NoiseBase)

    protected:
        static void _bind_methods();

    public:
        NoiseBase3D() = default;
        virtual ~NoiseBase3D() = default;

        virtual double sample(double x, double y, double z) const = 0;

        double fbm(double x, double y, double z) const;
        PackedByteArray get_fbm_buffer_3d(int64_t width, int64_t height, int64_t depth, double scale, double offset_x, double offset_y, double offset_z) const;
    };
}
