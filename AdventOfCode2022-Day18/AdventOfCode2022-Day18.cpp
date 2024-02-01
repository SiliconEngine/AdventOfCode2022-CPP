/**
 * Advent of Code 2022 challenge, Day 18.
 * Link: https://adventofcode.com/2022/day/18
 *
 * Challenge: Calculate total surface areas of set of cubes with adjacent
 * faces.
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

vector<string> str_split(const string& s, const char* pattern) {
    regex sep_regex = regex{ pattern };
    sregex_token_iterator iter(s.begin(), s.end(), sep_regex, -1);
    sregex_token_iterator end;
    return { iter, end };
}

typedef chrono::high_resolution_clock Clock;

string make(string x, string y, string z)
{
    return x + "," + y + "," + z;
}

string makeKey(vector<string> list)
{
    return list[0] + " - " + list[1] + " - " + list[2] + " - " + list[3];
}

enum FaceDir { DIR_XM, DIR_XP, DIR_YM, DIR_YP, DIR_ZM, DIR_ZP };

struct Coord {
    int x;
    int y;
    int z;

    bool operator<(const Coord& other) const {
        if (x != other.x)
            return x < other.x;
        if (y != other.y)
            return y < other.y;
        return z < other.z;
    }
};

ostream& operator<<(ostream& out, const Coord& c) {
    out << c.x << "," << c.y << "," << c.z;
    return out;
}

struct Face {
    Coord c{};
    string key;
    FaceDir dir;
};

struct Block {
    Coord c;
    Face faces[6];
};

enum BlockStat { STAT_UNKNOWN, STAT_OUTSIDE, STAT_INSIDE };

Face makeFace(Coord c, FaceDir dir)
{
    Face face;
    face.dir = dir;
    face.c = c;
    vector<string> faceList;

    string xm = to_string(c.x - 1) + ".5";
    string xp = to_string(c.x) + ".5";
    string ym = to_string(c.y - 1) + ".5";
    string yp = to_string(c.y) + ".5";
    string zm = to_string(c.z - 1) + ".5";
    string zp = to_string(c.z) + ".5";

    switch (dir) {
    case DIR_XM:
        // xm face
        faceList = { make(xm, ym, zm), make(xm, ym, zp), make(xm, yp, zm), make(xm, yp, zp) };
        break;

    case DIR_XP:
        // xp face
        faceList = { make(xp, ym, zm), make(xp, ym, zp), make(xp, yp, zm), make(xp, yp, zp) };
        break;

    case DIR_YM:
        // ym face
        faceList = { make(xm, ym, zm), make(xm, ym, zp), make(xp, ym, zm), make(xp, ym, zp) };
        break;

    case DIR_YP:
        // yp face
        faceList = { make(xm, yp, zm), make(xm, yp, zp), make(xp, yp, zm), make(xp, yp, zp) };
        break;

    case DIR_ZM:
        // zm face
        faceList = { make(xm, ym, zm), make(xp, ym, zm), make(xm, yp, zm), make(xp, yp, zm) };
        break;

    case DIR_ZP:
        // zp face
        faceList = { make(xm, ym, zp), make(xp, ym, zp), make(xm, yp, zp), make(xp, yp, zp) };
        break;
    }

	sort(faceList.begin(), faceList.end());
    face.key = makeKey(faceList);
    return face;
}

map<Coord, Block> blocks;
map<Coord, BlockStat> status;
int max_x = -999, max_y = -999, max_z = -999, min_x = 999, min_y = 999, min_z = 999;

bool testOOR(Coord c) {
    return (c.x < min_x || c.x > max_x || c.y < min_x || c.y > max_x || c.z < min_x || c.z > max_x);
}

bool testOutside(const Face& face) {
    Coord cur;

    cur = face.c;
    switch (face.dir) {
    case DIR_XM: --cur.x; break; 
    case DIR_XP: ++cur.x; break; 
    case DIR_YM: --cur.y; break; 
    case DIR_YP: ++cur.y; break; 
    case DIR_ZM: --cur.z; break; 
    case DIR_ZP: ++cur.z; break;
    }

    map<Coord, bool> visited;
    vector<Coord> checked;
    queue<Coord> q;
    q.push(cur);
    checked.push_back(cur);
    cout << "Checking: " << cur << " stat is " << status[cur] << endl;

    BlockStat stat = STAT_UNKNOWN;
    while (!q.empty()) {
        cur = q.front();
	cout << "    QUEUE: " << cur << " stat is " << status[cur] << ", length is " << q.size() << endl;

	q.pop();
        visited[cur] = true;

        if (status[cur] != STAT_UNKNOWN) {
            stat = status[cur];
            break;
        }

        Coord testList[6] = { cur, cur, cur, cur, cur, cur };
        --testList[0].x;
        ++testList[1].x;
        --testList[2].y;
        ++testList[3].y;
        --testList[4].z;
        ++testList[5].z;

        for (Coord tc : testList) {
            // Hit another block?
            if (blocks.count(tc) > 0) {
                cout << "        HIT: " << tc << endl;
                continue;
            }

            // Already visited?
            if (visited.count(tc) > 0) {
                cout << "        VISITED: " << tc << endl;
                continue;
            }

            // Out of range?
            if (testOOR(tc)) {
                cout << "        OOR: " << tc << endl;
                stat = STAT_OUTSIDE;
                goto DONE;
            }

	    cout << "        QUEUING: " << tc << endl;
            q.push(tc);
	    visited[tc] = true;
	    checked.push_back(tc);
        }
    }
DONE:
    cout << "    DONE, stat = " << stat << endl;

    // If no path outside, then must be inside
    if (stat == STAT_UNKNOWN) {
        stat = STAT_INSIDE;
    }

    // Update all checked blocks with what we just found out
    for (auto c : checked) {
	cout << "    UPDATING << " << c << " to stat = " << stat << endl;
	if (status[c] != STAT_UNKNOWN && status[c] != stat) {
	    cout << "CONTRADICTION: c = " << c << ", current stat is " << status[c] << " updating to " << stat << endl;
	    exit(0);
	}

	status[c] = stat;
    }

    cout << "    RETURNING STAT " << stat << endl;
    return stat == STAT_OUTSIDE;
}

/**
 * Part 1: Calculates the total surface area of a set of cubes.
 * The method reads cube coordinates from a file, then generates and counts
 * the unique faces of these cubes.
 * Each face is identified by its coordinates, and faces are considered
 * unique if they are not shared between cubes.
 * The total count of unique faces gives the total exposed surface area.
 */
