#ifndef SOCKET_CONNECTOR_SOLVER_H
#define SOCKET_CONNECTOR_SOLVER_H

#include <iostream>
#include <vector>
#include "Reader.h"
#include <unordered_set>
#include <fstream>
#include <filesystem>

struct pair_hash {
  inline std::size_t operator()(const std::pair<int,int> & v) const {
    return v.first*31+v.second;
  }
};

class Solver {
private:
    Reader reader;
    struct Action {
        unsigned int y,x,val;
    };

    std::vector<std::vector<std::vector<unsigned int>>> infection_history;
    std::vector<std::vector<std::vector<unsigned int>>> healing_history; // magától mennyi gyógyulás volt a területen
    std::vector<std::vector<std::vector<unsigned int>>> infection_rate_history;
    std::vector<std::vector<std::vector<unsigned int>>> health_rate_history;
    std::vector<std::vector<std::vector<unsigned int>>> vaccinated_history;  // vakcina által mennyi gyógyulás volt a területen
    std::vector<std::vector<std::vector<unsigned int>>> field_vaccine_history;  // vakcina által mennyi gyógyulás volt a területen
    std::vector<std::vector<std::unordered_set<int>>> clean_nbs_history;
    std::vector<std::vector<std::string>> msg_history; // a kiírt válaszunk körönként
    std::vector<int> TPC_history;
    std::vector<int> RV_history;
    int TPC_0;


    std::vector<std::unordered_set<std::pair<int, int>, pair_hash>> keruletek;
    std::vector<std::unordered_set<int>> szomszedsag;

    std::unordered_set<unsigned int> infected_districts;
    std::vector<Action> BACK;
    std::vector<Action> PUT;

    unsigned int healing(unsigned int y, unsigned int x);
    unsigned int infection(unsigned int y, unsigned int x);
    static void update_factor(uint32_t&);
    void update_infected_districts();
    void cleaned_back(); // 3mas pont a második fordulóban
    void answer_msg(std::vector<std::string>&);
    void vaccine_production();
    void back(const Action &temp);
    void put(const Action &temp);
    std::unordered_set<std::pair<int, int>, pair_hash> from_reserve();
    void district_areas();
    void DFS(std::vector<std::unordered_set<int>> &clear_szomszedsag);
    void possibilities(std::unordered_set<std::pair<int, int>,pair_hash> &possible_choice, const std::unordered_set<int> &possible_districts, const std::vector<std::unordered_set<int>> &clear_szomszedsag);
public:
    Solver();
    ~Solver();
    std::vector<std::string> process(const std::vector<std::string>& infos);
    void create_json_from_data();
};


#endif //SOCKET_CONNECTOR_SOLVER_H
