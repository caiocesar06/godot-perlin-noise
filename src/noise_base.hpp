#pragma once

#include <godot_cpp/classes/resource.hpp>

namespace godot {
    class NoiseBase : public Resource {
        GDCLASS(NoiseBase, Resource)

    private:
        int32_t _octaves = 6;
        double _persistence = 0.5;
        double _lacunarity = 2.0;

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
    };
}
