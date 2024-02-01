/**
 * Advent of Code 2022 challenge, Day 13.
 * Link: https://adventofcode.com/2022/day/13
 *
 * Challenge: Decode nested packet format and determine "decoder key"
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

enum Type { T_SCALAR, T_VECTOR };

struct Entry {
    Type type;
    Entry* parent;
    union {
        int scalar;
        vector<Entry*>* list;
    } data;
};

string dumpEntry(Entry* ent) {
    string s = "[";
    bool first = true;
    for (auto e : *ent->data.list) {
        if (!first) {
            s += ",";
        }
		first = false;
        if (e->type == T_SCALAR) {
            s += to_string(e->data.scalar);
        } else {
            s += dumpEntry(e);
        }
    }
    s += "]";
    return s;
}

enum Result { R_GOOD, R_BAD, R_EQUAL };

string fmtResult(Result r) {
    if (r == R_GOOD) {
        return "GOOD";
    } else if (r == R_BAD) {
        return "BAD";
    } else if (r == R_EQUAL) {
        return "EQUAL";
    }
}

Result cmpEntry(Entry* first, Entry* second)
{
    vector<Entry*>* list1 = first->data.list;
    vector<Entry*>* list2 = second->data.list;
    int minSize = min(list1->size(), list2->size());
    cout << "    Size: " << list1->size() << " / " << list2->size() << endl;

    for (int i = 0; i < minSize; ++i) {
        Entry* ent1 = list1->at(i);
        Entry* ent2 = list2->at(i);

        if (ent1->type == T_SCALAR && ent2->type == T_SCALAR) {
            if (ent1->data.scalar < ent2->data.scalar) {
                cout << "    SCALARS IN GOOD ORDER" << endl;
                return R_GOOD;
            }
            if (ent1->data.scalar > ent2->data.scalar) {
                cout << "    SCALARS IN BAD ORDER" << endl;
                return R_BAD;
            }
            cout << "    SCALARS EQUAL" << endl;

        } else if (ent1->type == T_VECTOR && ent2->type == T_VECTOR) {
            cout << "    COMPARING VECTORS" << endl;
            Result r = cmpEntry(ent1, ent2);
            cout << "    BACK, RESULT WAS " << fmtResult(r) << endl;
            if (r != R_EQUAL) {
                cout << "    RETURNING ABOVE" << endl;
                return r;
            }

        } else {
            // One list, one scalar

            Entry temp;
            temp.type = T_VECTOR;
            temp.parent = NULL;
            temp.data.list = new vector<Entry*>;
            Result r;

            if (ent1->type == T_VECTOR) {
                cout << "    FIRST WAS VECTOR, 2nd SCALAR" << endl;
                temp.data.list->push_back(ent2);
                r = cmpEntry(ent1, &temp);
            } else {
                cout << "    FIRST WAS SCALAR, 2nd VECTOR" << endl;
                temp.data.list->push_back(ent1);
                r = cmpEntry(&temp, ent2);
            }

            cout << "    RESULT WAS " << fmtResult(r) << endl;
            if (r != R_EQUAL) {
                return r;
            }
        }
    }

    // All equal, see if second shorter
    if (list2->size() > list1->size()) {
        cout << "    SECOND LONGER, IS GOOD" << endl;
        return R_GOOD;
    }

    if (list2->size() < list1->size()) {
        cout << "    FIRST LONGER, IS BAD" << endl;
        return R_BAD;
    }

    cout << "    LENGTHS WERE EQUAL, RETURNING EQUAL" << endl;
    return R_EQUAL;
}

typedef pair<Entry*, Entry*> Pair;

Entry* parseEnt(string& s) {
    Entry* ent = NULL;
    Entry* curEnt = NULL;
    int n = -1;
    for (auto c : s) {
        cout << "TOKEN: " << c << endl;

        if (c == '[') {
            Entry* newEnt = new Entry;
            cout << "    NEW LIST " << newEnt << endl;
            newEnt->type = T_VECTOR;
            newEnt->data.list = new vector<Entry*>;
            newEnt->parent = curEnt;

            if (curEnt != NULL) {
                curEnt->data.list->push_back(newEnt);
            }

            curEnt = newEnt;
            if (ent == NULL) {
                ent = newEnt;
            }

        }

        if (c == ',' || c == ']') {
            if (n >= 0) {
                cout << "    HAVE SCALAR " << n << endl;
                Entry* newEnt = new Entry;
                newEnt->type = T_SCALAR;
                newEnt->data.scalar = n;
                newEnt->parent = curEnt;
                curEnt->data.list->push_back(newEnt);
                n = -1;
            }
        }

        if (c >= '0' && c <= '9') {
            if (n < 0) {
                n = 0;
            }
            n = n * 10 + (c - '0');
        }

        if (c == ']') {
            cout << "    END LIST" << endl;
            curEnt = curEnt->parent;
        }
    }

    cout << "DUMPING " << ent << endl;
    cout << dumpEntry(ent) << endl;
    return ent;
}

bool sortcmp(Entry* a, Entry* b) {
    Result r = cmpEntry(a, b);
    return r == R_GOOD ? true : false;
}

#define DECODE1 "[[2]]"
#define DECODE2 "[[6]]"

/**
 * Part 1: Process each pair of packet entries in the given list, identifying
 * those that are already in the correct order based on the problem's rules. It then
 * calculates and returns the sum of the indices of these correctly ordered pairs.
 */
