#pragma once

#include <array>
#include <cstdint>
#include <numeric>
#include <random>

namespace godot {

    class PermutationTable {
        private:
            int64_t _seed = 0;
            std::array<int, 512> _table{};

        public:
            PermutationTable() {
                reseed(std::random_device{}());
            }

            explicit PermutationTable(int64_t p_seed) {
                reseed(p_seed);
            }

            void reseed(int64_t p_seed) {
                _seed = p_seed;

                std::array<int, 256> temp;
                std::iota(temp.begin(), temp.end(), 0);

                std::mt19937_64 engine(
                    static_cast<std::uint64_t>(_seed)
                );

                for (std::size_t i = temp.size() - 1; i > 0; --i) {
                    std::uniform_int_distribution<std::size_t>
                        distribution(0, i);

                    const std::size_t j = distribution(engine);
                    std::swap(temp[i], temp[j]);
                }

                for (std::size_t i = 0; i < temp.size(); ++i) {
                    _table[i] = temp[i];
                    _table[i + 256] = temp[i];
                }
            }

            int64_t get_seed() const { return _seed; }

            inline int hash(int x) const {
                return _table[static_cast<std::size_t>(x) & 511u];
            }
    };
}
