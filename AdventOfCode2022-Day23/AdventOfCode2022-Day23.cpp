/**
 * Advent of Code 2022 challenge, Day 23.
 * Link: https://adventofcode.com/2022/day/23
 *
 * Challenge: Simulate the Elves' travel process and find the round where no
 * elf moves.
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
#include <unordered_map>
#include <regex>
#include <queue>
#include <numeric>
#include <queue>
#include <chrono>

using namespace std;

typedef chrono::high_resolution_clock Clock;

ostream& operator<<(ostream& out, const vector<string>& v) {
    bool first = true;
    for (auto s : v) {
        if (!first)
            cout << ",";
        cout << s;
        first = false;
    }
    return out;
}

ostream& operator<<(ostream& out, const vector<int>& v) {
    bool first = true;
    for (auto s : v) {
        if (!first)
            cout << ",";
        cout << s;
        first = false;
    }
    return out;
}

struct Elf
{
    int x;
    int y;
    bool moving;
    int propX;
    int propY;
};

ostream& operator<<(ostream& out, const Elf& e) {
    cout << "[" << e.x << ", " << e.y << "]";
    return out;
}
ostream& operator<<(ostream& out, const Elf* e) {
    cout << "[" << e->x << ", " << e->y << "]";
    return out;
}

enum Dir { NORTH, SOUTH, WEST, EAST };

string makeKey(int x, int y) {
    return to_string(x) + "," + to_string(y);
}

int countScore(vector<Elf*> elves) {
    int min_x = 9999999, min_y = 9999999, max_x = -9999999, max_y = -9999999;

    for (Elf* e : elves) {
        if (e->x < min_x)
            min_x = e->x;
        if (e->x > max_x)
            max_x = e->x;
        if (e->y < min_y)
            min_y = e->y;
        if (e->y > max_y)
            max_y = e->y;
    }
    cout << "min_x = " << min_x << ", min_y = " << min_y << ", max_x = " << max_x << ", max_y = " << max_y << endl;

    return (max_x - min_x + 1) * (max_y - min_y + 1) - elves.size();
}


void dsp(vector<Elf*> elves) {
    cout << "MAP:" << endl;
    int min_x = 9999999, min_y = 9999999, max_x = -9999999, max_y = -9999999;

    for (Elf* e : elves) {
        if (e->x < min_x)
            min_x = e->x;
        if (e->x > max_x)
            max_x = e->x;
        if (e->y < min_y)
            min_y = e->y;
        if (e->y > max_y)
            max_y = e->y;
    }
    cout << "min_x = " << min_x << ", min_y = " << min_y << ", max_x = " << max_x << ", max_y = " << max_y << endl;

    int offsetX = 0;
    int offsetY = 0;
    if (min_x < 0) {
        offsetX = -min_x;
    }
    if (min_y < 0) {
        offsetY = -min_y;
    }

    if (min_x > 0) {
        min_x = 0;
    }
    if (min_y > 0) {
        min_y = 0;
    }


    vector<string> out;
    int len = max_x - min_x + 1;
    for (int i = min_y + offsetY; i <= max_y + offsetY; ++i) {
        out.push_back(string(len, '.'));
    }

    for (Elf* e : elves) {
        out[e->y + offsetY][e->x + offsetX] = '#';
    }

    for (string s : out) {
        cout << s << endl;
    }
}

void part2()
{
    string s;
    ifstream in;
    Dir dirs[4] = { NORTH, SOUTH, WEST, EAST };
    string dirnames[4];
    dirnames[NORTH] = "NORTH";
    dirnames[SOUTH] = "SOUTH";
    dirnames[WEST] = "WEST";
    dirnames[EAST] = "EAST";

    int curDir = 0;

    in.open("elves.txt");
#define NUM_ROUNDS 1000000000

    vector<Elf*> elves;
    unordered_map<string, Elf*> pos;

    int y = 0;
    while (getline(in, s)) {
        cout << s << endl;
        int x = 0;
        for (char c : s) {
            if (c == '#') {
                Elf* e = new Elf;
                e->x = x;
                e->y = y;
                e->moving = false;
                e->propX = 0;
                e->propY = 0;
                elves.push_back(e);

                string key = makeKey(x, y);
                pos[key] = e;
            }
            ++x;
        }

        ++y;
    }
	dsp(elves);

    for (int round = 0; round < NUM_ROUNDS; ++round) {
        unordered_map<string, int> prop;

        for (Elf* e : elves) {
			string key1, key2, key3;
            //cout << "Checking elf " << e << endl;

            int newDir = -1;
            int goodCount = 0;
            for (int i = 0; i < 4; ++i) {
                int dir = dirs[(curDir + i) % 4];
                if (dir == NORTH) {
                    key1 = makeKey(e->x - 1, e->y - 1);
                    key2 = makeKey(e->x, e->y - 1);
                    key3 = makeKey(e->x + 1, e->y - 1);
                }
                else if (dir == SOUTH) {
                    key1 = makeKey(e->x - 1, e->y + 1);
                    key2 = makeKey(e->x, e->y + 1);
                    key3 = makeKey(e->x + 1, e->y + 1);
                }
                else if (dir == EAST) {
                    key1 = makeKey(e->x + 1, e->y - 1);
                    key2 = makeKey(e->x + 1, e->y);
                    key3 = makeKey(e->x + 1, e->y + 1);
                }
                else if (dir == WEST) {
                    key1 = makeKey(e->x - 1, e->y - 1);
                    key2 = makeKey(e->x - 1, e->y);
                    key3 = makeKey(e->x - 1, e->y + 1);
                }

                //cout << "    Checking dir " << dirnames[dir] << endl;
                if (pos.count(key1) == 0 && pos.count(key2) == 0 && pos.count(key3) == 0) {
                    ++goodCount;
                    //cout << "    WAS OK" << endl;
                    if (newDir < 0)
						newDir = dir;
                }
            }

			if (goodCount == 4) {
                //cout << "    All directions good, not moving" << endl;
                newDir = -1;
			}

            if (newDir < 0) {
                e->moving = false;
                //cout << "    NO MOVE" << endl;
            } else {
                //cout << "    Tentative move to " << dirnames[newDir] << endl;
                e->moving = true;
                switch (newDir) {
                case NORTH:
                    e->propX = e->x;
                    e->propY = e->y-1;
                    break;
                case SOUTH:
                    e->propX = e->x;
                    e->propY = e->y+1;
                    break;
                case EAST:
                    e->propX = e->x+1;
                    e->propY = e->y;
                    break;
                case WEST:
                    e->propX = e->x-1;
                    e->propY = e->y;
                    break;
                }

                string chkKey = makeKey(e->propX, e->propY);
                //cout << "Set prop for key " << chkKey << endl;
                ++prop[chkKey];
            }
        }

        //cout << "PROP IS" << endl;
        //for (auto p : prop) {
            //cout << "    " << p.first << ": " << p.second << endl;
        //}

        int numMoves = 0;
        for (Elf* e : elves) {
            if (e->moving) {
		string key = makeKey(e->propX, e->propY);
		//cout << "Checking ok to move for elf " << e << "(" << key << ") count is " << prop[key] << endl;
		if (prop[key] == 1) {
		    pos.erase(makeKey(e->x, e->y));
		    e->x = e->propX;
		    e->y = e->propY;
		    pos[key] = e;
		    ++numMoves;
		}
            }
        }

        if (numMoves == 0) {
            cout << "Did not do a move, round = " << round + 1 << endl;
            break;
        }

        cout << endl << "ROUND " << round + 1 << ", moves = " << numMoves << endl;

        curDir = (curDir + 1) % 4;
    }

    cout << "Score: " << countScore(elves) << endl;
}

int main()
{
    part2();
}
