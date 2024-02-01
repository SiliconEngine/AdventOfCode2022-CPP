/**
 * Advent of Code 2022 challenge, Day 16.
 * Link: https://adventofcode.com/2022/day/16
 *
 * Challenge: Simulate elephants opening valves in a cavern map and find the
 * optimal strategy.
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
#include <numeric>
#include <queue>
#include <windows.h>
#include <chrono>
#include <bitset>
#include <mutex>
#include <thread>

#define INP_FILE "valves.txt"
#define DEBUG 0

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

struct Valve {
    int rate;
    string name;
    vector<string> paths;
};

map<string, Valve> valveList;
map<string, int> valveDist;
vector<string> activeValves;

int maxOpen = 0;
int bestPres = 0;
int maxPres = 0;


int calcDist(string name1, string name2)
{
    struct DistNode {
        string name;
        int dist;
    };

    set<string> visited;
    queue<DistNode> q;

    q.push({ name1, 0 });
    visited.insert(name1);

    while (q.size()) {
        DistNode node = q.front();
        q.pop();
        if (node.name == name2) {
            return node.dist;
        }

        Valve* valve = &valveList[node.name];
        for (string p : valve->paths) {
            if (visited.count(p) == 0) {
                visited.insert(p);
                q.push({ p, node.dist + 1 });
            }
        }
    }

    return -1;
}


void part1CalcBig(int valveIdx, uint32_t checkedVal, int minute, int numOpen, int curPres, int totalPres)
{
    bitset<32> checked = checkedVal;
    string name = activeValves[valveIdx];
    if (DEBUG) cout << "Checking valve " << valveIdx << " (" << name << "), checked = " << checkedVal << " (" << checked << ")" << endl;

    if (numOpen == maxOpen) {
        if (DEBUG) cout << "    All valves open, time left = " << (30 - minute) << ", cur=" << curPres << ", curtotal = " << totalPres;
        totalPres += curPres * (30 - minute);
        if (DEBUG) cout << ", final total is " << totalPres << endl;
        if (totalPres > bestPres) {
            if (DEBUG) cout << "    New best" << endl;
            bestPres = totalPres;
        }
        return;
    }

    for (int i = 0; i < activeValves.size(); ++i) {
        if (DEBUG) cout << "    Loop " << i << ": checked = " << checked << endl;
        if (!checked[i]) {
            Valve* valve = &valveList[activeValves[i]];
            int dist = valveDist[name + "-" + valve->name];
            if (DEBUG) cout << "    Trying " << i << "(" << valve->name << "), dist = " << dist << endl;
            if (DEBUG) cout << "    checked = " << checked << endl;

            // Add one for time needed to open valve
            ++dist;

            // Check not enough time to get there and turn on, calculate how we would do just waiting
            if (minute + dist > 30) {
                int tempTotal = totalPres + curPres * (30 - minute);
                if (DEBUG) cout << "    Not enough time, tempTotal = " << tempTotal << endl;
                if (tempTotal > bestPres) {
                    if (DEBUG) cout << "    New best" << endl;
                    bestPres = tempTotal;
                }

                continue;
            }

            checked[i] = 1;
            part1CalcBig(i, checked.to_ulong(), minute + dist, numOpen + 1, curPres + valve->rate, totalPres + curPres * dist);
            checked[i] = 0;
        }
    }
}

struct Runner {
    int travelToIdx;
    int travelTimeLeft;
};
struct RunnerSet {
    Runner r[2];
};

ostream& operator<<(ostream& out, const RunnerSet& runners) {
    const Runner& r1 = runners.r[0];
    const Runner& r2 = runners.r[1];
    out << "r1 = [v-" << r1.travelToIdx << ", t-" << r1.travelTimeLeft << "] / r2 = [v-" << r2.travelToIdx << ", t-" << r2.travelTimeLeft << "]";
    return out;
}

int maxMin = 0;

struct Entry
{
    RunnerSet runners;
    uint32_t checkedVal;
    char minute;
    char numOpen;
    short curPres;
    short totalPres;
};
ostream& operator<<(ostream& out, const Entry& e) {
    cout << "{minute=" << (int)e.minute << ", numOpen=" << (int)e.numOpen << ", curPres=" << e.curPres << ", totalPres=" << e.totalPres << " " << e.runners << "}";
    return out;
}

// Example path:
// 0: [  AA] [  AA]
// 1: [T-II] [T-DD]
// 2: [T-JJ] [O-DD]
// 3: [O-JJ] [T-EE]
// 4: [T-II] [T-FF]
// 5: [T-AA] [T-GG]
// 6: [T-BB] [T-HH]
// 7: [O-BB] [O-HH]
// 8: [T-CC] [T-GG]
// 9: [O-CC] [T-FF]
//10: [    ] [T-EE]
//11: [    ] [O-EE]

// 0: [3-JJ] [2-DD] WAS: [0-AA] [0-AA]
// 2: [1-JJ] [5-HH] WAS: [3-JJ] [2-DD]
// 3: [4-BB] [4-HH] WAS: [1-JJ] [5-HH]
// 7: [2-CC] [4-EE] WAS: [4-BB] [4-HH]
// 9: [    ] [2-EE] WAS: [2-CC] [4-EE]
//11: [    ] [    ] WAS: [    ] [2-EE]

string makeKey(Entry& node)
{
    Runner* r1 = &node.runners.r[0];
    Runner* r2 = &node.runners.r[1];

    string key = to_string((int)node.minute) + "-" + to_string(node.checkedVal) + "-" + to_string(node.totalPres) + "-";
    int i1 = r1->travelToIdx * 20 + r1->travelTimeLeft;
    int i2 = r2->travelToIdx * 20 + r2->travelTimeLeft;
    if (i1 > i2)
        key += to_string(i1) + to_string(i2);
    else
        key += to_string(i2) + to_string(i1);

    return key;
}

map<string, bool> visited;
queue<Entry> q;

/**
 * Part 1 calculates the maximum pressure that can be released within a
 * 30-minute window.  The algorithm involves iterating through all valves,
 * determining the optimal sequence to open them to maximize the total
 * pressure released. It uses a recursive depth-first search approach to
 * explore all possible sequences of valve openings, considering the time
 * constraints and the pressure each valve can release.
 *
 * It tracks the current state (which valves are open, the current pressure,
 * total pressure released so far) and explores different paths by opening
 * one more valve at a time. The recursion terminates when either all valves
 * are opened or there's not enough time left to open more valves. The
 * method keeps track of the best pressure achieved across all paths.
 */
