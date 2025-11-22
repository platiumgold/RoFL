#include <vector>
#include <map>
#include <set>
#include <stack>
#include <algorithm>
#include <numeric>
#include "DFA.h"


void DFA::add_state(const std::string& state, bool is_final, bool is_start) {
    states.insert(state);
    if (is_final) {
        end_states.insert(state);
    }
    if (is_start) {
        start_state = state;
    }
}

void DFA::add_transition(const std::string& from, const std::string& symbol, const std::string& to) {
    transitions[from][symbol] = to;
}

std::string DFA::ToDot() {
    std::string result {"digraph DFA {\n"
                        "  rankdir=LR;\n"
                        "  node [shape = circle];\n"};
    for (const auto& state : states) {
        if (end_states.find(state) != end_states.end()) {
            result += "  " + state + "[shape=doublecircle];\n";
        } else {
            result += "  " + state + ";\n";
        }
    }
    for (const auto& from_pair : transitions) {
        const std::string &from = from_pair.first;
        for (const auto &symbol_pair: from_pair.second) {
            const std::string &symbol = symbol_pair.first;
            const std::string &to = symbol_pair.second;
            result += "  " + from + "->" + to + "[label=" + symbol + "];\n";
        }
    }
    return result + "}";
}

DFA NFAtoDFA(NFA& nfa) {
    std::map<std::set<std::string>, std::string> set_to_name{};
    int state_counter{0};
    std::stack<std::set<std::string>> stack{};
    DFA dfa{};

    auto start_closure = epsilon_closure(nfa, {nfa.start_state});
    set_to_name[start_closure] = std::to_string(state_counter++);

    bool is_final{false};
    if (start_closure.find(nfa.end_state) != start_closure.end()) {
        is_final = true;
    }

    dfa.add_state(set_to_name[start_closure], is_final, true);
    stack.push(start_closure);

    while(!stack.empty()) {
        auto state = stack.top();
        stack.pop();
        for (auto ch : "abc") {
            std::string sch(1, ch);
            auto move_set = move(nfa, state, sch);
            if (move_set.empty()) {
                continue;
            }
            auto eps_closure = epsilon_closure(nfa, move_set);
            if (!set_to_name.count(eps_closure)) {
                set_to_name[eps_closure] = std::to_string(state_counter++);
                bool is_final{false};
                if (eps_closure.count(nfa.end_state)) {
                    is_final = true;
                }
                dfa.add_state(set_to_name[eps_closure], is_final);
                stack.push(eps_closure);
            }
            dfa.add_transition(set_to_name[state], sch, set_to_name[eps_closure]);
        }
    }
    return dfa;
}

class DSU {
private:
    std::vector<int> parent;
public:
    DSU(int n) {
        parent.resize(n);
        std::iota(parent.begin(), parent.end(), 0);
    }

    int find(int i) {
        if (parent[i] == i) {
            return i;
        }
        return parent[i] = find(parent[i]);
    }

    void unite(int i, int j) {
        int root_i = find(i);
        int root_j = find(j);
        if (root_i != root_j) {
            parent[root_i] = root_j;
        }
    }
};

DFA minimize_DFA(DFA& dfa) {
    int n = dfa.states.size();
    std::vector<std::vector<int>> table(n, std::vector<int>(n, 0));
    for(int i=0; i<n; ++i) {
        for(int j=0; j<i; ++j) {
            if (dfa.end_states.count(std::to_string(i)) != dfa.end_states.count(std::to_string(j))) {
                table[i][j] = 1;
            }
        }
    }

    bool is_min{false};
    while(!is_min) {
        is_min = true;
        for(int i=0; i<n; ++i) {
            for(int j=0; j<i; ++j) {
                if (table[i][j] == 1) {
                    continue;
                }
                for (auto ch : "abc") {
                    std::string sch(1, ch);

                    auto it_i = dfa.transitions.find(std::to_string(i));
                    if (it_i == dfa.transitions.end() || it_i->second.find(sch) == it_i->second.end()) {
                        continue;
                    }
                    std::string qi_ = dfa.transitions[std::to_string(i)][sch];
                    std::string qj_ = dfa.transitions[std::to_string(j)][sch];

                    int mi = std::min(std::stoi(qi_), std::stoi(qj_));
                    int ma = std::max(std::stoi(qi_), std::stoi(qj_));
                    if (table[ma][mi]) {
                        table[i][j] = 1;
                        is_min = false;
                    }
                }
            }
        }
    }

    DFA min_dfa{};
    DSU dsu(n);
    for(int i=0; i<n; ++i) {
        for (int j = 0; j < i; ++j) {
            if (!table[i][j]) {
                dsu.unite(i, j);
            }
        }
    }

    min_dfa.start_state = std::to_string(dsu.find(std::stoi(dfa.start_state)));
    for (const auto& state : dfa.states) {
        bool is_final{false};
        if (dfa.end_states.count(state)) {
            is_final = true;
        }
        std::string new_state = std::to_string(dsu.find(std::stoi(state)));
        min_dfa.add_state(new_state, is_final);
    }

    for (const auto& from_pair : dfa.transitions) {
        std::string from = std::to_string(dsu.find(std::stoi(from_pair.first)));
        for (const auto& symbol_pair : from_pair.second) {
            std::string symbol = symbol_pair.first;
            std::string to = std::to_string(dsu.find(std::stoi(symbol_pair.second)));
            min_dfa.add_transition(from, symbol, to);
        }
    }
    return min_dfa;
}