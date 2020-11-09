#include "Solver.h"
#include <iostream>
#include <sstream>
#include <cmath>
#include <algorithm>

using namespace std;

vector<string> Solver::process(const vector<string>& infos) {
    /********************************************/      /// A tesztelés a beadott verziótól itt különül el
    if (infos.size() == 1 && infos[0] == "unagi") {     // amikor a consolon keresztül kommunikál a program
        reader.readDataConsole();
    } else {                                            // amikor hálózaton keresztül kommunikál a program
        reader.readDataInfo(infos);
    }
    /********************************************/      /// Speciális esetek, amikor véget ée a program, vagy ha nem kell válasz
    if (reader.hasEnd) {
        return vector<string>{"Game over"};             // a main.cpp-ben megszakad a while loop és véget ér a program
    }

    if (!reader.needAnsw) {                             // ha nem kell válasz, üres vectort adunk vissza, amire a while loop tovább lép
        return vector<string>{};
    }
    /********************************************/      /// Általános válasz:

    vector<vector<vector<string>>> disp_msg (reader.dimension[0], vector<vector<string>>(reader.dimension[1], vector<string>()));

    // healing - fertőzöttek gyógyulása
    vector<vector<unsigned int>> tmp(reader.dimension[0],
                                     vector<unsigned int>(reader.dimension[1], 0));
    // 1) vakcina elhelyezés, csoportosítás
    vaccine_history.push_back(tmp);

    // 2) healing - fertőzöttek gyógyulása
    healing_history.push_back(tmp);
    for (size_t x = 0; x <
                       reader.dimension[1]; x++) { // oszlop és sorfolytonos indexelés, ez fontos, mert számít hogy a faktorok melyik iterációban frissülnek
        for (size_t y = 0; y < reader.dimension[0]; y++) {
            if (reader.areas[y][x].infectionRate > 0) {
                unsigned int h = healing(y, x);
                healing_history[reader.data[1]][y][x] = h;
                reader.areas[y][x].healthRate += h;
                reader.areas[y][x].infectionRate -= h;
            }
        }
    }
    // 3) megtisztítottról visszekerül az országraktárba
    update_infected_districts();
    cleaned_back();


    // 4) infection - vírus terjed
    infection_history.push_back(tmp);
    for (size_t x = 0; x < reader.dimension[1]; x++) {
        for (size_t y = 0; y < reader.dimension[0]; y++) {
            if (reader.data[1] == 0) {
                if (reader.areas[y][x].infectionRate > 0) {
                    infection_history[0][y][x] = 1;
                }
            } else if (infected_districts.find(reader.areas[y][x].district) !=
                       infected_districts.end()) { // ha a terület kerületében van még fertőzött
                unsigned int inf = infection(y, x);
                if ((inf + reader.areas[y][x].healthRate + reader.areas[y][x].infectionRate) > 100) {
                    inf = 100 - reader.areas[y][x].healthRate - reader.areas[y][x].infectionRate;
                }

                infection_history[reader.data[1]][y][x] = inf;
                reader.areas[y][x].infectionRate += inf;
            }
        }
    }

    tick_info.push_back(reader.areas); // update tick info with current tick
    update_infected_districts();

    // 5) vakcinagyártás
    vaccine_production();

    field_display.push_back(disp_msg);
    // Válasz
    vector<string> commands;
    answer_msg(commands); // readerből beletölti a megfelelő infokat
    return commands; // küldés vagy kiiratás
}

