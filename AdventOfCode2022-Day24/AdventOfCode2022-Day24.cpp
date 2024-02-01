/**
 * Advent of Code 2022 challenge, Day 24.
 * Link: https://adventofcode.com/2022/day/24
 *
 * Challenge: Navigate through a map avoiding blizzards and calculate fewest
 * number of minutes
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

#define DEBUG 0
#define DEBUG_SHOW_MAP 0
#define DEBUG_MAX_MIN 2000
#define MAP_FILE "map.txt"
#define SHOW_PROG 1

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

enum Dir { STAY, NORTH, SOUTH, WEST, EAST };

ostream& operator<<(ostream& out, const Dir d) {
    switch (d) {
    case STAY: out << "STAY"; break;
    case NORTH: out << "NORTH"; break;
    case SOUTH: out << "SOUTH"; break;
    case EAST: out << "EAST"; break;
    case WEST: out << "WEST"; break;
    }
    return out;
}

/**
 * Represents a blizzard in the valley with a position (x, y) and a
 * direction of movement.
 * The direction is one of the enum values: NORTH, SOUTH, EAST, WEST, or STAY.
 */
struct Blizzard
{
    int x;
    int y;
    Dir dir;
};

struct PotentialMove {
    int x;
    int y;
    Dir dir;
};

ostream& operator<<(ostream& out, const Blizzard& b) {
    cout << "[" << b.x << ", " << b.y << "] " << b.dir;
    return out;
}
ostream& operator<<(ostream& out, const Blizzard* b) {
    cout << "[" << b->x << ", " << b->y << "]";
    return out;
}

class Valley
{
public:
    unordered_map<string, int> bcount;
    vector<Blizzard> blist;
    int min_x;
    int min_y;
    int max_x;
    int max_y;

    string makeKey(Blizzard& b) {
        return to_string(b.x) + "," + to_string(b.y);
    }

    string makeKey(int x, int y) {
        return to_string(x) + "," + to_string(y);
    }

    void addBlizzard(Blizzard& b) {
        blist.push_back(b);
        ++bcount[makeKey(b)];
    }

    bool testBlizzard(int x, int y) {
        if (y < 0 || y > max_y || x < 0 || x > max_x)
            return true;

        return bcount[makeKey(x, y)] > 0;
    }

    void dumpBlizzards() {
	for (Blizzard &b : blist) {
	    cout << "BLIZZARD: ";
	    cout << b << endl;
	}
    }

    void dumpBCount()
    {
        cout << "BCOUNT:" << endl;
        for (auto b : bcount) {
            cout << "    " << b.first << ": " << b.second << endl;
        }
    }

    void moveBlizzards() {
	for (Blizzard &b : blist) {
	    --bcount[makeKey(b)];

	    switch (b.dir) {
	    case WEST:
		if (--b.x < 1) {
		    b.x = max_x-1;
		}
		break;
	    case EAST:
		if (++b.x >= max_x) {
		    b.x = 1;
		}
		break;
	    case NORTH:
		if (--b.y < 1) {
		    b.y = max_y-1;
		}
		break;
	    case SOUTH:
		if (++b.y >= max_y) {
		    b.y = 1;
		}
		break;
	    }

	    ++bcount[makeKey(b)];
	}
    }

    vector<string> makeMap() {
        vector<string> m;
        //m.push_back(string(max_x + 2, '#'));
        for (int y = min_y; y <= max_y; ++y) {
            string s(max_x + 1, '.');
            m.push_back(s);
        }

        for (Blizzard& b : blist) {
            char c = '.';
            switch (b.dir) {
            case STAY: c = '#'; break;
            case WEST: c = '<'; break;
            case EAST: c = '>'; break;
            case NORTH: c = '^'; break;
            case SOUTH: c = 'v';  break;
            }

            char curC = m[b.y][b.x];
            if (curC == '.') {
                m[b.y][b.x] = c;
            }
            else if (curC >= '0' && curC <= '9') {
                m[b.y][b.x] = curC + 1;
            }
            else {
                m[b.y][b.x] = '2';
            }
        }

        //dumpBCount();
        return m;
    }

