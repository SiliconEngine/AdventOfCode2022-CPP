/**
 * Advent of Code 2022 challenge, Day 6.
 * Link: https://adventofcode.com/2022/day/6
 *
 * Challenge: Decode communication stream and find start-of-message markers.
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

struct Move {
    char myMove;
    int score;
};

bool checkDup(string q)
{
    map <char, bool> chk;
    cout << "Checking: " << q << endl;
    for (auto c : q) {
        if (chk.find(c) != chk.end()) {
            cout << "true" << endl;
            return true;
        }
        chk[c] = true;
    }
    cout << "false" << endl;
    return false;
}

bool checkDupx(string q)
{
    string chk = "";
    cout << "Checking: " << q << endl;
    for (auto c : q) {
        cout << "   check (" << chk << "): " << c << endl;
        if (chk.find(c) != string::npos) {
            cout << "true" << endl;
            return true;
        }
        chk += c;
    }
    cout << "false" << endl;
    return false;
}

/**
 * Part 1: Process the data stream to find the first start-of-packet marker,
 * identified as a sequence of 4 characters where all characters are distinct.
 * This method reads from a file character by character, maintaining a rolling
 * sequence of 4 characters, checks for uniqueness within this sequence.
 * The count of characters processed is returned when a valid start-of-packet
 * marker is found.
 */
void day6_part1()
{
    string s;
    string q;
    int count = 0;

    //istringstream in("mjqjpqmgbljsphdztnvjfqwrcgsmlb");
    ifstream in;
    in.open("stream.txt");
    while (! in.eof()) {
        char c = in.get();
        cout << c << " ";
        ++count;

        q = q + c;
        if (q.length() > 4) {
            q = q.substr(1);
        }

        cout << "queue: " << q << endl;

        if (q.length() > 3 && q[0] != q[1] && q[0] != q[2] && q[0] != q[3]
            && q[1] != q[2] && q[1] != q[3]
            && q[2] != q[3])
        {
            break;
        }
    }

    cout << "count is " << count << endl;
}

/**
 * Part 2: Process the data stream to find the first start-of-message marker,
 * which is identified as a sequence of 14 distinct characters.
 * This method reads from a file character by character, maintaining a
 * rolling sequence of 14 characters, and checks for uniqueness using the
 * checkDup method. The count of characters processed is returned when
 * a valid start-of-message marker is found.
 */
void day6_part2()
{
    string s;
    string q;
    int count = 0;

    //istringstream in("mjqjpqmgbljsphdztnvjfqwrcgsmlb");
    ifstream in;
    in.open("stream.txt");
    while (! in.eof()) {
        char c = in.get();
        cout << c << " ";
        ++count;

        q = q + c;
        if (q.length() > 14) {
            q = q.substr(1);
        }

        cout << "queue: " << q << endl;

        if (q.length() >= 14 && !checkDup(q)) {
            break;
        }
    }

    cout << "count is " << count << endl;
}


int main()
{
    day6_part2();
}