/// Solver fv. defek
unsigned int Solver::infection(unsigned int y, unsigned int x) {
    unsigned int curr_tick = reader.data[1];

    unsigned int n = min(reader.factors[1] % 10 + 10,
                         curr_tick); // rekurzív visszalépések száma (számláló): 10. tick alatt curr_tick, 11. tick-től {10,...20} közül valami
    update_factor(reader.factors[1]);

    unsigned int t = reader.factors[2] % 7 + 3; // átfertőződési hajlandóság: {3,...10} közül valami
    update_factor(reader.factors[2]);

    // átlagos átfertőződés
    unsigned int sum = 0;
    for (std::size_t i = 1; i <= n; i++) {
        sum += infection_history[curr_tick - i][y][x];
    }

    vector<std::pair<int, int>> neighbours{{0,  0},
                                           {-1, 0},
                                           {0,  -1},
                                           {1,  0},
                                           {0,  1}};

    // additív átfertőződés
    unsigned int sum_infection_rate = 0;
    double avg_infection_rate;
    double counter = 5;
    for (auto nbs : neighbours) {
        pair<int, int> c(y - nbs.first, x - nbs.second);

        if (!(0 <= c.first and c.first < reader.dimension[0] and
              0 <= c.second and c.second < reader.dimension[1])) // határpontok szomszédait nem vizsgáljuk
        {
            counter--;
            continue;
        }

        // dist kiszámítása
        unsigned int dist; // távolság {0,...2] közül valami
        if (tick_info[0][y][x].district !=
            tick_info[0][c.first][c.second].district) { dist = 2; } // különböző kerületben vannak
        else if (y != unsigned(c.first) or x != unsigned(c.second)) { dist = 1; } // azonos kerületben vannak
        else { dist = 0; } // a terület önmaga

        if (reader.data[1] == 13 && y == 19 && x == 17) {
//            cout << "dist: " << dist << endl;
//            cout << "t: " << t << endl;
//            cout << "t * dist: " << t*dist << endl;
//            cout << "prev inf rate: " << tick_info[curr_tick - 1][c.first][c.second].infectionRate << endl;
        }

        // sum_infection_rate kiszámítása a szomszédos fertőző területek népessége alapján, ha eléggé fertőzőek voltak előző körben
        if (tick_info[curr_tick - 1][c.first][c.second].infectionRate > t * dist) {
            unsigned int population_diff =
                    clamp(int(tick_info[0][y][x].population - tick_info[0][c.first][c.second].population), 0, 2) + 1;
            if (reader.data[1] == 13 && y == 19 && x == 17) {
//                cout << "first population: " << tick_info[0][y][x].population << endl;
//                cout << "second population: " << tick_info[0][c.first][c.second].population << endl;
//                cout << "population diff: " << population_diff << endl;
            }

            // helyek populációs különbsége: {1,...3} közül valami
            // clamp(n, lower, upper): ha n < lower, akkor lower; ha n > upper, akkor upper; különben n
            sum_infection_rate += population_diff;
        }
    }
    avg_infection_rate = sum_infection_rate / counter;
    unsigned int rand = reader.factors[3] % 25; // {50,...75} közé eső valami
    update_factor(reader.factors[3]);

    double r = (double(double(sum) / double(n)) + avg_infection_rate) * double(rand) / 100;
    unsigned int result = ceil(round(r * 1000) / 1000);

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

unsigned int Solver::healing(unsigned int y, unsigned int x) {
    unsigned int width = reader.dimension[0];
    unsigned int height = reader.dimension[1];
    unsigned int curr_tick = reader.data[1];
    unsigned int curr_infection_rate;

    if (width + height < curr_tick) {
        ///******************* MÁSODIK FORDULÓ *****************///
        unsigned int IR = tick_info[curr_tick - 1][y][x].infectionRate;
        unsigned int P = tick_info[0][y][x].population; //start_info

        unsigned int n = 0; //összes ország tartalék vakcinái
        for (const auto &a : reader.countries) {
            n += a.second.RV;
        }

        int X = min(n * P, IR); //ennyivel csökken az infection és nő a healthRate vakcinázás után
        int m = ceil(X / P); //ennyivel csökken a tartalék vakcinaszám (összesen)
        ///vakcina miatti gyógyulás
        if (IR > 0 && n > 0) {
            reader.areas[y][x].healthRate += X;
            reader.areas[y][x].infectionRate -= X;
            ///tartalék vakcinaszám csökkentése
            for (auto a : reader.countries) {
                a.second.RV = floor(a.second.RV * (n - m) / n);
            }
        }
        ///******************* MÁSODIK FORDULÓ *****************///
        // min. ker. az adott cella múltbeli fertőzöttésgei között width+height tickre visszamenően
        unsigned int min_infection_rate = 100;
        for (unsigned int i = 1; i <= (width + height); i++) {
            curr_infection_rate = tick_info[curr_tick - i][y][x].infectionRate;
            if (curr_infection_rate < min_infection_rate) {
                min_infection_rate = curr_infection_rate;
            }
        }
        unsigned int h = floor(min_infection_rate * (reader.factors[0] % 10) / 20.0);
        update_factor(reader.factors[0]);
        return floor(h * (IR - X) / IR); ///2. fordulós visszatérési érték
    } else {
        return 0;
    }
}

void Solver::vaccine_production() {
    int country_id = reader.data[2];
    unsigned int sum_of_areas = 0;
    unsigned int minus_val = 0;
    vector<std::pair<int, int>> neighbours{{-1, 0},
                                           {0,  -1},
                                           {1,  0},
                                           {0,  1}};

    for (auto clean: reader.countries[country_id].ASID) {
        for (size_t y = 0; y < reader.areas.size(); y++) {
            for (std::size_t x = 0; x < reader.areas[y].size(); x++) {
                if (reader.areas[y][x].district == clean) {
                    for (auto nbs : neighbours) {
                        pair<int, int> c(y - nbs.first, x - nbs.second);
                        if (reader.countries[country_id].ASID.find(reader.areas[c.first][c.second].district) ==
                            reader.countries[country_id].ASID.end()) {
                            //szomszed még nem megtisztított
                            minus_val += (6 - tick_info[0][c.first][c.second].population);
                        }
                    }
                    sum_of_areas++;
                }
            }
        }
    }
    int value = 2 * sum_of_areas - ceil(minus_val / 3);
    if (value < 0) {
        value = 0;
    }
    reader.countries[country_id].TPC = value;
}

void Solver::cleaned_back() {
    unsigned int curr_tick = reader.data[1];
    int country_id = reader.data[2];
    // for(auto district : reader.countries[country_id].ASID){

    for (size_t y = 0; y < reader.areas.size(); y++) {

        for (std::size_t x = 0; x < reader.areas[y].size(); x++) {
            if (reader.countries[country_id].ASID.find(reader.areas[y][x].district)
                != reader.countries[country_id].ASID.end() and vaccine_history[curr_tick][y][x] != 0) {
                Action temp;
                temp.val = vaccine_history[curr_tick][y][x];
                temp.x = x;
                temp.y = y;
                BACK.push_back(temp);
                //itt majd am meg  kell hívni a visszarakos függvényt csak ilyen még nincs
                //de megy a raktárba a val és nullázódik a terület raktára
            }
        }
    }

}


void Solver::answer_msg(vector<std::string> &commands) {
    stringstream ss;
    string command;

    ss << "RES " << reader.data[0] << " " << reader.data[1] << " " << reader.data[2] << std::endl;
    getline(ss, command);
    commands.push_back(command);
    ss.clear();

    for (const auto &b : BACK) {
        ss << "BACK " << b.y << " " << b.x << " " << b.val << "\n";
        getline(ss, command);
        commands.push_back(command);
        ss.clear();
    }

    for (const auto p : PUT) {
        ss << "PUT " << p.y << " " << p.x << " " << p.val << "\n";
        getline(ss, command);
        commands.push_back(command);
        ss.clear();
    }

//    for (auto& row : reader.areas) {
//        for (auto& area : row) {
//            ss << area.infectionRate << " ";
//        }
//        getline(ss, command);
//        commands.push_back(command);
//        ss.clear();
//    }
    commands.emplace_back(".");
}
