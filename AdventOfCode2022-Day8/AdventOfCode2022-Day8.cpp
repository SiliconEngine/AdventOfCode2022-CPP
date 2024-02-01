/**
 * Advent of Code 2022 challenge, Day 8.
 * Link: https://adventofcode.com/2022/day/8
 *
 * Challenge: From forest map calculate visibility distance and "scenic scores".
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

/**
 * Part 1: Calculates and prints the number of visible trees in the grid.
 * Visibility is determined by checking each tree's height against other trees
 * in the same row and column. Trees are considered visible if no other tree
 * of equal or greater height blocks them when viewed from the grid's edges.
 */
void day8_part1()
{
    string s;
    string q;

    //istringstream in("mjqjpqmgbljsphdztnvjfqwrcgsmlb");
    ifstream in;
    in.open("trees.txt");

    vector<string> trees;
    while (getline(in, s)) {
        trees.push_back(s);
    }

    int numLines = trees.size();
    int visible = 0;
    for (int r = 0; r < numLines; ++r) {
        string row = trees[r];
        cout << row << endl;
        for (int c = 0; c < row.length(); ++c) {
            char ht = row[c];
            cout << "Checking r=" << r << ", c=" << c << " ht is " << ht << endl;

            bool vis = 1;
            for (int left = c - 1; left >= 0; --left) {
                //cout << "    left chk, left = " << left << ", ht = " << ht << ", tree = " << row[left] << endl;
                if (row[left] >= ht) {
                    //cout << "NOT VISIBLE" << endl;
                    vis = 0;
                    break;
                }
            }
            if (vis) {
                ++visible;
                cout << "coord " << r << ", " << c << " is visible from left (" << visible << ")" << endl;
                continue;
            }

            vis = 1;
            for (int right = c + 1; right < row.length(); ++right) {
                if (row[right] >= ht) {
                    vis = 0;
                    break;
                }
            }
            if (vis) {
                ++visible;
                cout << "coord " << r << ", " << c << " is visible from right (" << visible << ")" << endl;
                continue;
            }

            vis = 1;
            for (int up = r - 1; up >= 0; --up) {
                if (trees[up][c] >= ht) {
                    vis = 0;
                    break;
                }
            }
            if (vis) {
                ++visible;
                cout << "coord " << r << ", " << c << " is visible from up (" << visible << ")" << endl;
                continue;
            }

            vis = 1;
            for (int down = r + 1; down < numLines; ++down) {
                //cout << "    down chk, down = " << down << ", ht = " << ht << ", tree = " << trees[down][c] << endl;
                if (trees[down][c] >= ht) {
                    //cout << "NOT VISIBLE" << endl;
                    vis = 0;
                    break;
                }
            }
            if (vis) {
                ++visible;
                cout << "coord " << r << ", " << c << " is visible from down (" << visible << ")" << endl;
                continue;
            }

	    cout << "coord " << r << ", " << c << " is NOT VISIBLE" << endl;
        }
    }

    cout << "Total visible is " << visible << endl;
}

/**
 * Part 2: Calculates and prints the highest scenic score possible for any
 * tree in the grid. The scenic score is calculated by multiplying the viewing
 * distance in each of the four directions (up, down, left, right). The
 * viewing distance is the count of trees seen before encountering a tree of
 * the same or greater height or reaching the grid edge.
 */
void day8_part2()
{
    string s;
    string q;

    //istringstream in("mjqjpqmgbljsphdztnvjfqwrcgsmlb");
    ifstream in;
    in.open("trees.txt");

    vector<string> trees;
    while (getline(in, s)) {
        trees.push_back(s);
    }

    vector<pair<string, int>*> scores;

    int numLines = trees.size();
    int visible = 0;
    for (int r = 0; r < numLines; ++r) {
        string row = trees[r];
        cout << row << endl;
        for (int c = 0; c < row.length(); ++c) {
            char ht = row[c];
            //cout << "Checking r=" << r << ", c=" << c << " ht is " << ht << endl;

            int leftCount = 0;
            for (int left = c - 1; left >= 0; --left) {
                //cout << "    left chk, left = " << left << ", ht = " << ht << ", tree = " << row[left] << endl;
		++leftCount;
                if (row[left] >= ht) {
                    break;
                }
            }

            int rightCount = 0;
            for (int right = c + 1; right < row.length(); ++right) {
                ++rightCount;
                if (row[right] >= ht) {
                    break;
                }
            }

            int upCount = 0;
            for (int up = r - 1; up >= 0; --up) {
                ++upCount;
                if (trees[up][c] >= ht) {
                    break;
                }
            }

            int downCount = 0;
            for (int down = r + 1; down < numLines; ++down) {
                //cout << "    down chk, down = " << down << ", ht = " << ht << ", tree = " << trees[down][c] << endl;
                ++downCount;
                if (trees[down][c] >= ht) {
                    break;
                }
            }

            cout << "left = " << leftCount << ", right = " << rightCount << ", up = " << upCount << ", down = " << downCount << endl;
            int score = leftCount * rightCount * upCount * downCount;
            cout << "coord " << r << ", " << c << " (" << row[c] << ") score is " << score << endl;

            string key = to_string(r) + "-" + to_string(c);

            //pair<string, int> p = new pair<string, int>(key, score);
            auto *p = new pair<string, int>(key, score);
            scores.push_back(p);
        }
    }

    sort(scores.begin(), scores.end(), [](pair<string, int>* a, pair<string, int>* b) {
	return a->second < b->second;
    });

    for (auto p : scores) {
	cout << "Score: " << p->second << ", key is " << p->first << endl;
	//cout << "Score: " << p->second << ", key is " << p->first << end;
    }
}

int main()
{
    day8_part2();
}
