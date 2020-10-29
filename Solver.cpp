#include "Solver.h"
#include <iostream>
#include <sstream>
#include <cmath>

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

    if (reader.hasEnd) {
        return vector<string>{"Game over"};          // a main.cpp-ben megszakad a while loop és véget ér a program
    }

    if (!reader.needAnsw) {                          // ha nem kell válasz, üres vectort adunk vissza, amire a while loop tovább lép
        return vector<string>{};
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
    getline(ss, command);
    commands.push_back(command);
    ss.clear();

    for (auto &row : reader.areas) {
        for (auto &area : row) {
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

unsigned int Solver::infection( unsigned int y, unsigned int x) {
    unsigned int avg_plus_sum_infection_rate;
    unsigned int avg_infection_rate;
    unsigned int sum_infection_rate = 0;
    unsigned int curr_tick = reader.data[1];
    /*  ceil((avg(i = [1 .. mini(factor2() % 10 + 10, curr_tick)], infection(curr_tick - i, coord)) +
      sum(c = {coord, neighbours(coord)};
           t = factor3() % 7 + 3,
           tick_info[curr_tick-1, c].infection_rate >   (start_info[coord].district != start_info[c].district ? 2 : coord != c ? 1 : 0) * t ?
             clamp(start_info[coord].population - start_info[c].population, 0, 2) + 1 : 0)

      ) * (factor4() % 25 + 50) / 100.0)
   */
    unsigned int osszeg = 0;
    unsigned int sum = min(reader.factors[1] % 10 + 10, curr_tick);
    cout << " sum " << sum << endl;

    for (unsigned i = 1; i <= sum; i++) {
        //cout << "belepett  " <<  infection_history[curr_tick-i][y][x] << "igy az osszeg   ";
        osszeg += infection_history[curr_tick - i][y][x];
        cout << osszeg << endl;
    }
    avg_infection_rate = osszeg / sum + reader.factors[1] % 10; // tick = 0ra ez nem jó
    cout << "avg_infection_rate   " << avg_infection_rate << endl;
}

