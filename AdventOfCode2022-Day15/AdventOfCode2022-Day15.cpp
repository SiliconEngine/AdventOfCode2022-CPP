/**
 * Advent of Code 2022 challenge, Day 16.
 * Link: https://adventofcode.com/2022/day/16
 *
 * Challenge: Simulate elephants opening valves in a cavern map and find the optimal strategy.
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

vector<string> str_split(const string& s, const char* pattern) {
    regex sep_regex = regex{ pattern };
    sregex_token_iterator iter(s.begin(), s.end(), sep_regex, -1);
    sregex_token_iterator end;
    return { iter, end };
}

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

struct Sensor {
    int sx;
    int sy;
    int bx;
    int by;
    int dist;
};

typedef pair<int, int> Segment;

bool senContains(Sensor* sen, int x, int y) {
    if (sen->bx == x && sen->by == y) {
        return false;
    }
    int chkDist = abs(sen->sx - x) + abs(sen->sy - y);
    //cout << "    Check dist = " << chkDist << ", sensor dist is " << sen->dist << endl;
    if (chkDist > sen->dist) {
        return false;
    }

    return true;
}

typedef vector<Segment> SegList;

void dumpList(const SegList &list)
{
    for (auto s : list) {
        cout << "[" << s.first << " - " << s.second << "] ";
    }
    cout << endl;
}

SegList merge(const SegList& list)
{
    SegList newList;

    newList = list;

    if (newList.size() == 1) {
        return newList;
    }

    sort(newList.begin(), newList.end(), [](const Segment& a, const Segment& b) {
        return a.first < b.first;
    });

    SegList mergeList;
    Segment lastSeg = newList[0];
    for (int i = 1; i < newList.size(); ++i) {
        Segment seg = newList[i];

        if (lastSeg.second + 1 == seg.first) {
            // Right next to each other, combine

            lastSeg.second = seg.second;

        } else if (lastSeg.second < seg.first) {
            // No overlap, keep lastseg and move on

            mergeList.push_back(lastSeg);
            lastSeg = seg;
        } else if (seg.second <= lastSeg.second) {
            // Seg contained within lastseg, can just throw away

        } else {
            // Seg longer than lastseg, combine into lastSeg, throw away seg

            lastSeg.second = seg.second;
        }
    }
    mergeList.push_back(lastSeg);

    //cout << "MERGED:" << endl;
    //dumpList(mergeList);
    return mergeList;
}

/**
 * Part 1 calculates the maximum pressure that can be released within a
 * 30-minute window. The algorithm involves iterating through all valves,
 * determining the optimal sequence to open them to maximize the total pressure
 * released. It uses a recursive depth-first search approach to explore all
 * possible sequences of valve openings, considering the time constraints and
 * the pressure each valve can release.
 *
 * It tracks the current state (which valves are open, the current pressure,
 * total pressure released so far) and explores different paths by opening one
 * more valve at a time. The recursion terminates when either all valves are opened
 * or there's not enough time left to open more valves. The method keeps track
 * of the best pressure achieved across all paths.
 */
void part1()
{
    string s;
    string q;

    ifstream in;
    in.open("sensor.txt");
    vector<Sensor*> senList;

    int min_x, min_y, max_x, max_y;
    bool first = true;
    while (getline(in, s)) {
        cout << s << endl;

        auto tokens = get_matches(s, "[-\\d]+");
        int sx = stoi(tokens[0]);
        int sy = stoi(tokens[1]);
        int bx = stoi(tokens[2]);
        int by = stoi(tokens[3]);

        Sensor* sen = new Sensor;
        sen->sx = sx;
        sen->sy = sy;
        sen->bx = bx;
        sen->by = by;
        int dist = abs(sx - bx) + abs(sy - by);
        sen->dist = dist;
        senList.push_back(sen);
		cout << "    Sensor [" << sen->sx << ", " << sen->sy << "], Beacon = [" << sen->bx << ", " << sen->by << "]: dist = " << sen->dist << endl;

        int n = sx - dist;
        if (first || n < min_x)
            min_x = n;

        n = sx + dist;
        if (first || n > max_x)
            max_x = n;

        n = sy - dist;
        if (first || n < min_y)
            min_y = n;

        n = sy + dist;
        if (first || n > max_y)
            max_y = n;

	first = false;
    }
    cout << "Range is [" << min_x << ", " << min_y << "] to [" << max_x << ", " << max_y << "]" << endl;

    int check_y = 2000000;
    int noCount = 0;
    for (int check_x = min_x; check_x <= max_x; ++check_x) {
        //cout << "Check coord [" << check_x << ", " << check_y << "]" << endl;
        int within = false;
        for (auto sen : senList) {
            //cout << "    Checking sensor [" << sen->sx << ", " << sen->sy << "], Beacon = [" << sen->bx << ", " << sen->by << "]: " << endl;
            if (senContains(sen, check_x, check_y)) {
                //cout << "Within" << endl;
                within = true;
                break;
            } else {
                //cout << "Not within" << endl;
            }
        }

        if (within)
			++noCount;
    }

    cout << "No beacon count is " << noCount << endl;
}

 /**
  * Part 2 extends the calculation of maximum pressure release by including the help of
  * an elephant, effectively allowing two valves to be opened simultaneously. The algorithm
  * now accounts for two agents (the player and the elephant) and explores the opening
  * sequences that can be achieved together in a reduced time window of 26 minutes.
  *
  * Similar to part1, it uses a recursive approach but now must consider two separate paths
  * of movement and valve opening. Each recursive call determines the next move for both the
  * player and the elephant, tracking their positions, the valves they open, and the
  * accumulated pressure. The complexity increases as the algorithm has to consider combinations
  * of movements for both agents. The method keeps track of the highest pressure achieved with
  * this collaborative effort.
  */
