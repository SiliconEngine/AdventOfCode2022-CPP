/**
 * Advent of Code 2022 challenge, Day 7.
 * Link: https://adventofcode.com/2022/day/7
 *
 * Challenge: Interpret shell commands from log file and simulate file system,
 * and find smallest directory that would free up sufficient space to run an
 * update.
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

using namespace std;

enum entry_type { DirType, FileType };

class Entry {
public:

    struct File {
	    int size;
    };

    struct Dir {
	Entry* parent;
	vector<Entry*> *entries;
	int totalSize;
    };

    string name;
    union {
        struct File f;
        struct Dir d;
    } data;
    entry_type type;
};

class FS {
private:
    Entry root;

public:
    FS() {
        root.type = DirType;
        root.name = "~";
        root.data.d.parent = NULL;
        root.data.d.entries = new vector<Entry*>;
        root.data.d.totalSize = 0;
    }

    Entry& getRoot()
    {
        return root;
    }

    Entry& addDir(Entry &parent, const string& name)
    {
        cout << "--> Adding dir " << name << " to " << parent.name << endl;

        Entry *newDir = new Entry();
        newDir->type = DirType;
        newDir->name = name;
        newDir->data.d.parent = &parent;
        newDir->data.d.entries = new vector<Entry*>;
        newDir->data.d.totalSize = 0;
        parent.data.d.entries->push_back(newDir);
        cout << "    NEW: name = " << newDir->name << ", parent is " << newDir->data.d.parent->name << endl;
        //dumpFs();
        return *newDir;
    }

    Entry &addFile(Entry &dir, const string &name, int size)
    {
        cout << "--> Adding file " << name << ", size " << size <<  " to " << dir.name << " (" << &dir << ")" << endl;
        Entry *ent = new Entry();
        ent->type = FileType;
        ent->name = name;
        ent->data.f.size = size;
        dir.data.d.entries->push_back(ent);

        Entry* parent = &dir;
        do {
            //cout << "--> Updating dir (" << parent << "): " << parent->name << endl;
            parent->data.d.totalSize += size;
            parent = parent->data.d.parent;
        } while (parent != NULL);

        //dumpFs();
        return *ent;
    }

    Entry& chDir(Entry& entry, const string& name)
    {
//        Entry::Dir& dir = entry.data.d;
        cout << "CHDIR " << name << " FROM " << entry.name << endl;
        if (name == "/") {
            return root;
        }

        if (name == "..") {
            //cout << "    BACK TO " << entry.data.d.parent->name << endl;
            return *entry.data.d.parent;
        }

        for (Entry* ent : *entry.data.d.entries) {
            //cout << "   SCANNING: " << ent->name << endl;
            if (ent->type == DirType && ent->name == name) {
                //cout << "DIR FOUND " << name << " (" << ent << ")" << endl;
				//dumpFs();
                return *ent;
            }
        }
        cout << "NOT FOUND: " << name << endl;
        dumpFs();
        exit(0);
    }

    void dumpNode(Entry* ent)
    {
        string parent = (ent->data.d.parent != NULL) ? ent->data.d.parent->name : "";

        cout << endl << "DIR NODE (" << ent << "): " << ent->name << ", parent : " << parent << ", has "
            << ent->data.d.entries->size() << " nodes, total size is " << ent->data.d.totalSize << endl;
        for (Entry* sub : *ent->data.d.entries) {
            cout << "    SUB (" << sub << "): " << sub->type << " " << sub->name << endl;
        }
        for (Entry* sub : *ent->data.d.entries) {
            if (sub->type == DirType) {
                dumpNode(sub);
            }
        }
        cout << endl;
    }

    void dumpFs()
    {
        cout << "------------- DUMP" << endl << "ROOT: " << root.name << endl;
        dumpNode(&root);
    }

    void findLess(Entry* ent, int &total) {
	if (ent->data.d.totalSize < 100000) {
	    cout << "Found " << ent->name << " total size is " << ent->data.d.totalSize << endl;
            total += ent->data.d.totalSize;
	}
        for (Entry* sub : *ent->data.d.entries) {
            if (sub->type == DirType) {
                findLess(sub, total);
            }
        }
    }

    void findAll()
    {
        int total = 0;
        findLess(&root, total);

        cout << "Grand total is " << total << endl;
    }

    void dsp(Entry* ent) {
	cout << "Size " << ent->name << " total size is " << ent->data.d.totalSize << endl;
        for (Entry* sub : *ent->data.d.entries) {
            if (sub->type == DirType)
                dsp(sub);
        }
    }

    void dspSizes()
    {
        dsp(&root);
    }

    void findIt(Entry* ent, int need, vector<Entry*>& nodes) {
	if (ent->data.d.totalSize >= need) {
	    cout << "Found " << ent->name << " total size is " << ent->data.d.totalSize << endl;
            nodes.push_back(ent);
	}
        for (Entry* sub : *ent->data.d.entries) {
            if (sub->type == DirType) {
                findIt(sub, need, nodes);
            }
        }
    }

    void findSmallest(int need) {
        vector<Entry*> nodes;
        findIt(&root, need, nodes);

        sort(nodes.begin(), nodes.end(), [](const Entry* a, const Entry* b) {
            return a->data.d.totalSize < b->data.d.totalSize;
        });
        //std::sort(nodes.begin(), nodes.end(), compare);

        for (Entry* ent : nodes) {
            cout << "Candidate: " << ent->name << ", size: " << ent->data.d.totalSize << endl;
        }

        cout << "Smallest is: " << nodes[0]->data.d.totalSize << endl;
    }
};

std::vector<std::string> str_split(const std::string& s, const char* pattern) {
    std::regex sep_regex = std::regex{ pattern };
    std::sregex_token_iterator iter(s.begin(), s.end(), sep_regex, -1);
    std::sregex_token_iterator end;
    return { iter, end };
}

/**
 * Part 1: Reads input from a file representing filesystem commands and
 * structure, builds a representation of the filesystem, and then finds
 * directories whose combined size is at most 100000.
 *
 * Part 2: Find the smallest directory that, if deleted,
 * would free up enough space on the filesystem to run the update.
 * This involves calculating the free space, determining the additional space
 * needed, and identifying the smallest directory that meets this requirement.
 */
