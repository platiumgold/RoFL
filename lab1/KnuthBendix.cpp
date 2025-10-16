#include "Knuth-Bendix.h"
#include <iostream>
#include <vector>
#include <deque>
using namespace std;


bool IsLess(const string& s1, const string& s2) {
    if (s1.length() == s2.length()) {
        return s1 < s2;
    }
    return s1.length() < s2.length();
}

string NormalForm(string t1, vector<Rule>& R) {
    bool isNF = false;
    while(!isNF) {
        isNF = true;
        for (const auto& rule : R) {
            auto pos = t1.find(rule.lhs);
            if (pos != string::npos) {
                t1.replace(pos, rule.lhs.length(), rule.rhs);
                isNF = false;
                break;
            }
        }
    }
    return t1;
}

void FindCriticalPairsForTwoRules(const Rule& rule1, const Rule& rule2, deque<pair<string, string>>& critPairs) {
    const string& l1 = rule1.lhs;
    const string& r1 = rule1.rhs;
    const string& l2 = rule2.lhs;
    const string& r2 = rule2.rhs;

    //full word overlappings
    for (size_t pos = 0; (pos = l1.find(l2, pos)) != string::npos; ++pos) {
        if (&rule1 == &rule2 && pos == 0) continue;
        string s1 = r1;
        string s2 = l1;
        s2.replace(pos, l2.length(), r2);
        critPairs.push_back({s1, s2});
    }

    //suffix-prefix overlappings
    for (size_t i = 1; i < min(l1.length(), l2.length()); ++i) {
        if (l1.substr(l1.length() - i) == l2.substr(0, i)) {
            string s1 = r1 + l2.substr(i);
            string s2 = l1.substr(0, l1.length() - i) + r2;
            critPairs.push_back({s1, s2});
        }
    }
}

void Interreduce(const Rule& new_rule, vector<Rule>& R, deque<pair<string, string>>& P){
    vector<Rule> R_updated;
    vector<Rule> new_rule_as_vector = {new_rule};
    for (const auto& old_rule : R) {
        if (old_rule.lhs.find(new_rule.lhs) != string::npos) {
            P.push_back({old_rule.lhs, old_rule.rhs});
        } else {
            string reduced_rhs = NormalForm(old_rule.rhs, new_rule_as_vector);
            R_updated.push_back({old_rule.lhs, reduced_rhs});
        }
    }
    string final_new_rhs = NormalForm(new_rule.rhs, R_updated);
    R_updated.push_back({new_rule.lhs, final_new_rhs});
    R = R_updated;

}
std::vector<Rule> KnuthBendixCompletion(std::vector<Rule>& T) {
    auto R = T;
    deque<pair<string, string>> P {};
    for (size_t i = 0; i < R.size(); ++i) {
        for (size_t j = i; j < R.size(); ++j) {
            FindCriticalPairsForTwoRules(R[i], R[j], P);
            if (i != j) {
                FindCriticalPairsForTwoRules(R[j], R[i], P);
            }
        }
    }
    while(!P.empty()) {
        auto [s1, s2] = P.front();
        P.pop_front();
        auto s1nf = NormalForm(s1, R);
        auto s2nf = NormalForm(s2, R);
        if (s1nf != s2nf) {
            Rule new_rule;
            if (IsLess(s2nf, s1nf)) {
                new_rule = {s1nf, s2nf};
            } else {
                new_rule = {s2nf, s1nf};
            }

            //cout << "New rule: " << new_rule.lhs << " -> " << new_rule.rhs << endl;

            Interreduce(new_rule, R, P);
            const Rule& final_new_rule = R.back();
            for (const auto& existing_rule : R) {
                FindCriticalPairsForTwoRules(final_new_rule, existing_rule, P);
                if (&final_new_rule != &existing_rule) {
                    FindCriticalPairsForTwoRules(existing_rule, final_new_rule, P);
                }
            }
        }
    }
    return R;
}