void part1()
{
    bitset<32> checked;
    checked[0] = 1;
    part1CalcBig(0, checked.to_ulong(), 0, 0, 0, 0);
}

 /**
  * Part 2 extends the calculation of maximum pressure release by including
  * the help of an elephant, effectively allowing two valves to be opened
  * simultaneously. The algorithm now accounts for two agents (the player and
  * the elephant) and explores the opening sequences that can be achieved
  * together in a reduced time window of 26 minutes.
  *
  * Similar to part1, it uses a recursive approach but now must consider two
  * separate paths of movement and valve opening. Each recursive call
  * determines the next move for both the player and the elephant, tracking
  * their positions, the valves they open, and the accumulated pressure. The
  * complexity increases as the algorithm has to consider combinations
  * of movements for both agents. The method keeps track of the highest
  * pressure achieved with this collaborative effort.
  */
void part2()
{
    void bfs();

    bitset<32> checked;
    checked[0] = 1;

    Entry startEntry;
    startEntry.runners.r[0].travelTimeLeft = 0;
    startEntry.runners.r[0].travelToIdx = 0;
    startEntry.runners.r[1].travelTimeLeft = 0;
    startEntry.runners.r[1].travelToIdx = 0;
    startEntry.checkedVal = 1;
    startEntry.minute = 0;
    startEntry.numOpen = 0;
    startEntry.curPres = 0;
    startEntry.totalPres = 0;
    q.push(startEntry);

    bfs();
}

