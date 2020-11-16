#include "Solver.h"

using namespace std;

void Solver::cleaned_back() {
    unsigned int curr_tick = reader.info.curr_tick;

    for (size_t x = 0; x < reader.dimension[1]; x++) {
        for (size_t y = 0; y < reader.dimension[0]; y++) {
            if (reader.safe_districts.find(reader.areas[reader.mat2sub(y,x)].district)
                != reader.safe_districts.end() and reader.areas[reader.mat2sub(y,x)].field_vaccine != 0) {
                Action temp{};
                temp.val = reader.areas[reader.mat2sub(y,x)].field_vaccine;
                temp.x = x;
                temp.y = y;
                back(temp);
            }
        }
    }
}