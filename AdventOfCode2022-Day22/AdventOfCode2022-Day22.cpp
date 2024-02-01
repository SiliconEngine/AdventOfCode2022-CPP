/**
 * Advent of Code 2022 challenge, Day 22.
 * Link: https://adventofcode.com/2022/day/22
 *
 * Challenge: Simulate a map on the surface of a cube and trace path used by
 * monkeys.
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

struct Adjoin {
    struct Face* face;
    int newDir;
    void (*transpose)(int length, int &row, int &col);
};

struct Face {
    int num;
    int rowOff;
    int colOff;
    vector<string> rows;
    struct Adjoin adjoin[4];
};

ostream& operator<<(ostream& out, const Face& f) {
    for (string s : f.rows) {
        cout << s << endl;
    }
    return out;
}

vector<string> board;

enum Dir { D_RIGHT, D_DOWN, D_LEFT, D_UP };

 /**
 * Part 1: Reads the board layout and movement instructions from a file,
 * then simulates the movement across the board. The movement accounts for
 * wrapping around the board edges and stopping at walls. The final position
 * and facing direction are used to calculate a score representing the puzzle
 * solution.
 */
void part1()
{
    string s;
    string path_s;
    ifstream in;

    in.open("map.txt");

    string dirs[4];
    dirs[D_RIGHT] = "RIGHT";
    dirs[D_UP] = "UP";
    dirs[D_LEFT] = "LEFT";
    dirs[D_DOWN] = "DOWN";

    int scoreDir[4];
    scoreDir[D_RIGHT] = 0;
    scoreDir[D_DOWN] = 1;
    scoreDir[D_LEFT] = 2;
    scoreDir[D_UP] = 3;

    enum Dir turnLeft[4];
    turnLeft[D_RIGHT] = D_UP;
    turnLeft[D_UP] = D_LEFT;
    turnLeft[D_LEFT] = D_DOWN;
    turnLeft[D_DOWN] = D_RIGHT;

    enum Dir turnRight[4];
    turnRight[D_RIGHT] = D_DOWN;
    turnRight[D_DOWN] = D_LEFT;
    turnRight[D_LEFT] = D_UP;
    turnRight[D_UP] = D_RIGHT;

    while (getline(in, s)) {
        cout << s << endl;
        if (s == "") {
            getline(in, path_s);
            break;
        }
        board.push_back(s);
    }

    cout << path_s << endl;

    vector<string> path;
    string accum = "";
    for (char c : path_s) {
        if (c >= '0' && c <= '9') {
            accum += c;
        } else {
            if (accum != "") {
                path.push_back(accum);
                accum = "";
            }
	    path.push_back((string)"" + c);
        }
    }
    if (accum != "")
	path.push_back(accum);

    int row = 0;
    int col = 0;
    int dir = D_RIGHT;
    for (col = 0; col < board[0].length(); ++col) {
        if (board[0][col] == '.')
            break;
    }
    cout << "Initial coordinates: [" << row << ", " << col << "]" << endl;

    for (string p : path) {
        cout << "Path: " << p << ", coord: [" << row << ", " << col << "], dir = " << dirs[dir] << endl;
        if (p == "L") {
            dir = turnLeft[dir];
            cout << "    DIR now " << dirs[dir] << endl;
            continue;
        }
        if (p == "R") {
            dir = turnRight[dir];
            cout << "    DIR now " << dirs[dir] << endl;
            continue;
        }

        int amt = stoi(p);
        while (amt--) {
            int saveR = row;
            int saveC = col;

            switch (dir) {
            case D_UP:
                --row;
                cout << "    Move up to row " << row << endl;
                if (row < 0 || col >= board[row].length() || board[row][col] == ' ') {
                    row = board.size() - 1;
		    while (col >= board[row].length() || board[row][col] == ' ')
                        --row;
                    cout << "        Wraparound, row is now " << row << endl;
                }
                break;
            case D_DOWN:
                ++row;
                cout << "    Move down to row " << row << endl;
                if (row >= board.size() || col >= board[row].length() || board[row][col] == ' ') {
                    row = 0;
		    while (col >= board[row].length() || board[row][col] == ' ')
                        ++row;
                    cout << "        Wraparound, row is now " << row << endl;
                }
                break;
            case D_LEFT:
                --col;
                cout << "    Move left to col " << col << endl;
                if (col < 0 || board[row][col] == ' ') {
                    col = board[row].length() - 1;
		    while (board[row][col] == ' ')
                        --col;
                    cout << "        Wraparound, col is now " << col << endl;
                }
                break;
            case D_RIGHT:
                ++col;
                cout << "    Move right to col " << col << endl;
                if (col >= board[row].length() || board[row][col] == ' ') {
                    col = 0;
		    while (board[row][col] == ' ')
                        ++col;
                    cout << "        Wraparound, col is now " << col << endl;
                }
                break;
            }

            if (board[row][col] == '#') {
                row = saveR;
                col = saveC;
                cout << "    Hit wall, back to [" << row << ", " << col << "]" << endl;
                break;
            }
        }
    }

    int score = 1000 * (row + 1) + 4 * (col + 1) + scoreDir[dir];
    cout << "score is " << score << endl;
}

