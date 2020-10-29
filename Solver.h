#ifndef SOCKET_CONNECTOR_SOLVER_H
#define SOCKET_CONNECTOR_SOLVER_H

#include <iostream>
#include <vector>
#include "Reader.h"

class Solver {
private:
  Reader reader;
  std::vector<std::vector<std::vector<Area>>> tick_info;
  std::vector<std::vector<std::vector<unsigned int >>> infection_history;
  void load_tick_info();
  unsigned int healing(unsigned int y, unsigned int x);
  unsigned int infection(unsigned int y,  unsigned int x);
  void update_factors();

public:
  Solver();
  ~Solver();

  std::vector<std::string> process(const std::vector<std::string>& infos);
};


#endif //SOCKET_CONNECTOR_SOLVER_H
