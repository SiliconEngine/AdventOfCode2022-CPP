/**
 * Advent of Code 2022 challenge, Day 14.
 * Link: https://adventofcode.com/2022/day/14
 *
 * Challenge: Simulate falling sand and calculate how much sand under scenarios.
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
#include <numeric>
#include <queue>
#include <windows.h>

using namespace std;

std::vector<std::string> str_split(const std::string& s, const char* pattern) {
    std::regex sep_regex = std::regex{ pattern };
    std::sregex_token_iterator iter(s.begin(), s.end(), sep_regex, -1);
    std::sregex_token_iterator end;
    return { iter, end };
}

int sgn(int val) {
    return (0 < val) - (val < 0);
}

void dspGrid(vector<string> grid)
{
    int min_x = 9999;
    int max_x = 0;
    for (auto s : grid) {
        int p = s.find_first_not_of('.');
        if (p != 0 && p != string::npos) {
            if (p < min_x)
                min_x = p;
        }

        p = s.find_last_not_of('.');
        if (p != 999 && p != string::npos) {
            if (p > max_x)
                max_x = p;
        }
    }

    min_x -= 5;
    if (min_x < 0) min_x = 0;
    max_x += 5;
    if (max_x > 999) max_x = 999;

    for (auto s : grid) {
        cout << s.substr(min_x, max_x - min_x) << endl;
    }
}

/**
 * Part 1: Simulate sand falling in a cave system until all sand falls into the abyss.
 *
 * This method initializes a grid based on input data representing cave structures.
 * It then simulates the movement of sand, starting from a fixed point, and tracks
 * how the sand falls through the grid. The simulation continues until sand no longer
 * comes to rest within the grid boundaries, indicating it has fallen into the abyss.
 * The method finally outputs the count of sand units that came to rest.
 */
void part1()
{
    string s;
    string q;

    ifstream in;
    in.open("caves.txt");

    vector<string> grid;

    grid.push_back(string(1000, '.'));
    grid[0][500] = '+';

    while (getline(in, s)) {
        cout << s << endl;

        auto list = str_split(s, " -> ");
        int lastx = -1;
        int lasty = -1;
        for (auto p : list) {
            auto c = str_split(p, ",");
            int x = stoi(c[0]);
            int y = stoi(c[1]);

            if (lastx < 0) {
                lastx = x;
                lasty = y;
                continue;
            }

            int dx = sgn(x - lastx);
            int dy = sgn(y - lasty);

            int tx = lastx;
            int ty = lasty;
            cout << "MOVE FROM [" << lastx << ", " << lasty << "] TO [" << x << ", " << y << "] dx=" << dx << ", dy=" << dy << endl;
            for(;;) {
                while (ty >= grid.size()) {
                    grid.push_back(string(1000, '.'));
				}
                cout << "SET: [" << tx << ", " << ty << "] size = " << grid.size() << endl;
                grid[ty][tx] = '#';

                if (tx == x && ty == y)
                    break;

                tx += dx;
                ty += dy;
            }

	    lastx = x;
	    lasty = y;
        }
    }

    // Simulate the movement of sand starting from a fixed source point.
    // The sand follows a specific set of rules to move: it tries to fall straight down,
    // then diagonally left, and finally diagonally right. If all paths are blocked, the
    // sand comes to rest. This loop continues for each sand unit until it either comes to
    // rest or falls out of the grid.
    int sandCount = 0;
    for (;;) {
        int sand_x = 500;
        int sand_y = 0;
        ++sandCount;

        while (++sand_y < grid.size()) {
            cout << "TESTING: [" << sand_x << ", " << sand_y << "]: ";

            if (grid[sand_y][sand_x] == '.') {
                cout << "    PATH BELOW" << endl;
                continue;
            }

            --sand_x;
            if (grid[sand_y][sand_x] == '.') {
                cout << "    PATH LEFT" << endl;
                continue;
            }

            sand_x += 2;
            if (grid[sand_y][sand_x] == '.') {
                cout << "    PATH RIGHT" << endl;
                continue;
            }

            --sand_y;
            --sand_x;
			cout << "BLOCKED" << endl;
            grid[sand_y][sand_x] = 'o';
            break;
        }

        cout << "    SAND DROP COMPLETE" << endl;
        if (sand_y == grid.size()) {
			cout << "    HIT BOTTOM" << endl;
            goto done;
        }

		//dspGrid(grid);
        //if (sandCount == 22) exit(0);
    }

done:
    dspGrid(grid);
    cout << "sand count is " << sandCount << endl;

}

