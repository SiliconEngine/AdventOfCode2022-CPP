/**
 * Advent of Code 2022 challenge, Day 17.
 * Link: https://adventofcode.com/2022/day/17
 *
 * Challenge: Simulate falling rocks and calculate height of tower of rocks.
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
#include <chrono>

using namespace std;

typedef chrono::high_resolution_clock Clock;

enum CollType { CLEAR, BOTTOM, SIDE, COLLISION };

class Rock
{
    int x = 0;
    int y = 0;

public:
    void setXY(int new_x, int new_y) {
        x = new_x;
        y = new_y;
    }

    // Define as abstract with "= 0".
    virtual CollType testColl(vector<string>&, int, int) = 0;
    virtual void setRock(vector<string>&, int, int) = 0;
    virtual string type() = 0;
    virtual int height() = 0;

};

class Dash : public Rock
{
public:
    int height() {
        return 1;
    }

    CollType testColl(vector<string>& chamber, int x, int y) {
        if (y < 0)
            return BOTTOM;
        if (x < 0 || x + 3 > 6)
            return SIDE;
        if (chamber[y][x] != '.' || chamber[y][x + 1] != '.' || chamber[y][x + 2] != '.' || chamber[y][x + 3] != '.')
            return COLLISION;
	return CLEAR;
    }

    void setRock(vector<string>& chamber, int x, int y) {
        chamber[y][x] = '#';
        chamber[y][x + 1] = '#';
        chamber[y][x + 2] = '#';
        chamber[y][x + 3] = '#';
    }

    string type()
    {
        return "Dash";
    }
};

class Plus : public Rock
{
public:
    int height() {
        return 3;
    }

    CollType testColl(vector<string>& chamber, int x, int y) {
        if (y-2 < 0)
            return BOTTOM;
        if (x < 0 || x + 2 > 6)
            return SIDE;
        if (chamber[y][x+1] != '.' || chamber[y-1][x] != '.' || chamber[y-1][x + 1] != '.' || chamber[y-1][x + 2] != '.' || chamber[y-2][x+1] != '.')
            return COLLISION;
		return CLEAR;
    }

    void setRock(vector<string>& chamber, int x, int y) {
        chamber[y][x + 1] = '#';
        chamber[y - 1][x] = '#';
        chamber[y - 1][x + 1] = '#';
        chamber[y - 1][x + 2] = '#';
        chamber[y - 2][x + 1] = '#';
    }

    string type()
    {
        return "Plus";
    }
};

class Ell : public Rock
{
public:
    int height() {
        return 3;
    }

    CollType testColl(vector<string>& chamber, int x, int y) {
        if (y-2 < 0)
            return BOTTOM;
        if (x < 0 || x + 2 > 6)
            return SIDE;
        if (chamber[y][x+2] != '.' || chamber[y-1][x+2] != '.' || chamber[y-2][x] != '.' || chamber[y-2][x+1] != '.' || chamber[y-2][x+2] != '.')
            return COLLISION;
	return CLEAR;
    }

    void setRock(vector<string>& chamber, int x, int y) {
        chamber[y][x+2] = '#';
        chamber[y-1][x+2] = '#';
        chamber[y-2][x] = '#';
        chamber[y-2][x+1] = '#';
        chamber[y-2][x+2] = '#';
    }

    string type()
    {
        return "Ell";
    }
};

class Bar : public Rock
{
public:
    int height() {
        return 4;
    }

    CollType testColl(vector<string>& chamber, int x, int y) {
        if (y-3 < 0)
            return BOTTOM;
        if (x < 0 || x > 6)
            return SIDE;
        if (chamber[y][x] != '.' || chamber[y-1][x] != '.' || chamber[y-2][x] != '.' || chamber[y-3][x] != '.')
            return COLLISION;
	return CLEAR;
    }

    void setRock(vector<string>& chamber, int x, int y) {
        chamber[y][x] = '#';
        chamber[y - 1][x] = '#';
        chamber[y - 2][x] = '#';
        chamber[y - 3][x] = '#';
    }

    string type()
    {
        return "Bar";
    }
};

class Box : public Rock
{
public:
    int height() {
        return 2;
    }

    CollType testColl(vector<string>& chamber, int x, int y) {
        if (y-1 < 0)
            return BOTTOM;
        if (x < 0 || x+1 > 6)
            return SIDE;
        if (chamber[y][x] != '.' || chamber[y][x+1] != '.' || chamber[y-1][x] != '.' || chamber[y-1][x+1] != '.')
            return COLLISION;
	return CLEAR;
    }

    void setRock(vector<string>& chamber, int x, int y) {
        chamber[y][x] = '#';
	chamber[y][x+1] = '#';
	chamber[y-1][x] = '#';
	chamber[y-1][x+1] = '#';
    }

    string type()
    {
        return "Box";
    }
};

void dumpChamber(vector<string>& chamber) {
    char num[20];

    cout << "========" << endl;
    for (int y = chamber.size() - 1; y >= 0; --y) {
        sprintf_s(num, sizeof(num), "%5d", y);
        cout << num << ": " << chamber[y] << endl;
    }
    cout << "========" << endl;
}

int getHeight(vector<string>& chamber) {
    int tower = 0;
    for (tower = chamber.size() - 1; tower >= 0; --tower) {
        if (chamber[tower] != ".......")
            break;
    }
    return tower + 1;
}

/**
 * Part 1: This method simulates the falling of a fixed number of rocks
 * (2022) in a chamber. It calculates the final height of the pile of rocks
 * after all rocks have fallen.
 *
 * The method iterates through each rock, simulating its movement and
 * handling collisions with the chamber walls, floor, or other rocks.
 */
