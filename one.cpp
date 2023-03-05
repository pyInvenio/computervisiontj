#include <fstream>
#include <iostream>
#include <vector>

using namespace std;

int main() {
    ifstream file("gymnastics.in", ios::in);

    int k, n;

    file >> k >> n;

    vector< pair<int, int> > pairing;
   
	vector<int> buffer(n);

    for (int j = 0; j < n; j++) {
        file >> buffer[j];
    }
    for (int i = 0; i < n; i++)
        for (int j = i + 1; j < n; j++) {
            pairing.emplace_back(make_pair(buffer[i], buffer[j]));
        }

    for (int o = 1; o < k; o++) {
        for (int j = 0; j < n; j++) {
            file >> buffer[j];
        }
        vector< pair<int, int> > v;
        pair<int, int> t;
        for (int i = 0; i < n; i++)
            for (int j = i + 1; j < n; j++) {
                t = make_pair(buffer[i], buffer[j]);

                for (pair<int, int>& p : pairing) {

                    if (p.first == t.first && p.second == t.second) {
                        v.emplace_back(t);
                        break;
                    }

                }
            }

        pairing = v;
    }

    file.close();
    ofstream fOut ("gymnastics.out", ios::out);
	fOut << pairing.size();

	fOut.close();
    return 0;
}