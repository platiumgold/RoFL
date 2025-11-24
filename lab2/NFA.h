#ifndef ROFL_NFA_H
#define ROFL_NFA_H

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <set>

using NFATransitionMap = std::unordered_map<std::string, std::unordered_map<std::string, std::unordered_set<std::string>>>;

class NFA {
public:
    std::unordered_set<std::string> states {};
    NFATransitionMap transitions {};
    std::string start_state{};
    std::set<std::string> end_states{};

    NFA() = default;
    NFA(const std::string& start, const std::string& trans, const std::string& end);

    void AddState(const std::string& state);
    void AddTransition(const std::string& from, const std::string& trans, const std::string& to);

    void MergeTransitions(const NFATransitionMap& other_transitions);
    void Union(const NFA& nfa2, const std::string& new_start, const std::string& new_end);
    void Concat(const NFA& nfa2);
    void Kleene(const std::string& new_start, const std::string& new_end);
    bool IsInLanguage(const std::string& word);
    std::string ToDot();
};

NFA PolishToThompson(const std::string& tokens);

std::set<std::string> EpsilonClosure(const NFA& nfa, const std::set<std::string>& states);
std::set<std::string> Move(const NFA& nfa, const std::set<std::string>& states, std::string symbol);

#endif //ROFL_NFA_H
