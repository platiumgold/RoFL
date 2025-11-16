#include <string>
#include <stack>
#include <vector>
#include <unordered_map>
#include <map>
#include <unordered_set>
#include <set>
#include <iostream>
std::string RegexToPolish(const std::string& regex) {
    std::stack<char> ops{};
    std::string polish {};
    std::unordered_map<char, int> priority{{'*', 3},
                                           {'.', 2},
                                           {'|', 1}};
    for (auto& ch : regex) {
        if (isalpha(ch)) {
            polish += ch;
        } else if (ch == '(') {
            ops.push('(');
        } else if (ch == ')') {
            while (ops.top() != '(') {
                polish += ops.top();
                ops.pop();
            }
            ops.pop();
        } else if (ch == '|' || ch == '*' || ch == '.') {
            while (!ops.empty() && ops.top() != '(' &&
            (priority[ops.top()] > priority[ch] ||
            priority[ops.top()] == priority[ch] && (ch == '.' || ch == '|'))){
                polish += ops.top();
                ops.pop();
            }
            ops.push(ch);
        }
    }
    while(!ops.empty()) {
        polish += ops.top();
        ops.pop();
    }
    return polish;
}

using NFATransitionMap = std::unordered_map<std::string, std::unordered_map<std::string, std::unordered_set<std::string>>>;
using DFATransitionMap = std::unordered_map<std::string, std::unordered_map<std::string, std::string>>;

class NFA {
public:
    std::unordered_set<std::string> states {};
    NFATransitionMap transitions {};
    std::string start_state{};
    std::string end_state{};


    NFA() = default;
    NFA(const std::string& start, const std::string& trans, const std::string& end) : start_state(start), end_state(end) {
        states.insert(start);
        states.insert(end);
        add_transition(start, trans, end);
    }

    void add_state(const std::string& state) {
        states.insert(state);
    }

    void add_transition(const std::string& from, const std::string& trans, const std::string& to) {
        transitions[from][trans].insert(to);
    }

    void merge_transitions(const NFATransitionMap& other_transitions) {
        for (const auto& from_pair : other_transitions) {
            for (const auto& symbol_pair : from_pair.second) {
                for (const auto& to : symbol_pair.second) {
                    add_transition(from_pair.first, symbol_pair.first, to);
                }
            }
        }
    }

    void Union(const NFA& nfa2, const std::string& new_start, const std::string& new_end) {
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

    void Concat(const NFA& nfa2) {
        states.insert(nfa2.states.begin(), nfa2.states.end());
        merge_transitions(nfa2.transitions);

        add_transition(end_state, "e", nfa2.start_state);
        end_state = nfa2.end_state;
    }

    void Kleene(const std::string& new_start, const std::string& new_end) {
        states.insert(new_start);
        states.insert(new_end);

        add_transition(new_start, "e", new_end);
        add_transition(new_start, "e", start_state);
        add_transition(end_state, "e", start_state);
        add_transition(end_state, "e", new_end);

        start_state = new_start;
        end_state = new_end;
    }

    std::string ToDot() {
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
};

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

class DFA {
public:
    std::unordered_set<std::string> states {};
    DFATransitionMap transitions {};
    std::string start_state{};
    std::unordered_set<std::string> end_states{};

    DFA() = default;

    void add_state(const std::string& state, bool is_final = false, bool is_start = false) {
        states.insert(state);
        if (is_final) {
            end_states.insert(state);
        }
        if (is_start) {
            start_state = state;
        }
    }

    void add_transition(const std::string& from, const std::string& symbol, const std::string& to) {
        transitions[from][symbol] = to;
    }

    std::string ToDot() {
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
};

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

int main() {
    /*
    ((a*.b*.c*)*.a.b.(a*.b*.c*)*.b.c.(a|b|c)*)|((a|b|c)*.b.c.(a*.b*.c*)*.a.b.(a|b.c|c.c|b.b)*)|a.b.c
    a*b*.c*.*a.b.a*b*.c*.*.b.c.ab|c|*.ab|c|*b.c.a*b*.c*.*.a.b.abc.|cc.|bb.|*.|ab.c.|
    */
    std::string test {"((a*.b*.c*)*.a.b.(a*.b*.c*)*.b.c.(a|b|c)*)|((a|b|c)*.b.c.(a*.b*.c*)*.a.b.(a|b.c|c.c|b.b)*)|a.b.c"};
    std::string polish = RegexToPolish(test);
    std::cout << polish << std::endl;

    NFA Thompson = PolishToThompson(polish);
    std::cout << Thompson.ToDot() << std::endl;

    auto closure = epsilon_closure(Thompson, {Thompson.start_state});
    for (auto& i : closure) {
        //std::cout << i << " ";
    }
    DFA dfa = NFAtoDFA(Thompson);
    std::cout << dfa.ToDot() << std::endl;
    return 0;
}