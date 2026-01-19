#ifndef ROFL_PARSING_H
#define ROFL_PARSING_H

#include <string>
#include <stack>
#include <unordered_map>
#include <cctype>

inline std::string RegexToPolish(const std::string& regex) {
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

inline std::string preprocess_regex(const std::string& regex) {
    std::string res = "";
    if (regex.empty()) return res;

    for (size_t i = 0; i < regex.length(); ++i) {
        res += regex[i];
        if (i + 1 < regex.length()) {
            char current = regex[i];
            char next = regex[i+1];
            if ((isalpha(current) || current == '*' || current == ')') &&
                (isalpha(next) || next == '(')) {
                res += '.';
            }
        }
    }
    return res;
}
#endif //ROFL_PARSING_H
