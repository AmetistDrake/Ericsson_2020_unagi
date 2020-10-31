#ifndef SOCKET_CONNECTOR_SOLVER_H
#define SOCKET_CONNECTOR_SOLVER_H

#include <iostream>
#include <vector>
#include "Reader.h"
#include <set>


class Solver {
private:
  Reader reader;

  std::vector<std::vector<std::vector<Area>>> tick_info;
  std::vector<std::vector<std::vector<unsigned int>>> infection_history;
  std::vector<std::vector<unsigned int>> heal;
  std::set<unsigned int> not_clean_districts;

  void load_tick_info();
  unsigned int healing(unsigned int y, unsigned int x);
  unsigned int infection(unsigned int y,  unsigned int x);
  void update_factor(uint32_t&);
  void load_clean_districts();


public:
  Solver();
  ~Solver();

  std::vector<std::string> process(const std::vector<std::string>& infos);
};


#endif //SOCKET_CONNECTOR_SOLVER_H
