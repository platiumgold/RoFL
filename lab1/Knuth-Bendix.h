#ifndef KNUTH_BENDIX_H
#define KNUTH_BENDIX_H

#include <string>
#include <vector>
#include <deque>
#include <utility>

struct Rule {
    std::string lhs;
    std::string rhs;
};

bool IsLess(const std::string& s1, const std::string& s2);
std::string NormalForm(std::string t1, std::vector<Rule>& R);
void FindCriticalPairsForTwoRules(const Rule& rule1, const Rule& rule2, std::deque<std::pair<std::string, std::string>>& critPairs);
void Interreduce(const Rule& new_rule, std::vector<Rule>& R, std::deque<std::pair<std::string, std::string>>& P);
std::vector<Rule> KnuthBendixCompletion(std::vector<Rule>& T);

#endif //KNUTH_BENDIX_H

