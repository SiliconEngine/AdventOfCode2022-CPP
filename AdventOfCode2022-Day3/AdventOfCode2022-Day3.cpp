/**
 * Advent of Code 2022 challenge, Day 3.
 * Link: https://adventofcode.com/2022/day/3
 *
 * Challenge: Analyze elf "ruck sacks" and determine badge priorities.
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

int cvtToPriority(char c)
    /**
     * Part 2: This method reads groups of three lines (representing three Elves' rucksacks) from a file
     * and identifies the badge item type common to all three rucksacks in each group. It calculates the
     * sum of the priorities of these badge item types.
     */
{
    if (c >= 'a' && c <= 'z') {
        return 1 + c - 'a';
    }
    if (c >= 'A' && c <= 'Z') {
        return 27 + c - 'A';
    }
    return 0;
}

/**
 * Part 1: This method reads the content of rucksacks from a file and
 * identifies the item type that appears in both compartments of each
 * rucksack. It calculates the sum of the priorities of these item types.
 */
void day3_part1()
{
    std::ifstream invFile;
    string s;

    invFile.open("rucksack.txt");
    int totalPriority = 0;
    while (std::getline(invFile, s)) {
        int compLen = s.length() / 2;
        string comp1 = s.substr(0, compLen);
        string comp2 = s.substr(compLen, compLen);
        cout << comp1 << " : " << comp2 << endl;

        for (int i = 0; i < compLen; ++i) {
            char c = comp1[i];
            int pos = comp2.find(c);
            if (pos != string::npos) {
                int priority = cvtToPriority(c);
                cout << "    bad: " << c << " : " << priority << endl;
                totalPriority += priority;
                break;
            }
        }
    }

    cout << "total priority is " << totalPriority << endl;
}

/**
 * Part 2: This method reads groups of three lines (representing three Elves'
 * rucksacks) from a file and identifies the badge item type common to all
 * three rucksacks in each group. It calculates the sum of the priorities
 * of these badge item types.
 */
void day3_part2()
{
    std::ifstream invFile;
    string s1,s2,s3;

    invFile.open("rucksack.txt");
    int totalPriority = 0;
    while (std::getline(invFile, s1)) {
        std::getline(invFile, s2);
        std::getline(invFile, s3);

        int len = s1.length();

        int flag = 0;
        for (int i = 0; i < len; ++i) {
            char c = s1[i];
            int pos2 = s2.find(c);
            int pos3 = s3.find(c);
            if (pos2 != string::npos && pos3 != string::npos) {
                int priority = cvtToPriority(c);
                cout << "    bad: " << c << " : " << priority << endl;
                totalPriority += priority;
                flag = 1;
                break;
            }
        }
        if (!flag) {
            cout << "NOT FOUND";
            exit(0);
        }

    }
    cout << "total priority is " << totalPriority << endl;
}

int main()
{
    day3_part2();
}
