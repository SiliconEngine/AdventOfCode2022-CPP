/**
 * Advent of Code 2022 challenge, Day 12.
 * Link: https://adventofcode.com/2022/day/12
 *
 * Challenge: Navigate map and determine fewest steps to get to location with best comm signal.
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

enum entry_type { DirType, FileType };

std::vector<std::string> str_split(const std::string& s, const char* pattern) {
    std::regex sep_regex = std::regex{ pattern };
    std::sregex_token_iterator iter(s.begin(), s.end(), sep_regex, -1);
    std::sregex_token_iterator end;
    return { iter, end };
}

struct Hill {
    int row;
    int col;
    char height;
    int dist;
    bool visited;
};

bool hillcmp(struct Hill* a, struct Hill* b) {
    return a->dist > b->dist;
}

#define BOLD "\x1b[33m"
#define UNBOLD "\x1b[0m"

void moveCursor(int row, int col)
{
    COORD c = { col, row };
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleCursorPosition(h, c);
}

void dumpHills(int rows, int cols, Hill*** hills) {
    //system("CLS");
    for (int r = 0; r < rows; ++r) {
	moveCursor(r, 0);
        for (int c = 0; c < cols; ++c) {
            Hill* hill = hills[r][c];
            if (hill->visited) {
		cout << BOLD << hill->height << UNBOLD;
	    } else {
		cout << hill->height;
	    }
	}
        cout << endl;
    }
}

/**
 * Part 2: * This method extends the pathfinding algorithm from part1.
 * It now evaluates multiple starting points and finds the overall shortest path
 * to the end point from any of the starting points.
 */
void part2()
{
    string s;
    string q;

    ifstream in;
    in.open("hills.txt");
    vector<string> heights;
    int dsp = false;

    int srow = 0, scol = 0, erow = 0, ecol = 0;

    if (dsp) {
	cout << "enter: ";
	cin >> s;
    }


    int rows = 0;
    while (getline(in, s)) {
        //cout << s << endl;
        heights.push_back(s);

        int pos = s.find('S');
        if (pos != string::npos) {
            srow = rows;
            scol = pos;
        }

        pos = s.find('E');
        if (pos != string::npos) {
            erow = rows;
            ecol = pos;
        }

        ++rows;
    }
    int cols = heights[0].length();

    // Setup the hills array and a list of potential lowest starting points.
    // Initialize distances and visited flags.
    Hill*** hills = new Hill** [rows];

    for (int r = 0; r < rows; ++r) {
        hills[r] = new Hill*[cols];
    }

    vector<struct Hill*> lowestList;

    for (int r = 0; r < rows; ++r) {
        for (int c = 0; c < cols; ++c) {
            int dist = (r == srow && c == scol) ? 0 : 999999999;
            char h = heights[r][c];
            Hill *hill = new Hill{ r, c, h, dist, false };
            hills[r][c] = hill;
            if (h == 'a' || h == 'S') {
                lowestList.push_back(hill);
            }
        }
    }

    // Iterate over all starting hills to find the shortest path from any of them.
    // Each iteration resets the hills' distances and visited flags.
    int lowSteps = -1;
    for (auto* startHill : lowestList) {
        vector<struct Hill*> pq;

        for (int r = 0; r < rows; ++r) {
            for (int c = 0; c < cols; ++c) {
                Hill* hill = hills[r][c];
                hill->dist = (hill == startHill) ? 0 : 999999999;
                hill->visited = false;
                pq.push_back(hill);
            }
        }
        srow = startHill->row;
        scol = startHill->col;

        if (dsp) {
			dumpHills(rows, cols, hills);
        }

	// Pathfinding logic for each starting point.
	// Uses a similar approach as in part1 but resets the hills' information for each new start.
        bool found = false;

        while (pq.size() > 0) {
            //dumpHills(rows, cols, hills);

            sort(pq.begin(), pq.end(), &hillcmp);
            Hill* nextHill = pq.back();
            pq.pop_back();

            int curRow = nextHill->row;
            int curCol = nextHill->col;

            if (curRow == erow && curCol == ecol) {
                found = true;
                break;
            }

            char c = heights[curRow][curCol];
            Hill* curHill = hills[curRow][curCol];
            int curDist = curHill->dist;

            if (curDist > 10000000) {
                Hill* endHill = hills[erow][ecol];
                break;
            }

            int qsize = 0;
            for (int i = pq.size() - 1; i >= 0; --i) {
                Hill* q = pq[i];
                if (q->dist > 1000000) {
                    break;
                }

                ++qsize;
            }

            //cout << "TESTING [" << curRow << ", " << curCol << "], c = " << c << ", dist = " << curDist << ", Q size is " << qsize << endl;
            if (c == 'E') {
                //cout << "REACHED END, len = " << len << endl;
                cout << "PATH WAS: ";
            }
            if (c == 'S') {
                c = 'a';
            }

            if (dsp) {
		curHill->visited = true;
		moveCursor(curRow, curCol);
		cout << BOLD << curHill->height << UNBOLD;
	    }

            auto testDir = [c, heights, hills, curDist, curRow, curCol](int rowOff, int colOff) {
                int trow = curRow + rowOff;
                int tcol = curCol + colOff;
                if (trow < 0 || tcol < 0 || tcol >= heights[0].length() || trow >= heights.size())
                    return;

                char tc = heights[trow][tcol];
                if (tc == 'E') {
                    tc = 'z';
                }
                Hill* hill = hills[trow][tcol];
                //cout << "    [" << trow << ", " << tcol << "]: c = " << c << ", tc = " << tc << ", v=" << hill->visited;

                if ((tc <= c + 1) && !hill->visited) {
                    int newDist = curDist + 1;

                    //cout << "  Valid to check, dist = " << hill->dist << ", new = " << newDist;
                    if (newDist < hill->dist) {
                        hill->dist = newDist;
                        //cout << ", UPDATING";
                    }
                    //cout << endl;
                }
                //cout << endl;

                return;
            };

            testDir(-1, 0);     // up
            testDir(1, 0);      // down
            testDir(0, -1);     // left
            testDir(0, 1);      // right
        }

        if (!found) {
            cout << "NOT FOUND, start = [" << srow << ", " << scol << "]" << endl;
        } else {
            Hill* endHill = hills[erow][ecol];
            cout << "REACHED END FOR [" << srow << ", " << scol << "] END = [" << erow << ", " << ecol << "], dist = " << endHill->dist << endl;

            if (lowSteps < 0 || lowSteps > endHill->dist) {
                lowSteps = endHill->dist;
            }
        }
    }

    cout << "Lowest was " << lowSteps << endl;
}

