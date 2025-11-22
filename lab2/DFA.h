#ifndef ROFL_DFA_H
#define ROFL_DFA_H

#include <string>
#include <unordered_map>
#include <unordered_set>
#include "NFA.h"

using DFATransitionMap = std::unordered_map<std::string, std::unordered_map<std::string, std::string>>;

class DFA {
public:
    std::unordered_set<std::string> states {};
    DFATransitionMap transitions {};
    std::string start_state{};
    std::unordered_set<std::string> end_states{};

    DFA() = default;

    void add_state(const std::string& state, bool is_final = false, bool is_start = false);
    void add_transition(const std::string& from, const std::string& symbol, const std::string& to);

    std::string ToDot();
};

DFA NFAtoDFA(NFA& nfa);

DFA minimize_DFA(DFA& dfa);

#endif //ROFL_DFA_H
