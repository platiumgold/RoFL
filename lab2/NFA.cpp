#include "NFA.h"
#include <vector>
#include <stack>
#include <cctype>

NFA::NFA(const std::string& start, const std::string& trans, const std::string& end) : start_state(start), end_state(end) {
    states.insert(start);
    states.insert(end);
    add_transition(start, trans, end);
}

void NFA::add_state(const std::string& state) {
    states.insert(state);
}

void NFA::add_transition(const std::string& from, const std::string& trans, const std::string& to) {
    transitions[from][trans].insert(to);
}

void NFA::merge_transitions(const NFATransitionMap& other_transitions) {
    for (const auto& from_pair : other_transitions) {
        for (const auto& symbol_pair : from_pair.second) {
            for (const auto& to : symbol_pair.second) {
                add_transition(from_pair.first, symbol_pair.first, to);
            }
        }
    }
}

void NFA::Union(const NFA& nfa2, const std::string& new_start, const std::string& new_end) {
    states.insert(new_start);
    states.insert(new_end);
    states.insert(nfa2.states.begin(), nfa2.states.end());
    merge_transitions(nfa2.transitions);

    add_transition(new_start, "e", nfa2.start_state);
    add_transition(new_start, "e", start_state);
    add_transition(nfa2.end_state, "e", new_end);
    add_transition(end_state, "e", new_end);

    start_state = new_start;
    end_state = new_end;
}

void NFA::Concat(const NFA& nfa2) {
    states.insert(nfa2.states.begin(), nfa2.states.end());
    merge_transitions(nfa2.transitions);

    add_transition(end_state, "e", nfa2.start_state);
    end_state = nfa2.end_state;
}

void NFA::Kleene(const std::string& new_start, const std::string& new_end) {
    states.insert(new_start);
    states.insert(new_end);

    add_transition(new_start, "e", new_end);
    add_transition(new_start, "e", start_state);
    add_transition(end_state, "e", start_state);
    add_transition(end_state, "e", new_end);

    start_state = new_start;
    end_state = new_end;
}

std::string NFA::ToDot() {
    std::string result {"digraph NFA {\n"
                        "  rankdir=LR;\n"
                        "  node [shape = circle];\n"};
    for (auto state : states) {
        if (state == end_state) {
            result += "  " + state + "[shape=doublecircle];\n";
        } else {
            result += "  " + state + ";\n";
        }
    }
    for (const auto& from_pair : transitions) {
        const std::string &from = from_pair.first;
        for (const auto &symbol_pair: from_pair.second) {
            const std::string &symbol = symbol_pair.first;
            for (const auto &to: symbol_pair.second) {
                result += "  " + from + "->" + to + "[label=" + symbol + "];\n";
            }
        }
    }

    return result + "}";
}

NFA PolishToThompson(const std::string& tokens) {
    int state_counter{0};
    std::stack<NFA> nfa_stack{};
    for (char ch : tokens) {
        std::string sch(1, ch);
        if (isalpha(ch)) {
            NFA nfa1(std::to_string(state_counter), sch, std::to_string(state_counter+1));
            state_counter += 2;

            nfa_stack.push(nfa1);
        } else if (ch == '.') {
            NFA nfa2 = nfa_stack.top();
            nfa_stack.pop();
            NFA nfa1 = nfa_stack.top();
            nfa_stack.pop();

            nfa1.Concat(nfa2);

            nfa_stack.push(nfa1);
        } else if (ch == '|') {
            NFA nfa2 = nfa_stack.top();
            nfa_stack.pop();
            NFA nfa1 = nfa_stack.top();
            nfa_stack.pop();

            nfa1.Union(nfa2, std::to_string(state_counter), std::to_string(state_counter+1));
            state_counter += 2;

            nfa_stack.push(nfa1);
        } else if (ch == '*') {
            NFA nfa1 = nfa_stack.top();
            nfa_stack.pop();

            nfa1.Kleene(std::to_string(state_counter), std::to_string(state_counter+1));
            state_counter += 2;

            nfa_stack.push(nfa1);
        }
    }
    NFA Thompson = nfa_stack.top();
    return Thompson;
}



std::set<std::string> epsilon_closure(const NFA& nfa, const std::set<std::string>& states) {
    std::set<std::string> closure = states;
    std::stack<std::string> stack{};
    for (const auto& state : states) {
        stack.push(state);
    }

    while(!stack.empty()) {
        std::string state = stack.top();
        stack.pop();

        auto it_from = nfa.transitions.find(state);
        if(it_from == nfa.transitions.end()) {
            continue;
        }
        auto it_symbol = it_from->second.find("e");
        if(it_symbol == it_from->second.end()) {
            continue;
        }
        for (const auto& to : it_symbol->second) {
            if (closure.find(to) == closure.end()) {
                closure.insert(to);
                stack.push(to);
            }
        }

    }
    return closure;
}

std::set<std::string> move(NFA& nfa, std::set<std::string>& states, std::string symbol) {
    std::set<std::string> achievable;
    for (const auto& state : states) {
        if (nfa.transitions.count(state) && nfa.transitions.at(state).count(symbol)) {
            for(const auto& achieve_state : nfa.transitions.at(state).at(symbol)) {
                achievable.insert(achieve_state);
            }
        }
    }
    return achievable;
}