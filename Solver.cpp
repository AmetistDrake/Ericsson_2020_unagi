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
    if (infos.size() == 1 && infos[0] == "unagi") {     // amikor a consolon keresztül kommunikál a program
        reader.readDataConsole();
    } else {                                            // amikor hálózaton keresztül kommunikál a program
        reader.readDataInfo(infos);
    }

    if (reader.hasEnd) {
        return vector<string>{"Game over"};             // a main.cpp-ben megszakad a while loop és véget ér a program
    }

    if (!reader.needAnsw) {                             // ha nem kell válasz, üres vectort adunk vissza, amire a while loop tovább lép
        return vector<string>{};
    }

    // healing - fertőzöttek gyógyulása
    heal.resize(reader.dimension[0], vector<unsigned int>(reader.dimension[1], 0));

    for (size_t x = 0; x < reader.dimension[1]; x++) {
        for (size_t y = 0; y < reader.dimension[0]; y++) {
            if (reader.areas[y][x].infectionRate > 0) {
                unsigned int h = healing(y, x);
                reader.areas[y][x].healthRate += h;
                heal[y][x] = h;
                reader.areas[y][x].infectionRate = reader.areas[y][x].infectionRate - h;
                if (reader.areas[y][x].infectionRate < 0) {
                    reader.areas[y][x].infectionRate = 0;
                }
            }
        }
    }

    // infection - vírus terjed
    vector<vector<unsigned int>> tmp(reader.dimension[0],
                                     vector<unsigned int>(reader.dimension[1], 0));
    infection_history.push_back(tmp);

    for (size_t x = 0; x < reader.dimension[1]; x++) {
        for (size_t y = 0; y < reader.dimension[0]; y++) {
            if (reader.data[1] == 0) {
                if (reader.areas[y][x].infectionRate > 0) {
                    infection_history[0][y][x] = 1;
                }
            } else if (not_clean_districts.find(reader.areas[y][x].district) !=
                       not_clean_districts.end()) { // ha a terület kerületében van még fertőzött
                unsigned int inf = infection(y, x);
                if ((inf + reader.areas[y][x].healthRate + reader.areas[y][x].infectionRate) > 100) {
                    inf = 100 - reader.areas[y][x].healthRate - reader.areas[y][x].infectionRate;
                }

                infection_history[reader.data[1]][y][x] = inf;
                reader.areas[y][x].infectionRate += inf;
            }
        }
    }

    load_tick_info();
    load_clean_districts();

    // Válasz
    answer_msg(commands);

    /// Ez ne íródjon ki a beadottban
    /**************************************/
    //debug_msg(commands);
    /**************************************/

    return commands;
}

//Solver fv. defek
void Solver::load_tick_info() {
    tick_info.push_back(reader.areas);
}

unsigned int Solver::infection(unsigned int y, unsigned int x) {
    double avg_plus_sum_infection_rate;
    double avg_infection_rate;
    double sum_infection_rate = 0;
    unsigned int curr_tick = reader.data[1];

    double osszeg = 0;
    double sum = min(reader.factors[1] % 10 + 10, curr_tick);
    update_factor(reader.factors[1]);

    for (std::size_t i = 1; i <= sum; i++) {
        osszeg += infection_history[curr_tick - i][y][x];
    }
    avg_infection_rate = osszeg / sum; // tick = 0ra ez nem jó

    vector<std::pair<int, int>> neighbours{{0,  0},
                                           {-1, 0},
                                           {0,  -1},
                                           {1,  0},
                                           {0,  1}};

    unsigned int t = reader.factors[2] % 7 + 3;
    update_factor(reader.factors[2]);

    for (auto n:neighbours) {
        pair<int, int> c{y - n.first, x - n.second};

        if (!(0 <= c.first and c.first < reader.dimension[0] and
              0 <= c.second and c.second < reader.dimension[1]))
            continue;
        unsigned int a;
        if (tick_info[0][y][x].district != tick_info[0][c.first][c.second].district) { a = 2; }
        else if (y != c.first or x != c.second) { a = 1; }
        else { a = 0; }
        if (tick_info[curr_tick - 1][c.first][c.second].infectionRate > t * a) {
            sum_infection_rate +=
                    double(clamp(int(tick_info[0][y][x].population - tick_info[0][c.first][c.second].population), 0, 2) +
                          1);
        } else {
            sum_infection_rate += 0;
        }
    }

    avg_plus_sum_infection_rate = avg_infection_rate + sum_infection_rate;
    unsigned int solution = ceil(avg_plus_sum_infection_rate * double((reader.factors[3] % 25) + 50) / 100.0);
    update_factor(reader.factors[3]);
    return solution;
}

unsigned int Solver::healing(unsigned int y, unsigned int x) {
    unsigned int width = reader.dimension[0];
    unsigned int height = reader.dimension[1];
    unsigned int curr_tick = reader.data[1];
    unsigned int curr_infection_rate;

    if (width + height < curr_tick) {
        // min. ker. az adott cella múltbeli fertőzöttésgei között width+height tickre visszamenően
        unsigned int min_infection_rate = 100;
        for (unsigned int i = 1; i <= (width + height); i++) {
            curr_infection_rate = tick_info[curr_tick - i][y][x].infectionRate;
            if (curr_infection_rate < min_infection_rate) {
                min_infection_rate = curr_infection_rate;
            }
        }
        unsigned int result = floor(min_infection_rate * (reader.factors[0] % 10) / 20.0);
        update_factor(reader.factors[0]);
        return result;
    } else {
        return 0;
    }
}

void Solver::update_factor(uint32_t &factor) {
    factor = uint64_t(factor) * uint64_t(48271) % uint64_t(0x7fffffff);
}

void Solver::load_clean_districts() {
    not_clean_districts.clear();
    for (size_t x = 0; x < reader.dimension[1]; x++) {
        for (size_t y = 0; y < reader.dimension[0]; y++) {
            if (reader.areas[y][x].infectionRate > 0) {
                not_clean_districts.insert(reader.areas[y][x].district);
            }
        }
    }
}

void Solver::answer_msg(vector<std::string> & commands) {
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
    commands.emplace_back(".");
}


[[maybe_unused]] void Solver::debug_msg(vector<string> &commands) {
    stringstream ss;
    string command;

    ss << "FACTORS " << reader.factors[0] << " " << reader.factors[1] << " " << reader.factors[2] << " "
       << reader.factors[3];
    getline(ss, command);
    commands.push_back(command);
    ss.clear();

    commands.emplace_back("INFECTION");
    for (const auto &y : infection_history[reader.data[1]]) {
        for (const auto &inf : y) {
            ss << inf << " ";
        }
        getline(ss, command);
        commands.push_back(command);
        ss.clear();
    }

    commands.emplace_back("HEALING");
    for (const auto &y : heal) {
        for (const auto &h : y) {
            ss << h << " ";
        }
        getline(ss, command);
        commands.push_back(command);
        ss.clear();
    }

    commands.emplace_back(); // ez egy üres sort generál
}