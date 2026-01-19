#include "NFA.h"
#include "DFA.h"
#include "Parsing.h"
#include <iostream>
#include <algorithm>
#include <random>
#include <regex>
#include <cassert>

using MemoCache = std::map<std::pair<std::string, size_t>, bool>;

bool dfs_afa(NFA& nfa, const std::string& current_state, const std::string& word, size_t idx, MemoCache& memo) {
    if (memo.count({current_state, idx})) {
        return memo[{current_state, idx}];
    }

    if (idx == word.size()) {
        return memo[{current_state, idx}] = (nfa.end_states.count(current_state) > 0);
    }

    char symbol = word[idx];
    std::string s_symbol(1, symbol);

    if (nfa.transitions.count(current_state) == 0 ||
        nfa.transitions.at(current_state).count(s_symbol) == 0) {

        return memo[{current_state, idx}] = false;
    }

    const auto& next_states = nfa.transitions.at(current_state).at(s_symbol);

    if (current_state == "12") {
        bool result = true;
        if (next_states.empty()) result = false;

        for (const auto& next : next_states) {
            if (!dfs_afa(nfa, next, word, idx + 1, memo)) {
                result = false;
                break;
            }
        }
        return memo[{current_state, idx}] = result;
    }
    else {
        bool result = false;
        for (const auto& next : next_states) {
            if (dfs_afa(nfa, next, word, idx + 1, memo)) {
                result = true;
                break;
            }
        }
        return memo[{current_state, idx}] = result;
    }
}

bool IsInLanguageAFA(NFA& nfa, const std::string& word) {
    MemoCache memo;

    std::set<std::string> start_set = {nfa.start_state};
    std::set<std::string> initial_states = EpsilonClosure(nfa, start_set);

    for (const auto& state : initial_states) {
        if (dfs_afa(nfa, state, word, 0, memo)) {
            return true;
        }
    }

    return false;
}

NFA BuildAFA() {
    NFA nfa;

    nfa.start_state = "0";

    nfa.end_states.insert("3");
    nfa.end_states.insert("8");
    nfa.end_states.insert("12");
    nfa.end_states.insert("15");


    for (int i = 0; i <= 16; ++i) {
        nfa.AddState(std::to_string(i));
    }

    nfa.AddTransition("0", "a", "1");
    nfa.AddTransition("1", "b", "2");
    nfa.AddTransition("2", "c", "3");

    nfa.AddTransition("0", "e", "4");


    nfa.AddTransition("4", "a", "4");
    nfa.AddTransition("4", "b", "4");
    nfa.AddTransition("4", "c", "4");

    nfa.AddTransition("4", "a", "5");
    nfa.AddTransition("5", "b", "6");

    nfa.AddTransition("6", "a", "6");
    nfa.AddTransition("6", "b", "6");
    nfa.AddTransition("6", "c", "6");

    nfa.AddTransition("6", "b", "7");
    nfa.AddTransition("7", "c", "8");

    nfa.AddTransition("8", "a", "8");
    nfa.AddTransition("8", "b", "8");
    nfa.AddTransition("8", "c", "8");


    nfa.AddTransition("4", "b", "9");
    nfa.AddTransition("9", "c", "10");

    nfa.AddTransition("10", "a", "10");
    nfa.AddTransition("10", "b", "10");
    nfa.AddTransition("10", "c", "10");

    nfa.AddTransition("10", "a", "11");
    nfa.AddTransition("11", "b", "12");


    nfa.AddTransition("12", "a", "12");

    nfa.AddTransition("12", "b", "13");
    nfa.AddTransition("13", "b", "15");
    nfa.AddTransition("13", "c", "15");
    nfa.AddTransition("15", "a", "15");
    nfa.AddTransition("15", "b", "15");
    nfa.AddTransition("15", "c", "15");

    nfa.AddTransition("12", "b", "14");
    nfa.AddTransition("14", "a", "12");
    nfa.AddTransition("14", "b", "12");
    nfa.AddTransition("14", "c", "12");

    nfa.AddTransition("12", "c", "16");
    nfa.AddTransition("16", "c", "12");

    return nfa;
}

