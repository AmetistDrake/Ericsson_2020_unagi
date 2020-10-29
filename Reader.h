#ifndef UNAGI_READER_H
#define UNAGI_READER_H

#include <iostream>
#include <array>
#include <vector>
#include <sstream>

struct Area {
  unsigned int district;
  unsigned int infectionRate;
  unsigned int healthRate;
  unsigned int population;
};

class Reader {
private:
  bool readHelperFunc(std::string& line);
public:
  std::array<unsigned int, 3> data {};
  std::array<std::uint32_t, 4> factors {};
  std::array<unsigned short, 2> dimension {};
  std::vector<std::vector<Area>> areas {};
  std::string message {};
  bool hasEnd = false;
  bool needAnsw = true;

  Reader() = default;
  void readDataConsole();
  void readDataInfo(const std::vector<std::string>& infos);
};

#endif //UNAGI_READER_H
