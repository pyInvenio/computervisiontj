#include <algorithm>
#include <fstream>
#include <iostream>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>

using namespace std;

int alphaScore(const vector<vector<string>>& input) {
    vector<string> s;
    int score = 0;
    for (const vector<string>& vec : input)
        for (const string& cow : vec) s.emplace_back(cow);
    for (int i = 0; i < s.size(); i++)
        for (int j = i + 1; j < s.size(); j++) {
            if (s[i] > s[j]) score++;
        }
    return score;
}

void insertList(vector<vector<string>>& output, const vector<string>& insert) {
    vector<vector<string>> temp(output);
    temp.insert(temp.begin(), insert);
    int min_score = alphaScore(temp);
    int score;
    vector<vector<string>> mostAlpha(temp);
    for (int i = 1; i <= output.size(); i++) {
        temp = output;
        temp.insert(temp.begin() + i, insert);
        score = alphaScore(temp);
        if (score < min_score) {
            min_score = score;
            mostAlpha = temp;
        }
    }

    output = mostAlpha;
    // for (auto iter = output.begin(); iter != output.end(); ++iter) {
    //     if ((*iter)[0] > *insert.begin()) {
    //         temp.insert(iter, insert);
    //         return;
    //     }
    // }
    // output.emplace_back(insert);
}

int main() {
    vector<string> cows{"Beatrice",   //
                        "Belinda",    //
                        "Bella",      //
                        "Bessie",     //
                        "Betsy",      //
                        "Blue",       //
                        "Buttercup",  //
                        "Sue"};

    ifstream file("lineup.in", ios::in);
    int len;
    file >> len;
    string first, must, be, milked, beside, second;

    unordered_map<string, set<string>> adj;

    for (int i = 0; i < len; i++) {
        file >> first >> must >> be >> milked >> beside >> second;
        cows.erase(remove(cows.begin(), cows.end(), first), cows.end());
        cows.erase(remove(cows.begin(), cows.end(), second), cows.end());
        adj[second].insert(first);
    }

    vector<vector<string>> output;
    for (const string& cow : cows) {
        output.emplace_back(vector<string>{cow});
    }
    for (auto const& s : adj) {
        vector<string> insert;
        if (s.second.size() > 1) {
            auto second = s.second.begin();
            advance(second, 1);
            if (*s.second.begin() < *second) {
                insert.emplace_back(*s.second.begin());
                insert.emplace_back(s.first);
                insert.emplace_back(*second);
            } else {
                insert.emplace_back(*second);
                insert.emplace_back(s.first);
                insert.emplace_back(*s.second.begin());
            }
        } else {
            if (s.first < *s.second.begin()) {
                insert.emplace_back(s.first);
                insert.emplace_back(*s.second.begin());
            } else {
                insert.emplace_back(*s.second.begin());
                insert.emplace_back(s.first);
            }
        }
        insertList(output, insert);
    }

    ofstream out("lineup.out", ios::out);

    for (const vector<string>& cowvec : output) {
        for (const string& cow : cowvec) out << cow << '\n';
    }

    return 0;
}