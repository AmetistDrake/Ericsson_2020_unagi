#ifndef SOCKET_CONNECTOR_SOLVER_H
#define SOCKET_CONNECTOR_SOLVER_H

#include <iostream>
#include <vector>
#include "Reader.h"

class Solver {
private:
  Reader reader;
public:
  Solver();
  ~Solver();

  std::vector<std::string> process(const std::vector<std::string>& infos);
};


#endif //SOCKET_CONNECTOR_SOLVER_H
