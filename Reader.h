#ifndef UNAGI_READER_H
#define UNAGI_READER_H

#include <iostream>
#include <array>
#include <vector>
#include <sstream>
#include <unordered_map>
#include <unordered_set>

struct Country {
    int TPC; //total production capacity
    int RV; //reserved vaccine
    std::unordered_set<unsigned int> ASID; //meggyógyított területek listája
};

struct Area {
    unsigned int district;
    unsigned int infectionRate;
    unsigned int healthRate;
    unsigned int population;
};

class Reader {
private:
    bool readHelperFunc(std::string& line);
public:
    int countries_count {};
    std::array<unsigned int, 3> data{};
    std::array<std::uint32_t, 4> factors{};
    std::array<unsigned short, 2> dimension{};
    std::vector<std::vector<Area>> areas{};
    std::string message{};
    bool hasEnd = false;
    bool needAnsw = true;

    std::unordered_map<int, int> safe_districts; // a solveres set emiatt valszeg majd nem kell
    std::unordered_map<int, Country> countries;
    std::vector<std::vector<int>> sum_of_previous_vaccine_on_areas;
    std::vector<std::vector<int>> vaccinated;

    Reader() = default;
    void readDataConsole();
    void readDataInfo(const std::vector<std::string>& infos);
};

#endif //UNAGI_READER_H
