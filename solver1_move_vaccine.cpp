#include "Solver.h"

using namespace std;

void Solver::move_vaccine() {
    vector<pair<size_t, size_t>> infected_fields = get_infected_fields();
    vector<pair<size_t, size_t>> starting_fields = get_starting_fields();
    vector<pair<size_t, size_t>> selected = where_to_put(starting_fields, infected_fields);
    while (!selected.empty() && reader.countries[reader.info.country_id].RV > 0) {
        pair<size_t, size_t> curr = selected.back();
        selected.pop_back();
        size_t amount = vaccine_amount(curr);
        if (amount != 0) {
            put(Action(curr.first, curr.second, amount));
        }
    }
}

size_t Solver::vaccine_amount (const pair<size_t,size_t>& selected) {
    if (reader.areas[selected.first][selected.second].infectionRate > 0) { //ha nem tiszta terület
        if (reader.areas[selected.first][selected.second].field_vaccine > 0) { // ha a területen már van vakcina
            int val = reader.areas[selected.first][selected.second].infectionRate;
            if (reader.countries[0].RV < val) {
                val = reader.countries[0].RV;
            }
            return val;
        }
        else{// ha még nincs a területen vakcina
            int val = reader.areas[selected.first][selected.second].infectionRate;
            if ( val< (6-reader.areas[selected.first][selected.second].population)) {
                val = 6-reader.areas[selected.first][selected.second].population; // kell a minimum!
                if (reader.countries[0].RV < val) {
                    return 0;
                }
                else{
                    return val;
                }
            }
        }
    }
    else{// ha a terület tiszta
        int val = 6-reader.areas[selected.first][selected.second].population;
        if (reader.countries[0].RV < val) {
            return 0;
        }
        else{
            return val;
        }
    }
}


vector<pair<size_t, size_t>> Solver::get_starting_fields (){
    vector<pair<size_t,size_t>> from;
    vector<pair<size_t,size_t>> null_from;
    vector<pair<size_t,size_t>> poten;
    // int sum_of_field_vaccine = 0;
    for (size_t x = 0; x < reader.dimension[1]; x++) { // oszlop és sorfolytonos indexelés, ez fontos, mert számít hogy a faktorok melyik iterációban frissülnek
        for (size_t y = 0; y < reader.dimension[0]; y++) {
            if(reader.areas[y][x].field_vaccine >0){
                poten.emplace_back(y,x);
            }
            if(y == 0 or x ==0 or y ==  reader.dimension[0]-1 or x== reader.dimension[1]-1){
                if(reader.safe_districts.find(reader.areas[y][x].district) == reader.safe_districts.end()){
                    null_from.emplace_back(y,x);
                }
            }
        }
    }
    if(from.empty()){
        return null_from;
    }
    else{
        from = poten;
        for(auto f:poten){
            vector<pair<size_t, size_t>> nbs = get_nbs(f.first, f.second);
            for(auto nb:nbs){
                if (reader.safe_districts.find(reader.areas[nb.first][nb.second].district) == reader.safe_districts.end()){
                    from.emplace_back(nb.first,nb.second);
                }
            }
        }
        return from;
    }
}

vector<pair<size_t, size_t>> Solver::get_infected_fields() {
    vector<pair<size_t, size_t>> coords;
    for (size_t x = 0; x < reader.dimension[1]; x++) {
        for (size_t y = 0; y < reader.dimension[0]; y++) {
            if (reader.areas[y][x].infectionRate > 0) {
                coords.emplace_back(y,x);
            }
        }
    }
    return coords;
}

vector<pair<size_t, size_t>> Solver::where_to_put (const vector<pair<size_t,size_t>>& from1, const vector<pair<size_t,size_t>>& to1){

    vector<pair<size_t, size_t>> from;
    vector<pair<size_t, size_t>> to;

    for(auto f : from1) {
        cout << "(" << f.first << "," << f.second << ")";
    }
    cout << endl;
    for(auto f : to1) {
        cout << "(" << f.first << "," << f.second << ")";
    }
    cout << endl;

    from.push_back({23,21}); // Valamiért y = 24 és x = 22 tönkreteszi. Dijkstra sem biztosan jó.
    to.push_back({17, 14});


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
                            q.push(Node_dist(nb, table[nb].dist));
                        }
                    }
                }
            }
        }
//        for (const auto& m : table) {
//            cout << "(" << m.first.first << "," << m.first.second<<")" << " " << m.second.dist << " " <<"(" << m.second.prev_visited.first << "," << m.second.prev_visited.second <<")" << " " << m.second.is_visited << endl;
//        }

        for (const auto& coord : to) {
            pair<size_t, size_t> curr_node = coord;
            if (table[coord].dist == 0) {
                possible_coords.push(Node_dist(coord, 0));
                continue;
            }
            cout << "(" << curr_node.first << "," << curr_node.second << ")";
            while (table[curr_node].prev_visited != f)
            {
                curr_node = table[curr_node].prev_visited;
                cout << "(" << curr_node.first << "," << curr_node.second << ")";
            }
            possible_coords.push({curr_node, table[coord].dist});
            cout << table[coord].dist << endl;
        }
    }
    cout << "(" << possible_coords.top().node.first << "," << possible_coords.top().node.second << ") " << possible_coords.top().dist << endl;

    vector<pair<size_t,size_t>> selected;
    while (!possible_coords.empty()) {
        // cout << "(" << possible_coords.top().node.first << "," << possible_coords.top().node.second << ")";
        pair<size_t, size_t> tmp = possible_coords.top().node;
        possible_coords.pop();
        if (find(selected.begin(), selected.end(), tmp) == selected.end()) {
            selected.push_back(tmp);
        }
    }
    reverse(selected.begin(), selected.end());
    for (auto s : selected) {
        cout << "(" << s.first << "," << s.second << ")";
    }
    return selected;
}