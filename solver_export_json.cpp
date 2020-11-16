#include "Solver.h"
#include <filesystem>
#include <fstream>

using namespace std;

void write_json_vv(ofstream& kf, const Reader& reader, const string& what) {
    kf << "\t\"" + what + "\" : [";
    for (size_t y = 0; y < reader.dimension[0]; y++) {
        kf << "[";
        for (size_t x = 0; x < reader.dimension[1]; x++) {
            if (what == "population") {
                if (x == reader.dimension[1] - 1) {
                    kf << to_string(reader.areas[reader.mat2sub(y,x)].population);
                } else {
                    kf << to_string(reader.areas[reader.mat2sub(y,x)].population) << ", ";
                }
            } else if (what == "district") {
                if (x == reader.dimension[1] - 1) {
                    kf << to_string(reader.areas[reader.mat2sub(y,x)].district);
                } else {
                    kf << to_string(reader.areas[reader.mat2sub(y,x)].district) << ", ";
                }
            }
        }
        if (y == reader.dimension[0] - 1) {
            kf << "]";
        } else {
            kf << "], ";
        }
    }
    kf << "],\n";
}

void write_json_vvv(ofstream& kf, const vector<vector<vector<size_t>>>& data, const string& what) {
    kf << "\t\"" + what + "\" : [";
    for (size_t i = 0; i < data.size(); i++) {
        kf << "[";
        for (size_t y = 0; y < data[i].size(); y++) {
            kf << "[";
            for (size_t x = 0; x < data[i][y].size(); x++) {
                if (x == data[i][y].size() - 1) {
                    kf << to_string(data[i][y][x]);
                } else {
                    kf << to_string(data[i][y][x]) << ", ";
                }
            }
            if (y == data[i].size() - 1) {
                kf << "]";
            } else {
                kf << "], ";
            }
        }
        if (i == data.size() - 1) {
            kf << "]";
        } else {
            kf << "], ";
        }
    }
    kf << "],\n";
}

void Solver::create_json_from_data() {
    ofstream kf;
    string path_str = "../displays/json_files/";
    filesystem::path path = path_str;
    if (!filesystem::exists(path)) {
        filesystem::create_directories(path);
    }
    kf.open(path_str + to_string(reader.info.game_id) + ".json");
    kf << "{\n";

    kf << "\t\"N\" : " << to_string(reader.dimension[0]) << ",\n";
    kf << "\t\"M\" : " << to_string(reader.dimension[1]) << ",\n";
    kf << "\t\"num_of_countries\" : " << to_string(reader.info.countries_count) << ",\n";
    kf << "\t\"country_id\" : " << to_string(reader.info.country_id) << ",\n";
    kf << "\t\"max_tick\" : " << to_string(reader.info.max_tick) << ",\n";
    kf << "\t\"RV\" : [";
    for (size_t i = 0; i < RV_history.size(); i++) {
        if (i == RV_history.size() - 1) {
            kf << to_string(RV_history[i]);
        } else {
            kf << to_string(RV_history[i]) << ", ";
        }
    }
    kf << "],\n";

    kf << "\t\"TPC\" : [";
    for (size_t i = 0; i < TPC_history.size(); i++) {
        if (i == TPC_history.size() - 1) {
            kf << to_string(TPC_history[i]);
        } else {
            kf << to_string(TPC_history[i]) << ", ";
        }
    }
    kf << "],\n";

    kf << "\t\"clean_groups\" : [";
    for (size_t i = 0; i < clean_groups_history.size(); i++) { // tick
        kf << "[";
        for (size_t j = 0; j < clean_groups_history[i].size(); j++) { // group
            kf << "[";
            int num = 0;
            for (const auto& distr : clean_groups_history[i][j]) {  // district
                num++;
                if (num != clean_groups_history[i][j].size()) {
                    for (auto coord : reader.districts[distr]) { // terület
                        kf << "[" << to_string(coord.first) + ", " << to_string(coord.second) << "], ";
                    }
                } else {
                    int num2 = 0;
                    for (auto coord : reader.districts[distr]) { // terület
                        num2++;
                        if (num2 != reader.districts[distr].size()) {
                            kf << "[" << to_string(coord.first) + ", " << to_string(coord.second) << "], ";
                        } else {
                            kf << "[" << to_string(coord.first) + ", " << to_string(coord.second) << "]";
                        }
                    }
                }
            }
            if (j == clean_groups_history[i].size() - 1) {
                kf << "]";
            } else {
                kf << "], ";
            }
        }
        if (i == clean_groups_history.size() - 1) {
            kf << "]";
        } else {
            kf << "], ";
        }
    }
    kf << "],\n";

    write_json_vv(kf, reader, "population");
    write_json_vv(kf, reader, "district");

    write_json_vvv(kf, infection_rate_history, "infection_rate");
    write_json_vvv(kf, health_rate_history, "health_rate");
    write_json_vvv(kf, infection_history, "infection");
    write_json_vvv(kf, healing_history, "healing");
    write_json_vvv(kf, vaccinated_history, "vaccinated");
    write_json_vvv(kf, field_vaccine_history, "field_vaccine");

    kf << "\t\"messages\" : [";
    for (size_t i = 0; i < msg_history.size(); i++) {
        kf << "[";
        for (size_t j = 0; j < msg_history[i].size(); j++) {
            if (j == 0) {
                kf << "\"" << msg_history[i][j] << "<br/>";
            } else if (j == msg_history[i].size() - 1) {
                kf << msg_history[i][j] << "\"";
            } else {
                kf << msg_history[i][j] << "\\n";
            }
        }
        if (i == msg_history.size() - 1) {
            kf << "]";
        } else {
            kf << "], ";
        }
    }
    kf << "]\n";

    kf << "}";
    kf.close();
}
