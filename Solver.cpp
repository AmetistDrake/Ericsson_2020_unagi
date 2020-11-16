#include "Solver.h"

using namespace std;

Solver::Solver() = default;

Solver::~Solver() {
    std::cerr << "END (message): " << reader.message << std::endl;
}

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

    PUT.clear();
    BACK.clear();
    vector<vector<size_t>> tmp(reader.dimension[0], vector<size_t>(reader.dimension[1], 0));

    // 1) vakcina elhelyezés, csoportosítás
    move_vaccine();

    // 2) healing - fertőzöttek gyógyulása
    healing_history.push_back(tmp);
    vaccinated_history.push_back(tmp);
    healing();

    // 3) megtisztítottról visszekerül az országraktárba
    cleaned_back();

    // 4) infection - vírus terjed
    infection_history.push_back(tmp);
    infection();


    // 5) vakcinagyártás
    vaccine_production();

    // history-k feltöltése
    health_rate_history.push_back(tmp);
    infection_rate_history.push_back(tmp);
    for (size_t x = 0; x < reader.dimension[1]; x++) {
        for (size_t y = 0; y < reader.dimension[0]; y++) {
            health_rate_history[reader.info.curr_tick][y][x] = reader.areas[y][x].healthRate;
            infection_rate_history[reader.info.curr_tick][y][x] = reader.areas[y][x].infectionRate;
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

void Solver::update_factor(uint32_t& factor) {
    factor = uint64_t(factor) * uint64_t(48271) % uint64_t(0x7fffffff);
}

vector<pair<size_t, size_t>> Solver::get_nbs(size_t y, size_t x) {
    vector<pair<size_t, size_t>> nbs;
    vector<pair<size_t, size_t>> shifts {{-1,0},{1,0},{0,-1},{0,1}};
    for (auto s : shifts) {
        pair<size_t, size_t> nb = {y + s.first, x + s.second};
        if (nb.first < reader.dimension[0] && nb.first >= 0 &&
                nb.second < reader.dimension[1] && nb.second >= 0){
            nbs.push_back(nb);
        }
    }

    return nbs;
}

//vissza a központba
void Solver::back(const Solver::Action &temp) {
    int country_id = reader.info.country_id;
    if (reader.areas[temp.y][temp.x].field_vaccine - temp.val >= 1) {
        reader.areas[temp.y][temp.x].field_vaccine -= temp.val;
        reader.countries[country_id].RV += int(temp.val);
        BACK.push_back(temp);
    }
}

void Solver::put(const Solver::Action &temp) {
    int country_id = reader.info.country_id;
    if (reader.countries[country_id].RV >= int(temp.val)) {
        reader.areas[temp.y][temp.x].field_vaccine += temp.val;
        reader.countries[country_id].RV -= int(temp.val);
        PUT.push_back(temp);
    }
}

void Solver::answer_msg(vector<std::string> &commands) {
    stringstream ss;
    string command;

    ss << "RES " << reader.info.game_id << " " << reader.info.curr_tick << " " << reader.info.country_id << std::endl;
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

