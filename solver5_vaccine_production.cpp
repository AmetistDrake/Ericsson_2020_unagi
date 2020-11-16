#include "Solver.h"

using namespace std;

void Solver::vaccine_production() {
    int country_id = reader.info.country_id;
    unsigned int sum_of_areas = 0;
    unsigned int minus_val = 0;


    for (auto clean: reader.safe_districts) {
            for (size_t x = 0; x < reader.dimension[1]; x++) {
                for (size_t y = 0; y < reader.dimension[0]; y++) {
                if (reader.safe_districts.find(reader.areas[reader.mat2sub(y,x)].district) != reader.safe_districts.end()) {
                    vector<std::pair<size_t, size_t>> neighbours = get_nbs(y, x);
                    for (auto nbs : neighbours) {
                        if (reader.safe_districts.find(reader.areas[reader.mat2sub(nbs.first,nbs.second)].district) ==
                            reader.safe_districts.end()) {
                            //szomszed még nem megtisztított
                            minus_val += (6 - reader.areas[reader.mat2sub(nbs.first,nbs.second)].population);
                        }
                    }
                    sum_of_areas++;
                }
            }
        }

    int value = 2 * sum_of_areas - ceil(minus_val / 3);
    if (value < (0 - reader.info.TPC_0)) {
        value = 0;
    }
    reader.countries[country_id].TPC = reader.info.TPC_0 + value;
    reader.countries[country_id].RV += value;
}