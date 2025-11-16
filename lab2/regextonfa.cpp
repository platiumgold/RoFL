#include <string>
#include <stack>
#include <vector>
#include <unordered_map>
#include <unordered_set>
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

class NFA {
public:
    std::unordered_set<std::string> states {};
    std::vector<std::tuple<std::string, std::string, std::string>> transitions {};
    std::string start_state{};
    std::string end_state{};


    NFA() = default;
    NFA(const std::string& start, const std::string& trans, const std::string& end) : start_state(start), end_state(end) {
        states.insert(start);
        states.insert(end);
        transitions.emplace_back(start, trans, end);
    }
    void add_state(const std::string& state) {
        states.insert(state);
    }
    void add_transition(const std::string& from, const std::string& trans, const std::string& to) {
        transitions.emplace_back(from, trans, to);
    }

    void Union(NFA nfa2, const std::string& new_start, const std::string& new_end) {

        states.insert(new_start);
        states.insert(new_end);
        states.insert(nfa2.states.begin(), nfa2.states.end());
        transitions.insert(transitions.end(), nfa2.transitions.begin(), nfa2.transitions.end());

        transitions.emplace_back(new_start, "e", nfa2.start_state);
        transitions.emplace_back(new_start, "e", start_state);
        transitions.emplace_back(nfa2.end_state, "e", new_end);
        transitions.emplace_back(end_state, "e", new_end);

        start_state = new_start;
        end_state = new_end;
    }

    void Concat(NFA nfa2) {
        states.insert(nfa2.states.begin(), nfa2.states.end());
        transitions.insert(transitions.end(), nfa2.transitions.begin(), nfa2.transitions.end());


        transitions.emplace_back(end_state, "e", nfa2.start_state);
        end_state = nfa2.end_state;
    }

    void Kleene(const std::string& new_start, const std::string& new_end) {
        states.insert(new_start);
        states.insert(new_end);

        transitions.emplace_back(new_start, "e", new_end);
        transitions.emplace_back(new_start, "e", start_state);
        transitions.emplace_back(end_state, "e", start_state);
        transitions.emplace_back(end_state, "e", new_end);

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
        for (auto trans : transitions) {
            result += "  " + std::get<0>(trans) + "->" + std::get<2>(trans) + "[label=" + std::get<1>(trans) + "];\n";
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

    return 0;
}