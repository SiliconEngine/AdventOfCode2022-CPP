/**
 * Advent of Code 2022 challenge, Day 19.
 * Link: https://adventofcode.com/2022/day/19
 *
 * Challenge: Calculate largest number of geodes that can be produced by
 * machine blueprints.
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
#include <chrono>

using namespace std;

vector<string> get_matches(const string& str, const string& pattern)
{
    vector<string> matches;
    regex regex_pattern(pattern);
    smatch match_results;
    string search_str = str;

    while (regex_search(search_str, match_results, regex_pattern))
    {
        matches.push_back(match_results[0]);
        search_str = match_results.suffix().str();
    }

    return matches;
}

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

struct Blueprint {
    int num;
    int oreRobotCost;
    int clayRobotCost;
    int obsRobotOreCost;
    int obsRobotClayCost;
    int geodeRobotOreCost;
    int geodeRobotObsCost;
    int oreRobotCap;
    int clayRobotCap;
};

struct Resources {
    int numOre;
    int numClay;
    int numObs;
    int numGeode;
    int numOreRobots;
    int numClayRobots;
    int numObsRobots;
    int numGeodeRobots;
};

ostream& operator<<(ostream& out, const Resources& r) {
    cout << "numOre=" << r.numOre << ", numClay=" << r.numClay << ", numObs=" << r.numObs << ", numGeode=" << r.numGeode <<
        ", numOreRobots=" << r.numOreRobots << ", numClayRobots=" << r.numClayRobots << ", numObsRobots=" << r.numObsRobots <<
        ", numGeodeRobots=" << r.numGeodeRobots;
    return out;
}

int maxGeodes;
int numMinutes;
int maxBlueprints;

//#define NUM_MINUTES 24
//#define MAX_BLUEPRINTS 999
//#define NUM_MINUTES 32
//#define MAX_BLUEPRINTS 3

int triangle[] = { 0, 1, 3, 6, 10, 15, 21, 28, 36, 45, 55 };

void nextMin(int min, const Blueprint& b, Resources r) {
    ++min;

    Resources r2 = r;
    r2.numOre += r2.numOreRobots;
    r2.numClay += r2.numClayRobots;
    r2.numObs += r2.numObsRobots;
    r2.numGeode += r2.numGeodeRobots;
    //cout << "    Min " << min << ": collected " << r2 << endl;
	int minLeft = numMinutes - min;

    if (min == numMinutes) {
        if (r2.numGeode > maxGeodes) {
            maxGeodes = r2.numGeode;
            cout << "END: maxGeodes = " << maxGeodes << endl;
        }
        return;
    }

    if (true) {
        int t = minLeft + 1;
        int check = r.numGeode + r.numGeodeRobots * t + (t * (t - 1)) / 2;
        if (check < maxGeodes) {
            //cout << "PRUNE at " << minLeft << endl;
            return;
        }
    }

    if (min > numMinutes) {
        cout << "BAD";
        exit(0);
    }

    bool canBuildClay = (r.numOre >= b.clayRobotCost);
    bool canBuildObs = (r.numOre >= b.obsRobotOreCost && r.numClay >= b.obsRobotClayCost);
    bool canBuildGeode = (r.numOre >= b.geodeRobotOreCost && r.numObs >= b.geodeRobotObsCost);

    if (r.numOre >= b.oreRobotCost && r.numOreRobots < b.oreRobotCap) {
        Resources r3 = r2;
        //cout << "    Building ore robot" << endl;
        ++r3.numOreRobots;
        r3.numOre -= b.oreRobotCost;
        nextMin(min, b, r3);
    }

    if (r.numOre >= b.clayRobotCost && r.numClayRobots < b.obsRobotClayCost) {
        Resources r3 = r2;
        //cout << "    Building clay robot" << endl;
        ++r3.numClayRobots;
        r3.numOre -= b.clayRobotCost;
        nextMin(min, b, r3);
    }

    if (r.numOre >= b.obsRobotOreCost && r.numClay >= b.obsRobotClayCost && r.numObsRobots < b.geodeRobotObsCost) {
        Resources r3 = r2;
        //cout << "    Building obs robot" << endl;
        ++r3.numObsRobots;
        r3.numOre -= b.obsRobotOreCost;
        r3.numClay -= b.obsRobotClayCost;
        nextMin(min, b, r3);
    }

    if (r.numOre >= b.geodeRobotOreCost && r.numObs >= b.geodeRobotObsCost) {
        Resources r3 = r2;
        //cout << "    Building geode robot" << endl;
        ++r3.numGeodeRobots;
        r3.numOre -= b.geodeRobotOreCost;
        r3.numObs -= b.geodeRobotObsCost;

        int maxCost = max(b.geodeRobotOreCost - r3.numOre, b.geodeRobotObsCost - r3.numObs);
        if (0 && minLeft < maxCost) {
            int n = r3.numGeode + r3.numGeodeRobots * minLeft;
            //cout << "SHORT: numGeode = " << r3.numGeode << ", numGeodeRobots = " << r3.numGeodeRobots << ", minLeft = " << minLeft << ", maxCost = " << maxCost << " n = " << n << endl;
            if (n > maxGeodes) {
                maxGeodes = n;
		cout << "SHORT: maxGeodes = " << maxGeodes << endl;
	    }
	    return;
        }

        nextMin(min, b, r3);
    }

    nextMin(min, b, r2);
}

int calcMax(const Blueprint& b) {
    Resources r = { 0 };
    r.numOreRobots = 1;
    maxGeodes = 0;
    nextMin(0, b, r);

    return maxGeodes;
}

/**
 * Solves the Advent of Code Day 19 challenge for a given part (either part 1
 * or part 2).
 *
 * This method reads a set of robot blueprints from a file and determines the
 * maximum number of geodes that can be opened within a specified time limit
 * using each blueprint.
 * <p>
 * 1. Read and parse blueprints from a file, creating a Blueprint object for
 *    each.
 * 2. Depending on the part (part 1 or part 2), set the time limit and the
 *    number of blueprints to process.
 * 3. For each blueprint:
 *    a. Initialize the resource state with one ore-collecting robot.
 *    b. Use a recursive method (nextMin) to simulate each minute of the
 *       operation, making decisions on what type of robot to build next
 *       based on the blueprint specifications and current resource
 *       availability.
 *    c. Within nextMin, resources are gathered, and the possibility of
 *       constructing new robots is evaluated at each minute. The state is
 *       cloned to explore different building strategies.
 *    d. Track the maximum number of geodes that can be opened by the end of
 *       the time limit.
 * 4. For part 1, sum up the product of blueprint ID and maximum geodes for
 *    each blueprint. For part 2, calculate the product of maximum geodes for
 *    the first three blueprints.
 * 5. Return the total quality level for part 1 or the product for part 2.
 */
