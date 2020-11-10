#include "Solver.h"
#include <filesystem>
#include <fstream>

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



/*****************************************************************************************/ // json file kiírás
void write_json_vv (ofstream& kf, const Reader& reader, const string& what) {
    kf << "\t\""+ what +"\" : [";
    for (size_t i = 0; i < reader.areas.size(); i++) {
        kf << "[";
        for(size_t j = 0; j < reader.areas[i].size(); j++) {
            if (what == "population") {
                if (j == reader.areas[i].size()-1) {
                    kf << to_string(reader.areas[i][j].population);
                }
                else {
                    kf << to_string(reader.areas[i][j].population) << ", ";
                }
            }
            else if (what == "district") {
                if (j == reader.areas[i].size()-1) {
                    kf << to_string(reader.areas[i][j].district);
                }
                else {
                    kf << to_string(reader.areas[i][j].district) << ", ";
                }
            }
        }
        if (i == reader.areas.size()-1) {
            kf << "]";
        }else {
            kf << "], ";
        }
    }
    kf << "],\n";
}

void write_json_vvv (ofstream& kf, const vector<vector<vector<unsigned int>>>& data, const string& what) {
    kf << "\t\""+ what +"\" : [";
    for (size_t i = 0; i < data.size(); i++) {
        kf << "[";
        for(size_t j = 0; j < data[i].size(); j++) {
            kf << "[";
            for (size_t k = 0; k < data[i][j].size(); k++) {
                if (k == data[i][j].size()-1) {
                    kf << to_string(data[i][j][k]);
                }
                else {
                    kf << to_string(data[i][j][k]) << ", ";
                }
            }
            if (j == data[i].size()-1) {
                kf << "]";
            }else {
                kf << "], ";
            }
        }
        if (i == data.size()-1) {
            kf << "]";
        }else {
            kf << "], ";
        }
    }
    kf << "],\n";
}

void Solver::create_json_from_data() {
    ofstream kf;
    string path_str = "../displays/json_files/";
    filesystem::path path = path_str;
    if (!filesystem::exists(path))
    {
        filesystem::create_directories(path);
    }
    kf.open(path_str + to_string(reader.data[0]) + ".json");
    kf << "{\n";

    kf << "\t\"N\" : " << to_string(reader.dimension[1]) <<",\n";
    kf << "\t\"M\" : " << to_string(reader.dimension[0]) <<",\n";
    kf << "\t\"counry_id\" : " << to_string(reader.data[2]) <<",\n";
    kf << "\t\"num_of_countries\" : " << to_string(reader.countries_count) <<",\n";
    kf << "\t\"country_id\" : " << to_string(reader.data[2]) <<",\n";
    kf << "\t\"max_tick\" : " << to_string(reader.max_tick) <<",\n";


    write_json_vv(kf, reader, "population");
    write_json_vv(kf, reader, "district");

    write_json_vvv(kf, infection_rate_history, "infection_rate");
    write_json_vvv(kf, health_rate_history, "health_rate");
    write_json_vvv(kf, infection_history, "infection");
    write_json_vvv(kf, healing_history, "healing");
    write_json_vvv(kf, vaccine_history, "vaccines");

    kf << "\t\"messages\" : [";
    for (size_t i = 0; i < msg_history.size(); i++) {
        kf << "[";
        for(size_t j = 0; j < msg_history[i].size(); j++) {
            kf << "[";
            for (size_t k = 0; k < msg_history[i][j].size(); k++) {
                kf << "[";
                for (size_t l = 0; l < msg_history[i][j][k].size(); l++) {
                    if (k == msg_history[i][j].size()-1) {
                        kf << "\"" << msg_history[i][j][k][l] << "\"";
                    }
                    else {
                        kf << "\"" << msg_history[i][j][k][l] << "\"" << ", ";
                    }
                }
                if (k == msg_history[i][j].size()-1) {
                    kf << "]";
                }else {
                    kf << "], ";
                }

            }
            if (j == msg_history[i].size()-1) {
                kf << "]";
            }else {
                kf << "], ";
            }
        }
        if (i == msg_history.size()-1) {
            kf << "]";
        }else {
            kf << "], ";
        }
    }
    kf << "]\n";

    kf << "}";
    kf.close();
}