void bfs()
{
    int loopCount = 0;
    bitset<32> checked;

    while (! q.empty()) {
        Entry node = q.front();
        q.pop();
        if (DEBUG) cout << "NODE: " << node << endl;

        ++loopCount;
        //if (loopCount == 10) exit(0);

        checked = node.checkedVal;
        Runner& r1 = node.runners.r[0];
        Runner& r2 = node.runners.r[1];
        Runner* rlist[2];
        rlist[0] = &node.runners.r[0];
        rlist[1] = &node.runners.r[1];

        RunnerSet& runners = node.runners;
        int minute = node.minute;
        int curPres = node.curPres;
        int totalPres = node.totalPres;

        // See if both travelers arrive in the future and we need to advance the clock
        int minTime = min(r1.travelTimeLeft, r2.travelTimeLeft);
        if (DEBUG) cout << runners << ", minTime = " << minTime << endl;
        if (minTime > 0) {
            minute += minTime;
            totalPres += curPres * minTime;

            for (int ridx = 0; ridx < 2; ++ridx) {
                if ((rlist[ridx]->travelTimeLeft -= minTime) == 0) {
                    // Arrived and turned on valve

                    Valve* valve = &valveList[activeValves[rlist[ridx]->travelToIdx]];
                    if (DEBUG) cout << "    Turned on valve " << valve->name << endl;
                    curPres += valve->rate;
                    ++node.numOpen;
                }
            }
        }
        //cout << "    TIME ADVANCED by " << minTime << ": minute = " << minute << ", curPres = " << curPres << ", totalPres = " << totalPres << ", numOpen = " << (int)node.numOpen << " (of " << maxOpen << ")" << endl;

        // See if all valves open
        if (node.numOpen == maxOpen) {
            if (DEBUG) cout << "    All valves open, time left = " << (26 - minute) << ", cur=" << curPres << ", curtotal = " << totalPres;
            //cout << "    All valves open, time left = " << (26 - minute) << ", cur=" << curPres << ", curtotal = " << totalPres;
            totalPres += curPres * (26 - minute);
            if (DEBUG) cout << ", final total is " << totalPres << endl;
            if (totalPres > bestPres) {
                bestPres = totalPres;
                if (DEBUG) cout << "    (ALL OPEN) New best " << bestPres << endl;
                cout << "    (ALL OPEN) New best " << bestPres << endl;
            }
            continue;
        }

        // If only one valve left and we're traveling for it, calculate final value
        //cout << "    r1.ttl = " << r1.travelTimeLeft << ", r2.ttl = " << r2.travelTimeLeft << endl;
        if ((node.numOpen + 1) == maxOpen && (r1.travelTimeLeft != 0 || r2.travelTimeLeft != 0)) {
            if (DEBUG) cout << "    ONE LEFT: " << node << endl;
            Runner* r = r1.travelTimeLeft != 0 ? &r1 : &r2;
            Valve* valve = &valveList[activeValves[r->travelToIdx]];

            totalPres += curPres * r->travelTimeLeft;
            minute += r->travelTimeLeft;
            curPres += valve->rate;
            totalPres += (26 - minute) * curPres;
            if (DEBUG) cout << "        minute = " << minute << ", totalPres = " << totalPres << ", curPres = " << curPres << endl;

            //int total = totalPres + curPres * (26 - minute) + valve->rate * (26 - (minute + r->travelTimeLeft));;
            //cout << "        minute = " << minute << ", totalPres = " << totalPres << ", curPres = " << curPres << ", total = " << total << endl;
            if (totalPres > bestPres) {
                bestPres = totalPres;
                cout << "    (ONE LEFT) New best " << bestPres << endl;
            }
            continue;
        }

        // Check our best if we just wait here or let other one complete
        int tempTotal = totalPres + curPres * (26 - minute);
        for (int ridx = 0; ridx < 2; ++ridx) {
            if (rlist[ridx]->travelTimeLeft != 0) {
                int dest = runners.r[ridx].travelToIdx;
                Valve* valve = &valveList[activeValves[dest]];
                int timeLeft = runners.r[ridx].travelTimeLeft;
                int t = 26 - (minute + timeLeft);
                int extra = 0;
                if (t > 0) {
                    extra = valve->rate * t;
                }
                tempTotal += extra;
            }
        }
        if (tempTotal > bestPres) {
            bestPres = tempTotal;
            cout << "WAIT CHECK: new best " << bestPres << endl;
        }

        // See if impossible to beat best pressure so far
        int trial = totalPres + maxPres * (26 - minute);
        if (trial < bestPres) {
            if (DEBUG) cout << "Can't beat best, trial = " << trial << ", bestPres = " << bestPres << endl;
            continue;
        }

        // Build list of currently reachable indexes for each
        typedef struct {
            int v;
            int d;
        } Pot;

        Pot potentialList[2][50];
        int potCount[2] = { 0 };

        for (int ridx = 0; ridx < 2; ++ridx) {
            //cout << "    Figure out paths, ridx=" << ridx << ", checked=" << checked << endl;
            // Is runner stopped and we need next destination?
            if (rlist[ridx]->travelTimeLeft == 0) {
                int valveIdx = rlist[ridx]->travelToIdx;    // Current location
                string name = activeValves[valveIdx];
                //cout << "    " << ridx << ": " << name << endl;

                // Runner can move, figure out where
                for (int v = 0; v < activeValves.size(); ++v) {
                    if (!checked[v]) {
                        Valve* valve = &valveList[activeValves[v]];
                        int dist = valveDist[name + "-" + valve->name] + 1;

                        // See if reachable in time
                        if (minute + dist < 26) {
                            potentialList[ridx][potCount[ridx]++] = { v, dist };
                            //cout << "    index " << v << " is reachable" << endl;
                        }
                    }
                }
            }
        }
    SKIP:

        // See if no further paths -- just calculate the rest if so
        if (r1.travelTimeLeft == 0 && r2.travelTimeLeft == 0 && potCount[0] == 0 && potCount[1] == 0) {
            totalPres += curPres * (26 - minute);
            if (totalPres > bestPres) {
                bestPres = totalPres;
                if (DEBUG) cout << "    (NO PATHS) New best " << bestPres << endl;
                cout << "    (NO PATHS) New best " << bestPres << endl;
            }
            continue;
        }

        // If either has no further progress, just set to the current one
        for (int ridx = 0; ridx < 2; ++ridx) {
            if (potCount[ridx] == 0) {
                ++potCount[ridx];
                potentialList[ridx][0] = { rlist[ridx]->travelToIdx, rlist[ridx]->travelTimeLeft };
            }
        }

        // Add in potential path of staying put until the end, if not already there
        for (int ridx = 0; ridx < 2; ++ridx) {
            if (! (potCount[ridx] == 1 && potentialList[ridx][0].d > 100)) {
                potentialList[ridx][potCount[ridx]++] = { rlist[ridx]->travelToIdx, 999 };
            }
        }

        // Build potential paths from here and add to queue
        //for (int ridx = 0; ridx < 1; ++ridx) {
	//int otherIdx = 1 - ridx;
        int ridx = 0;
        int otherIdx = 1;
        for (int rpotIdx = 0; rpotIdx < potCount[ridx]; ++rpotIdx) {
            for (int opotIdx = 0; opotIdx < potCount[otherIdx]; ++opotIdx) {
                Entry newNode;
                newNode.runners.r[ridx] = { potentialList[ridx][rpotIdx].v, potentialList[ridx][rpotIdx].d };
                newNode.runners.r[otherIdx] = { potentialList[otherIdx][opotIdx].v, potentialList[otherIdx][opotIdx].d };

                int dest1 = newNode.runners.r[0].travelToIdx;
                int dest2 = newNode.runners.r[1].travelToIdx;
                if (DEBUG) cout << "    Potential moving to [" << dest1 << " / " << newNode.runners.r[0].travelTimeLeft << "] and [" << dest2 << " / " << newNode.runners.r[1].travelTimeLeft << "]";

                // Check case where going to same place and skip
                if (dest1 == dest2) {
                    if (DEBUG) cout << "    SKIP" << endl;
                    continue;
                }

                long checkedSave = checked.to_ulong();

                checked[dest1] = 1;
                checked[dest2] = 1;
                newNode.checkedVal = checked.to_ulong();
                newNode.minute = minute;
                newNode.numOpen = node.numOpen;
                newNode.curPres = curPres;
                newNode.totalPres = totalPres;

                string key = makeKey(newNode);
                if (!visited[key]) {
                    q.push(newNode);
                    visited[key] = true;
                }
                if (DEBUG) cout << "    New node:" << newNode << endl;

                checked = checkedSave;
                //goto ONE_NODE;
            }
        }
        //}
//ONE_NODE:
        ;
    }

    cout << "Loop count was " << loopCount << endl;
}

