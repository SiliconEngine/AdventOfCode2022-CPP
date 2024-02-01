/**
 * Advent of Code 2022 challenge, Day 9.
 * Link: https://adventofcode.com/2022/day/9
 *
 * Challenge: Simulate rope motion and calculate number of positions visited
 * at least once.
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

int sgn(int val) {
    return (0 < val) - (val < 0);
}

/**
 * Part 1: Simulate the movement of a rope with two knots (head and tail)
 * based on a series of given directions and distances. It calculates the
 * number of unique positions that the tail of the rope occupies at least once.
 */
void part1()
{
    string s;
    string q;

    //istringstream in("mjqjpqmgbljsphdztnvjfqwrcgsmlb");
    ifstream in;
    in.open("movesx.txt");

    map<string, bool> check;

    int hx = 1, hy = 5, tx = 1, ty = 5;
    while (getline(in, s)) {
        cout << s << endl;

        vector<string> tokens = str_split(s, " ");
        char cmd = tokens[0][0];
        int dist = stoi(tokens[1]);

        for (int i = 0; i < dist; ++i) {
	    switch (cmd) {
	    case 'L':
                --hx;
		break;
	    case 'R':
                ++hx;
		break;
	    case 'U':
                --hy;
		break;
	    case 'D':
                ++hy;
		break;
	    default:
		cout << "BAD";
		exit(0);
	    }

            // Update the tail
            int dx = abs(hx - tx);
            int dy = abs(hy - ty);
            int sx = sgn(hx - tx);
            int sy = sgn(hy - ty);

            if (dx > 0 && dy > 1 || dy > 0 && dx > 1) {
                // Need diagonal move

                tx += sx;
                ty += sy;

            } else if (dx > 1) {
                // Need x move

                tx += sx;
            } else if (dy > 1) {
                // Need y move

                ty += sy;
            }

            string key = to_string(tx) + " " + to_string(ty);
            check[key] = true;

	    cout << "   " << i << ": HEAD: [" << hx << ", " << hy << "] TAIL : [" << tx << ", " << ty << "] " << endl;
	}
    }

    int size = check.size();
    cout << "size is " << size;
}

/**
 * part2 - This method extends the rope simulation from part1 to include a rope
 * consisting of ten knots. The head of the rope moves according to a series
 * of commands, and each subsequent knot follows the preceding knot using
 * the same movement rules.
 */
void part2()
{
    string s;
    string q;

    //istringstream in("mjqjpqmgbljsphdztnvjfqwrcgsmlb");
    ifstream in;
    in.open("moves.txt");

    //map<string, bool> check;
    set<string> check;

    int init_x = 30;
    int init_y = 50;

    int hx = init_x;
    int hy = init_y;

    int tails_x[9];
    int tails_y[9];
    for (int i = 0; i < 9; ++i) {
        tails_x[i] = init_x;
        tails_y[i] = init_y;
    }

    while (getline(in, s)) {
        cout << s << endl;

        vector<string> tokens = str_split(s, " ");
        char cmd = tokens[0][0];
        int dist = stoi(tokens[1]);

        for (int i = 0; i < dist; ++i) {
	    switch (cmd) {
	    case 'L':
                --hx;
		break;
	    case 'R':
                ++hx;
		break;
	    case 'U':
                --hy;
		break;
	    case 'D':
                ++hy;
		break;
	    default:
		cout << "BAD";
		exit(0);
	    }

            int cur_hx = hx;
            int cur_hy = hy;

            // Update the tails
            for (int t = 0; t < 9; ++t) {
                int tx = tails_x[t];
                int ty = tails_y[t];
                //cout << "    TAIL " << t << " IS [" << tx << ", " << ty << "], HEAD IS [" << cur_hx << ", " << cur_hy << "]" << endl;

                int dx = abs(cur_hx - tx);
                int dy = abs(cur_hy - ty);
                int sx = sgn(cur_hx - tx);
                int sy = sgn(cur_hy - ty);

                if (dx > 0 && dy > 1 || dy > 0 && dx > 1) {
                    // Need diagonal move

                    tx += sx;
                    ty += sy;

                }
                else if (dx > 1) {
                    // Need x move

                    tx += sx;
                }
                else if (dy > 1) {
                    // Need y move

                    ty += sy;
                }

                tails_x[t] = tx;
                tails_y[t] = ty;
                //cout << "    TAIL " << t << " IS NOW [" << tx << ", " << ty << "]" << endl;

                //cout << "   " << i << ": HEAD: [" << cur_hx << ", " << cur_hy << "] TAIL : [" << tx << ", " << ty << "] " << endl;

                cur_hx = tx;
                cur_hy = ty;
            }

	    string key = to_string(tails_x[8]) + " " + to_string(tails_y[8]);
	    //check[key] = true;
            check.insert(key);

            cout << "TAIL WAS: " << key << endl;
	}
    }

    int size = check.size();
    cout << "size is " << size;
}

int main()
{
    part2();
}