NFA BuildMiniNFA() {
    NFA nfa;
    nfa.start_state = "0";
    nfa.end_states = {"3", "7", "14"};

    for (int i = 0; i <= 14; ++i) {
        nfa.AddState(std::to_string(i));
    }

    nfa.AddTransition("0", "a", "1");
    nfa.AddTransition("1", "b", "2");
    nfa.AddTransition("2", "c", "3");

    nfa.AddTransition("0", "e", "10");

    nfa.AddTransition("10", "a", "10");
    nfa.AddTransition("10", "b", "10");
    nfa.AddTransition("10", "c", "10");

    nfa.AddTransition("10", "b", "4");
    nfa.AddTransition("4", "c", "5");

    nfa.AddTransition("5", "a", "5");
    nfa.AddTransition("5", "b", "5");
    nfa.AddTransition("5", "c", "5");

    nfa.AddTransition("5", "a", "6");
    nfa.AddTransition("6", "b", "7");

    nfa.AddTransition("7", "a", "7");

    nfa.AddTransition("7", "b", "8");
    nfa.AddTransition("8", "b", "7");
    nfa.AddTransition("8", "c", "7");

    nfa.AddTransition("7", "c", "9");
    nfa.AddTransition("9", "c", "7");

    nfa.AddTransition("10", "a", "11");
    nfa.AddTransition("11", "b", "12");

    nfa.AddTransition("12", "a", "12");
    nfa.AddTransition("12", "b", "12");
    nfa.AddTransition("12", "c", "12");

    nfa.AddTransition("12", "b", "13");
    nfa.AddTransition("13", "c", "14");

    nfa.AddTransition("14", "a", "14");
    nfa.AddTransition("14", "b", "14");
    nfa.AddTransition("14", "c", "14");

    return nfa;
}

struct WordGenerationParams {
    int min_len;
    int max_len;
    std::string alphabet;
};

std::string GenRandomWord(std::mt19937& gen, WordGenerationParams& word_params) {
    std::uniform_int_distribution<> word_len_distrib(word_params.min_len, word_params.max_len);
    int length = word_len_distrib(gen);
    std::uniform_int_distribution<> char_distrib(0, static_cast<int>(word_params.alphabet.length()) - 1);
    std::string rand_str {};
    for (int i = 0; i < length; ++i) {
        rand_str += word_params.alphabet[char_distrib(gen)];
    }
    return rand_str;
}

int main() {

    std::string regexp {"((a*b*c*)*ab(a*b*c*)*bc(a|b|c)*)|((a|b|c)*bc(a*b*c*)*ab(a|bc|cc|bb)*)|abc"};
    std::string prepr_regexp = preprocess_regex(regexp);
    std::string polish = RegexToPolish(prepr_regexp);
    //std::cout << polish << std::endl;
    NFA Thompson = PolishToThompson(polish);
    //std::cout << Thompson.ToDot() << std::endl;
    DFA dfa = NFAtoDFA(Thompson);
    //std::cout << dfa.ToDot() << std::endl;
    DFA min_dfa = MinimizeDFA(dfa);
    //std::cout << min_dfa.ToDot() << std::endl;
    NFA afa = BuildAFA();
    //std::cout << afa.ToDot() << std::endl;
    NFA mini_nfa = BuildMiniNFA();
    //std::cout << mini_nfa.ToDot() << std::endl;
    std::regex re(R"(((a*b*c*)*ab(a*b*c*)*bc(a|b|c)*)|((a|b|c)*bc(a*b*c*)*ab(a|bc|cc|bb)*)|abc)");


    std::random_device rd;
    auto seed = rd();
    std::cout << "Seed: " << seed << std::endl;
    std::mt19937 gen(seed);

    WordGenerationParams wgp{0, 10, "abc"};
    std::cout << "fuzzing with ground truth" << std::endl;
    for (int i=0; i<10000; ++i) {
        std::string test = GenRandomWord(gen, wgp);
        std::cout << test << " ";
        bool ground_truth_res = std::regex_match(test, re);
        bool min_dfa_res = min_dfa.IsInLanguage(test);
        bool mini_nfa_res = mini_nfa.IsInLanguage(test);
        bool afa_res = IsInLanguageAFA(afa, test);
        assert(min_dfa_res == ground_truth_res);
        assert(mini_nfa_res == ground_truth_res);
        assert(afa_res == mini_nfa_res);
        std::cout << i << " OK" << std::endl;
    }

    WordGenerationParams wgpbig{0, 100, "abc"};
    std::cout << "fuzzing without <regex>" << std::endl;
    for (int i=0; i<10000; ++i) {
        std::string test = GenRandomWord(gen, wgpbig);
        std::cout << test << " ";
        bool min_dfa_res = min_dfa.IsInLanguage(test);
        bool mini_nfa_res = mini_nfa.IsInLanguage(test);
        bool afa_res = IsInLanguageAFA(afa, test);
        assert(min_dfa_res == mini_nfa_res);
        assert(mini_nfa_res == afa_res);
        std::cout << i << " OK" << std::endl;
    }

    std::cout << "cool" << std::endl;
    return 0;

}