void part1()
{
    string s;
    string q;

    ifstream in;
    in.open("packets.txt");

    vector<Pair> cmpList;
    Entry* first = NULL;

    while (getline(in, s)) {
        cout << s << endl;

        if (s == "") {
            continue;
        }

        //vector<string> tokens = str_split(s, "([,\\]\\[])");
        Entry* ent = NULL;
	Entry* curEnt = NULL;
        int n = -1;
        for (auto c : s) {
            cout << "TOKEN: " << c << endl;

            if (c == '[') {
                Entry* newEnt = new Entry;
                cout << "    NEW LIST " << newEnt << endl;
                newEnt->type = T_VECTOR;
                newEnt->data.list = new vector<Entry*>;
                newEnt->parent = curEnt;

                if (curEnt != NULL) {
		    curEnt->data.list->push_back(newEnt);
		}

                curEnt = newEnt;
                if (ent == NULL) {
                    ent = newEnt;
                }
            }
            
            if (c == ',' || c == ']') {
                if (n >= 0) {
                    cout << "    HAVE SCALAR " << n << endl;
		    Entry *newEnt = new Entry;
		    newEnt->type = T_SCALAR;
		    newEnt->data.scalar = n;
		    newEnt->parent = curEnt;
		    curEnt->data.list->push_back(newEnt);
                    n = -1;
                }
            }
            
            if (c >= '0' && c <= '9') {
                if (n < 0) {
                    n = 0;
                }
                n = n * 10 + (c - '0');
            }

            if (c == ']') {
                cout << "    END LIST" << endl;
                curEnt = curEnt->parent;
            }
        }

        cout << "DUMPING " << ent << endl;
        cout << dumpEntry(ent) << endl;

        if (first == NULL) {
            first = ent;
        } else {
            cmpList.push_back(Pair{first, ent});
            first = NULL;
        }
    }

    cout << "Have " << cmpList.size() << " pairs" << endl;

    int i = 0;
    int goodSum = 0;
    for (auto p : cmpList) {
        ++i;
        Result r = cmpEntry(p.first, p.second);
        cout << "RESULT #" << i << ": " << fmtResult(r) << endl;
        if (r == R_GOOD) {
            goodSum += i;
        }
    }
    cout << "Sum is " << goodSum << endl;
}

/**
 * Part 2: Organize the entire set of packets, including divider packets, into a sequence that
 * meets the ordering criteria specified. After sorting, it identifies the positions of key divider packets
 * and computes their product, which acts as the decoder key for the distress signal.
 */
void part2()
{
    string s;
    string q;

    ifstream in;
    in.open("packets.txt");

    vector<Entry *> packetList;
    Entry* first = NULL;

    while (getline(in, s)) {
        cout << s << endl;

        if (s == "") {
            continue;
        }

        Entry* ent = parseEnt(s);
        //vector<string> tokens = str_split(s, "([,\\]\\[])");

        packetList.push_back(ent);
    }

    s = DECODE1;
    packetList.push_back(parseEnt(s));
    s = DECODE2;
    packetList.push_back(parseEnt(s));

    cout << "Have " << packetList.size() << " packets" << endl;

    sort(packetList.begin(), packetList.end(), sortcmp);

    cout << endl << "SORT:" << endl;
    int i = 0;
    int prod = 1;
    for (auto ent : packetList) {
        ++i;
        string s = dumpEntry(ent);
        cout << "Entry #" << i << ": " << s << endl;

        if (s == DECODE1 || s == DECODE2) {
            prod *= i;
        }
    }

    cout << "Prod is " << prod << endl;
}

int main()
{
    part2();
}
