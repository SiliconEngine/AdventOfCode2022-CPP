/**
 * Advent of Code 2022 challenge, Day 2.
 * Link: https://adventofcode.com/2022/day/2
 *
 * Challenge: Simulate elf strategy game and determine total score.
 *
 * @author Tim Behrendsen
 * @version 1.0
 */

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <map>

using namespace std;

struct Move {
    char myMove;
    int score;
};

/**
 * Part 1: Calculates the total score by reading the strategy guide and
 * computing the score based on the predetermined outcomes and the scores
 * associated with each shape.
 */
void day2_part1()
{
    std::ifstream invFile;
    string s;

    map<char, int>shapeScores = {
        { 'X', 1 },
        { 'Y', 2 },
        { 'Z', 3 },
    };


    // Opponent: A = Rock, B = Paper, C = Scissors
    // Me: X = Rock, Y = Paper, Z = Scissors
    map<string, int> winScores = {
        { "AX", 3 },
        { "AY", 6 },
        { "AZ", 0 },
        { "BX", 0 },
        { "BY", 3 },
        { "BZ", 6 },
        { "CX", 6 },
        { "CY", 0 },
        { "CZ", 3 },
    };


    invFile.open("rps_strategy.txt");
    int totalScore = 0;
    while (std::getline(invFile, s)) {
        char om = s[0];
        char mm = s[2];
        string key = "";
        key = key + om + mm;
        cout << key << endl;
        
        int score = shapeScores[mm] + winScores[key];
        totalScore += score;

        //cout << s.length() << endl;
        //cout << s << ": score is " << score << endl;
    }
    cout << "total score is " << totalScore << endl;
}

/**
 * Part 2: Calculates the total score, but the strategy guide is interpreted
 * differently: it specifies the desired outcome of each round (win, lose,
 * draw). The method determines the appropriate move to achieve this
 * outcome and calculates the score accordingly.
 */
void day2_part2()
{
    std::ifstream invFile;
    string s;

    map<char, int>shapeScores = {
        { 'A', 1 },
        { 'B', 2 },
        { 'C', 3 },
    };

    // Opponent: A = Rock, B = Paper, C = Scissors
    // Me: X = must lose, Y = must draw, Z = must win
    map<string, Move> winScores = {
        { "AX", { 'C', 0 } },
        { "AY", { 'A', 3 } },
        { "AZ", { 'B', 6 } },
        { "BX", { 'A', 0 } },
        { "BY", { 'B', 3 } },
        { "BZ", { 'C', 6 } },
        { "CX", { 'B', 0 } },
        { "CY", { 'C', 3 } },
        { "CZ", { 'A', 6 } },
    };

    invFile.open("rps_strategy.txt");
    int totalScore = 0;
    while (std::getline(invFile, s)) {
        char om = s[0];
        char end = s[2];
        //char mm = winSel[om].myMove;
        //cout << "end: " << end << ", mm: " << mm << endl;

        string key = "";
        key = key + om + end;
        cout << key << endl;

        char mm = winScores[key].myMove;
        int score = shapeScores[mm] + winScores[key].score;
        totalScore += score;

        //cout << s.length() << endl;
        cout << s << ": om=" << om << ", end = " << end << ", mm=" << mm <<", score is " << score << endl;
    }
    cout << "total score is " << totalScore << endl;
}

int main()
{
    day2_part2();
}
