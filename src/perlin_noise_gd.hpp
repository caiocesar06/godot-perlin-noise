#pragma once

#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/variant/packed_byte_array.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/ref.hpp>
#include "perlin_noise_core.hpp"

namespace godot {

    // TODO: Refatorar para herdar de uma classe abstrata genérica 'RuidoBase'
    // quando o algoritmo Simplex Noise for integrado ao projeto, permitindo polimorfismo
    // direto na GDExtension (ex: RuidoBase* gerador = novo SimplexNoise()).
    class PerlinNoise : public RefCounted {
        GDCLASS(PerlinNoise, RefCounted)

    private:
        PerlinNoiseCore core;
        int32_t octaves;
        double persistence;
        double lacunarity;

    protected:
        static void _bind_methods();

    public:
        PerlinNoise();
        ~PerlinNoise();

        static Ref<PerlinNoise> create_with_seed(int p_seed);

        void set_octaves(int32_t p_octaves);
        void set_persistence(double p_persistence);
        void set_lacunarity(double p_lacunarity);
        void set_seed(int64_t p_seed);
        void set_fade_mode(int32_t p_mode);

        int32_t get_octaves() const;
        double get_persistence() const;
        double get_lacunarity() const;
        int32_t get_fade_mode() const;

        double sample_2D(double x, double y) const;
        double get_fbm_2D(double x, double y) const;

        double sample_3D(double x, double y, double z) const;
        double get_fbm_3D(double x, double y, double z) const;

        PackedByteArray get_fbm_buffer(int64_t width, int64_t height, double scale, double offset_x, double offset_y) const;
    };

} // namespace godot
