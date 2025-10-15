#include <iostream>
#include <vector>
#include <string>
#include <random>
#include "utils.h"
#include <cassert>
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

pair<string, string> GenRandomTest(mt19937& gen, vector<Rule>& srs, WordGenerationParams& word_params,
                                        ChainGenerationParams& chain_params) {
    //random word generation
    uniform_int_distribution<> word_len_distrib(word_params.min_len, word_params.max_len);
    int length = word_len_distrib(gen);
    uniform_int_distribution<> char_distrib(0, word_params.alphabet.length()-1);
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
        uniform_int_distribution<> rule_distrib(0, possible_rules.size() - 1);
        int rule_index = possible_rules[rule_distrib(gen)];
        
        //position selection
        vector<int> positions {};
        int pos = new_str.find(srs[rule_index].lhs);
        while(pos != string::npos) {
            positions.push_back(pos);
            pos = new_str.find(srs[rule_index].lhs, pos + 1);
        }
        int position {};
        if (positions.size() == 1) {
            position = positions[0];
        } else {
            uniform_int_distribution<> pos_distrib(0, positions.size() - 1);
            position = positions[pos_distrib(gen)];
        }

        //completing the step
        Rule rule = srs[rule_index];
        new_str.replace(position, rule.lhs.length(), rule.rhs);
        cout << srs[rule_index].lhs << "->" << srs[rule_index].rhs << " " << new_str << std::endl;
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
    std::vector<Rule> new_srs = {
            {"bbb",   "bb"},
            {"baba",  "bb"},
            {"bba",   "bb"},
            {"abb",   "bb"},
            {"baa",   "bb"},
            {"aabab", "bb"}
    };

    random_device rd;
    int seed = rd();
    cout << "Seed: " << seed << std::endl;
    mt19937 gen(seed);

    WordGenerationParams wgp{5, 5000, "ab"};
    ChainGenerationParams cgp{2,70};
    for (int i=0; i<100; ++i) {
        assert(FuzzTest(gen, initial_srs, new_srs, wgp, cgp));
    }
    cout << "cool" << std::endl;
    return 0;
}