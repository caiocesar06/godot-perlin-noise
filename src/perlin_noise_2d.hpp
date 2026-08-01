#pragma once

#include <godot_cpp/classes/resource.hpp>
#include "perlin_hash.hpp"

namespace godot {
    class PerlinNoise2D : public Resource {
        GDCLASS(PerlinNoise2D, Resource)

    private:
        PerlinHash _hash;
        int32_t octaves = 4;
        double persistence = 0.5;
        double lacunarity = 2.0;

        float grad(int hash, float x, float y) const;

    protected:
        static void _bind_methods();

    public:
        PerlinNoise2D();
        ~PerlinNoise2D() = default;

        void set_seed(int64_t p_seed);
        void set_octaves(int32_t p_octaves);
        int32_t get_octaves() const;
        void set_persistence(double p_persistence);
        double get_persistence() const;
        void set_lacunarity(double p_lacunarity);
        double get_lacunarity() const;

        void set_fade_mode(int32_t p_mode);
        int32_t get_fade_mode() const;

        double sample(double x, double y) const;
        double fbm(double x, double y) const;
    };
}
