/**
 * Advent of Code 2022 challenge, Day 21.
 * Link: https://adventofcode.com/2022/day/21
 *
 * Challenge: Interpret monkey math operations and pass monkey root's
 * "equality test".
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

vector<string> str_split(const string& s, const char* pattern) {
    regex sep_regex = regex{ pattern };
    sregex_token_iterator iter(s.begin(), s.end(), sep_regex, -1);
    sregex_token_iterator end;
    return { iter, end };
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

struct Monkey {
    string name;
    string math;
    __int64 value;
    bool have;
};

unordered_map<string, Monkey> monkeys;

__int64 getValue(string name)
{
    Monkey* m = &monkeys[name];

    //cout << "GET: " << name << endl;
    if (m->have)
        return m->value;

    auto list = str_split(m->math, " ");
    string param1 = list[0];
    string op = list[1];
    string param2 = list[2];

    __int64 value1 = getValue(param1);
    __int64 value2 = getValue(param2);
    __int64 newval = 0;

    switch (op[0]) {
    case '+':   newval = value1 + value2; break;
    case '-':	newval = value1 - value2; break;
    case '*':	newval = value1 * value2; break;
    case '/':	newval = value1 / value2; break;
    }

    m->value = newval;
    m->have = true;

    return newval;
}

string getString(string name) {
	Monkey* m = &monkeys[name];
	if (m->math[0] >= '0' && m->math[0] <= '9')
		return m->math;

	if (m->name == "humn")
		return m->name;

	//if (m->have)
		//return to_string(m->value);

	auto list = str_split(m->math, " ");
	string param1 = list[0];
	string op = list[1];
	string param2 = list[2];

	if (param1 != "humn")
		param1 = getString(param1);
	if (param2 != "humn")
		param2 = getString(param2);

	return "(" + param1 + " " + op + " " + param2 + ")";
};

string getStringReduce(string name) {
	Monkey* m = &monkeys[name];
	if (m->math[0] >= '0' && m->math[0] <= '9')
		return m->math;

	if (m->name == "humn")
		return m->name;

	//if (m->have)
		//return to_string(m->value);

	auto list = str_split(m->math, " ");
	string param1 = list[0];
	string op = list[1];
	string param2 = list[2];

	if (param1 != "humn")
		param1 = getStringReduce(param1);
	if (param2 != "humn")
		param2 = getStringReduce(param2);

    bool isNum1 = (param1[0] >= '0' && param1[0] <= '9');
	bool isNum2 = (param2[0] >= '0' && param2[0] <= '9');
    if (! isNum1 || ! isNum2)
		return "(" + param1 + " " + op + " " + param2 + ")";

    __int64 value1 = stoull(param1);
    __int64 value2 = stoull(param2);
    __int64 newval = 0;

    switch (op[0]) {
    case '+':   newval = value1 + value2; break;
    case '-':	newval = value1 - value2; break;
    case '*':	newval = value1 * value2; break;
    case '/':	newval = value1 / value2; break;
    }

    return to_string(newval);
};

/**
 * Part 2: Uses a gradient ascent approach.
 * This method reads the monkey data from a file and then iteratively adjusts
 * the 'human' monkey's yelled number to minimize the difference between the
 * values yelled by two specified monkeys. The goal is to find the human's
 * number that makes these values equal (or as close as possible).
 * It starts from an initial guess and incrementally adjusts this number in a
 * small range to find the optimal solution.
 */
void part2()
{
    string s;
    string q;
    ifstream in;

    in.open("monkeys.txt");

    while (getline(in, s)) {
        cout << s << endl;

        Monkey m;
        auto list = str_split(s, ": ");
        m.name = list[0];
        m.math = list[1];
        m.have = false;
        if (m.math[0] >= '0' && m.math[0] <= '9') {
            //cout << "math = " << m.math << endl;
            m.value = stoi(m.math);
            m.have = true;
        }

        monkeys[list[0]] = m;
    }

    // Gradient ascent
    unordered_map<string, Monkey> saveMonkeys = monkeys;

    Monkey* root = &monkeys["root"];
    auto list = str_split(root->math, " ");
    string monkey1 = list[0];
    string monkey2 = list[2];

    auto comp = [monkey1, monkey2, saveMonkeys](__int64 human) {
        monkeys = saveMonkeys;
        Monkey* humn = &monkeys["humn"];
        humn->value = human;
        humn->have = true;

        __int64 rootValue = getValue("root");
        __int64 m1Val = getValue(monkey1);
        __int64 m2Val = getValue(monkey2);

        cout << human << ": numbers are " << m1Val << " - " << m2Val << ", diff = " << abs(m1Val - m2Val) << endl;

        //if (m1Val == m2Val) {
        //    cout << "FOUND IT: " << human << " num is " << m1Val << endl;
        //    exit(0);
        //}

        return abs(m1Val - m2Val);
    };

    __int64 current = 3759569926192;
    __int64 inc = 5;
    __int64 low = current-inc;
    __int64 high = current+inc;

    for (__int64 i = low; i < high; i += 1) {
	//cout << i << ": numbers are " << m1Val << " - " << m2Val << ", diff = " << abs(m1Val - m2Val) << endl;

        __int64 diff = comp(i);
        cout << i << ": " << diff << endl;
    }
}

int main()
{
    part2();
}
