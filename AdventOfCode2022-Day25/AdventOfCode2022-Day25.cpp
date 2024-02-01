/**
 * Advent of Code 2022 challenge, Day 25.
 * Link: https://adventofcode.com/2022/day/25
 *
 * Challenge: Convert "SNAFU"-encoded strings to and from integers.
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
#define INP_FILE "snafu.txt"

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

ostream& operator<<(ostream& out, const pair<int,int>& p) {
    cout << "[" << p.first << ", " << p.second << "]";
    return out;
}

/**
 * Converts a SNAFU (Special Numeral-Analogue Fuel Units) string to its
 * decimal equivalent.
 *
 * SNAFU is a numeral system based on powers of 5 with unique character
* representations:
 * '2', '1', '0', '-', and '=' correspond to 2, 1, 0, -1, and -2, respectively.
 *
 * The method iterates through each character of the SNAFU string from left
 * to right, converting each character to its decimal value and accumulating
 * the result considering the powers of 5.
 *
 * @param snafu The SNAFU string to be converted to decimal.
 * @return The decimal equivalent of the SNAFU string.
 */
__int64 to_dec(string snafu) {
    __int64 n = 0;
    for (char c : snafu) {
        int val = 0;
        if (c >= '0' && c <= '2') {
            val = c - '0';
        } else if (c == '-') {
            val = -1;
        } else if (c == '=') {
            val = -2;
        }

        n = n * 5 + val;
        //cout << c << ": " << val << ", n is now " << n << endl;
    }

    return n;
}

string dsp(int *digits) {
	string s = "";
	int leftmost = 0;
	for (int i = 100; --i >= 0; ) {
		if (digits[i] != 0) {
			leftmost = i;
			break;
		}
	}

	bool first = true;
	for (int i = leftmost; i >= 0; --i) {
		if (!first)
			s += ", ";
		first = false;
		s += to_string(digits[i]);
	}

	return s;
}

string cvt(int *digits) {
	string sym = "=-012";
	string s = "";
	int leftmost = 0;
	for (int i = 100; --i >= 0; ) {
		if (digits[i] != 0) {
			leftmost = i;
			break;
		}
	}
	for (int i = leftmost; i >= 0; --i)
		s += sym[digits[i] + 2];
	return s;
}

/**
 * Converts a decimal number to its SNAFU (Special Numeral-Analogue Fuel Units)
 * representation.
 * The SNAFU format uses a base-5 numbering system with digits represented as
 * '2', '1', '0', '-', and '='.
 *
 * This method applies a modified base conversion algorithm. It processes the
 * decimal number by repeatedly dividing it by 5 and determining the
 * remainder. Each remainder is then mapped to its corresponding SNAFU digit.
 * The process handles borrowings and carryovers for negative remainders.
 *
 * @param dec The decimal number to be converted to SNAFU format.
 * @return The SNAFU representation of the given decimal number.
 */
string to_snafu(__int64 dec)
{
    __int64 n = dec;
    int base = 5;
    int digits[100]{ 0 };
    int place = 0;
    pair<int, int> sdigits[5] = {
        { 0, 0 },  // 0
        { 0, 1 },  // 1
        { 0, 2 },  // 2
        { 1, -2 }, // 3
        { 1, -1 }, // 4
    };


    while (n) {
        int m = n % base;
        digits[place] += sdigits[m].second;
        digits[place + 1] += sdigits[m].first;
        ++place;

        for (int chk = 0; chk <= place; ++chk) {
            int n = digits[chk] % base;
            if (n > 2) {
                //cout << "[POS: n = " << n << "] pair=" << sdigits[n] << " " << dsp(digits) << endl;
                digits[chk] -= n;
                digits[chk] += sdigits[n].second;
                digits[chk + 1] += sdigits[n].first;
                //cout << "[FIX: n = " << n << "] " << dsp(digits) << endl;
            }
            if (n < -2) {
                cout << "[NEG: n = " << n << "]" << endl;
                // Equivalent: Add 5 to lsd, subtract 1 from msd
                digits[chk] += 5;
                digits[chk + 1] -= 1;
                cout << "[FIX: n = " << n << "] " << dsp(digits) << endl;
exit(0);
            }
        }

        n = n / base;
        //cout << digits.size() << ": " << m << ", sdigits = [" << sdigits[m].first << ", " << sdigits[m].second << "], n is now " << n << endl;
        //cout << place << ": " << dsp(digits) << ", n = " << n << endl;
    }

    return cvt(digits);
}

void part1()
{
    string s;
    ifstream in;

    in.open(INP_FILE);
    __int64 total = 0;
    while (getline(in, s)) {
        cout << s << endl;
        //cout << s << ": " << to_dec(s) << endl;
        __int64 dec = to_dec(s);
        total += dec;

        string chk = to_snafu(dec);
        if (chk != s) {
            cout << "BAD: " << s << ", chk = " << chk << endl;
            exit(0);
        }
    }
    cout << "Total is " << total << endl;
    cout << "Snafu is " << to_snafu(total) << " (" << to_dec(to_snafu(total)) << ")" << endl;

    //string snafu = "1=-0-2";
    //cout << snafu << ": " << to_dec(snafu) << endl;

    //char sym[5] = { '=', '-', '0', '1', '2' };

    string sym = "ABCDEFGHIJKL";
    string snafu = "";
    //srand(time(0));
    //__int64 dec = rand();
    //__int64 dec = 314159265;

    //cout << dec << ": " << cvt(digits) << " (" << dsp(digits) << ")" << endl;

    //for (__int64 x = 123555; x < 1235557; x += 777) {
		//cout << x << ": " << to_snafu(x) << endl;
    //}

}


int main()
{
    part1();
}
