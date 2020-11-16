#include "Solver.h"

using namespace std;

void Solver::healing() {
    for (size_t x = 0; x < reader.dimension[1]; x++) { // oszlop és sorfolytonos indexelés, ez fontos, mert számít hogy a faktorok melyik iterációban frissülnek
        for (size_t y = 0; y < reader.dimension[0]; y++) {
            if (reader.areas[reader.mat2sub(y,x)].infectionRate > 0 && (y + x < reader.info.curr_tick)) {
                unsigned int h = field_healing(y,x); //ha nincs vakcina alapból ennyi a healing

                ///******************* MÁSODIK FORDULÓ *****************///
                unsigned int IR = infection_rate_history[reader.info.curr_tick - 1][y][x]; //prev InfectionRate
                unsigned int P = reader.areas[reader.mat2sub(y,x)].population; //start_info

                //összes ország tartalék vakcinái
                ///A KÖVI 3 SORT KI KELL KOMMENTELNI, HA VAKCINÁZNI AKARUNK!!!
                unsigned int n = reader.areas[reader.mat2sub(y, x)].field_vaccine;
                //n = reader.sum_of_previous_vaccine_on_areas[y][x];

                unsigned int X = min(n * P, IR); //ennyivel csökken az infection és nő a healthRate vakcinázás után
                int m = ceil(X / P); //ennyivel csökken a tartalék vakcinaszám az adott területen
                //std::cout << "ennyivel fog csokkeni a tartalek vakcinaszam az adott területen: " << m << '\n';

                ///vakcina miatti gyógyulás
                //mi van ha a területen nincs is vakcina?? feladatleírás alapján nem egyértelmű ennek a tesztelése
                if (IR > 0 && n > 0) { //ha előző körben volt fertőzött és vakcina is van -> oltsa be
                    std::cout<< "Nekunk " << reader.areas[reader.mat2sub(y,x)].field_vaccine << " db vakcinank van itt." <<'\n';
                    vaccinated_history[reader.info.curr_tick][y][x] = X; // vakcina által mennyi gyógyulás volt a területen
                    reader.areas[reader.mat2sub(y,x)].healthRate += X;
                    reader.areas[reader.mat2sub(y,x)].infectionRate -= X;
                    std::cout<< "Vakcina miatt gyogyult ennyivel: " << X <<'\n';
                    reader.areas[reader.mat2sub(y,x)].field_vaccine -= m;


                    std::cout<< "A tarcsi vakcinaszam ennyivel csokkent: " << m <<'\n';

                    h = floor(field_healing(y,x) * (IR - X) / IR); //ha van vakcina módosul a visszatérési érték
                } else {
                    vaccinated_history[reader.info.curr_tick][y][x] = 0; //ha nem vakcináztunk
                }
                ///******************* MÁSODIK FORDULÓ *****************///
                healing_history[reader.info.curr_tick][y][x] = h;
                reader.areas[reader.mat2sub(y,x)].healthRate += h;
                reader.areas[reader.mat2sub(y,x)].infectionRate -= h;
            }
        }
    }
}

size_t Solver::field_healing(size_t y, size_t x) {
    unsigned int width = reader.dimension[0];
    unsigned int height = reader.dimension[1];
    unsigned int curr_tick = reader.info.curr_tick;
    unsigned int curr_infection_rate;

    if (width + height < curr_tick) {
        // min. ker. az adott cella múltbeli fertőzöttésgei között width+height tickre visszamenően
        unsigned int min_infection_rate = 100;
        for (unsigned int i = 1; i <= (width + height); i++) {
            curr_infection_rate = infection_rate_history[curr_tick - i][y][x];
            if (curr_infection_rate < min_infection_rate) {
                min_infection_rate = curr_infection_rate;
            }
        }
        unsigned int result = floor(min_infection_rate * (reader.factors[0] % 10) / 20.0);
        update_factor(reader.factors[0]);
        return result;
    } else {
        return 0;
    }
}