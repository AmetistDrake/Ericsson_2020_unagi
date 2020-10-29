#include "Solver.h"
#include <iostream>
#include <sstream>
#include <cmath>
#include <tuple>

using namespace std;

Solver::Solver() = default;

Solver::~Solver() {
  std::cerr << "END (message): " << reader.message << std::endl;
}

unsigned int Solver::healing(unsigned int y, unsigned int x){
    unsigned int width = reader.dimension[0];
    unsigned int height = reader.dimension[1];
    unsigned int curr_tick = reader.data[1];
    unsigned int curr_infection_rate;

    if(width + height < curr_tick){
        unsigned int min_infection_rate = 100;
        for(unsigned int i=1; i <= (width + height); i++){
            curr_infection_rate = tick_info[curr_tick - i][y][x].infectionRate;
            if(curr_infection_rate < min_infection_rate){
                min_infection_rate = curr_infection_rate;
            }
        }
        return floor(min_infection_rate) * (reader.factors[0] % 10) / 20.0;
    }else
        return 0;
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
  load_tick_info();

  for(size_t y = 0; y < reader.dimension[0]; y++){
      for(size_t x = 0; x < reader.dimension[1]; x++){
          if(reader.areas[y][x].infectionRate > 0){
              healing(y, x);
          }
      }
  }

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

void Solver::load_tick_info() {
    tick_info.push_back(reader.areas);
    //tick_info.resize(data[1], std::vector<std::vector<Area>>{reader.dimension[0],  std::vector<Area>{reader.dimension[1]}});
}