/**
 * Part 2: Simulates falling sand with an additional floor until the source is blocked.
 *
 * This method is an extension of the part1 simulation. It includes an extra horizontal line
 * at the bottom, representing the cave floor. The simulation proceeds similar to part1, but
 * here, it continues until the sand blocks the source, effectively stopping further sand flow.
 * The total count of sand units that come to rest before the source is blocked is then output.
 */
void part2()
{
    string s;
    string q;

    ifstream in;
    in.open("caves.txt");

    // Initialize a grid representing the cave system. Each cell in the grid can either
    // be air ('.'), rock ('#'), or sand ('o').
    vector<string> grid;

    grid.push_back(string(1000, '.'));
    grid[0][500] = '+';

    while (getline(in, s)) {
        cout << s << endl;

        auto list = str_split(s, " -> ");
        int lastx = -1;
        int lasty = -1;
        for (auto p : list) {
            auto c = str_split(p, ",");
            int x = stoi(c[0]);
            int y = stoi(c[1]);

            if (lastx < 0) {
                lastx = x;
                lasty = y;
                continue;
            }

            int dx = sgn(x - lastx);
            int dy = sgn(y - lasty);

            int tx = lastx;
            int ty = lasty;
            cout << "MOVE FROM [" << lastx << ", " << lasty << "] TO [" << x << ", " << y << "] dx=" << dx << ", dy=" << dy << endl;
            for(;;) {
                while (ty >= grid.size()) {
                    grid.push_back(string(1000, '.'));
				}
                cout << "SET: [" << tx << ", " << ty << "] size = " << grid.size() << endl;
                grid[ty][tx] = '#';

                if (tx == x && ty == y)
                    break;

                tx += dx;
                ty += dy;
            }

	    lastx = x;
	    lasty = y;
        }
    }
    grid.push_back(string(1000, '.'));
    grid.push_back(string(1000, '#'));

    // Simulate the movement of sand starting from a fixed source point.
    // The sand follows a specific set of rules to move: it tries to fall straight down,
    // then diagonally left, and finally diagonally right. If all paths are blocked, the
    // sand comes to rest. This loop continues for each sand unit until it blocks the source.
    int sandCount = 0;
    for (;;) {
        int sand_x = 500;
        int sand_y = 0;
        ++sandCount;

        for (;;) {
            ++sand_y;
            //cout << "TESTING: [" << sand_x << ", " << sand_y << "]: ";

            if (grid[sand_y][sand_x] == '.') {
                //cout << "    PATH BELOW" << endl;
                continue;
            }

            --sand_x;
            if (grid[sand_y][sand_x] == '.') {
                //cout << "    PATH LEFT" << endl;
                continue;
            }

            sand_x += 2;
            if (grid[sand_y][sand_x] == '.') {
                //cout << "    PATH RIGHT" << endl;
                continue;
            }

            --sand_y;
            --sand_x;

	    //cout << "BLOCKED" << endl;
            grid[sand_y][sand_x] = 'o';
            break;
        }

        cout << "    SAND DROP COMPLETE" << endl;
        if (sand_y == 0) {
	    cout << "    HIT TOP" << endl;
            goto done;
        }

	//dspGrid(grid);
        //if (sandCount == 100) exit(0);
    }

done:
    dspGrid(grid);
    cout << "sand count is " << sandCount << endl;

}

int main()
{
    part2();
}