void part1()
{
    string s;
    string q;
    ifstream in;
    map<string, int> faces;

    in.open("blocks.txt");
    while (getline(in, s)) {
	cout << s << endl;
        vector<string> coords = str_split(s, ",");
        int x = stoi(coords[0]);
        int y = stoi(coords[1]);
        int z = stoi(coords[2]);

        string xm = to_string(x - 1) + ".5";
        string xp = to_string(x) + ".5";
        string ym = to_string(y - 1) + ".5";
        string yp = to_string(y) + ".5";
        string zm = to_string(z - 1) + ".5";
        string zp = to_string(z) + ".5";

        // xm face
        vector<string> face1 = { make(xm, ym, zm), make(xm, ym, zp), make(xm, yp, zm), make(xm, yp, zp) };

        // xp face
        vector<string> face2 = { make(xp, ym, zm), make(xp, ym, zp), make(xp, yp, zm), make(xp, yp, zp) };

        // ym face
        vector<string> face3 = { make(xm, ym, zm), make(xm, ym, zp), make(xp, ym, zm), make(xp, ym, zp) };

        // yp face
        vector<string> face4 = { make(xm, yp, zm), make(xm, yp, zp), make(xp, yp, zm), make(xp, yp, zp) };

        // zm face
        vector<string> face5 = { make(xm, ym, zm), make(xp, ym, zm), make(xm, yp, zm), make(xp, yp, zm) };

        // zp face
        vector<string> face6 = { make(xm, ym, zp), make(xp, ym, zp), make(xm, yp, zp), make(xp, yp, zp) };

        sort(face1.begin(), face1.end());
        sort(face2.begin(), face2.end());
        sort(face3.begin(), face3.end());
        sort(face4.begin(), face4.end());
        sort(face5.begin(), face5.end());
        sort(face6.begin(), face6.end());

        ++faces[makeKey(face1)];
        ++faces[makeKey(face2)];
        ++faces[makeKey(face3)];
        ++faces[makeKey(face4)];
        ++faces[makeKey(face5)];
        ++faces[makeKey(face6)];
    }

    int count = 0;
    for (auto e : faces) {
        cout << e.first << ": " << e.second << endl;
        if (e.second == 1)
            ++count;
    }

    cout << "count is " << count << endl;
}

/**
 * Part 2: Calculates the exterior surface area of a set of cubes, considering
 * only the faces exposed to the outside environment.
 * The method reads cube coordinates, creates a 3D grid representation, and
 * determines which cube faces are external.
 * It checks each face of every cube to see if it has an unobstructed path
 * to the outside, indicating it's an exterior face.
 * The count of such faces gives the total exterior surface area.
 */
void part2()
{
    string s;
    string q;
    ifstream in;
    map<string, int> faces;

    in.open("blocks.txt");

    while (getline(in, s)) {
	cout << s << endl;
        vector<string> coords = str_split(s, ",");
        int x = stoi(coords[0]);
        int y = stoi(coords[1]);
        int z = stoi(coords[2]);
        if (x > max_x)
            max_x = x;
        if (y > max_y)
            max_y = y;
        if (z > max_z)
            max_z = z;
        if (x < min_x)
            min_x = x;
        if (y < min_y)
            min_y = y;
        if (z < min_z)
            min_z = z;

        Coord c = { x, y, z };
        //Face f1, f2, f3, f4, f5, f6;

        //blocks[{ x, y, z }] = { { x, y, z }, [ f1, f2, f3, f4, f5, f6 ]};

//        string xm = to_string(x - 1) + ".5";
//        string xp = to_string(x) + ".5";
//        string ym = to_string(y - 1) + ".5";
//        string yp = to_string(y) + ".5";
//        string zm = to_string(z - 1) + ".5";
//        string zp = to_string(z) + ".5";

        Face face1 = makeFace(c, DIR_XM);
        Face face2 = makeFace(c, DIR_XP);
        Face face3 = makeFace(c, DIR_YM);
        Face face4 = makeFace(c, DIR_YP);
        Face face5 = makeFace(c, DIR_ZM);
        Face face6 = makeFace(c, DIR_ZP);

        ++faces[face1.key];
        ++faces[face2.key];
        ++faces[face3.key];
        ++faces[face4.key];
        ++faces[face5.key];
        ++faces[face6.key];

        Block block = { c, { face1, face2, face3, face4, face5, face6 } };

        blocks[c] = block;
        status[c] = STAT_UNKNOWN;
    }

    int rawCount = 0;
    int outCount = 0;
    for (auto b : blocks) {
        Block block = b.second;
        for (auto f : block.faces) {
	    cout << "Doing face: " << f.key << ", get = " << faces[f.key] << endl;
            if (faces[f.key] == 1) {
                ++rawCount;
                if (testOutside(f)) {
                    ++outCount;
                }
            }
	}
    }

    cout << "rawCount is " << rawCount << endl;
    cout << "outCount is " << outCount << endl;
}

int main()
{
    part2();
}