void part1()
{
    string s;
    string q;
    ifstream in;
    string windList;

    Rock* rocks[5];
    rocks[0] = new Dash;
    rocks[1] = new Plus;
    rocks[2] = new Ell;
    rocks[3] = new Bar;
    rocks[4] = new Box;

    in.open("wind_x.txt");
    getline(in, windList);
    cout << windList << endl;

    vector<string> chamber;
    chamber.push_back(".......");
    chamber.push_back(".......");
    chamber.push_back(".......");
    chamber.push_back(".......");

#define MAX_ROCKS 2022

//enum CollType { CLEAR, BOTTOM, SIDE, COLLISION };
    int curRock = 0;
    int curWind = 0;
    for (__int64 i = 0; i < MAX_ROCKS; ++i) {
	CollType coll;
	Rock* rock = rocks[curRock++];
        int x, y;
	if (curRock >= 5)
	    curRock = 0;

        // Figure out if we need to add space to top
        int h = rock->height();

        //cout << "FIGURE SPACE, h= " << h << ", CHAMBER IS : " << endl;
	//dumpChamber(chamber);

        // Find first non-empty
        int empCount = 0;
        for (y = chamber.size() - 1; y >= 0; --y) {
            if (chamber[y] != ".......")
                break;
            ++empCount;
        }

        int need = h + 3;
        while (empCount < need) {
            chamber.push_back(".......");
            ++empCount;
        }
        //cout << "ABOUT TO DROP, CHAMBER IS:" << endl;
	//dumpChamber(chamber);

        // Drop rock
	y = chamber.size() - empCount + need - 1;
	x = 2;

        for (;;) {
	    //cout << "Current rock: " << rock->type() << " @ [" << x << ", " << y << "]" << endl;

            char wind = windList[curWind++];
            if (curWind >= windList.length())
                curWind = 0;

            // Move from the wind
            int dir = (wind == '<' ? -1 : 1);
            x += dir;
            coll = rock->testColl(chamber, x, y);
            if (coll == SIDE || coll == COLLISION)
                x -= dir;
	    //cout << "    After wind: [" << x << ", " << y << "]" << endl;

            // Move down
            --y;
            coll = rock->testColl(chamber, x, y);
	    //cout << "    Move: [" << x << ", " << y << "], test is " << coll << endl;
            if (coll == BOTTOM || coll == COLLISION) {
                ++y;
                //cout << "    Move back, setting rock at [" << x << ", " << y << "], test is " << endl;
                rock->setRock(chamber, x, y);
                break;
            }

            // Continue movement
        }
    }

    //dumpChamber(chamber);

    // Find first non-empty
    int tower = 0;
    for (tower = chamber.size() - 1; tower >= 0; --tower) {
	if (chamber[tower] != ".......")
	    break;
    }
    cout << "Tower is " << tower+1 << endl;
}

/**
 * Part 2: This method extends the simulation for a much larger number of
 * rocks (1 trillion). Due to the large number, it includes an optimization
 * to detect patterns in rock falling and calculates the final height without
 * simulating each individual rock.
 *
 * It handles the large iteration count by skipping iterations based on
 * detected patterns in the rock pile's growth.
 */