/**
 * Part 1: This method reads a map of hills from a file and finds the shortest path
 * from a starting point (S) to an ending point (E) through the hills.
 * The pathfinding algorithm used is Dijkstra's algorithm.
 */
void part1()
{
    string s;
    string q;

    ifstream in;
    in.open("hills.txt");
    vector<string> heights;

    int srow = 0, scol = 0, erow = 0, ecol = 0;

    int rows = 0;
    while (getline(in, s)) {
        //cout << s << endl;
        heights.push_back(s);

        int pos = s.find('S');
        if (pos != string::npos) {
            srow = rows;
            scol = pos;
        }

        pos = s.find('E');
        if (pos != string::npos) {
            erow = rows;
            ecol = pos;
        }

        ++rows;
    }
    int cols = heights[0].length();

    Hill*** hills = new Hill** [rows];

    for (int r = 0; r < rows; ++r) {
        hills[r] = new Hill*[cols];
    }

    vector<struct Hill*> pq;

    for (int r = 0; r < rows; ++r) {
        for (int c = 0; c < cols; ++c) {
            int dist = (r == srow && c == scol) ? 0 : 999999999;
            Hill *hill = new Hill{ r, c, heights[r][c], dist, false };
            pq.push_back(hill);
            hills[r][c] = hill;
        }
    }

    dumpHills(rows, cols, hills);

    int testLoop = 20000;
    while (pq.size() > 0) {
        if (--testLoop == 0) {
            cout << "queue size is " << pq.size() << endl;
            cout << "end node is [" << erow << ", " << ecol << "]" << endl;
	    exit(0);
        }

	sort(pq.begin(), pq.end(), &hillcmp);
        Hill* nextHill = pq.back();
        pq.pop_back();

        int curRow = nextHill->row;
        int curCol = nextHill->col;

        if (curRow == erow && curCol == ecol) {
            cout << "REACHED END [" << erow << ", " << ecol << "], dist = " << nextHill->dist << endl;
            exit(0);
        }

	char c = heights[curRow][curCol];
        Hill* curHill = hills[curRow][curCol];
        int curDist = curHill->dist;

        if (curDist > 10000000) {
            Hill* endHill = hills[erow][ecol];
            moveCursor(45, 0);
            cout << "NO PATH? End dist is " << endHill->dist << endl;
            exit(0);
        }

        int qsize = 0;
        for (int i = pq.size()-1; i >= 0; --i) {
	    Hill* q = pq[i];
            if (q->dist > 1000000) {
                break;
            }

	    ++qsize;
        }

	//cout << "TESTING [" << curRow << ", " << curCol << "], c = " << c << ", dist = " << curDist << ", Q size is " << qsize << endl;
	if (c == 'E') {
	    //cout << "REACHED END, len = " << len << endl;
	    cout << "PATH WAS: ";
	}
	if (c == 'S') {
	    c = 'a';
	}

        curHill->visited = true;
        moveCursor(curRow, curCol);
	cout << BOLD << curHill->height << UNBOLD;

        auto testDir = [c, heights, hills, curDist, curRow, curCol](int rowOff, int colOff) {
            int trow = curRow + rowOff;
            int tcol = curCol + colOff;
            if (trow < 0 || tcol < 0 || tcol >= heights[0].length() || trow >= heights.size())
                return;

            char tc = heights[trow][tcol];
            if (tc == 'E') {
                tc = 'z';
            }

	    Hill* hill = hills[trow][tcol];
            //cout << "    [" << trow << ", " << tcol << "]: c = " << c << ", tc = " << tc << ", v=" << hill->visited;

            if ((tc <= c + 1) && ! hill->visited) {
		int newDist = curDist + 1;

		//cout << "  Valid to check, dist = " << hill->dist << ", new = " << newDist;
		if (newDist < hill->dist) {
		    hill->dist = newDist;
		    //cout << ", UPDATING";
		}
		//cout << endl;
            }
            //cout << endl;

            return;
        };

        testDir(-1, 0);     // up
        testDir(1, 0);      // down
        testDir(0, -1);     // left
        testDir(0, 1);      // right
    }
}

int main()
{
    part2();
}