/**
 * Part 2 -  This method extends the simulation from part1() to a
 * cube-shaped map. It reads the same initial map and instructions, but now
 * the movement wraps around the faces of a cube. The algorithm accounts for
 * the cube's geometry when moving off one face onto another. The final score
 * is calculated based on the adjusted position and orientation considering
 * the cube structure.
 */
void part2()
{
    string s;
    string path_s;
    ifstream in;

    in.open("map.txt");
    int length = 50; // 4;

    string dirs[4];
    dirs[D_RIGHT] = "RIGHT";
    dirs[D_UP] = "UP";
    dirs[D_LEFT] = "LEFT";
    dirs[D_DOWN] = "DOWN";

    int scoreDir[4];
    scoreDir[D_RIGHT] = 0;
    scoreDir[D_DOWN] = 1;
    scoreDir[D_LEFT] = 2;
    scoreDir[D_UP] = 3;

    enum Dir turnLeft[4];
    turnLeft[D_RIGHT] = D_UP;
    turnLeft[D_UP] = D_LEFT;
    turnLeft[D_LEFT] = D_DOWN;
    turnLeft[D_DOWN] = D_RIGHT;

    enum Dir turnRight[4];
    turnRight[D_RIGHT] = D_DOWN;
    turnRight[D_DOWN] = D_LEFT;
    turnRight[D_LEFT] = D_UP;
    turnRight[D_UP] = D_RIGHT;

    struct Face* faces[6];

    while (getline(in, s)) {
        cout << s << endl;
        if (s == "") {
            getline(in, path_s);
            break;
        }
        board.push_back(s);
    }

    Face face1, face2, face3, face4, face5, face6;
    int offset = 0;

    for (int r = 0; r < length; ++r) {
        offset = length;
        s = board[r].substr(offset, length);
        face1.rows.push_back(s);
	face1.rowOff = 0;
        face1.colOff = offset;

        offset += length;
        s = board[r].substr(offset, length);
        face2.rows.push_back(s);
	face2.rowOff = 0;
        face2.colOff = offset;
    }

    for (int r = length; r < length*2; ++r) {
        offset = length;
        s = board[r].substr(offset, length);
        face3.rows.push_back(s);
	face3.rowOff = length;
        face3.colOff = offset;
    }

    for (int r = length*2; r < length*3; ++r) {
        offset = 0;
        s = board[r].substr(offset, length);
        face4.rows.push_back(s);
	face4.rowOff = length*2;
        face4.colOff = offset;

        offset += length;
        s = board[r].substr(offset, length);
        face5.rows.push_back(s);
	face5.rowOff = length*2;
        face5.colOff = offset;
    }

    for (int r = length * 3; r < length * 4; ++r) {
        offset = 0;
        s = board[r].substr(offset, length);
        face6.rows.push_back(s);
        face6.rowOff = length * 3;
        face6.colOff = offset;
    }

    face1.adjoin[D_UP] = { &face6, D_RIGHT,
        [](int length, int& row, int& col) {
            ++row;
            row = col;
	    col = 0;
	    return;
	}
    };
    face1.adjoin[D_DOWN] = { &face3, D_DOWN,
        [](int length, int& row, int& col) {
            --row;
            row = 0;
	    return;
	}
    };
    face1.adjoin[D_LEFT] = { &face4, D_RIGHT,
        [](int length, int& row, int& col) {
            ++col;
	    col = 0;
            row = (length - 1) - row;
	    return;
	}
    };
    face1.adjoin[D_RIGHT] = { &face2, D_RIGHT,
        [](int length, int& row, int& col) {
            --col;
	    col = 0;
	    return;
	}
    };

    face2.adjoin[D_UP] = { &face6, D_UP,
        [](int length, int& row, int& col) {
            ++row;
	    row = length - 1;
	    return;
	}
    };
    face2.adjoin[D_DOWN] = { &face3, D_LEFT,
        [](int length, int& row, int& col) {
            --row;
            row = col;
	    col = length - 1;
	    return;
	}
    };
    face2.adjoin[D_LEFT] = { &face1, D_LEFT,
        [](int length, int& row, int& col) {
            ++col;
	    col = length - 1;
	    return;
	}
    };
    face2.adjoin[D_RIGHT] = { &face5, D_LEFT,
        [](int length, int& row, int& col) {
            --col;
	    row = (length - 1) - row;
            col = length - 1;
	    return;
	}
    };

    face3.adjoin[D_UP] = { &face1, D_UP,
        [](int length, int& row, int& col) {
            ++row;
	    row = length - 1;
	    return;
	}
    };
    face3.adjoin[D_DOWN] = { &face5, D_DOWN,
        [](int length, int& row, int& col) {
            --row;
	    row = 0;
	    return;
	}
    };
    face3.adjoin[D_LEFT] = { &face4, D_DOWN,
        [](int length, int& row, int& col) {
            ++col;
            col = row;
	    row = 0;
	    return;
	}
    };
    face3.adjoin[D_RIGHT] = { &face2, D_UP,
        [](int length, int& row, int& col) {
            --col;
            col = row;
	    row = length - 1;
	    return;
	}
    };

    face4.adjoin[D_UP] = { &face3, D_RIGHT,
        [](int length, int& row, int& col) {
            ++row;
	    row = col;
	    col = 0;
	    return;
	}
    };
    face4.adjoin[D_DOWN] = { &face6, D_DOWN,
        [](int length, int& row, int& col) {
            --row;
	    row = 0;
	    return;
	}
    };
    face4.adjoin[D_LEFT] = { &face1, D_RIGHT,
        [](int length, int& row, int& col) {
            ++col;
	    col = 0;
	    row = (length - 1) - row;
	    return;
	}
    };
    face4.adjoin[D_RIGHT] = { &face5, D_RIGHT,
        [](int length, int& row, int& col) {
            --col;
            col = 0;
	    return;
	}
    };

    face5.adjoin[D_UP] = { &face3, D_UP,
        [](int length, int& row, int& col) {
            ++row;
            row = (length - 1);
	    return;
	}
    };
    face5.adjoin[D_DOWN] = { &face6, D_LEFT,
        [](int length, int& row, int& col) {
            --row;
	    row = col;
	    col = length - 1;
	    return;
	}
    };
    face5.adjoin[D_LEFT] = { &face4, D_LEFT,
        [](int length, int& row, int& col) {
            ++col;
	    col = length - 1;
	    return;
	}
    };
    face5.adjoin[D_RIGHT] = { &face2, D_LEFT,
        [](int length, int& row, int& col) {
            --col;
	    col = length - 1;
	    row = (length - 1) - row;
	    return;
	}
    };

    face6.adjoin[D_UP] = { &face4, D_UP,
        [](int length, int& row, int& col) {
            ++row;
	    row = length - 1;
	    return;
	}
    };
    face6.adjoin[D_DOWN] = { &face2, D_DOWN,
        [](int length, int& row, int& col) {
            --row;
	    row = 0;
	    return;
	}
    };
    face6.adjoin[D_LEFT] = { &face1, D_DOWN,
        [](int length, int& row, int& col) {
            ++col;
	    col = row;
	    row = 0;
	    return;
	}
    };
    face6.adjoin[D_RIGHT] = { &face5, D_UP,
        [](int length, int& row, int& col) {
            --col;
	    col = row;
	    row = length - 1;
	    return;
	}
    };


    faces[0] = &face1;
    faces[1] = &face2;
    faces[2] = &face3;
    faces[3] = &face4;
    faces[4] = &face5;
    faces[5] = &face6;


    int n = 0;
    for (auto f : faces) {
        f->num = ++n;
        cout << endl << "FACE: " << f->num << endl;
        cout << *f;
    }

    cout << path_s << endl;

    vector<string> path;
    string accum = "";
    for (char c : path_s) {
        if (c >= '0' && c <= '9') {
            accum += c;
        } else {
            if (accum != "") {
                path.push_back(accum);
                accum = "";
            }
	    path.push_back((string)"" + c);
        }
    }
    if (accum != "")
	path.push_back(accum);

    Face* face = faces[0];

    int row = 0;
    int col = 0;
    int dir = D_RIGHT;
    for (col = 0; col < board[0].length(); ++col) {
        if (face->rows[0][col] == '.')
            break;
    }
    cout << "Initial coordinates: face " << face->num << " [" << row << ", " << col << "]" << endl;

    for (string p : path) {
        cout << "Path: " << p << ", face " << face->num << ", coord: [" << row << ", " << col << "] , dir = " << dirs[dir] << endl;
        if (p == "L") {
            dir = turnLeft[dir];
            cout << "    DIR now " << dirs[dir] << endl;
            continue;
        }
        if (p == "R") {
            dir = turnRight[dir];
            cout << "    DIR now " << dirs[dir] << endl;
            continue;
        }

        int amt = stoi(p);
        while (amt--) {
            int saveR = row;
            int saveC = col;
            Face* saveFace = face;
            int saveDir = dir;

            switch (dir) {
            case D_UP:
                --row;
                cout << "    Move up to row " << row << endl;
                if (row < 0) {
                    Adjoin* a = &face->adjoin[dir];
                    a->transpose(length, row, col);
                    face = a->face;
                    dir = a->newDir;
                    cout << "        Wraparound, moved to face " << face->num << " coords [" << row << ", " << col << "], dir = " << dirs[dir] << endl;
                }
                break;
            case D_DOWN:
                ++row;
                cout << "    Move down to row " << row << endl;
                if (row >= length) {
                    Adjoin* a = &face->adjoin[dir];
                    a->transpose(length, row, col);
                    face = a->face;
                    dir = a->newDir;
                    cout << "        Wraparound, moved to face " << face->num << " coords [" << row << ", " << col << "], dir = " << dirs[dir] << endl;
                }
                break;
            case D_LEFT:
                --col;
                cout << "    Move left to col " << col << endl;
                if (col < 0) {
                    Adjoin* a = &face->adjoin[dir];
                    a->transpose(length, row, col);
                    face = a->face;
                    dir = a->newDir;
                    cout << "        Wraparound, moved to face " << face->num << " coords [" << row << ", " << col << "], dir = " << dirs[dir] << endl;
                }
                break;
            case D_RIGHT:
                ++col;
                cout << "    Move right to col " << col << endl;
                if (col >= length) {
                    Adjoin* a = &face->adjoin[dir];
                    a->transpose(length, row, col);
                    face = a->face;
                    dir = a->newDir;
                    cout << "        Wraparound, moved to face " << face->num << " coords [" << row << ", " << col << "], dir = " << dirs[dir] << endl;
                }
                break;
            }

            if (face->rows[row][col] == '#') {
                row = saveR;
                col = saveC;
                dir = saveDir;
                face = saveFace;
				cout << "    Hit wall, back to face " << face->num << " coords [" << row << ", " << col << "], dir = " << dirs[dir] << endl;
                break;
            }
        }
    }

    cout << "End, at face " << face->num << " coords [" << row << ", " << col << "], dir = " << dirs[dir] << endl;

    int adjRow = face->rowOff + row;
    int adjCol = face->colOff + col;

    cout << "adjRow = " << adjRow << ", adjCol = " << adjCol << endl;

    int score = 1000 * (adjRow + 1) + 4 * (adjCol + 1) + scoreDir[dir];
    cout << "score is " << score << endl;
}