void part2()
{
    string s;
    string q;
    ifstream in;

    //int rangeX = 20;
    //int rangeY = 20;
    //in.open("sensor_x.txt");

    int rangeX = 4000000;
    int rangeY = 4000000;
    in.open("sensor.txt");

    vector<Sensor*> senList;
    vector<SegList> segs(rangeY);

    int min_x, min_y, max_x, max_y;
    bool first = true;
    while (getline(in, s)) {
        cout << s << endl;

        auto tokens = get_matches(s, "[-\\d]+");

        int sx = stoi(tokens[0]);
        int sy = stoi(tokens[1]);
        int bx = stoi(tokens[2]);
        int by = stoi(tokens[3]);

        Sensor* sen = new Sensor;
        sen->sx = sx;
        sen->sy = sy;
        sen->bx = bx;
        sen->by = by;
        int dist = abs(sx - bx) + abs(sy - by);
        sen->dist = dist;
        senList.push_back(sen);
	cout << "    Sensor [" << sen->sx << ", " << sen->sy << "], Beacon = [" << sen->bx << ", " << sen->by << "]: dist = " << sen->dist << endl;

        int n = sx - dist;
        if (first || n < min_x)
            min_x = n;

        n = sx + dist;
        if (first || n > max_x)
            max_x = n;

        n = sy - dist;
        if (first || n < min_y)
            min_y = n;

        n = sy + dist;
        if (first || n > max_y)
            max_y = n;

	first = false;
    }
    cout << "Range is [" << min_x << ", " << min_y << "] to [" << max_x << ", " << max_y << "]" << endl;

    auto checkrange = [rangeX, rangeY](int n, int m) {
        if (n < 0)
            n = 0;
        if (n > m)
            n = m;
        return n;
    };

    auto t1 = Clock::now();

    for (int y = 0; y < rangeY; ++y) {
        //cout << "TESTING ROW " << y << endl;
        vector<Segment> segList;

        for (auto sen : senList) {
            int dist = sen->dist;
            //cout << "CHECKING [" << sen->sx << ", " << sen->sy << "], Beacon = [" << sen->bx << ", " << sen->by << "]: dist = " << sen->dist << endl;
            // Does y intersect with this sensor area?
            if (y < sen->sy - dist || y > sen->sy + dist) {
                //cout << "    DID NOT INTERSECT" << endl;
                continue;
            }

            int diff = sen->dist - abs(sen->sy - y);
            segList.push_back(Segment(checkrange(sen->sx - diff, rangeX), checkrange(sen->sx + diff, rangeX)));
        }

        //cout << "LIST IS NOW       : ";
        //dumpList(segList);
        SegList mergeList = merge(segList);
        //cout << "AFTER MERGE IS NOW: ";
        //dumpList(mergeList);
        if (mergeList.size() > 1) {
	    cout << "GAP y = " << y << ": ";
	    dumpList(mergeList);
	}
    }

    auto t2 = Clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(t2 - t1).count();

    cout << "Duration: " << duration << " milliseconds" << endl;
}

int main()
{
    part2();
}
