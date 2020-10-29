#ifndef SOCKET_CONNECTOR_SOLVER_H
#define SOCKET_CONNECTOR_SOLVER_H

#include <iostream>
#include <vector>
#include "Reader.h"

class Solver {
private:
  Reader reader;
  std::vector<std::vector<std::vector<Area>>> tick_info;
  void load_tick_info();
  unsigned int healing(unsigned int y, unsigned int x);

public:
  Solver();
  ~Solver();

  std::vector<std::string> process(const std::vector<std::string>& infos);
};


#endif //SOCKET_CONNECTOR_SOLVER_H
