/**
 * Advent of Code 2022 challenge, Day 4.
 * Link: https://adventofcode.com/2022/day/4
 *
 * Challenge: Simulate elf "cleaning sections" and determine overlapping
 * assignment pairs.
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
#include <regex>

using namespace std;

struct Move {
    char myMove;
    int score;
};

std::vector<std::string> str_split(const std::string& s, const char *pattern) {
    std::regex sep_regex = std::regex{ pattern };
    std::sregex_token_iterator iter(s.begin(), s.end(), sep_regex, -1);
    std::sregex_token_iterator end;
    return { iter, end };
}

/**
 * Part 1: Determines how many assignment pairs have one range that fully
 * contains the other. It reads each line from the file, splits it into two
 * ranges for each elf, and checks if one range is fully contained within
 * the other.
 */
void day4_part1()
{
    std::ifstream invFile;
    string s;

    invFile.open("elf_ranges.txt");
    int isContained = 0;
    while (std::getline(invFile, s)) {
        //int pos = s.find(',');
        //string s1 = s.substr(0, pos);
        //string s2 = s.substr(pos + 1, string::npos);
        vector<string> list = str_split(s, "[,-]");

        cout << s << endl;
        //for (const auto x : list) {
        //    cout << "    " << x << endl;
        //}

        int elf1_r1 = stoi(list[0]);
        int elf1_r2 = stoi(list[1]);
        int elf2_r1 = stoi(list[2]);
        int elf2_r2 = stoi(list[3]);

        if ((elf1_r1 >= elf2_r1 && elf1_r2 <= elf2_r2) || (elf2_r1 >= elf1_r1 && elf2_r2 <= elf1_r2)) {
            ++isContained;
            cout << "     " << elf1_r1 << " : " << elf1_r2 << " : " << elf2_r1 << " : " << elf2_r2 << endl;
        }

        cout << s << " : " << elf1_r1 << " : " << elf1_r2 << " : " << elf2_r1 << " : " << elf2_r2 << endl;
    }
    cout << "total contained is " << isContained << endl;
}

void day4_part2()
{
    std::ifstream invFile;
    string s;

    invFile.open("elf_ranges.txt");
    int noOverlapCount = 0;
    int overlapCount = 0;
    while (std::getline(invFile, s)) {
        //int pos = s.find(',');
        //string s1 = s.substr(0, pos);
        //string s2 = s.substr(pos + 1, string::npos);
        vector<string> list = str_split(s, "[,-]");

        cout << s << endl;
        //for (const auto x : list) {
        //    cout << "    " << x << endl;
        //}

        int elf1_r1 = stoi(list[0]);
        int elf1_r2 = stoi(list[1]);
        int elf2_r1 = stoi(list[2]);
        int elf2_r2 = stoi(list[3]);

        int overlap = 0;
        for (int i = elf1_r1; i <= elf1_r2; ++i) {
            if (i >= elf2_r1 && i <= elf2_r2) {
                overlap = 1;
            }
        }

        for (int i = elf2_r1; i <= elf2_r2; ++i) {
            if (i >= elf1_r1 && i <= elf1_r2) {
                overlap = 1;
            }
        }

        
        if ((elf1_r2 < elf2_r1 || elf1_r1 > elf2_r2) || (elf2_r2 < elf1_r1 || elf2_r1 > elf1_r2)) {
            ++noOverlapCount;
            cout << s << " : " << elf1_r1 << " : " << elf1_r2 << " : " << elf2_r1 << " : " << elf2_r2 << endl;
            if (overlap) {
                cout << overlap << " BUG, should be overlap" << endl;
                exit(0);
            }
        } else {
            ++overlapCount;
            if (! overlap) {
                cout << overlap << " BUG, should be no overlap" << endl;
                exit(0);
            }
        }


    }
    cout << "total overlap is " << overlapCount << endl;
    cout << "total no overlap is " << noOverlapCount << endl;
}

int main()
{
    day4_part2();
}
