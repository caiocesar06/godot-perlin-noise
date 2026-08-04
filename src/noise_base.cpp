#include "noise_base.hpp"
#include <godot_cpp/variant/utility_functions.hpp>
#include <algorithm>

namespace godot {
    NoiseBase::~NoiseBase() = default;

    void NoiseBase::_bind_methods() {
        ClassDB::bind_method(D_METHOD("set_octaves", "octaves"), &NoiseBase::set_octaves);
        ClassDB::bind_method(D_METHOD("get_octaves"), &NoiseBase::get_octaves);
        ClassDB::bind_method(D_METHOD("set_persistence", "persistence"), &NoiseBase::set_persistence);
        ClassDB::bind_method(D_METHOD("get_persistence"), &NoiseBase::get_persistence);
        ClassDB::bind_method(D_METHOD("set_lacunarity", "lacunarity"), &NoiseBase::set_lacunarity);
        ClassDB::bind_method(D_METHOD("get_lacunarity"), &NoiseBase::get_lacunarity);

        ADD_PROPERTY(PropertyInfo(Variant::INT, "octaves", PROPERTY_HINT_RANGE, "1,16,1"), "set_octaves", "get_octaves");
        ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "persistence", PROPERTY_HINT_RANGE, "0.0,1.0,0.05"), "set_persistence", "get_persistence");
        ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "lacunarity", PROPERTY_HINT_RANGE, "1.0,4.0,0.1"), "set_lacunarity", "get_lacunarity");
    }

    void NoiseBase::set_octaves(int32_t p_octaves) {
        if (p_octaves < 1 || p_octaves > 16)
            WARN_PRINT("NoiseBase: 'octaves' deve estar entre 1 e 16.");
        _octaves = std::clamp(p_octaves, 1, 16);
    }
    int32_t NoiseBase::get_octaves() const { return _octaves; }

    void NoiseBase::set_persistence(double p_persistence) {
        if (p_persistence < 0.0 || p_persistence > 1.0)
            WARN_PRINT("NoiseBase: 'persistence' deve estar entre 0.0 e 1.0.");
        _persistence = std::clamp(p_persistence, 0.0, 1.0);
    }
    double NoiseBase::get_persistence() const { return _persistence; }

    void NoiseBase::set_lacunarity(double p_lacunarity) {
        if (p_lacunarity < 1.0 || p_lacunarity > 4.0)
            WARN_PRINT("NoiseBase: 'lacunarity' deve estar entre 1.0 e 4.0.");
        _lacunarity = std::clamp(p_lacunarity, 1.0, 4.0);
    }
    double NoiseBase::get_lacunarity() const { return _lacunarity; }
}