void part2()
{
    string s;
    string q;
    ifstream in;
    string windList;

    auto t1 = Clock::now();

    Rock* rocks[5];
    rocks[0] = new Dash;
    rocks[1] = new Plus;
    rocks[2] = new Ell;
    rocks[3] = new Bar;
    rocks[4] = new Box;

    in.open("wind.txt");
    getline(in, windList);
    cout << windList << endl;

    vector<string> chamber;
    chamber.push_back(".......");
    chamber.push_back(".......");
    chamber.push_back(".......");
    chamber.push_back(".......");

//#define MAX_ROCKS 2022
#define MAX_ROCKS 1000000000000
//#define MAX_ROCKS 10000

//enum CollType { CLEAR, BOTTOM, SIDE, COLLISION };
    int curRock = 0;
    int curWind = 0;
    int lastCurWind = 999999;
    __int64 extraHeight = 0;
    int counter = 0;
    __int64 lastHeight = 0;
    __int64 lastI = 0;
    string lastPatt = "";

    for (__int64 i = 0; i < MAX_ROCKS; ++i) {
        if (curRock == 0 && curWind < lastCurWind) {
        //if (curRock == 0 && curWind == 0) {
            __int64 curH = getHeight(chamber) + extraHeight;
            __int64 diff = curH - lastHeight;
            __int64 i_diff = i - lastI;
            //cout << "At i = " << i << ", height is " << curH << endl;
			cout << "At curRock = " << curRock << ", curWind = " << curWind << ", i = " << i << ", idiff = " << i_diff << ", height is " << curH << ", diff = " << diff << endl;
            lastHeight = curH;
            lastCurWind = 10; // 10; // 4
			lastI = i;

            string pattern = to_string(i_diff) + "-" + to_string(diff);
            if (pattern == lastPatt) {
                cout << "found pattern" << endl;
                __int64 amtLeft = MAX_ROCKS - i;
                __int64 repeatCount = amtLeft / i_diff;
                cout << "Repeat count: " << repeatCount << endl;
                i += repeatCount * i_diff;
                extraHeight += repeatCount * diff;
            }
            lastPatt = pattern;
        }

		//int curH = getHeight(chamber);
        //cout << "At curRock = " << curRock << ", curWind = " << curWind << ", i = " << i << ", height is " << curH << endl;

	CollType coll;
	Rock* rock = rocks[curRock++];
        int x, y;
	if (curRock >= 5)
		curRock = 0;

        // Figure out if we need to add space to top
        int h = rock->height();

        //cout << "FIGURE SPACE, h= " << h << ", CHAMBER IS : " << endl;
	//dumpChamber(chamber);

        // Find first non-empty
        int empCount = 0;
        for (y = chamber.size() - 1; y >= 0; --y) {
            if (chamber[y] != ".......")
                break;
            ++empCount;
        }

        int need = h + 3;
        while (empCount < need) {
            chamber.push_back(".......");
            ++empCount;
        }

        while (chamber.size() > 20000) {
            chamber.erase(chamber.begin());
            ++extraHeight;
        }

        //cout << "ABOUT TO DROP, CHAMBER IS:" << endl;
	//dumpChamber(chamber);

        // Drop rock
	y = chamber.size() - empCount + need - 1;
	x = 2;

        for (;;) {
            char wind = windList[curWind++];
            if (curWind >= windList.length())
                curWind = 0;

            // Move from the wind
            int dir = (wind == '<' ? -1 : 1);
            x += dir;
            coll = rock->testColl(chamber, x, y);
            if (coll == SIDE || coll == COLLISION)
                x -= dir;

            // Move down
            --y;
            coll = rock->testColl(chamber, x, y);
            if (coll == BOTTOM || coll == COLLISION) {
                ++y;
                rock->setRock(chamber, x, y);
                break;
            }

            // Continue movement
        }
    }

    //dumpChamber(chamber);

    // Find first non-empty
    int tower = 0;
    for (tower = chamber.size() - 1; tower >= 0; --tower) {
	if (chamber[tower] != ".......")
	    break;
    }
    cout << "Tower is " << tower+1 << endl;
    cout << "Extra is " << extraHeight << endl;
    cout << "Total is " << extraHeight+tower+1 << endl;

    {
        __int64 i = MAX_ROCKS;
        __int64 curH = getHeight(chamber) + extraHeight;
        __int64 diff = curH - lastHeight;
        __int64 i_diff = i - lastI;
        //cout << "At i = " << i << ", height is " << curH << endl;
        cout << "At curRock = " << curRock << ", curWind = " << curWind << ", i = " << i << ", idiff = " << i_diff << ", height is " << curH << ", diff = " << diff << endl;
    }
}

int main()
{
    part2();
}
