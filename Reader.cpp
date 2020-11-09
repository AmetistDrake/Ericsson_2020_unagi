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
        message = std::move(line);
    } else if (!line.rfind("REQ", 0)) {
        needAnsw = true;
        std::stringstream(std::move(line).substr(4)) >> data[0] >> data[1] >> data[2];
    } else if (!line.rfind("START", 0)) {
        needAnsw = false;
        std::stringstream(std::move(line).substr(6)) >> data[0] >> data[1] >> countries_count;
    } else if (!line.rfind("FACTORS", 0)) {
        std::stringstream(std::move(line).substr(8)) >> factors[0] >> factors[1] >> factors[2] >> factors[3];
    } else if (!line.rfind("FIELDS", 0)) {
        std::stringstream(std::move(line).substr(7)) >> dimension[0] >> dimension[1];
        areas.resize(dimension[0], std::vector<Area>{dimension[1]});
        sum_of_previous_vaccine_on_areas.resize(dimension[0], std::vector<int>{dimension[1]});
        vaccinated.resize(dimension[0], std::vector<int>{dimension[1]});
    } else if (!line.rfind("FD", 0)) {
        std::stringstream ss(std::move(line).substr(3));
        std::size_t y, x;
        ss >> y >> x;
        Area& a = areas[y][x];
        ss >> a.district >> a.infectionRate >> a.population;
    } else if (!line.rfind("VAC", 0)) {
        std::stringstream ss(std::move(line).substr(4));
        std::size_t y, x;
        ss >> y >> x;
        int& a = sum_of_previous_vaccine_on_areas[y][x];
        int&b = vaccinated[y][x];
        ss >> a >> b;
    } else if (!line.rfind("SAFE", 0)) {
        unsigned int country_id;
        unsigned int safe_distict;
        std::stringstream(std::move(line).substr(5)) >> country_id >> safe_distict;
        safe_districts[safe_distict] = country_id;
    } else if (!line.rfind("WARN", 0)) {
        message = std::move(line);
    }
    else {
        std::cerr << "READER ERROR HAPPENED: unrecognized command line: " << line << std::endl;
        hasEnd = true;
        return true;
    }
    return false;
}

void Reader::readDataConsole() {
    std::string line;

    while (std::getline(std::cin, line)) {
        if (readHelperFunc(line)) { // ha igaz, akkor vége a beolvasásnak
            return;
        }
        if (!line.rfind("REQ", 0)) {
            for (size_t i = 0; i < data[2]; i++) {
                getline(std::cin, line);
                int country_id;
                std::stringstream(line) >> country_id >> countries[country_id].TPC >> countries[country_id].RV;
            }
        }
    }
    std::cerr << "Unexpected input end." << std::endl;
    hasEnd = true;
}

void Reader::readDataInfo(const std::vector<std::string>& infos) {
    for (auto it = infos.begin(); it != infos.end(); it++) {
        string line = *it;
        if (readHelperFunc(line)) {
            return;
        }
        if (!line.rfind("REQ", 0)) {
            for (size_t i = 0; i < countries_count; i++) {
                it++;
                line = *it;
                int country_id;
                std::stringstream(line) >> country_id >> countries[country_id].TPC >> countries[country_id].RV;
            }
        }
    }
    std::cerr << "Unexpected input end." << std::endl;
    hasEnd = true;
}

