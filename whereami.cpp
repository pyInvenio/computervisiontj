#include <fstream>
#include <iostream>
#include <set>
#include <string>

using namespace std;

bool minK(const string& boxes, int& i) {
    set<string> mail;
    int sizebefore;
    for (int s = 0; s <= boxes.size() - i; s++) {
        sizebefore = mail.size();
        mail.insert(boxes.substr(s, i));
        if (mail.size() == sizebefore) return false;
    }
    return true;
}

int main() {
    ifstream file("whereami.in", ios::in);

    short int size;
    file >> size;
    string boxes;
    file >> boxes;

    bool works = false;
    int i = 0;
    while (!works) {
        works = minK(boxes, i);
        if (!works) i++;
    }
    file.close();
    ofstream out("whereami.out", ios::out);
    out << i;
    out.close();
    cout << i;
    return 0;
}