    void dumpMap()
    {
        vector<string> list = makeMap();
	for (string s : list) {
	    cout << s << endl;
	}
    }
};

struct Entry {
    int x;
    int y;
    int currDest;
    int minute;
    int dist;
};

/**
 * Computes the shortest path to navigate through the valley while avoiding
 * blizzards. Implements a breadth-first search (BFS) algorithm to explore
 * all possible paths and selects the one with the minimum number of steps
 * required to reach the destination.
 */
void compPath(Valley vly, int start_x, int start_y, vector<pair<int, int>> destList)
{
    map<int, vector<string>> mapList;

    // Maps each minute to the corresponding state of the valley, allowing
    // us to track the positions of blizzards over time
    map<int, Valley> valleyList;
    valleyList[0] = vly;
    int min_x = vly.min_x;
    int max_x = vly.max_x;
    int min_y = vly.min_y;
    int max_y = vly.max_y;

    int numDest = destList.size();

    // Queue for BFS. Stores the positions to be explored along with the
    // current time and destination.
    queue<Entry> q;
    q.push({ start_x, start_y, 0, 0 });

    unordered_map<string, bool> visited;

    int progMin = 0;
    int progX = 0;
    int progY = 0;
    int maxQ = 0;
    int bestList[10];

    int testCount = 4;
    int bestTime = -1;
    while (q.size()) {
        Entry& e = q.front();
        int x = e.x;
        int y = e.y;
	int currDest = e.currDest;
	int end_x = destList[currDest].first;
	int end_y = destList[currDest].second;
        int minute = e.minute;
        int nextMin = minute + 1;

        if (DEBUG_MAX_MIN && minute >= DEBUG_MAX_MIN) {
            cout << "TOO MANY MINUTES" << endl;
            break;
        }

        Valley& currVly = valleyList[minute];

        if (SHOW_PROG) {
	    if (nextMin > progMin) {
		cout << "Progress minute: " << nextMin << endl;
		progMin = nextMin;
	    }

	    if (x > progX) {
		cout << "Progress x: " << x << endl;
		progX = x;
	    }

	    if (y > progY) {
		cout << "Progress y: " << y << endl;
		progY = y;
	    }

	    if (q.size() > maxQ) {
		cout << "Q SIZE: " << q.size() << endl;
		maxQ = q.size() + 100;
	    }
	}

        bool showDebug = DEBUG;
        bool showMap = DEBUG_SHOW_MAP;
        //if (nextMin > 18 && currDest != 1) showDebug = 0, showMap = 0;

        if (showDebug) cout << "Processing queue: " << currDest << " / " << minute << " [" << x << ", " << y << "] END: [" << end_x << ", " << end_y << "] " << endl;
        if (showMap) currVly.dumpMap();

        // If already worse, we can stop this path
        if (bestTime > 0 && nextMin > bestTime) {
            cout << "     Already worse than best time " << bestTime << " (nextMin = " << nextMin << ")" << endl;
            q.pop();
            continue;
        }

        // Do end check
        if (x == end_x && y == end_y) {
	    int t = e.dist;
            if (bestList[currDest] < 0 || t < bestList[currDest]) {
                bestList[currDest] = t;
            }

            cout << "======" << endl << "REACHED END for dest " << currDest+1 << " [" << end_x << ", " << end_y << "] bestList = " << bestList[currDest];
            ++currDest;
            if (currDest == numDest) {
				cout << " FINAL: dist was " << t << endl;
                q.pop();
                if (bestTime < 0 || t < bestTime) {
                    bestTime = t;
                }
                continue;
            }
            cout << endl;

            // Reached end of this destination, move to next
	    end_x = destList[currDest].first;
	    end_y = destList[currDest].second;
            cout << "DEST IS NOW [" << end_x << ", " << end_y << "]" << endl;
        }

        if (valleyList.count(nextMin) == 0) {
            //cout << "    Creating new valley at time " << minute << endl;
            valleyList[nextMin] = currVly;
	    valleyList[nextMin].moveBlizzards();
            mapList[nextMin] = valleyList[nextMin].makeMap();
        }
        Valley& nextVly = valleyList[nextMin];

        if (showMap) nextVly.dumpMap();

        // Build list of potential moves
        int moveCount = 0;
        PotentialMove moveList[5];

	// SOUTH
	if (y < max_y) {
	    if (!nextVly.testBlizzard(x, y + 1)) {
		moveList[moveCount++] = { x, y + 1, SOUTH };
	    }
	}
	// EAST
	if (x < max_x) {
	    if (!nextVly.testBlizzard(x + 1, y)) {
		moveList[moveCount++] = { x + 1, y, EAST };
	    }
	}
	// WEST
	if (x > 1) {
	    if (!nextVly.testBlizzard(x - 1, y)) {
		moveList[moveCount++] = { x - 1, y, WEST };
	    }
	}
	// NORTH
	if (y > 0) {
	    if (!nextVly.testBlizzard(x, y - 1)) {
		moveList[moveCount++] = { x, y - 1, NORTH };
	    }
	}

        // STAY
        if (! nextVly.testBlizzard(x, y)) {
	    moveList[moveCount++] = { x, y, STAY };
	}

        auto makeKey = [](int x, int y, int currDest, int min) {
            return to_string(x) + "-" + to_string(y) + "-" + to_string(currDest) + "-" + to_string(min);
        };

        for (int i = 0; i < moveCount; ++i) {
            PotentialMove& m = moveList[i];
            if (showDebug) cout << "    POTENTIAL MOVE: min = " << nextMin << " [" << m.x << ", " << m.y << "] " << m.dir << endl;
            string key = makeKey(m.x, m.y, currDest, nextMin);
			if (visited.count(key) > 0) {
                if (showDebug) cout << "        " << key << " VISITED" << endl;
                continue;
            }

            Entry newent;
            newent.x = m.x;
            newent.y = m.y;
            newent.currDest = currDest;
            newent.minute = nextMin;
            newent.dist = e.dist + 1;
            q.push(newent);

            if (nextMin <= 18 || (nextMin > 18 && currDest == 1)) {
		mapList[nextMin][m.y][m.x] = '*';
            }

            //cout << "SET: " << key << endl;
            visited[key] = true;
        }

        q.pop();
        //if (--testCount == 0) exit(0);
    }

    cout << "Best time was " << bestTime << endl;
}

