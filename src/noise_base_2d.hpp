#pragma once

#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/variant/packed_byte_array.hpp>

namespace godot {
    class NoiseBase2D : public Resource {
        GDCLASS(NoiseBase2D, Resource)

    private:
        int32_t _octaves = 6;
        double _persistence = 0.5;
        double _lacunarity = 2.0;

    protected:
        static void _bind_methods();

    public:
        NoiseBase2D() = default;
        virtual ~NoiseBase2D() = default;

        void set_octaves(int32_t p_octaves);
        int32_t get_octaves() const;
        void set_persistence(double p_persistence);
        double get_persistence() const;
        void set_lacunarity(double p_lacunarity);
        double get_lacunarity() const;

        virtual double sample(double x, double y) const = 0;
        double fbm(double x, double y) const;
        PackedByteArray get_fbm_buffer(int64_t width, int64_t height, double scale, double offset_x, double offset_y) const;
    };
}
