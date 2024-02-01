/**
 * Advent of Code 2022 challenge, Day 11.
 * Link: https://adventofcode.com/2022/day/11
 *
 * Challenge: Calculate monkey "worry level" based on optimizing monkey chasing strategy.
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

using namespace std;

enum entry_type { DirType, FileType };

std::vector<std::string> str_split(const std::string& s, const char* pattern) {
    std::regex sep_regex = std::regex{ pattern };
    std::sregex_token_iterator iter(s.begin(), s.end(), sep_regex, -1);
    std::sregex_token_iterator end;
    return { iter, end };
}

void dumpVec(vector<string> vec) {
    for (auto t : vec) {
        cout << "'" << t << "', ";
    }
    cout << endl;
}

struct Monkey {
    vector<uint64_t> items;
    vector<string> op;
    int div = 0;
    int falseNum = 0;
    int trueNum = 0;
    int inspCount = 0;
};

void dumpMonkey(Monkey* m) {
    cout << "    Items: ";
    for (auto item : m->items) {
        cout << item << ", ";
    }
    cout << endl;
    cout << "    inspCount = " << m->inspCount << endl;
}

/**
 * Part 1: Simulate the behavior of monkeys throwing items for 20 rounds. Each monkey
 * performs operations on items, checks divisibility, and decides the next monkey to throw to.
 * The method calculates and outputs the two monkeys with the highest inspection counts
 * multiplied together to determine the level of monkey business.
 */
void part1()
{
    string s;
    string q;
    int numRounds = 20;

    //istringstream in("mjqjpqmgbljsphdztnvjfqwrcgsmlb");
    ifstream in;
    in.open("monkey_x.txt");
    vector<struct Monkey*> monkeys;

    int numMonkeys = 0;
    while (getline(in, s)) {
        vector<string> tokens;

        // Monkey #:
        cout << s.substr(7) << endl;
        int monkeyNum = stoi(s.substr(7));
        cout << "Monkey #" << monkeyNum << endl;
        ++numMonkeys;

        Monkey *m = new Monkey;
        monkeys.push_back(m);
        m->inspCount = 0;

        // Starting items: #, #
        getline(in, s);
        cout << s << endl;
        tokens = str_split(s.substr(18), ", ");
        dumpVec(tokens);
        vector<uint64_t> items;
        for (auto t : tokens) {
            items.push_back(stoi(t));
        }
        m->items = items;

        // Operation: new = [old|#] [+*] [old|#}
        getline(in, s);
        cout << s << endl;
        tokens = str_split(s.substr(19), " ");
        dumpVec(tokens);
        m->op = tokens;

        // Test: divisible by [#]
        getline(in, s);
        cout << s << endl;
        uint64_t div = stoi(s.substr(21));
        cout << "div: " << div << endl;
        m->div = div;

        // If true: throw to monkey [#]
        getline(in, s);
        cout << s << endl;
        tokens = str_split(s, " ");
        int trueNum = stoi(s.substr(29));
        cout << "true: " << trueNum << endl;
        m->trueNum = trueNum;

        // If false: throw to monkey [#]
        getline(in, s);
        cout << s << endl;
        tokens = str_split(s, " ");
        int falseNum = stoi(s.substr(30));
        cout << "false: " << falseNum << endl;
        m->falseNum = falseNum;

        // blank line
        getline(in, s);
    }

    for (int round = 0; round < numRounds; ++round) {
	for (int i = 0; i < numMonkeys; ++i) {
	    Monkey* m = monkeys[i];
	    cout << "Monkey #" << i << endl;
            for (auto item : m->items) {
                cout << "  Item " << item << endl;
                int val;
                if (m->op[2] == "old") {
                    val = item;
                } else {
                    val = stoi(m->op[2]);
                }

                switch (m->op[1][0]) {
                case '+':
                    item += val;
                    break;
                case '*':
                    item *= val;
                    break;
                }
                cout << "    Item is now " << item << " (" << m->op[1] << " / " << m->op[2] << " / " << val << ")" << endl;

		item /= 3;
		cout << "    Item is now " << item << " (div by 3)" << endl;

		bool test = (item % m->div) == 0;
		cout << "    test (" << m->div << ") is " << test << " [" << (item % m->div) << "]" << endl;

		if (test) {
		    monkeys[m->trueNum]->items.push_back(item);
		} else {
		    monkeys[m->falseNum]->items.push_back(item);
		}

                ++m->inspCount;
	    }

	    m->items.clear();
	}
    }

    vector<pair<int, int>> counts;
    for (int i = 0; i < numMonkeys; ++i) {
        Monkey* m = monkeys[i];
        cout << endl << "Monkey #" << i << endl;
        dumpMonkey(m);
        counts.push_back(make_pair(i, m->inspCount));
    }

    sort(counts.begin(), counts.end(), [](const pair<int, int>& a, const pair<int, int>& b) {
        return b.second < a.second;
    });

    cout << "end" << endl;
    for (auto c : counts) {
        cout << c.first << ": " << c.second << endl;
    }
    cout << "done" << endl;

    int answer = counts[0].second * counts[1].second;
    cout << "answer is " << answer << endl;
}

