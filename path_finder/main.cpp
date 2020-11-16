#include <iostream>
#include <vector>
#include <queue>
#include <unordered_map>

using namespace std;

struct Area {
    size_t population;
};

struct Reader {
    vector<vector<Area>> areas;
};

vector<pair<size_t, size_t>> where_to_put (const Reader& reader, const vector<pair<size_t,size_t>>& from, const vector<pair<size_t,size_t>>& to){
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
            cout << "(" << coord.first << "," << coord.second << ")";
            cout << table[coord].dist << endl;
            possible_coords.push({curr_node, table[coord].dist});
        }
    }
    // cout << "(" << possible_coords.top().node.first << "," << possible_coords.top().node.second << ") " << possible_coords.top().dist << endl;

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

int main() {
    vector<vector<size_t>> populations {{5,4,3,3,3,3,4,5,4},{5,4,3,2,1,3,3,5,4},{5,4,3,3,2,3,4,4,4},{5,3,1,2,4,5,4,5,5},{5,3,2,2,4,5,5,5,3}};
    Reader reader;
    reader.areas.resize(populations.size(),vector<Area> (populations[0].size()));
    for (size_t i = 0;i<populations.size();i++){
        for (size_t j = 0; j<populations[i].size();j++){
            reader.areas[i][j].population = populations[i][j];
        }
    }
    for (const auto& r : reader.areas) {
        for (auto rr : r) {
            cout << rr.population<<" ";
        }
        cout << endl;
    }

    vector<pair<size_t, size_t>> from {{3,2}, {2, 5}, {0,0}, {-1,-1}};
    vector<pair<size_t, size_t>> to {{2,1},{0,3}};
    where_to_put(reader, from, to);

    return 0;
}