void day7_run(int part)
{
    string s;
    string q;
    FS fs;
    Entry* curDir = &fs.getRoot();
    enum { st_normal, st_ls } state = st_normal;

    //istringstream in("mjqjpqmgbljsphdztnvjfqwrcgsmlb");
    ifstream in;
    in.open("log.txt");

    while (getline(in, s)) {
        cout << s << endl;
        vector<string> tokens = str_split(s, " ");

        for (;;) {
            switch (state) {
            case st_normal:
		if (tokens[0] == "$") {
		    string cmd = tokens[1];
		    cout << "command: " << cmd << endl;
                    if (cmd == "cd") {
                        curDir = &fs.chDir(*curDir, tokens[2]);
                        cout << "DID CHDIR, is now " << curDir->name << " (" << curDir << ")" << endl;
                    } else if (cmd == "ls") {
                        state = st_ls;
                    }
		}

                break;
            case st_ls:
                if (tokens[0] == "$") {
                    state = st_normal;
                    continue;
                }

                if (tokens[0] == "dir") {
		    fs.addDir(*curDir, tokens[1]);
                    break;
                }

                cout << "LS state, s is " << s << endl;
                cout << "tokens[0] is " << tokens[0] << endl;

                int size = stoi(tokens[0]);
                string name = tokens[1];
                cout << "ls mode: " << name << ": " << size << endl;
                fs.addFile(*curDir, name, size);
                break;
            }

            break;
        }
        cout << "0: " << tokens[0] << endl;

    }

    fs.dumpFs();

    // Part 1
    if (part == 1)
	fs.findAll();

    // Part 2
    if (part == 2)
	fs.dspSizes();

    Entry* root = &fs.getRoot();
    int freeSpace = 70000000 - root->data.d.totalSize;
    int updSize = 30000000;
    int needSize = updSize - freeSpace;
    cout << "Need size is " << needSize << endl;
    fs.findSmallest(needSize);
}

int main()
{
    day7_run(2);
}