void part2CalcBig(RunnerSet runners, uint32_t checkedVal, int minute, int numOpen, int curPres, int totalPres)
{
    bitset<32> checked = checkedVal;
    Runner& r1 = runners.r[0];
    Runner& r2 = runners.r[1];

    // See if impossible to beat best pressure so far
    int trial = totalPres + curPres + maxPres * (26 - minute);
    if (trial < bestPres) {
        if (DEBUG) cout << "Can't beat best, trial = " << trial << ", bestPres = " << bestPres << endl;
        return;
    }

    // See if all valves open
    if (numOpen == maxOpen) {
        if (DEBUG) cout << "    All valves open, time left = " << (26 - minute) << ", cur=" << curPres << ", curtotal = " << totalPres;
        cout << "    All valves open, time left = " << (26 - minute) << ", cur=" << curPres << ", curtotal = " << totalPres;
        totalPres += curPres * (26 - minute);
        if (DEBUG) cout << ", final total is " << totalPres << endl;
        if (totalPres > bestPres) {
            bestPres = totalPres;
            if (DEBUG) cout << "    (ALL OPEN) New best " << bestPres << endl;
            cout << "    (ALL OPEN) New best " << bestPres << endl;
        }
        return;
    }

    // See if both travelers arrive in the future
    int minTime = min(r1.travelTimeLeft, r2.travelTimeLeft);
    if (DEBUG) cout << runners << ", minTime = " << minTime << endl;
    if (minTime > 0) {
        // Check out of time
        if (minute + minTime >= 26) {
            if (DEBUG) cout << "    OUT OF TIME" << endl;
            //cout << "    OUT OF TIME" << endl;
            totalPres += curPres * (26 - minute);
            if (totalPres > bestPres) {
                bestPres = totalPres;
                if (DEBUG) cout << "    New best" << endl;
                cout << "    (TOP OOT) New best " << bestPres << endl;
            }
            return;
        }

        minute += minTime;
        totalPres += curPres * minTime;

        for (int ridx = 0; ridx < 2; ++ridx) {
            if ((runners.r[ridx].travelTimeLeft -= minTime) == 0) {
                // Arrived and turned on valve

                Valve* valve = &valveList[activeValves[runners.r[ridx].travelToIdx]];
                curPres += valve->rate;
                ++numOpen;
            }
        }
    }
    if (DEBUG) cout << "IS NOW " << runners << endl;

    if (minute > maxMin) {
        cout << "Minute: " << minute << endl;
        maxMin = minute;
    }

    // AFTER COMPLETING WITH ONE, COMPLETE THE OTHER AND LOOP
    // Whichever one is zero (or both), recursively check the next destination
    bool hadValve = false;
    for (int ridx = 0; ridx < 2; ++ridx) {
        if (runners.r[ridx].travelTimeLeft == 0) {
            int valveIdx = runners.r[ridx].travelToIdx;
            string name = activeValves[valveIdx];
            if (DEBUG) cout << ridx << ": Checking valve " << valveIdx << " (" << name << "), checked = " << checkedVal << " (" << checked << ")" << endl;

            for (int i = 0; i < activeValves.size(); ++i) {
                if (DEBUG) cout << "    Loop " << i << ": checked = " << checked << endl;
                if (!checked[i]) {
                    hadValve = true;

                    Valve* valve = &valveList[activeValves[i]];
                    int dist = valveDist[name + "-" + valve->name];
                    if (DEBUG) cout << "    Trying " << i << "(" << valve->name << "), dist = " << dist << endl;
                    if (DEBUG) cout << "    checked = " << checked << endl;

                    // Add one for time needed to open valve
                    ++dist;

                    // Check not enough time to get there and turn on, calculate how we would do just waiting
                    if (minute + dist >= 26) {
                        int tempTotal = totalPres + curPres * (26 - minute);

                        // Add in additional from other runner potentially arriving somewhere
                        int other = 1 - ridx;
                        int otherDest = runners.r[other].travelToIdx;
                        Valve* otherValve = &valveList[activeValves[otherDest]];
                        int otherTimeLeft = runners.r[other].travelTimeLeft;
                        int otherTime = (26 - (minute + otherTimeLeft));
                        int otherExtra = 0;
                        if (otherTimeLeft > 0 && otherTime > 0) {
                            otherExtra = otherValve->rate * otherTime;
                        }
                        tempTotal += otherExtra;

                        if (DEBUG) cout << "    Not enough time, tempTotal = " << tempTotal << endl;
                        //cout << "    Not enough time, tempTotal = " << tempTotal << endl;
                        if (tempTotal > bestPres) {
                            bestPres = tempTotal;
                            if (DEBUG) cout << "    New best" << endl;
                            cout << "    (MAIN OOT) New best " << bestPres << endl;
                            cout << "        minute = " << minute << ", ridx=" << ridx << ", dist = " << dist << ", oTimeLeft = " << otherTimeLeft
                                << ", oTime = " << otherTime << ", oExtra = " << otherExtra << ", oRate = " << otherValve->rate << ", checked: " << checked[otherDest] << endl;
                            cout << "        RUNNERS ARE:    " << runners << endl;
                        }

                        continue;
                    }

                    RunnerSet nextRunners = runners;
                    checked[i] = 1;
                    nextRunners.r[ridx].travelToIdx = i;
                    nextRunners.r[ridx].travelTimeLeft = dist;

                    if (DEBUG) cout << "    CALLING " << nextRunners << endl;
                    part2CalcBig(nextRunners, checked.to_ulong(), minute, numOpen, curPres, totalPres);

                    checked[i] = 0;
                }
            }
        }
    }

    // If nothing to process, then we're waiting for a runner to arrive, which will be the last valve.
    // We can then close off this line.

    if (!hadValve) {
        int t = max(r1.travelTimeLeft, r2.travelTimeLeft);

        if (DEBUG) cout << "NO VALVE, numOpen = " << numOpen << ", checked = " << checked << ", t = " << t << endl;
        if (DEBUG) cout << runners << endl;

        if (DEBUG) cout << runners << ", minTime = " << minTime << endl;
        if (t > 0) {
            // Check out of time
            if (minute + t >= 26) {
                if (DEBUG) cout << "    OUT OF TIME" << endl;
                //cout << "    OUT OF TIME" << endl;
                totalPres += curPres * (26 - minute);
                if (totalPres > bestPres) {
                    bestPres = totalPres;
                    if (DEBUG) cout << "    New best" << endl;
                    cout << "    (NO VALVE OOT) New best " << bestPres << endl;
                }
                return;
            }

            minute += t;
            totalPres += curPres * t;

            for (int ridx = 0; ridx < 2; ++ridx) {
                if (runners.r[ridx].travelTimeLeft > 0) {
                    runners.r[ridx].travelTimeLeft -= t;
                    // Arrived and turned on valve

                    Valve* valve = &valveList[activeValves[runners.r[ridx].travelToIdx]];
                    curPres += valve->rate;
                    ++numOpen;
                }
            }
        }

        if (numOpen != maxOpen) {
            cout << "SHOULDN'T HAPPEN! numOpen = " << numOpen << ", maxOpen = " << maxOpen << endl;
            exit(0);
        }

        if (DEBUG) cout << "    END: All valves open, time left = " << (26 - minute) << ", cur=" << curPres << ", curtotal = " << totalPres;
        totalPres += curPres * (26 - minute);
        if (DEBUG) cout << ", final total is " << totalPres << endl;
        if (totalPres > bestPres) {
            bestPres = totalPres;
            if (DEBUG) cout << "    New best" << endl;
            cout << "    (END) New best " << bestPres << ", minute = " << minute << ", curtotal = " << totalPres << ", maxPres = " << maxPres << endl;
        }

        return;
    }

}

