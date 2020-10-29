#include "Solver.h"
#include <iostream>
#include <sstream>
#include <cmath>
#include <algorithm>

using namespace std;

Solver::Solver() = default;

Solver::~Solver() {
    std::cerr << "END (message): " << reader.message << std::endl;
}

vector<string> Solver::process(const vector<string> &infos) {
    vector<string> commands;
    if (infos.size() == 1 && infos[0] == "unagi") { // amikor a consolon keresztül kommunikál a program
        reader.readDataConsole();
    } else {                                          // amikor hálózaton keresztül kommunikál a program
        reader.readDataInfo(infos);
    }

    if (reader.hasEnd) {
        return vector<string>{"Game over"};          // a main.cpp-ben megszakad a while loop és véget ér a program
    }

    if (!reader.needAnsw) {                          // ha nem kell válasz, üres vectort adunk vissza, amire a while loop tovább lép
        return vector<string>{};
    }

    //reader.transform();
    load_tick_info();

    // healing
    for (size_t y = 0; y < reader.dimension[0]; y++) {
        for (size_t x = 0; x < reader.dimension[1]; x++) {
            if (reader.areas[y][x].infectionRate > 0) {
                healing(y, x);
            }
        }
    }

    // infection
    vector<vector<unsigned int>> tmp (reader.dimension[0],vector<unsigned int> (reader.dimension[1],0));
    infection_history.push_back(tmp);

    for (size_t y = 0; y < reader.dimension[0]; y++) {
        for (size_t x = 0; x < reader.dimension[1]; x++) {

            if (reader.data[1] == 0) {
                if (reader.areas[y][x].infectionRate > 0) {
                    infection_history[0][y][x] = 1;
                }
            } else {
                infection(y, x);
            }
        }
    }

    //updating factors
    update_factors();

    // Válasz
    stringstream ss;
    string command;
    ss << "RES " << reader.data[0] << " " << reader.data[1] << " " << reader.data[2] << std::endl;
    getline(ss, command);
    commands.push_back(command);
    ss.clear();

    for (auto &row : reader.areas) {
        for (auto &area : row) {
            ss << area.infectionRate << " ";
        }
        getline(ss, command);
        commands.push_back(command);
        ss.clear();
    }

    return commands;
}

//Solver fv. defek
void Solver::load_tick_info() {
    tick_info.push_back(reader.areas);
}

unsigned int Solver::infection(unsigned int y, unsigned int x) {
    unsigned int avg_plus_sum_infection_rate;
    unsigned int avg_infection_rate;
    unsigned int sum_infection_rate = 0;
    unsigned int curr_tick = reader.data[1];

    unsigned int osszeg = 0;
    unsigned int sum = min(reader.factors[1] % 10 + 10, curr_tick);

    for (unsigned i = 1; i <= sum; i++) {
        osszeg += infection_history[curr_tick - i][y][x];
    }
    avg_infection_rate = osszeg / sum + reader.factors[1] % 10; // tick = 0ra ez nem jó

    vector<std::pair<int, int>> neighbours{{0,  0},
                                           {-1, 0},
                                           {0,  -1},
                                           {1,  0},
                                           {0,  1}};

    for (auto n:neighbours) {
        pair<int, int> c{y - n.first, x - n.second};

        if (!(0 <= c.first and c.first < reader.dimension[0] and
              0 <= c.second and c.second < reader.dimension[1]))
            continue; // lehet fel van cserélve
        unsigned int t = reader.factors[2] % 7 + 3;
        unsigned int a;
        if (tick_info[0][y][x].district != tick_info[0][c.first][c.second].district) { a = 2; }
        else if (y != c.first and x != c.second) { a = 1; }
        else { a = 0; }
        if (tick_info[curr_tick - 1][c.first][c.second].infectionRate > t * a) {
            sum_infection_rate +=
                    clamp(int(tick_info[0][y][x].population - tick_info[0][c.first][c.second].population), 0, 2) + 1;
        } else {
            sum_infection_rate += 0;
        }

    }

    avg_plus_sum_infection_rate = avg_infection_rate + sum_infection_rate;
    unsigned int solution = ceil(avg_plus_sum_infection_rate * ((reader.factors[3] % 25) + 50) / 100.0);
    infection_history[curr_tick][y][x] = solution;
    return solution;
}

unsigned int Solver::healing(unsigned int y, unsigned int x) {
    unsigned int width = reader.dimension[0];
    unsigned int height = reader.dimension[1];
    unsigned int curr_tick = reader.data[1];
    unsigned int curr_infection_rate;

    if (width + height < curr_tick) {
        unsigned int min_infection_rate = 100;
        for (unsigned int i = 1; i <= (width + height); i++) {
            curr_infection_rate = tick_info[curr_tick - i][y][x].infectionRate;
            if (curr_infection_rate < min_infection_rate) {
                min_infection_rate = curr_infection_rate;
            }
        }
        return size_t(floor(min_infection_rate) * (reader.factors[0] % 10) / 20.0);
    } else {
        return 0;
    }
}

void Solver::update_factors() {
    //factor = factor * 48271 % 0x7fffffff
    for(size_t i = 0; i < 4; i++){
        reader.factors[i] = (reader.factors[i] * 48271) % 0x7fffffff; //lehet kell zárójel % előtt?
    }
}
