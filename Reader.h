#ifndef UNAGI_READER_H
#define UNAGI_READER_H

#include <iostream>
#include <array>
#include <vector>
#include <sstream>
#include <unordered_map>

using namespace std;

struct Country {
    size_t TPC; //total production capacity
    size_t RV; //reserved vaccine
    // unordered_set<unsigned int> ASID; //meggyógyított kerületek listája, egyelőre nem lényeg
};

struct Area {
    size_t y, x;
    size_t district;
    size_t infectionRate;
    size_t healthRate;
    size_t population;
    size_t field_vaccine;
    // size_t sum_pre_vaccine;
    size_t vaccinated;
};

struct Info {
    size_t game_id;
    size_t countries_count;
    size_t max_tick;

    size_t curr_tick;
    size_t country_id;
    size_t TPC_0;
};

class Reader {
public:
    Info info{};
    array<uint32_t, 4> factors{};
    array<unsigned short, 2> dimension{};
    vector<Area> areas{};
    string message{};
    bool hasEnd = false;
    bool needAnsw = true;

    unordered_map<size_t, vector<pair<size_t, size_t>>> districts; // első vector index a district id, a belső vector a hozzá tartozó koordináták
    unordered_map<size_t, Country> countries; // az index a country id
    unordered_map<size_t, size_t> safe_districts; // kulcs a district id, érték az ország ID

    Reader() = default;
    void readDataConsole();
    void readDataInfo(const vector<string>& infos);
    [[nodiscard]] size_t mat2sub (const size_t& y, const size_t& x) const {
        return y*dimension[1]+x;
    }
    [[nodiscard]] pair<size_t, size_t> sub2mat (const size_t& z) const {
        size_t x = z % dimension[1];
        size_t y = (z-x) / dimension[1];
        return {y,x};
    }
private:
    bool readHelperFunc(string& line);
};

#endif //UNAGI_READER_H
