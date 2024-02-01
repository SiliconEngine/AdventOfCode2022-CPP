/**
 * Advent of Code 2022 challenge, Day 5.
 * Link: https://adventofcode.com/2022/day/5
 *
 * Challenge: Simulate cargo crane loading procedure and figure out top of stacks.
 *
 * @author Tim Behrendsen
 * @version 1.0
 */

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <set>
#include <algorithm>
#include <map>
#include <regex>

using namespace std;

struct Move {
    char myMove;
    int score;
};

std::vector<std::string> str_split(const std::string& s, const char* pattern) {
    std::regex sep_regex = std::regex{ pattern };
    std::sregex_token_iterator iter(s.begin(), s.end(), sep_regex, -1);
    std::sregex_token_iterator end;
    return { iter, end };
}

void dumpStack(const vector<vector<char>> stacks)
{
    int i = 1;
    for (auto s : stacks) {
        cout << "stack " << i++ << ": ";
        for (auto c : s) {
            cout << " " << c;
        }
        cout << endl;
    }
}

/**
 * Part 1: Simulates the cargo crane loading procedure using a CrateMover
 * 9000 model. This method reads the initial stacks configuration and the
 * series of moves from a file. It then simulates these moves under the
 * CrateMover 9000's constraints, where crates are moved individually,
 * potentially altering the order within the stacks.
 */
void day5_part1()
{
    std::ifstream invFile;
    string s;
    vector<vector<char>> stacks;
    for (int i = 0; i < 9; ++i) {
        stacks.push_back(vector<char>{});
    }
 
    invFile.open("crates.txt");
    while (std::getline(invFile, s)) {
        cout << s << endl;
        if (s == "")
            break;

        if (s.substr(1, 1) == "1")
            continue;

        for (int i = 0; i < 9; ++i) {
            char letter = s.substr(i * 4 + 1, 1)[0];
            if (letter != ' ') {
                cout << " : " << letter;
                stacks[i].insert(stacks[i].begin(), letter);
            }
        }
        cout << endl;
    }

    dumpStack(stacks);

    while (std::getline(invFile, s)) {
        vector<string> list = str_split(s, " ");
        int count = stoi(list[1]);
        int from = stoi(list[3]);
        int to = stoi(list[5]);

        cout << "MOVE: " << count << " FROM " << from << " TO " << to << endl;
        --from;
        --to;

        for (int i = 0; i < count; ++i) {
            char crate = stacks[from].back();
            stacks[from].pop_back();
            stacks[to].push_back(crate);
        }
    }

    dumpStack(stacks);
}

/**
 * Simulates the cargo crane loading procedure using the upgraded CrateMover
 * 9001 model. Similar to part1, this method reads the initial configuration
 * and the series of moves. The CrateMover 9001 can move multiple crates at
 * once, preserving their order.
 */
void day5_part2()
{
    std::ifstream invFile;
    string s;
    vector<vector<char>> stacks;
    for (int i = 0; i < 9; ++i) {
        stacks.push_back(vector<char>{});
    }

    invFile.open("crates.txt");
    while (std::getline(invFile, s)) {
        cout << s << endl;
        if (s == "")
            break;

        if (s.substr(1, 1) == "1")
            continue;

        for (int i = 0; i < 9; ++i) {
            char letter = s.substr(i * 4 + 1, 1)[0];
            if (letter != ' ') {
                cout << " : " << letter;
                stacks[i].insert(stacks[i].begin(), letter);
            }
        }
        cout << endl;
    }

    dumpStack(stacks);

    while (std::getline(invFile, s)) {
        vector<string> list = str_split(s, " ");
        int count = stoi(list[1]);
        int from = stoi(list[3]);
        int to = stoi(list[5]);

        cout << "MOVE: " << count << " FROM " << from << " TO " << to << endl;
        --from;
        --to;

        vector<char> move;
        for (int i = 0; i < count; ++i) {
            char crate = stacks[from].back();
            cout << "   pop: " << crate << endl;
            stacks[from].pop_back();
            move.push_back(crate);
        }
        for (int i = count-1; i >= 0; --i) {
            char crate = move[i];
            cout << "   push: " << crate << endl;
            stacks[to].push_back(crate);
        }
    }

    dumpStack(stacks);
}

int main()
{
    day5_part2();
}
