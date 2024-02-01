/**
 * Advent of Code 2022 challenge, Day 20.
 * Link: https://adventofcode.com/2022/day/20
 *
 * Challenge: Decrypt "grove positioning system" that produces grove
 * coordinates.
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

struct Element {
    struct Element* next;
    struct Element* prev;
    __int64 pos;
    __int64 value;
};

void dsp(Element *head)
{
    Element *current = head;
    bool first = true;
    //int badcount = 20;
    do {
        if (! first) {
            cout << ", ";
        }
        first = false;
        //cout << current->pos << "/" << current->value;
        cout << current->value;

        current = current->next;

        //if (--badcount == 0) {
            //cout << "BAD" << endl;
            //exit(0);
        //}

    } while (current != head);
    cout << endl;
}

void dsprev(Element *head)
{
    Element *current = head;
    bool first = true;
    do {
        if (! first) {
            cout << ", ";
        }
        first = false;
        //cout << current->pos << "/" << current->value;
        cout << current->value;

        current = current->prev;
    } while (current != head);
    cout << endl;
}

Element* find(Element* head, int idx) {
    Element *current = head;
    do {
        if (current->pos == idx)
            return current;
        current = current->next;
    } while (current != head);

    cout << "Didn't find index " << idx << endl;
    exit(0);
}

/**
 * Part 1: Decode the list of numbers representing encrypted coordinates.
 * The decoding is done by 'mixing' the numbers, moving each number forward
 * or backward in the list a number of positions equal to its value. The list
 * is treated as circular. After the mixing, coordinates are obtained by
 * checking values at specific positions relative to the value 0.
 * <p>
 * The sum of the coordinates at positions 1000th, 2000th, and 3000th after
 * the value 0 is calculated and outputted as the final result.
 */
void part1()
{
    string s;
    string q;
    ifstream in;
    Element* head = NULL;

    in.open("numbers.txt");

    int count = 0;
    Element* current = NULL;
    while (getline(in, s)) {
        cout << s << endl;
        int n = stoi(s);

        Element* e = new Element;
        e->pos = count++;
        e->value = n;
        if (head == NULL) {
            head = e;
            e->next = e;
            e->prev = e;
            current = head;
        }
        else {
            e->prev = current;
            e->next = current->next;
            current->next->prev = e;
            current->next = e;

            current = e;
        }
    }

    //dsp(head);

    for (int idx = 0; idx < count; ++idx) {
        Element* current = find(head, idx);
        int n = current->value;
        cout << endl << "Move value " << n << endl;;
        n = n % (count-1);

        if (n == 0) {
            cout << "SKIP ZERO" << endl;
            continue;
        }

        Element* move = current;
        if (n < 0) {
            while (n++) {
                move = move->prev;
            }
            cout << "    NEG: Move to node value " << move->value << endl;

            // Disconnect
            current->prev->next = current->next;
            current->next->prev = current->prev;

            // current -> move
            current->next = move;
            current->prev = move->prev;
            move->prev->next = current;
            move->prev = current;

        }
        else {
            while (n--) {
                move = move->next;
            }
            cout << "    POS: Move to node value " << move->value << endl;

            // Disconnect
            current->prev->next = current->next;
            current->next->prev = current->prev;

            // move -> current
            current->prev = move;
            current->next = move->next;
            move->next->prev = current;
            move->next = current;
        }
        //dsp(head);
    }

    Element* zeroent = head;
    do {
        if (zeroent->value == 0)
            break;
        zeroent = zeroent->next;
    } while (zeroent != head);

    int p1000 = 1000 % count;
    int p2000 = 2000 % count;
    int p3000 = 3000 % count;

    auto find = [](Element *e, int count) {
        while (count--) {
            e = e->next;
        }
        return e;
    };

    Element* e;
    e = find(zeroent, p1000);
    int n1000 = e->value;

    e = find(zeroent, p2000);
    int n2000 = e->value;

    e = find(zeroent, p3000);
    int n3000 = e->value;

    cout << "n1000: " << n1000 << endl;
    cout << "n2000: " << n2000 << endl;
    cout << "n3000: " << n3000 << endl;

    int sum = n1000 + n2000 + n3000;
    cout << "Sum is " << sum << endl;

}

/**
 * Part 2: Similar to part1, but with additional steps in the decryption
 * process.
 *
 * Each number is first multiplied by a decryption key before the mixing
 * process. The mixing is then performed ten times instead of once. The
 * coordinates are again obtained by checking values at specific positions
 * relative to the value 0, and the sum of these coordinates is outputted
 * as the final result.
 */
void part2()
{
    string s;
    string q;
    ifstream in;
    Element* head = NULL;

    in.open("numbers.txt");

    int count = 0;
    Element* current = NULL;
    while (getline(in, s)) {
        cout << s << endl;
        __int64 n = stoi(s);
        n *= 811589153;

        Element* e = new Element;
        e->pos = count++;
        e->value = n;
        if (head == NULL) {
            head = e;
            e->next = e;
            e->prev = e;
            current = head;
        }
        else {
            e->prev = current;
            e->next = current->next;
            current->next->prev = e;
            current->next = e;

            current = e;
        }
    }

    //dsp(head);

    for (int round = 0; round < 10; ++round) {
        cout << "=================================" << endl << "ROUND " << round << endl;

        for (int idx = 0; idx < count; ++idx) {
            Element* current = find(head, idx);
            __int64 n = current->value;
            //cout << endl << "Move value " << n << endl;;
            n = n % (count - 1);

            if (n == 0) {
                //cout << "SKIP ZERO" << endl;
                continue;
            }

            Element* move = current;
            if (n < 0) {
                while (n++) {
                    move = move->prev;
                }
                //cout << "    NEG: Move to node value " << move->value << endl;

                // Disconnect
                current->prev->next = current->next;
                current->next->prev = current->prev;

                // current -> move
                current->next = move;
                current->prev = move->prev;
                move->prev->next = current;
                move->prev = current;

            }
            else {
                while (n--) {
                    move = move->next;
                }
                //cout << "    POS: Move to node value " << move->value << endl;

                // Disconnect
                current->prev->next = current->next;
                current->next->prev = current->prev;

                // move -> current
                current->prev = move;
                current->next = move->next;
                move->next->prev = current;
                move->next = current;
            }
            //dsp(head);
        }
    }

    Element* zeroent = head;
    do {
        if (zeroent->value == 0)
            break;
        zeroent = zeroent->next;
    } while (zeroent != head);

    __int64 p1000 = 1000 % count;
    __int64 p2000 = 2000 % count;
    __int64 p3000 = 3000 % count;

    auto find = [](Element *e, int count) {
        while (count--) {
            e = e->next;
        }
        return e;
    };

    Element* e;
    e = find(zeroent, p1000);
    __int64 n1000 = e->value;

    e = find(zeroent, p2000);
    __int64 n2000 = e->value;

    e = find(zeroent, p3000);
    __int64 n3000 = e->value;

    cout << "n1000: " << n1000 << endl;
    cout << "n2000: " << n2000 << endl;
    cout << "n3000: " << n3000 << endl;

    __int64 sum = n1000 + n2000 + n3000;
    cout << "Sum is " << sum << endl;
}

int main()
{
    part2();
}