void part(int partNum)
{
    string s;
    string q;
    ifstream in;

    if (partNum == 1) {
        numMinutes = 24;
        maxBlueprints = 999;
    } else {
        numMinutes = 32;
        maxBlueprints = 3;
    }

    in.open("blueprint.txt");
    vector<Blueprint> blueList;

    while (getline(in, s)) {
        cout << s << endl;
        auto nums = get_matches(s, "\\d+");
        cout << nums << endl;

        Blueprint b;
	b.num = stoi(nums[0]);
	b.oreRobotCost = stoi(nums[1]);
	b.clayRobotCost = stoi(nums[2]);
	b.obsRobotOreCost = stoi(nums[3]);
	b.obsRobotClayCost = stoi(nums[4]);
	b.geodeRobotOreCost = stoi(nums[5]);
	b.geodeRobotObsCost = stoi(nums[6]);
        b.oreRobotCap = max(max(max(b.oreRobotCost, b.clayRobotCost), b.obsRobotOreCost), b.geodeRobotOreCost);
        b.clayRobotCap = b.obsRobotClayCost;
        blueList.push_back(b);
    }

    int maxCount = maxBlueprints;
    int total = 0;
    int prod = 1;
    for (auto b : blueList) {
        int num_g = calcMax(b);
        cout << "Max for blueprint " << b.num << " was " << num_g << endl;
        total += b.num * num_g;
        prod *= num_g;
        if (--maxCount == 0)
            break;
    }

    cout << "Total quality level is " << total << endl;
    cout << "Product was " << prod << endl;
}

int main()
{
    part(2);
}