/**
 * Part 2: Similar to part1, but with 10,000 rounds and without dividing the worry level by three
 * after each inspection. It computes and outputs the product of the inspection counts
 * of the two most active monkeys after 10,000 rounds to determine the level of monkey business.
 */
void part2()
{
    string s;
    string q;
    int numRounds = 10000;
    int lcmFact = 1;

    //istringstream in("mjqjpqmgbljsphdztnvjfqwrcgsmlb");
    ifstream in;
    in.open("monkey.txt");
    vector<struct Monkey*> monkeys;

    int numMonkeys = 0;
    vector<uint64_t> factors;
    while (getline(in, s)) {
        vector<string> tokens;

        // Monkey #:
        cout << s.substr(7) << endl;
        int monkeyNum = stoi(s.substr(7));
        //cout << "Monkey #" << monkeyNum << endl;
        ++numMonkeys;

        Monkey *m = new Monkey;
        monkeys.push_back(m);
        m->inspCount = 0;

        // Starting items: #, #
        getline(in, s);
        cout << s << endl;
        tokens = str_split(s.substr(18), ", ");
        dumpVec(tokens);
        vector<uint64_t> items;
        for (auto t : tokens) {
            items.push_back(stoi(t));
        }
        m->items = items;

        // Operation: new = [old|#] [+*] [old|#}
        getline(in, s);
        cout << s << endl;
        tokens = str_split(s.substr(19), " ");
        dumpVec(tokens);
        m->op = tokens;

        // Test: divisible by [#]
        getline(in, s);
        cout << s << endl;
        uint64_t div = stoi(s.substr(21));
        factors.push_back(div);
        lcmFact *= div;
        cout << "div: " << div << endl;
        m->div = div;

        // If true: throw to monkey [#]
        getline(in, s);
        cout << s << endl;
        tokens = str_split(s, " ");
        int trueNum = stoi(s.substr(29));
        cout << "true: " << trueNum << endl;
        m->trueNum = trueNum;

        // If false: throw to monkey [#]
        getline(in, s);
        cout << s << endl;
        tokens = str_split(s, " ");
        int falseNum = stoi(s.substr(30));
        cout << "false: " << falseNum << endl;
        m->falseNum = falseNum;

        // blank line
        getline(in, s);
    }

    for (int round = 0; round < numRounds; ++round) {
	for (int i = 0; i < numMonkeys; ++i) {
	    Monkey* m = monkeys[i];
	    cout << "Monkey #" << i << endl;
            for (auto item : m->items) {
                //cout << "  Item " << item << endl;
                uint64_t val;
                if (m->op[2] == "old") {
                    val = item;
                } else {
                    val = stoi(m->op[2]);
                }

                switch (m->op[1][0]) {
                case '+':
                    item += val;
                    break;
                case '*':
		    item *= val;
                    break;
                }
                //cout << "    Item is now " << item << " (" << m->op[1] << " / " << m->op[2] << " / " << val << ")" << endl;

                item = item % lcmFact;

		bool test = (item % m->div) == 0;
		//cout << "    test (" << m->div << ") is " << test << " [" << (item % m->div) << "]" << endl;

		if (test) {
		    monkeys[m->trueNum]->items.push_back(item);
		} else {
		    monkeys[m->falseNum]->items.push_back(item);
		}

                ++m->inspCount;
	    }

	    m->items.clear();
	}
    }

    vector<pair<int, uint64_t>> counts;
    for (int i = 0; i < numMonkeys; ++i) {
        Monkey* m = monkeys[i];
        cout << endl << "Monkey #" << i << endl;
        dumpMonkey(m);
        counts.push_back(make_pair(i, m->inspCount));
    }

    sort(counts.begin(), counts.end(), [](const pair<int, int>& a, const pair<int, int>& b) {
        return b.second < a.second;
    });

    cout << "end" << endl;
    for (auto c : counts) {
        cout << c.first << ": " << c.second << endl;
    }
    cout << "done" << endl;

    uint64_t answer = counts[0].second * counts[1].second;
    cout << "answer is " << answer << endl;
}

int main()
{
    part2();
}
