#ifndef SOCKET_CONNECTOR_SOLVER_H
#define SOCKET_CONNECTOR_SOLVER_H

#include <iostream>
#include <vector>
#include "Reader.h"
#include <unordered_set>
#include <fstream>
#include <filesystem>
# include <set>
class Solver {
private:
    Reader reader;
    struct Action {
        unsigned int y,x,val;
    };

    std::vector<std::vector<std::vector<unsigned int>>> infection_history;
    std::vector<std::vector<std::vector<unsigned int>>> healing_history;
    std::vector<std::vector<std::vector<unsigned int>>> infection_rate_history;
    std::vector<std::vector<std::vector<unsigned int>>> health_rate_history;
    std::vector<std::vector<std::vector<unsigned int>>> vaccine_history;
    std::vector<std::vector<std::vector<std::vector<std::string>>>> msg_history;
    std::vector<std::set<std::pair<int, int>>> keruletek;
    std::vector<std::set<int>> szomszedsag;

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
    std::set<std::pair<int, int>> from_reserve();
    void district_areas();
    void DFS(std::vector<std::set<int>> &clear_szomszedsag);
    void possibilities(std::set<std::pair<int, int>> &possible_choice, const std::set<int> &possible_districts, const std::vector<std::set<int>> &clear_szomszedsag);
public:
    Solver();
    ~Solver();
    std::vector<std::string> process(const std::vector<std::string>& infos);
    void create_json_from_data();
};


#endif //SOCKET_CONNECTOR_SOLVER_H
