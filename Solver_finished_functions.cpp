#include "Solver.h"

using namespace std;

void Solver::update_infected_districts() {
    infected_districts.clear();
    for (size_t x = 0; x < reader.dimension[1]; x++) {
        for (size_t y = 0; y < reader.dimension[0]; y++) {
            if (reader.areas[y][x].infectionRate > 0) {
                infected_districts.insert(reader.areas[y][x].district);
            }
        }
    }
}

void Solver::update_factor(uint32_t& factor) {
    factor = uint64_t(factor) * uint64_t(48271) % uint64_t(0x7fffffff);
}

Solver::Solver() = default;

Solver::~Solver() {
    std::cerr << "END (message): " << reader.message << std::endl;
}