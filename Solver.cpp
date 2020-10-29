#include "Solver.h"
#include <iostream>
#include <sstream>
#include <cmath>
#include <math.h>
#include <tuple>
//#include <cstdint>
#include <algorithm>
#include <bits/stdc++.h>

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
  load_tick_info();

  for(size_t y = 0; y < reader.dimension[0]; y++){
      for(size_t x = 0; x < reader.dimension[1]; x++){
          if(reader.areas[y][x].infectionRate > 0){
              healing(y, x);
          }
      }
  }
   //vector<vector<unsigned int >> tmp{reader.dimension[0],vector<unsigned int> {reader.dimension[1],0}};
    //infection_history.resize(reader.data[1]+1, vector<vector<unsigned int >> {reader.dimension[0],vector<unsigned int> {reader.dimension[1],0}});
vector<vector<unsigned  int>> tmp2;
    for(unsigned int y = 0; y < reader.dimension[0]; y++){
        vector<unsigned int> tmp;
        for(unsigned int x =0; x < reader.dimension[1]; x++){
           tmp.push_back(0);
        }
        tmp2.push_back(tmp);
    }

    infection_history.push_back(tmp2);
   // cerr << infection_history.size();

/*
    for(auto i:infection_history){
        for(auto j: i){
            for(auto k:j){
                cerr<< k << "  " ;
            }
            cerr << endl;
        }
        cerr << endl << endl;
    }
*/



    for(size_t y =0; y < reader.dimension[0]; y++){
        for (size_t x =0; x < reader.dimension[1]; x++){

            if (reader.data[1] == 0) {
                if (reader.areas[y][x].infectionRate > 0) {
                    infection_history[0][y][x] =1;
                }
            }
            else{
            infection(y,x);
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
unsigned int Solver::infection( unsigned int y, unsigned int x) {
    unsigned int avg_plus_sum_infection_rate;
    unsigned int avg_infection_rate;
    unsigned int sum_infection_rate =0;
    unsigned  int curr_tick = reader.data[1];
    /*  ceil((avg(i = [1 .. mini(factor2() % 10 + 10, curr_tick)], infection(curr_tick - i, coord)) +
      sum(c = {coord, neighbours(coord)};
           t = factor3() % 7 + 3,
           tick_info[curr_tick-1, c].infection_rate >   (start_info[coord].district != start_info[c].district ? 2 : coord != c ? 1 : 0) * t ?
             clamp(start_info[coord].population - start_info[c].population, 0, 2) + 1 : 0)

      ) * (factor4() % 25 + 50) / 100.0)
   */
    unsigned int osszeg = 0;
    unsigned  int sum = min(reader.factors[1] % 10 + 10, curr_tick);
    cout << " sum " << sum << endl;

    for (unsigned i = 1; i <= sum ;i++){
        //cout << "belepett  " <<  infection_history[curr_tick-i][y][x] << "igy az osszeg   ";
    osszeg+= infection_history[curr_tick-i][y][x];
    cout << osszeg << endl;
   }
    avg_infection_rate = osszeg /sum + reader.factors[1] % 10 ; // tick = 0ra ez nem jó
     cout << "avg_infection_rate   " << avg_infection_rate << endl;



    vector<std::pair< int, int>> neighbours{{0,0},{-1,0}, {0,-1},{1,0},{0,1}};

    for(auto n:neighbours) {
        pair<int, int> c {y-n.first,x-n.second};
        cout <<  "a meret elvben " << reader.dimension[0] << "  x  " << reader.dimension[1] << endl;
       cout << "szomszed: " << n.first << n.second  << "   "<< c.first << c.second << endl ;

        if(!(0 <= c.first  and  c.first < reader.dimension[0] and
             0 <= c.second  and  c.second <reader.dimension[1] )) continue;// lehet fel van cserélve
         //   cout  << "BELEFER" << endl ;
        unsigned int t = reader.factors[2] % 7 + 3;
        unsigned int a;
        if (tick_info[0][y][x].district != tick_info[0][c.first][c.second].district) { a = 2; }
        else if (y != c.first and x != c.second) { a = 1; }
        else { a = 0; }
       cerr <<  " a  " << a  << "   " <<tick_info[curr_tick - 1][c.first][c.second].infectionRate << endl;
        if (tick_info[curr_tick - 1][c.first][c.second].infectionRate > t * a) {
         //   cout << "NAGYOBB" << endl;
            sum_infection_rate += clamp(int(tick_info[0][y][x].population - tick_info[0][c.first][c.second].population), 0, 2) + 1;
        } else {
            sum_infection_rate += 0;
        }

    }

    avg_plus_sum_infection_rate = avg_infection_rate + sum_infection_rate;
   // cout << "elvben az osszeg   " << avg_plus_sum_infection_rate << endl;
    unsigned int solution = ceil(avg_plus_sum_infection_rate * ((reader.factors[3] % 25 )+50) / 100.0);
   // cout << "CURR_TICK  " << curr_tick << endl;
    infection_history[curr_tick][y][x] = solution;
    return solution;

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