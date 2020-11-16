#include "Solver.h"

using namespace std;

void Solver::move_vaccine() {
    vector<pair<size_t,size_t>> from;
    vector<pair<size_t,size_t>> null_from;
    vector<pair<size_t,size_t>> poten;
     int sum_of_field_vaccine = 0;
    for (size_t x = 0; x < reader.dimension[1]; x++) { // oszlop és sorfolytonos indexelés, ez fontos, mert számít hogy a faktorok melyik iterációban frissülnek
        for (size_t y = 0; y < reader.dimension[0]; y++) {
            if(reader.areas[y][x].field_vaccine >0){
                poten.push_back({y,x});
            }
            if(y == 0 or x ==0 or y ==  reader.dimension[0]-1 or x== reader.dimension[1]-1){
                if(reader.safe_districts.find(reader.areas[y][x].district) == reader.safe_districts.end()){
                    null_from.push_back({y,x});
                }
            }
        }
    }
    if(from.empty()){
     //null_fromra kell meghívni
    }
    else{
        from = poten;
     for(auto f:poten){
         vector<pair<size_t, size_t>> nbs = get_nbs(f.first, f.second);
         for(auto nb:nbs){
             if (reader.safe_districts.find(reader.areas[nb.first][nb.second].district) == reader.safe_districts.end()){
                 from.push_back( {nb.first,nb.second});
             }
         }
     }
     // fromra kell meghívni
    }



}

pair<size_t, size_t> Solver::where_to_put (const vector<pair<size_t,size_t>>& from, const vector<pair<size_t,size_t>>& to){
    struct Props {
        size_t dist = std::numeric_limits<size_t>::max();
        pair<size_t,size_t> prev_visited {};
        bool is_visited = false;
    };

    struct Node_dist {
        pair<size_t, size_t> node;
        size_t dist {};

        Node_dist() = default;
        Node_dist(pair<size_t, size_t> n, size_t d) : node(std::move(n)), dist(d) {};
        bool operator <(const Node_dist& other) const {
            return other.dist < dist; // azért van fordítva, hogy a queue csökkenőbe rendezzen, és a top() a legkisebb elem legyen
        }
    };

    struct pair_hash {
        inline std::size_t operator()(const pair<size_t, size_t>& coord) const {
            return coord.first*31+coord.second;
        }
    };

    unordered_map<pair<size_t,size_t>, Props, pair_hash> table;
    priority_queue<Node_dist> q;

    priority_queue<Node_dist> possible_coords;

    for (auto f: from) {
        for (size_t i = 0; i < reader.areas.size(); i++) {
            for (size_t j = 0; j < reader.areas[i].size(); j++) {
                pair<size_t, size_t> coord {i, j}; // koordináták oszlopfolytonosan
                Props props;
                table[coord] = props;

                if (coord == f) {
                    table[coord] = {0, {}, false};
                    q.push(Node_dist(coord, 0));
                }
            }
        }

        Node_dist curr;
        while(!q.empty()) {
            curr = q.top();
            q.pop();
            table[curr.node].is_visited = true;
            vector<pair<size_t,size_t>> shifts {{-1,0},{1,0},{0,-1},{0,1}};

            for (auto shift : shifts) {
                pair<size_t, size_t> nb {curr.node.first+shift.first, curr.node.second+shift.second};
                if (nb.first >= 0 && nb.first < reader.areas[0].size()
                    && nb.second >= 0 && nb.second < reader.areas.size()) {
                    if (!table[nb].is_visited) {
                        if (curr.dist + reader.areas[nb.second][nb.first].population < table[nb].dist) {
                            table[nb].prev_visited = curr.node;
                            table[nb].dist = curr.dist + reader.areas[nb.second][nb.first].population;
                        }
                        q.push(Node_dist(nb, table[nb].dist));
                    }
                }
            }
        }
//        for (const auto& m : table) {
//            cout << "(" << m.first.first << "," << m.first.second<<")" << " " << m.second.dist << " " <<"(" << m.second.prev_visited.first << "," << m.second.prev_visited.second <<")" << " " << m.second.is_visited << endl;
//        }

        for (const auto& coord : to) {
            pair<size_t, size_t> curr_node = coord;
            while (table[curr_node].prev_visited != f)
            {
                curr_node = table[curr_node].prev_visited;
            }
            possible_coords.push({curr_node, table[coord].dist});
            // cout << curr_node.first << "," << curr_node.second << " " << table[coord].dist << endl;
        }
    }

    // cout << "(" << possible_coords.top().node.first << "," << possible_coords.top().node.second << ") " << possible_coords.top().dist << endl;
    return possible_coords.top().node;
}