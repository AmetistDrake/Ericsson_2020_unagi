#include "Solver.h"
#include <iostream>
#include <sstream>
#include <cmath>
#include <algorithm>

using namespace std;

vector<string> Solver::process(const vector<string> &infos) {
    /********************************************/      /// A tesztelés a beadott verziótól itt különül el
    if (infos.size() == 1 && infos[0] == "unagi") {     // amikor a consolon keresztül kommunikál a program
        reader.readDataConsole();
    } else {                                            // amikor hálózaton keresztül kommunikál a program
        reader.readDataInfo(infos);
    }
    /********************************************/      /// Speciális esetek, amikor véget és a program, vagy ha nem kell válasz
    if (reader.hasEnd) {
        return vector<string>{"Game over"};             // a main.cpp-ben megszakad a while loop és véget ér a program
    }

    if (!reader.needAnsw) {                             // ha nem kell válasz, üres vectort adunk vissza, amire a while loop tovább lép
        return vector<string>{};
    }
    /********************************************/      /// Általános válasz:

    vector<vector<unsigned int>> tmp(reader.dimension[0], vector<unsigned int>(reader.dimension[1], 0));
    vector<unordered_set<int>> clean_temp;
    PUT.clear();
    BACK.clear();

    // 1) vakcina elhelyezés, csoportosítás
    field_vaccine_history.push_back(tmp);
    clean_nbs_history.push_back(clean_temp);

    if (reader.data[1] == 0) { // első körös feltöltés
        // district számolás
        upload_nbs();
        size_t district_count = 0;
        for (size_t x = 0; x < reader.dimension[1]; x++) {
            for (size_t y = 0; y < reader.dimension[0]; y++) {
                if (reader.areas[y][x].district > district_count) {
                    district_count = reader.areas[y][x].district;
                }
            }
        }

        // kerületek szomszédságai, kerületenkénti területek
        for (size_t i = 0; i <= district_count; i++) {
            unordered_set<pair<int, int>, pair_hash> temp;
            keruletek.push_back(temp);
            unordered_set<int> temp2;
            szomszedsag.push_back(temp2);
        }
        district_areas();

        field_vaccine_history.push_back(tmp); // legyen benne egy jövő, ami feltölthető a késleltetésekkel
        TPC_0 = reader.countries[reader.data[2]].TPC;
    } else {
        for (size_t x = 0; x < reader.dimension[1]; x++) {
            for (size_t y = 0; y < reader.dimension[0]; y++) {
                field_vaccine_history[reader.data[1] - 1][y][x] = reader.areas[y][x].field_vaccine;
                reader.areas[y][x].field_vaccine += field_vaccine_history[reader.data[1]][y][x];
            }
        }
    }

     //visszaad egy unordered_set<pair<int, int>, pair_hash> -et amiben a lehetséges területek vannak, ahova vakcinát lehet tenni
    //minden letétel után meg kell nézni az új lehetséges területeket
    bool enough_vaccine;
    if (reader.countries[reader.data[2]].RV > 0) {
        enough_vaccine = true;
    } else {
        enough_vaccine = false;
    }

    while (enough_vaccine) {
        from_reserve();
        pair<unsigned int, unsigned int> c = where_to_put(possible_choice, fields_to_vaccinate);
        cout <<"A hely ahova tenni szeretnenk : " << c.first << "  " << c.second << endl;
        Action temp{};
        temp.y = c.first;
        temp.x = c.second;
        if (reader.areas[c.first][c.second].infectionRate == 0) { //ha tiszta terület
            cout << "tiszta" << endl;
            if (reader.areas[c.first][c.second].field_vaccine == 0) { //és még nincs rajta vakcina akkor minimum megy rá
                int min_vaccine = 6 - reader.areas[c.first][c.second].population;
                if (reader.countries[reader.data[2]].RV >= min_vaccine) {
                    temp.val = min_vaccine;
                    put(temp);
                    possible_choice.erase(c);
                } else {
                    enough_vaccine = false;
                }
            }
            if (enough_vaccine) {
                //possible_choice.insert({c.first, c.second});
                std::vector<std::pair<int, int>> neighbours =return_nbs({c.first, c.second});
                for(auto nbs:neighbours){
                    if(reader.safe_districts.find(reader.areas[nbs.first][nbs.second].district) == reader.safe_districts.end()){
                        //possible_choice.insert(nbs);
                    }
                }
            }
        }
        else {// ha fertőzött területen vagyunk
            cout << " fertozott " << endl;
            int needed_vaccine = reader.areas[c.first][c.second].infectionRate ;//ez egy naaagy becsles
            //cout << needed_vaccine << endl;
            if (needed_vaccine < 0) { needed_vaccine = 0; }
            if (needed_vaccine <6- reader.areas[c.first][c.second].population) { needed_vaccine = 6- reader.areas[c.first][c.second].population; }
            if (reader.countries[reader.data[2]].RV >= needed_vaccine) { // ha van elég vakcina
                cout << "van eleg vakcina" << endl;
                temp.val = needed_vaccine;
                put(temp);
                fields_to_vaccinate.erase(c);
                possible_choice.erase(c);
            }
            else {
                cout << "nincs eleg vakcina" << endl;
                temp.val = reader.countries[reader.data[2]].RV;
                put(temp);
                enough_vaccine = false;
            }
        }
enough_vaccine = false;
    }




    // 2) healing - fertőzöttek gyógyulása
    healing_history.push_back(tmp);
    vaccinated_history.push_back(tmp);
    implement_healing();

    // 3) megtisztítottról visszekerül az országraktárba
    cleaned_back();

    // 4) infection - vírus terjed
    infection_history.push_back(tmp);
    for (size_t x = 0; x < reader.dimension[1]; x++) {
        for (size_t y = 0; y < reader.dimension[0]; y++) {
            if (reader.data[1] == 0) {
                if (reader.areas[y][x].infectionRate > 0) {
                    infection_history[0][y][x] = 1;
                }
            } else if (reader.safe_districts.find(reader.areas[y][x].district) ==
                       reader.safe_districts.end()) { // ha tiszta kerületek között nincs, akkor van infection
                unsigned int inf = infection(y, x);
                if ((inf + reader.areas[y][x].healthRate + reader.areas[y][x].infectionRate) > 100) {
                    inf = 100 - reader.areas[y][x].healthRate - reader.areas[y][x].infectionRate;
                }

                infection_history[reader.data[1]][y][x] = inf;
                reader.areas[y][x].infectionRate += inf;
            }
        }
    }


    // 5) vakcinagyártás
    vaccine_production();

    // history-k feltöltése
    health_rate_history.push_back(tmp);
    infection_rate_history.push_back(tmp);
    for (size_t x = 0; x < reader.dimension[1]; x++) {
        for (size_t y = 0; y < reader.dimension[0]; y++) {
            health_rate_history[reader.data[1]][y][x] = reader.areas[y][x].healthRate;
            infection_rate_history[reader.data[1]][y][x] = reader.areas[y][x].infectionRate;
        }
    }

    RV_history.push_back(reader.countries[0].RV);
    TPC_history.push_back(reader.countries[0].TPC);

    // Válasz
    vector<string> commands;
    answer_msg(commands); // readerből beletölti a megfelelő infokat
    msg_history.push_back(commands);
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

    vector<std::pair<int, int>> neighbours = return_nbs({y, x});
    neighbours.emplace_back(y, x);

    // additív átfertőződés
    unsigned int sum_infection_rate = 0;
    double avg_infection_rate;
    double counter = 5;
    for (auto nbs : neighbours) {

        // dist kiszámítása
        unsigned int dist; // távolság {0,...2] közül valami
        if (reader.areas[y][x].district !=
            reader.areas[nbs.first][nbs.second].district) { dist = 2; } // különböző kerületben vannak
        else if (y != unsigned(nbs.first) or x != unsigned(nbs.second)) { dist = 1; } // azonos kerületben vannak
        else { dist = 0; } // a terület önmaga
        if (reader.data[1] == 13 && y == 19 && x == 17) {
//            cout << "dist: " << dist << endl;
//            cout << "t: " << t << endl;
//            cout << "t * dist: " << t*dist << endl;
//            cout << "prev inf rate: " << tick_info[curr_tick - 1][c.first][c.second].infectionRate << endl;
        }

        // sum_infection_rate kiszámítása a szomszédos fertőző területek népessége alapján, ha eléggé fertőzőek voltak előző körben
        if (infection_rate_history[curr_tick - 1][nbs.first][nbs.second] > t * dist) {
            unsigned int population_diff =
                    clamp(int(reader.areas[y][x].population - reader.areas[nbs.first][nbs.second].population), 0, 2) +
                    1;
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
        // min. ker. az adott cella múltbeli fertőzöttésgei között width+height tickre visszamenően
        unsigned int min_infection_rate = 100;
        for (unsigned int i = 1; i <= (width + height); i++) {
            curr_infection_rate = infection_rate_history[curr_tick - i][y][x];
            if (curr_infection_rate < min_infection_rate) {
                min_infection_rate = curr_infection_rate;
            }
        }
        unsigned int result = floor(min_infection_rate * (reader.factors[0] % 10) / 20.0);
        update_factor(reader.factors[0]);
        return result;
    } else {
        return 0;
    }
}

void Solver::implement_healing() {
    for (size_t x = 0; x <
                       reader.dimension[1]; x++) { // oszlop és sorfolytonos indexelés, ez fontos, mert számít hogy a faktorok melyik iterációban frissülnek
        for (size_t y = 0; y < reader.dimension[0]; y++) {
            if (reader.areas[y][x].infectionRate > 0 && (y + x < reader.data[1])) {
                unsigned int h = healing(y, x); //ha nincs vakcina alapból ennyi a healing

                ///******************* MÁSODIK FORDULÓ *****************///
                unsigned int IR = infection_rate_history[reader.data[1] - 1][y][x]; //prev InfectionRate
                unsigned int P = reader.areas[y][x].population; //start_info

                unsigned int n = 0; //összes ország tartalék vakcinái
                ///A KÖVI 3 SORT KI KELL KOMMENTELNI, HA VAKCINÁZNI AKARUNK!!!
                for (const auto &a : reader.countries) {
                   n += a.second.RV;
                }
                //n = reader.sum_of_previous_vaccine_on_areas[y][x];

                unsigned int X = min(n * P, IR); //ennyivel csökken az infection és nő a healthRate vakcinázás után
                int m = ceil(X / P); //ennyivel csökken a tartalék vakcinaszám az adott területen
                //std::cout << "ennyivel fog csokkeni a tartalek vakcinaszam az adott területen: " << m << '\n';

                ///vakcina miatti gyógyulás
                //mi van ha a területen nincs is vakcina?? feladatleírás alapján nem egyértelmű ennek a tesztelése
                if (IR > 0 && n > 0) { //ha előző körben volt fertőzött és vakcina is van -> oltsa be
                    std::cout<< "Van " << n << " db vakcina a " << y <<" "<< x << " teruleten osszesen. " <<'\n';
                    std::cout<< "Nekunk " << reader.areas[y][x].field_vaccine << " db vakcinank van itt." <<'\n';
                    vaccinated_history[reader.data[1]][y][x] = X; // vakcina által mennyi gyógyulás volt a területen
                    reader.areas[y][x].healthRate += X;
                    reader.areas[y][x].infectionRate -= X;
                    std::cout<< "Vakcina miatt gyogyult ennyivel: " << X <<'\n';
                    ///tartalék vakcinaszám csökkentése terület és országok szintjén
                    //reader.areas[y][x].field_vaccine -= m;
                    std::cout<< "A tarcsi vakcinaszam ennyivel csokkent: " << m <<'\n';
                    //reader.sum_of_previous_vaccine_on_areas[y][x] -= m;
                    /*for (auto a : reader.countries) {
                        a.second.RV = floor(a.second.RV * (n - m) / n);
                    }*/
                    h = floor(healing(y, x) * (IR - X) / IR); //ha van vakcina módosul a visszatérési érték
                } else {
                    vaccinated_history[reader.data[1]][y][x] = 0; //ha nem vakcináztunk
                }
                ///******************* MÁSODIK FORDULÓ *****************///
                healing_history[reader.data[1]][y][x] = h;
                reader.areas[y][x].healthRate += h;
                reader.areas[y][x].infectionRate -= h;
            }
        }
    }
}

void Solver::vaccine_production() {
    int country_id = reader.data[2];
    unsigned int sum_of_areas = 0;
    unsigned int minus_val = 0;


    for (auto clean: reader.safe_districts) {
        for (size_t y = 0; y < reader.areas.size(); y++) {
            for (std::size_t x = 0; x < reader.areas[y].size(); x++) {
                if (reader.areas[y][x].district == clean.first) {
                    vector<std::pair<int, int>> neighbours = return_nbs({y, x});
                    for (auto nbs : neighbours) {
                        if (reader.countries[country_id].ASID.find(reader.areas[nbs.first][nbs.second].district) ==
                            reader.countries[country_id].ASID.end()) {
                            //szomszed még nem megtisztított
                            minus_val += (6 - reader.areas[nbs.first][nbs.second].population);
                        }
                    }
                    sum_of_areas++;
                }
            }
        }
    }
    int value = 2 * sum_of_areas - ceil(minus_val / 3);
    if (value < (0 - TPC_0)) {
        value = 0;
    }
    reader.countries[country_id].TPC = TPC_0 + value;
    reader.countries[country_id].RV += value;
}

//megtisztított területekről vissza a központba
void Solver::cleaned_back() {
    unsigned int curr_tick = reader.data[1];
    //int country_id = reader.data[2];

    for (size_t y = 0; y < reader.areas.size(); y++) {
        for (size_t x = 0; x < reader.areas[y].size(); x++) {
            if (reader.safe_districts.find(reader.areas[y][x].district)
                != reader.safe_districts.end() and reader.areas[y][x].field_vaccine != 0) {
                Action temp{};
                temp.val = reader.areas[y][x].field_vaccine;
                if (field_vaccine_history[curr_tick][y][x] > 0) { // késleltetettek
                    temp.val += field_vaccine_history[curr_tick + 1][y][x];
                    field_vaccine_history[curr_tick + 1][y][x] = 0;
                }
                temp.x = x;
                temp.y = y;
                back(temp);
            }
        }
    }
}

//vissza a központba
void Solver::back(const Solver::Action &temp) {
    int country_id = reader.data[2];
    if (reader.areas[temp.y][temp.x].field_vaccine - temp.val >= 1) {
        reader.areas[temp.y][temp.x].field_vaccine -= temp.val;
        reader.countries[country_id].RV += int(temp.val);
        BACK.push_back(temp);
    }

}

void Solver::put(const Solver::Action &temp) {
    int country_id = reader.data[2];
    if (reader.countries[country_id].RV >= int(temp.val)) {
        reader.areas[temp.y][temp.x].field_vaccine += temp.val;
        reader.countries[country_id].RV -= int(temp.val);
        PUT.push_back(temp);
    }
}

// nulladik tickben feltölti a kerületeket és a szomszédságokat
void Solver::district_areas() {
    for (size_t x = 0; x < reader.dimension[1]; x++) {
        for (size_t y = 0; y < reader.dimension[0]; y++) {
            keruletek[reader.areas[y][x].district].insert({y, x});
        }
    }

    for (size_t x = 0; x < reader.dimension[1]; x++) {
        for (size_t y = 0; y < reader.dimension[0]; y++) {
            vector<std::pair<int, int>> neighbours = return_nbs({y, x});
            for (auto nbs : neighbours) {
                if (reader.areas[nbs.first][nbs.second].district != reader.areas[y][x].district) {
                    szomszedsag[reader.areas[y][x].district].insert(reader.areas[nbs.first][nbs.second].district);
                }
            }
        }
    }
}

//tiszta kerületeket összerakja MEG KELL NÉZNI? HOGY JÓ-e !!!!
void Solver::DFS(std::vector<std::unordered_set<int>> &clear_szomszedsag) {
    if (reader.safe_districts.empty()) {
        return;
    }
    bool valtozas = true;
    while (valtozas) {
        valtozas = false;
        for (size_t i = 0; i < szomszedsag.size(); i++) {
            if (reader.safe_districts.find(i) != reader.safe_districts.end()) {

                for (auto j : szomszedsag[i]) {
                    if (reader.safe_districts.find(j) != reader.safe_districts.end()) {

                        auto temp = clear_szomszedsag[i];
                        clear_szomszedsag[i].insert(j);
                        if (!clear_szomszedsag[j].empty()) {
                            for (auto k: clear_szomszedsag[j]) {
                                clear_szomszedsag[i].insert(k);
                            }
                        }
                        if (temp != clear_szomszedsag[i]) { valtozas = true; }
                    }
                }
            }
        }
    }

    vector<unordered_set<int>> temp;
    for (size_t item = 0; item < clear_szomszedsag.size(); item++) {
        if (!clear_szomszedsag[item].empty()) {
            unordered_set<int> t;
            t.insert(item);
            for (auto szomszed: clear_szomszedsag[item]) {
                t.insert(szomszed);
            }
            temp.push_back(t);
        }
    }
    for (auto item = 0; item < temp.size(); item++) {
        for (auto belso_item = item + 1; belso_item < temp.size(); belso_item++) {
            if (temp[belso_item].count(*temp[item].begin()) > 0) {
                temp.erase(temp.begin() + belso_item - 1);
            }
        }
    }

    clean_nbs_history[reader.data[1]] = temp;

}


// kerületekellel élszomsédos területekeket kigyűjti, amik lehetséges vakcinahelyek
void Solver::possibilities(const std::unordered_set<int> &possible_districts,
                           const std::vector<std::unordered_set<int>> &clear_szomszedsag) {

    unordered_set<int> pd;
    for (auto dist: possible_districts) {
        pd.insert(dist);
        for (auto i : clear_szomszedsag[dist]) {
            pd.insert(i);
        }
    }
    for (auto i: pd) {
        for (auto terulet: keruletek[i]) {
            vector<std::pair<int, int>> neighbours = return_nbs({terulet});
            for (auto nbs : neighbours) {
                if (reader.safe_districts.find(reader.areas[nbs.first][nbs.second].district) ==
                    reader.safe_districts.end()) {
                    possible_choice.insert({nbs});
                }
            }
        }
    }
}

// hova lehet tenni vakcinát?
void Solver::from_reserve() {
    possible_choice.clear();
    vaccinated_fields.clear();
    vector<unordered_set<int>> clear_szomszedsag(szomszedsag.size());
    unordered_set<int> possible_districts;
    DFS(clear_szomszedsag);


    // ellenőrzés, hogy van-e valahol vakcina
    for (size_t x = 0; x < reader.dimension[1]; x++) {
        for (size_t y = 0; y < reader.dimension[0]; y++) {
            if (reader.areas[y][x].field_vaccine > 0) {
                //possible.insert({y, x});
                vaccinated_fields.insert({y, x}); //azok a területek ahol van vakcina
            }
            if (reader.areas[y][x].field_vaccine == 0 && reader.areas[y][x].infectionRate > 0) {
                fields_to_vaccinate.insert({y, x}); //fertőzött területek, ahol nincs vakcina
            }
        }
    }
    // Ha nincs egy területen se tartalék vakcinája az országnak
    if (vaccinated_fields.empty()) {
        for (size_t x = 0; x < reader.dimension[1]; x++) {
            for (size_t y = 0; y < reader.dimension[0]; y++) {
                if (x == 0 or y == 0 or x == reader.dimension[1] - 1 or y == reader.dimension[0] - 1) {
                    if (reader.safe_districts.find(reader.areas[y][x].district) == reader.safe_districts.end()) {
                        possible_choice.insert({y, x});
                    } else {
                        possible_districts.insert(reader.areas[y][x].district);
                    }
                }
            }

        }

        possibilities(possible_districts, clear_szomszedsag);

    }
        //Ha van legalább egy területen tartalék vakcinája az országnak
    else {
        cout << " MAR VAN VAKCINAAAA " << endl;
        for (auto i: vaccinated_fields) {
            if(reader.safe_districts.find(reader.areas[i.first][i.second].district) == reader.safe_districts.end()){
                possible_choice.insert(i); // akkor csak ezekre a területekre lehet tenni
            }

            vector<std::pair<int, int>> neighbours = return_nbs(i);
            for (auto nbs : neighbours) {
                if (reader.safe_districts.find(reader.areas[nbs.first][nbs.second].district) ==
                    reader.safe_districts.end()) { // vagy a velük élszomszédos, és nem tiszta kerületű területre.
                    possible_choice.insert({nbs.first, nbs.second});
                }
                    //Ha van egy olyan terület, ahol van az országnak tartalék vakcinája, és az élszomszédos egy olyan területtel, amelynek kerülete tiszta, akkor azon tiszta kerület területeinek élszomszédos területei, amelyek nem tiszta kerülethez tartoznak, oda is tehető vakcina.
                else {
                    possible_districts.insert(reader.areas[nbs.first][nbs.second].district);
                }
            }

        }
        possibilities(possible_districts, clear_szomszedsag);
    }

}


void Solver::upload_nbs() {
    for (size_t y = 0; y < reader.dimension[0]; y++) {
        for (size_t x = 0; x < reader.dimension[1]; x++) {

            if (x == 0) {
                reader.areas[y][x].left = nullptr;
            } else {
                reader.areas[y][x].left = &reader.areas[y][x - 1];
            }
            if (x == (reader.dimension[1] - 1)) {
                reader.areas[y][x].right = nullptr;
            } else {
                reader.areas[y][x].right = &reader.areas[y][x + 1];
            }
            if (y == 0) {
                reader.areas[y][x].up = nullptr;
            } else {
                reader.areas[y][x].up = &reader.areas[y - 1][x];
            }
            if (y == (reader.dimension[0] - 1)) {
                reader.areas[y][x].down = nullptr;
            } else {
                reader.areas[y][x].down = &reader.areas[y + 1][x];
            }
        }
    }
}


vector<pair<int, int>> Solver::return_nbs(const pair<int, int> &koord) {
    vector<pair<int, int>> returner;
    if (reader.areas[koord.first][koord.second].right != nullptr) {
        returner.emplace_back(koord.first, koord.second + 1);
    }
    if (reader.areas[koord.first][koord.second].left != nullptr) {
        returner.emplace_back(koord.first, koord.second - 1);
    }
    if (reader.areas[koord.first][koord.second].up != nullptr) {
        returner.emplace_back(koord.first - 1, koord.second);
    }
    if (reader.areas[koord.first][koord.second].down != nullptr) {
        returner.emplace_back(koord.first + 1, koord.second);
    }
    return returner;
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


    // Ez legyen kikommentelve a beadottban
    /***********************/
//    for (auto& row : reader.areas) {
//        for (auto& area : row) {
//            ss << area.infectionRate << " ";
//        }
//        getline(ss, command);
//        commands.push_back(command);
//        ss.clear();
//    }
    /***********************/

    commands.emplace_back(".");
}

