#pragma once

#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/variant/packed_byte_array.hpp>

namespace godot {

    enum class FractalType { FBM, RIDGED, BILLOW };

    class NoiseBase : public Resource {
        GDCLASS(NoiseBase, Resource)

    private:
        int32_t _octaves = 4;
        double _persistence = 0.5;
        double _lacunarity = 2.0;
        FractalType _fractal_type = FractalType::FBM;

    protected:
        static void _bind_methods();

    public:
        NoiseBase() = default;
        virtual ~NoiseBase() = 0;

        void set_octaves(int32_t p_octaves);
        int32_t get_octaves() const;

        void set_persistence(double p_persistence);
        double get_persistence() const;

        void set_lacunarity(double p_lacunarity);
        double get_lacunarity() const;

        void set_fractal_type(int32_t p_type);
        int32_t get_fractal_type() const;

        // -------------------------------------------------- //

        virtual double get_noise_2d(
            double x,
            double y
        ) const = 0;

        virtual double get_noise_3d(
            double x,
            double y,
            double z
        ) const = 0;


        double get_fractal_noise_2d(
            double x,
            double y
        ) const;

        double get_fractal_noise_3d(
            double x,
            double y,
            double z
        ) const;


        PackedByteArray get_fbm_image_data(
            int64_t width,
            int64_t height,
            double scale,
            double offset_x,
            double offset_y
        ) const;

        PackedByteArray get_fbm_volume_data(
            int64_t width,
            int64_t height,
            int64_t depth,
            double scale,
            double offset_x,
            double offset_y,
            double offset_z
        ) const;
    };
}