void part2()
{
    string s;
    ifstream in;
    Dir dirs[5] = { STAY, NORTH, SOUTH, WEST, EAST };
    string dirnames[5];
    dirnames[STAY] = "STAY";
    dirnames[NORTH] = "NORTH";
    dirnames[SOUTH] = "SOUTH";
    dirnames[WEST] = "WEST";
    dirnames[EAST] = "EAST";

    int curDir = 0;

    in.open(MAP_FILE);

    int min_x = 0;
    int max_x = 0;
    int min_y = 0;
    int max_y = 0;
    Valley vly;

    while (getline(in, s)) {
        cout << s << endl;
		max_x = s.length() - 1;

        int x = 0;
        for (char c : s) {
            int dir = -1;
            switch (c) {
            case '.':
                break;
            case '#':
                dir = STAY;
                break;
            case '<':
                dir = WEST;
                break;
            case '>':
                dir = EAST;
                break;
            case '^':
                dir = NORTH;
                break;
            case 'v':
                dir = SOUTH;
                break;
            }

            if (dir >= 0) {
                Blizzard b;
                b.dir = (Dir)dir;
                b.x = x;
                b.y = max_y;
                vly.addBlizzard(b);
            }

            ++x;
        }
        ++max_y;
    }
    --max_y;
    cout << "min_x = " << min_x << ", min_y = " << min_y << ", max_x = " << max_x << ", max_y = " << max_y << endl;

    vly.min_x = min_x;
    vly.max_x = max_x;
    vly.min_y = min_y;
    vly.max_y = max_y;

    vly.dumpMap();

    int start_x = 1;
    int start_y = 0;
    int end_x = max_x-1;
    int end_y = max_y;

    vector<pair<int, int>> destList;
    destList.push_back({end_x, end_y});
    destList.push_back({start_x, start_y});
    destList.push_back({end_x, end_y});

    compPath(vly, start_x, start_y, destList);
}

int main()
{
    part2();
}
