#include "Solver.h"

using namespace std;

void Solver::infection() {
    for (size_t x = 0; x < reader.dimension[1]; x++) {
        for (size_t y = 0; y < reader.dimension[0]; y++) {
            if (reader.info.curr_tick == 0) {
                if (reader.areas[reader.mat2sub(y,x)].infectionRate > 0) {
                    infection_history[0][y][x] = 1;
                }
            } else if (reader.safe_districts.find(reader.areas[reader.mat2sub(y,x)].district) ==
                       reader.safe_districts.end()) { // ha tiszta kerületek között nincs, akkor van infection
                size_t inf = field_infection(y, x);
                if ((inf + reader.areas[reader.mat2sub(y,x)].healthRate + reader.areas[reader.mat2sub(y,x)].infectionRate) > 100) {
                    inf = 100 - reader.areas[reader.mat2sub(y,x)].healthRate - reader.areas[reader.mat2sub(y,x)].infectionRate;
                }

                infection_history[reader.info.curr_tick][y][x] = inf;
                reader.areas[reader.mat2sub(y,x)].infectionRate += inf;
            }
        }
    }
}

size_t Solver::field_infection(size_t y, size_t x) {
    uint32_t curr_tick = reader.info.curr_tick;

    size_t n = min(reader.factors[1] % 10 + 10, curr_tick); // rekurzív visszalépések száma (számláló): 10. tick alatt curr_tick, 11. tick-től {10,...20} közül valami
    update_factor(reader.factors[1]);

    size_t t = reader.factors[2] % 7 + 3; // átfertőződési hajlandóság: {3,...10} közül valami
    update_factor(reader.factors[2]);

    // átlagos átfertőződés
    size_t sum = 0;
    for (std::size_t i = 1; i <= n; i++) {
        sum += infection_history[curr_tick - i][y][x];
    }

    vector<pair<size_t, size_t>> neighbours = get_nbs(y, x);
    neighbours.emplace_back(y, x);

    // additív átfertőződés
    size_t sum_infection_rate = 0;
    double avg_infection_rate;
    double counter = 5;
    for (auto nbs : neighbours) {

        // dist kiszámítása
        size_t dist; // távolság {0,...2] közül valami
        if (reader.areas[reader.mat2sub(y,x)].district !=
            reader.areas[reader.mat2sub(nbs.first,nbs.second)].district) { dist = 2; } // különböző kerületben vannak
        else if (y != unsigned(nbs.first) or x != unsigned(nbs.second)) { dist = 1; } // azonos kerületben vannak
        else { dist = 0; } // a terület önmaga

        // sum_infection_rate kiszámítása a szomszédos fertőző területek népessége alapján, ha eléggé fertőzőek voltak előző körben
        if (infection_rate_history[curr_tick - 1][nbs.first][nbs.second] > t * dist) {
            size_t population_diff =
                    clamp(int(reader.areas[reader.mat2sub(y,x)].population - reader.areas[reader.mat2sub(nbs.first,nbs.second)].population), 0, 2) +
                    1;

            // helyek populációs különbsége: {1,...3} közül valami
            // clamp(n, lower, upper): ha n < lower, akkor lower; ha n > upper, akkor upper; különben n
            sum_infection_rate += population_diff;
        }
    }
    avg_infection_rate = sum_infection_rate / counter;
    size_t rand = reader.factors[3] % 25; // {50,...75} közé eső valami
    update_factor(reader.factors[3]);

    double r = (double(double(sum) / double(n)) + avg_infection_rate) * double(rand) / 100;
    size_t result = ceil(round(r * 1000) / 1000);

//    if (reader.data[1] == 29 && y == 40 && x == 8) {
//        cout << "sum: " << sum << endl;
//        cout << "n: " << n << endl;
//        cout << "sum inf: " << sum_infection_rate << endl;
//        cout << "rand: " << rand << endl;
//        cout << "r: " << r << endl;
//        cout << "result: " << result << endl << endl;
//    }

    return result;
}

