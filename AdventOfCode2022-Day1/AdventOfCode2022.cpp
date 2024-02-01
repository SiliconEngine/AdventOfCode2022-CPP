/**
 * Advent of Code 2022 challenge, Day 1.
 * Link: https://adventofcode.com/2022/day/1
 *
 * Challenge: Calculate total calories carried by elves.
 *
 * @author Tim Behrendsen
 * @version 1.0
 */

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

void day1_part1()
{
    std::ifstream invFile;
    string s;

    invFile.open("elf_items.txt");
    int total = 0;
    int elfNum = 0;
    bool flag;
    int maxElf = 0, maxTotal = 0;
    do {
        flag = (bool)std::getline(invFile, s);

        cout << s.length() << endl;
        if (flag && s.length() > 0) {
            int n = stoi(s);
            total += n;
            cout << "int: " << n << endl;
        }
        else if (total > 0) {
            ++elfNum;
            cout << "Elf number " << elfNum << " is " << total << endl;
            if (total > maxTotal) {
                maxTotal = total;
                maxElf = elfNum;
            }
            total = 0;
        }
        //cout << s << endl;
    } while (flag);

    cout << "Max elf is " << maxElf << ", total is " << maxTotal;
}

void day1_part2()
{
    std::ifstream invFile;
    string s;

    vector<int> elfInv = {};

    invFile.open("elf_items.txt");
    int total = 0;
    int elfNum = 0;
    bool flag;

    do {
        flag = (bool)std::getline(invFile, s);

        if (flag && s.length() > 0) {
            int n = stoi(s);
            total += n;
        }
        else if (total > 0) {
            ++elfNum;
            cout << "Elf number " << elfNum << " is " << total << endl;
            elfInv.push_back(total);
            //elfInv[elfNum++] = total;
            ++elfNum;

            total = 0;
        }
        //cout << s << endl;
    } while (flag);

    sort(elfInv.begin(), elfInv.end(), greater<int>());

    for (int i = 0; i < elfInv.size(); ++i) {
        cout << "Elf " << i << ": " << elfInv[i] << endl;
    }

    cout << "First: " << elfInv[0] << endl;
    cout << "2nd: " << elfInv[1] << endl;
    cout << "3rd: " << elfInv[2] << endl;
    cout << "Total: " << elfInv[0]+elfInv[1]+elfInv[2] << endl;
}


int main()
{
    day1_part2();
}