// Alternate part 2
void part2_alt()
{
    string s;
    string path_s;
    ifstream in;

    in.open("map.txt");
    int length = 50; // 4;

    string dirs[4];
    dirs[D_RIGHT] = "RIGHT";
    dirs[D_UP] = "UP";
    dirs[D_LEFT] = "LEFT";
    dirs[D_DOWN] = "DOWN";

    int scoreDir[4];
    scoreDir[D_RIGHT] = 0;
    scoreDir[D_DOWN] = 1;
    scoreDir[D_LEFT] = 2;
    scoreDir[D_UP] = 3;

    enum Dir turnLeft[4];
    turnLeft[D_RIGHT] = D_UP;
    turnLeft[D_UP] = D_LEFT;
    turnLeft[D_LEFT] = D_DOWN;
    turnLeft[D_DOWN] = D_RIGHT;

    enum Dir turnRight[4];
    turnRight[D_RIGHT] = D_DOWN;
    turnRight[D_DOWN] = D_LEFT;
    turnRight[D_LEFT] = D_UP;
    turnRight[D_UP] = D_RIGHT;

    struct Face* faces[6];

    while (getline(in, s)) {
        cout << s << endl;
        if (s == "") {
            getline(in, path_s);
            break;
        }
        board.push_back(s);
    }

    Face face1, face2, face3, face4, face5, face6;
    int offset = length * 2;
    for (int r = 0; r < length; ++r) {
        s = board[r].substr(offset, length);
        face1.rows.push_back(s);
    }
    face1.rowOff = 0;
    face1.colOff = length*2;

    for (int r = length; r < length*2; ++r) {
        offset = 0;
        s = board[r].substr(offset, length);
        face2.rows.push_back(s);
	face2.rowOff = length;
        face2.colOff = 0;

        offset += length;
        s = board[r].substr(offset, length);
        face3.rows.push_back(s);
	face3.rowOff = length;
        face3.colOff = length;

        offset += length;
        s = board[r].substr(offset, length);
        face4.rows.push_back(s);
	face4.rowOff = length;
        face4.colOff = length*2;
    }

    for (int r = length*2; r < length*3; ++r) {
        offset = length*2;
        s = board[r].substr(offset, length);
        face5.rows.push_back(s);
	face5.rowOff = length*2;
        face5.colOff = length*2;

        offset += length;
        s = board[r].substr(offset, length);
        face6.rows.push_back(s);
	face6.rowOff = length*2;
        face6.colOff = length*3;
    }

    face1.adjoin[D_UP] = { &face5, D_UP,
        [](int length, int& row, int& col) {
            ++row;
            row = length - 1;
	    return;
	}
    };
    face1.adjoin[D_DOWN] = { &face4, D_DOWN,
        [](int length, int& row, int& col) {
            --row;
            row = 0;
	    return;
	}
    };
    face1.adjoin[D_LEFT] = { &face3, D_DOWN,
        [](int length, int& row, int& col) {
            ++col;
            col = row;
            row = 0;
	    return;
	}
    };
    face1.adjoin[D_RIGHT] = { &face6, D_LEFT,
        [](int length, int& row, int& col) {
            --col;
            row = (length-1) - row;
	    col = length - 1;
	    return;
	}
    };

    face2.adjoin[D_UP] = { &face1, D_DOWN,
        [](int length, int& row, int& col) {
            ++row;
            row = 0;
	    col = (length-1) - col;
	    return;
	}
    };
    face2.adjoin[D_DOWN] = { &face5, D_UP,
        [](int length, int& row, int& col) {
            --row;
            row = length-1;
	    col = (length - 1) - col;
	    return;
	}
    };
    face2.adjoin[D_LEFT] = { &face6, D_UP,
        [](int length, int& row, int& col) {
            ++col;
            col = (length - 1) - row;
            row = length - 1;
	    return;
	}
    };
    face2.adjoin[D_RIGHT] = { &face3, D_RIGHT,
        [](int length, int& row, int& col) {
            --col;
            col = 0;
	    return;
	}
    };

    face3.adjoin[D_UP] = { &face1, D_RIGHT,
        [](int length, int& row, int& col) {
            ++row;
            row = col;
	    col = 0;
	    return;
	}
    };
    face3.adjoin[D_DOWN] = { &face5, D_RIGHT,
        [](int length, int& row, int& col) {
            --row;
            row = (length - 1) - col;
	    col = 0;
	    return;
	}
    };
    face3.adjoin[D_LEFT] = { &face2, D_LEFT,
        [](int length, int& row, int& col) {
            ++col;
            col = length - 1;
	    return;
	}
    };
    face3.adjoin[D_RIGHT] = { &face4, D_RIGHT,
        [](int length, int& row, int& col) {
            --col;
            col = 0;
	    return;
	}
    };

    face4.adjoin[D_UP] = { &face1, D_UP,
        [](int length, int& row, int& col) {
            ++row;
            row = length - 1;
	    return;
	}
    };
    face4.adjoin[D_DOWN] = { &face5, D_DOWN,
        [](int length, int& row, int& col) {
            --row;
            row = 0;
	    return;
	}
    };
    face4.adjoin[D_LEFT] = { &face3, D_LEFT,
        [](int length, int& row, int& col) {
            ++col;
            col = length - 1;
	    return;
	}
    };
    face4.adjoin[D_RIGHT] = { &face6, D_DOWN,
        [](int length, int& row, int& col) {
            --col;
	    col = (length - 1) - row;
            row = 0;
	    return;
	}
    };

    face5.adjoin[D_UP] = { &face4, D_UP,
        [](int length, int& row, int& col) {
            ++row;
            row = length - 1;
	    return;
	}
    };
    face5.adjoin[D_DOWN] = { &face2, D_UP,
        [](int length, int& row, int& col) {
            --row;
            row = length - 1;
	    col = (length - 1) - col;
	    return;
	}
    };
    face5.adjoin[D_LEFT] = { &face3, D_UP,
        [](int length, int& row, int& col) {
            ++col;
	    col = (length - 1) - row;
            row = length - 1;
	    return;
	}
    };
    face5.adjoin[D_RIGHT] = { &face6, D_RIGHT,
        [](int length, int& row, int& col) {
            --col;
	    col = 0;
	    return;
	}
    };

    face6.adjoin[D_UP] = { &face4, D_LEFT,
        [](int length, int& row, int& col) {
            ++row;
	    row = (length - 1) - col;
            col = length - 1;
	    return;
	}
    };
    face6.adjoin[D_DOWN] = { &face2, D_RIGHT,
        [](int length, int& row, int& col) {
            --row;
            row = (length - 1) - col;
            col = 0;
	    return;
	}
    };
    face6.adjoin[D_LEFT] = { &face5, D_LEFT,
        [](int length, int& row, int& col) {
            ++col;
            col = length - 1;
	    return;
	}
    };
    face6.adjoin[D_RIGHT] = { &face1, D_LEFT,
        [](int length, int& row, int& col) {
            --col;
	    row = (length - 1) - row;
            col = length - 1;
	    return;
	}
    };


    faces[0] = &face1;
    faces[1] = &face2;
    faces[2] = &face3;
    faces[3] = &face4;
    faces[4] = &face5;
    faces[5] = &face6;

    int n = 0;
    for (auto f : faces) {
        cout << endl << "FACE:" << endl;
        cout << *f;
        f->num = ++n;
    }

    cout << path_s << endl;

    vector<string> path;
    string accum = "";
    for (char c : path_s) {
        if (c >= '0' && c <= '9') {
            accum += c;
        } else {
            if (accum != "") {
                path.push_back(accum);
                accum = "";
            }
	    path.push_back((string)"" + c);
        }
    }
    if (accum != "")
	path.push_back(accum);

    Face* face = faces[0];

    int row = 0;
    int col = 0;
    int dir = D_RIGHT;
    for (col = 0; col < board[0].length(); ++col) {
        if (face->rows[0][col] == '.')
            break;
    }
    cout << "Initial coordinates: face " << face->num << " [" << row << ", " << col << "]" << endl;

    for (string p : path) {
        cout << "Path: " << p << ", face " << face->num << ", coord: [" << row << ", " << col << "] , dir = " << dirs[dir] << endl;
        if (p == "L") {
            dir = turnLeft[dir];
            cout << "    DIR now " << dirs[dir] << endl;
            continue;
        }
        if (p == "R") {
            dir = turnRight[dir];
            cout << "    DIR now " << dirs[dir] << endl;
            continue;
        }

        int amt = stoi(p);
        while (amt--) {
            int saveR = row;
            int saveC = col;
            Face* saveFace = face;
            int saveDir = dir;

            switch (dir) {
            case D_UP:
                --row;
                cout << "    Move up to row " << row << endl;
                if (row < 0) {
                    Adjoin* a = &face->adjoin[dir];
                    a->transpose(length, row, col);
                    face = a->face;
                    dir = a->newDir;
                    cout << "        Wraparound, moved to face " << face->num << " coords [" << row << ", " << col << "], dir = " << dirs[dir] << endl;
                }
                break;
            case D_DOWN:
                ++row;
                cout << "    Move down to row " << row << endl;
                if (row >= length) {
                    Adjoin* a = &face->adjoin[dir];
                    a->transpose(length, row, col);
                    face = a->face;
                    dir = a->newDir;
                    cout << "        Wraparound, moved to face " << face->num << " coords [" << row << ", " << col << "], dir = " << dirs[dir] << endl;
                }
                break;
            case D_LEFT:
                --col;
                cout << "    Move left to col " << col << endl;
                if (col < 0) {
                    Adjoin* a = &face->adjoin[dir];
                    a->transpose(length, row, col);
                    face = a->face;
                    dir = a->newDir;
                    cout << "        Wraparound, moved to face " << face->num << " coords [" << row << ", " << col << "], dir = " << dirs[dir] << endl;
                }
                break;
            case D_RIGHT:
                ++col;
                cout << "    Move right to col " << col << endl;
                if (col >= length) {
                    Adjoin* a = &face->adjoin[dir];
                    a->transpose(length, row, col);
                    face = a->face;
                    dir = a->newDir;
                    cout << "        Wraparound, moved to face " << face->num << " coords [" << row << ", " << col << "], dir = " << dirs[dir] << endl;
                }
                break;
            }

            if (face->rows[row][col] == '#') {
                row = saveR;
                col = saveC;
                dir = saveDir;
                face = saveFace;
		cout << "    Hit wall, back to face " << face->num << " coords [" << row << ", " << col << "], dir = " << dirs[dir] << endl;
                break;
            }
        }
    }

    cout << "End, at face " << face->num << " coords [" << row << ", " << col << "], dir = " << dirs[dir] << endl;

    int adjRow = face->rowOff + row;
    int adjCol = face->colOff + col;

    cout << "adjRow = " << adjRow << ", adjCol = " << adjCol << endl;

    int score = 1000 * (adjRow + 1) + 4 * (adjCol + 1) + scoreDir[dir];
    cout << "score is " << score << endl;
}

int main()
{
    part2();
}
