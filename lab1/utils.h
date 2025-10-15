#include <vector>
#include <string>

struct Rule {
    std::string lhs;
    std::string rhs;
};

inline std::string NormalForm(std::string t1, std::vector<Rule>& R) {
    bool isNF = false;
    while(!isNF) {
        isNF = true;
        for (const auto& rule : R) {
            auto pos = t1.find(rule.lhs);
            if (pos != std::string::npos) {
                t1.replace(pos, rule.lhs.length(), rule.rhs);
                isNF = false;
                break;
            }
        }
    }
    return t1;
}