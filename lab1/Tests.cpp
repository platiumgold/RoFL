#include <iostream>
#include <vector>
#include <string>
#include <random>
#include <cassert>
#include "Knuth-Bendix.h"

using namespace std;

struct WordGenerationParams {
    int min_len;
    int max_len;
    std::string alphabet;
};
struct ChainGenerationParams {
    int min_len;
    int max_len;
};



inline pair<string, string> GenRandomTest(mt19937& gen, vector<Rule>& srs, WordGenerationParams& word_params,
                                          ChainGenerationParams& chain_params) {
    //random word generation
    uniform_int_distribution<> word_len_distrib(word_params.min_len, word_params.max_len);
    int length = word_len_distrib(gen);
    uniform_int_distribution<> char_distrib(0, static_cast<int>(word_params.alphabet.length()) - 1);
    string init_str {};
    for (int i = 0; i < length; ++i) {
        init_str += word_params.alphabet[char_distrib(gen)];
    }

    //random chain generation
    uniform_int_distribution<> chain_len_distrib(chain_params.min_len, chain_params.max_len);
    int chain_length = chain_len_distrib(gen);
    string new_str = init_str;
    for (int i=0; i<chain_length; ++i) {
        //rule selection
        vector<int> possible_rules {};
        for (int j=0; j<srs.size(); ++j) {
            if (new_str.find(srs[j].lhs) != string::npos) {
                possible_rules.push_back(j);
            }
        }
        if (possible_rules.empty()) {
            break;
        }
        uniform_int_distribution<> rule_distrib(0, static_cast<int>(possible_rules.size()) - 1);
        int rule_index = possible_rules[rule_distrib(gen)];

        //position selection
        vector<size_t> positions {};
        size_t pos = new_str.find(srs[rule_index].lhs);
        while(pos != string::npos) {
            positions.push_back(pos);
            pos = new_str.find(srs[rule_index].lhs, pos + 1);
        }
        size_t position;
        if (positions.size() == 1) {
            position = positions[0];
        } else {
            uniform_int_distribution<> pos_distrib(0, static_cast<int>(positions.size()) - 1);
            position = positions[pos_distrib(gen)];
        }

        //completing the step
        Rule rule = srs[rule_index];
        new_str.replace(position, rule.lhs.length(), rule.rhs);
        //cout << srs[rule_index].lhs << "->" << srs[rule_index].rhs << " " << new_str << endl;
    }

    return {init_str, new_str};
}

bool FuzzTest(mt19937& gen, vector<Rule>& init_srs, vector<Rule>& new_srs,
              WordGenerationParams& word_params, ChainGenerationParams& chain_params) {
    auto [init_str, new_str] = GenRandomTest(gen, init_srs, word_params, chain_params);
    string init_nf = NormalForm(init_str, new_srs);
    string new_nf = NormalForm(new_str, new_srs);
    if (init_nf == new_nf){
        return true;
    } else {
        return false;
    }
}

int main() {
    vector<Rule> initial_srs = {
            {"babab",     "aabab"},
            {"bbaab",     "bbbab"},
            {"bbb",       "bb"},
            {"abaaab",    "baba"},
            {"ababbab",   "ababaab"},
            {"baab",      "baa"},
            {"bbabbaa",   "bbaaa"},
            {"aabbabba",  "bbaa"},
            {"aabaababb", "bbb"},
            {"baababbaa", "bbabbabba"}
    };
    vector<Rule> terminating_srs = {
            {"babab",     "aabab"},
            {"bbbab",     "bbaab"},
            {"bbb",       "bb"},
            {"abaaab",    "baba"},
            {"ababbab",   "ababaab"},
            {"baab",      "baa"},
            {"bbabbaa",   "bbaaa"},
            {"aabbabba",  "bbaa"},
            {"aabaababb", "bbb"},
            {"bbabbabba", "baababbaa"},
    };
    vector<Rule> new_srs = KnuthBendixCompletion(terminating_srs);
    cout << "Final rules:" << endl;
    for (const auto& rule : new_srs) {
        cout << rule.lhs << " -> " << rule.rhs << endl;
    }

    random_device rd;
    auto seed = rd();
    cout << "Seed: " << seed << std::endl;
    mt19937 gen(seed);

    WordGenerationParams wgp{5, 5000, "ab"};
    ChainGenerationParams cgp{2,70};
    cout << "fuzzing" << endl;
    for (int i=0; i<10000; ++i) {
        assert(FuzzTest(gen, initial_srs, new_srs, wgp, cgp));
        cout << i << " OK" << endl;
    }

    cout << "cool" << endl;
    return 0;
}