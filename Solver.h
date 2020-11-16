#ifndef SOCKET_CONNECTOR_SOLVER_H
#define SOCKET_CONNECTOR_SOLVER_H

#include "Reader.h"
#include <iostream>
#include <vector>
#include <fstream>
#include <filesystem>
#include <queue>
#include <sstream>
#include <cmath>
#include <algorithm>

using namespace std;

class Solver {
private:
    Reader reader;
    struct Action {
        size_t y,x,val;
    };
    
    vector<vector<vector<size_t>>> infection_history;
    vector<vector<vector<size_t>>> healing_history; // magától mennyi gyógyulás volt a területen
    vector<vector<vector<size_t>>> infection_rate_history;
    vector<vector<vector<size_t>>> health_rate_history;
    vector<vector<vector<size_t>>> vaccinated_history;  //vakcina által mennyi gyógyulás volt a területen
    vector<vector<vector<size_t>>> field_vaccine_history;  //mennyi vakcinát tettünk le a területre
    vector<vector<vector<size_t>>> clean_groups_history; // tick, group, district
    vector<vector<string>> msg_history; // a kiírt válaszunk körönként
    vector<int> TPC_history;
    vector<int> RV_history;

    vector<Action> BACK;
    vector<Action> PUT;

    // 1)
    void move_vaccine();
        pair<size_t, size_t> where_to_put(const vector<pair<size_t,size_t>>& from, const vector<pair<size_t,size_t>>& to);

    // 2)
    void healing();
        size_t field_healing(size_t, size_t);

    // 3)
    void cleaned_back();

    // 4)
    void infection();
        size_t field_infection(size_t y, size_t x);

    // 5)
    void vaccine_production();
    
    static void update_factor(uint32_t&);
    vector<pair<size_t,size_t>> get_nbs(size_t, size_t);
    
    
    void back(const Action &);
    void put(const Action &);
    void answer_msg(vector<string>&);

public:
    Solver();
    ~Solver();
    vector<string> process(const vector<string>& infos);
    void create_json_from_data();
};


#endif //SOCKET_CONNECTOR_SOLVER_H
