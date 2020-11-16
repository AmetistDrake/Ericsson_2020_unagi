#include "Reader.h"

using namespace std;

bool Reader::readHelperFunc(std::string& line) {
    if (!line.rfind('.', 0)) {
        return true;
    }

    if (!line.rfind("WRONG", 0) ||
        !line.rfind("SUCCESS", 0) ||
        !line.rfind("FAILED", 0)) {
        hasEnd = true;
        message = move(line);
    } else if (!line.rfind("REQ", 0)) {
        needAnsw = true;
        stringstream(move(line).substr(4)) >> info.game_id >> info.curr_tick >> info.country_id;
    } else if (!line.rfind("START", 0)) {
        needAnsw = false;
        stringstream(move(line).substr(6)) >> info.game_id >> info.max_tick >> info.countries_count;
    } else if (!line.rfind("FACTORS", 0)) {
        stringstream(move(line).substr(8)) >> factors[0] >> factors[1] >> factors[2] >> factors[3];
    } else if (!line.rfind("FIELDS", 0)) {
        stringstream(move(line).substr(7)) >> dimension[0] >> dimension[1]; // dimension[0] = rows = N, dimension[1] = cols = M
        areas.resize(dimension[0]*dimension[1], Area());
    } else if (!line.rfind("FD", 0)) {
        stringstream ss(move(line).substr(3));
        size_t y, x;
        ss >> y >> x;
        Area a = areas[y*dimension[1]+x];
        a.x = x;
        a.y = y;
        ss >> a.district >> a.infectionRate >> a.population;
        districts[a.district].push_back({y,x});
    } else if (!line.rfind("VAC", 0)) {
        stringstream ss(move(line).substr(4));
        size_t y, x;
        ss >> y >> x;
        size_t sum_pre_vacc, vaccinated;
        ss >> sum_pre_vacc >> vaccinated;// areas[mat2sub(y,x)].sum_pre_vaccine >> areas[mat2sub(y,x)].vaccinated;
    } else if (!line.rfind("SAFE", 0)) {
        size_t country_id;
        size_t safe_distict;
        stringstream(move(line).substr(5)) >> country_id >> safe_distict;
        // safe_districts[safe_distict] = country_id;
    } else if (!line.rfind("WARN", 0)) {
        message = move(line);
    }
    else {
        cerr << "READER ERROR HAPPENED: unrecognized command line: " << line << endl;
        hasEnd = true;
        return true;
    }
    return false;
}

void Reader::readDataConsole() {
    string line;

    while (getline(cin, line)) {
        if (readHelperFunc(line)) { // ha igaz, akkor vége a beolvasásnak
            return;
        }
        if (!line.rfind("REQ", 0)) {
            for (size_t i = 0; i < info.countries_count; i++) {
                getline(cin, line);
                size_t country_id;
                stringstream(line) >> country_id >> countries[country_id].TPC >> countries[country_id].RV;
                //info.TPC_0 = countries[country_id].TPC;
            }
        }
    }
    cerr << "Unexpected input end." << endl;
    hasEnd = true;
}

void Reader::readDataInfo(const vector<string>& infos) {
    for (auto it = infos.begin(); it != infos.end(); it++) {
        string line = *it;
        if (readHelperFunc(line)) {
            return;
        }
        if (!line.rfind("REQ", 0)) {
            for (size_t i = 0; i < info.countries_count; i++) {
                it++;
                line = *it;
                int country_id;
                stringstream(line) >> country_id >> countries[country_id].TPC >> countries[country_id].RV;
                info.TPC_0 = countries[country_id].TPC;
            }
        }
    }
    cerr << "Unexpected input end." << endl;
    hasEnd = true;
}

