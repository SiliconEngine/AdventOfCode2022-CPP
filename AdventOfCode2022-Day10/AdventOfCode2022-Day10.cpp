/**
 * Advent of Code 2022 challenge, Day 10.
 * Link: https://adventofcode.com/2022/day/10
 *
 * Challenge: Simulate "CRT monitor" by interpreting control signal commands.
 *
 * @author Tim Behrendsen
 * @version 1.0
 */

#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include <set>
#include <algorithm>
#include <map>
#include <regex>
#include <queue>

using namespace std;

enum entry_type { DirType, FileType };

std::vector<std::string> str_split(const std::string& s, const char* pattern) {
    std::regex sep_regex = std::regex{ pattern };
    std::sregex_token_iterator iter(s.begin(), s.end(), sep_regex, -1);
    std::sregex_token_iterator end;
    return { iter, end };
}

void setpix(string &screen, int x, int cycle)
{
    int col = cycle % 40;

    if ((x - 1) <= col && col <= (x + 1))
	screen[cycle] = '#';
}

/**
 * Part 1: Calculates and prints the signal strength at specified cycle intervals.
 * The method reads a set of instructions from a file, processes them to update the value
 * of register X, and calculates the signal strength (cycle number multiplied by the value
 * of X) at every cycle. Specific signal strengths at cycles 20, 60, 100, 140, 180, and 220
 * are then summed and printed.
 */
void part1()
{
    string s;
    string q;

    //istringstream in("mjqjpqmgbljsphdztnvjfqwrcgsmlb");
    ifstream in;
    in.open("tube_x.txt");

    vector<int> strengths;
    strengths.push_back(0);

    int cycle = 0;
    int x = 1;
    while (getline(in, s)) {
        cout << s << endl;
        vector<string> tokens = str_split(s, " ");
        string cmd = tokens[0];

        if (cmd == "noop") {
            ++cycle;
            strengths.push_back(cycle * x);
            cout << "Cycle " << cycle << ": x = " << x << ", signal = " << (cycle * x) << endl;

        } else if (cmd == "addx") {
	    int amt = stoi(tokens[1]);

            ++cycle;
            strengths.push_back(cycle * x);
            cout << "Cycle " << cycle << ": x = " << x << ", signal = " << (cycle * x) << endl;
            ++cycle;
            strengths.push_back(cycle * x);
            cout << "Cycle " << cycle << ": x = " << x << ", signal = " << (cycle * x) << endl;
            x += amt;
        }
    }

    cout << "20=" << strengths[20] << ", 60=" << strengths[60] << ", 100=" << strengths[100] << ", 140=" << strengths[140] << ", 180=" << strengths[180] << ", 220=" << strengths[220] << endl;
    int total = strengths[20] + strengths[60] + strengths[100] + strengths[140] + strengths[180] + strengths[220];
    cout << "Total is " << total << endl;
}

/**
 * Part 2: Simulates a CRT monitor by rendering an image based on CPU instructions.
 * This method reads instructions from a file and updates the position of a sprite
 * on a simulated CRT screen. The sprite's position is determined by the value of
 * register X and is drawn on the screen accordingly.
 */
void part2()
{
    string s;
    string q;

    //istringstream in("mjqjpqmgbljsphdztnvjfqwrcgsmlb");
    ifstream in;
    in.open("tube.txt");

    string screen(240, '.');

    int cycle = 0;
    int x = 1;
    while (getline(in, s)) {
        cout << s << endl;
        vector<string> tokens = str_split(s, " ");
        string cmd = tokens[0];

	// Update the screen pixel based on the command
        if (cmd == "noop") {
            cout << "Cycle " << cycle << ": x = " << x << ", cycle = " << (cycle * x) << endl;
            setpix(screen, x, cycle);
            ++cycle;

        } else if (cmd == "addx") {
	    int amt = stoi(tokens[1]);

            cout << "Cycle " << cycle << ": x = " << x << ", cycle = " << (cycle * x) << endl;
            setpix(screen, x, cycle);
            ++cycle;

            cout << "Cycle " << cycle << ": x = " << x << ", cycle = " << (cycle * x) << endl;
            setpix(screen, x, cycle);
            ++cycle;

            x += amt;
        }
    }
    cout << cycle << endl;

    for (int line = 0; line < 6; ++line) {
        cout << screen.substr(line * 40, 40) << endl;
    }
}

int main()
{
    part2();
}
