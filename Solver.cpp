#include "Solver.h"
#include <iostream>
#include <sstream>

using namespace std;

Solver::Solver() = default;

Solver::~Solver() {
  std::cerr << "END (message): " << reader.message << std::endl;
}

vector<string> Solver::process(const vector<string>& infos) {
  vector<string> commands;
  if (infos.size() == 1 && infos[0] == "unagi") { // amikor a consolon keresztül kommunikál a program
    reader.readDataConsole();
  }
  else {                                          // amikor hálózaton keresztül kommunikál a program
    reader.readDataInfo(infos);
  }

  if (reader.hasEnd){
    return vector<string> {"Game over"};          // a main.cpp-ben megszakad a while loop és véget ér a program
  }

  if (!reader.needAnsw){                          // ha nem kell válasz, üres vectort adunk vissza, amire a while loop tovább lép
    return vector<string> {};
  }

  //reader.transform();

  // Válasz
  stringstream ss;
  string command;
  ss << "RES " << reader.data[0] << " " << reader.data[1] << " " << reader.data[2] << std::endl;
  getline(ss,command);
  commands.push_back(command);
  ss.clear();

  for (auto& row : reader.areas) {
    for (auto& area : row) {
      ss << area.infectionRate << " ";
    }
    getline(ss, command);
    commands.push_back(command);
    ss.clear();
  }

  return commands;
}