void run(int part)
{
    string s;
    string q;
    ifstream in;

    in.open(INP_FILE);

    maxPres = 0;

    // Always include AA, which is always off and always first
    activeValves.push_back("AA");

    while (getline(in, s)) {
        cout << s << endl;

        auto rates = get_matches(s, "\\d+");
        int rate = stoi(rates[0]);
        maxPres += rate;
        if (rate != 0) {
            ++maxOpen;
        }

        auto valves = get_matches(s, "[A-Z][A-Z]");
        string name = valves[0];
        valves.erase(valves.begin());

        cout << "TUNNELS: " << valves << endl;

        valveList[name] = Valve{ rate, name, valves };

        if (rate != 0) {
            activeValves.push_back(name);
        }
    }

    auto time1 = chrono::high_resolution_clock::now();
    for (int i = 0; i < activeValves.size() - 1; ++i) {
        for (int j = i + 1; j < activeValves.size(); ++j) {
            string valve1 = activeValves[i];
            string valve2 = activeValves[j];
            int dist = calcDist(valve1, valve2);
            cout << "CALC: " << valve1 << " to " << valve2 << ": dist = " << dist << endl;
            valveDist[valve1 + "-" + valve2] = dist;
            valveDist[valve2 + "-" + valve1] = dist;
        }
    }

    //for (auto x : valveDist) {
        //cout << x.first << " " << x.second << endl;
    //}
    auto time2 = chrono::high_resolution_clock::now();

    if (part == 1)
	part1();
    if (part == 2)
	part2();

    auto time3 = chrono::high_resolution_clock::now();

    auto diff1 = std::chrono::duration_cast<std::chrono::milliseconds>(time2 - time1).count();
    auto diff2 = std::chrono::duration_cast<std::chrono::milliseconds>(time3 - time1).count();

    cout << "Best was " << bestPres << endl;
    cout << "First phase: " << diff1 << endl;
    cout << "Total time: " << diff2 << endl;
}

int main()
{
    run(2);
}
