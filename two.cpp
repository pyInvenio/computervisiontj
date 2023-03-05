#include <fstream>
#include <iostream>
#include <set>
#include <string>

using namespace std;

bool minNum(const string line, int i) {};

int main() {
    ifstream file("whereami.in", ios::in);

    short int size;
    file >> size;
    string line;
    file >> line;

    bool ok = false;
    int i = 0;
    while (!ok) {
		ok = minNum(line, i);
        if (!ok) i++;
    }
    file.close();

    ofstream out("whereami.out", ios::out);
    out << i;
    out.close();
    cout << i;
    return 0;
}

bool minNum(const string line, int i) {
	set<string> mailBoxes;
	int size;
	for (int s = 0; s <= line.size() - i; s++) {
		size = mailBoxes.size();
		mailBoxes.insert(line.substr(s, i));
		if (mailBoxes.size() == size)
			return false;
	}
	return true;
}