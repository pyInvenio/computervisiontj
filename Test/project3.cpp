#include <iostream>
#include <bits/stdc++.h>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <math.h>
#include <vector>

#define IMAGE_SIZE 1000
#define NUM_POINTS 1000000

using namespace std;
int totalTimesToCalculateLength = 0;

class Point {
    private:
        double x;
        double y;
    public:
        Point() {
            this->x = (double) rand() / RAND_MAX;
            this->y = (double) rand() / RAND_MAX;
        }
        Point(double x, double y) {
            this->x = x;
            this->y = y;
        }
        double getX() {
            return x;
        }
        double getY() {
            return y;
        }
        friend ostream& operator<<(ostream &strm, const Point &a) {
            return strm << "X: " << a.x << " Y: " << a.y;
        }

};

bool compareX(Point a, Point b) {
    return a.getX() < b.getX();
}

bool compareY(Point a, Point b) {
    return a.getY() < b.getY();
}

struct Color {
    bool r, b, g;
    Color () {
        this->r = true;
        this->g = true;
        this->b = true;
    }
    Color(bool r, bool g, bool b) {
        this->r = r;
        this->g = g;
        this->b = b;
    }
};

void drawPoint(Color (&pixMap)[IMAGE_SIZE][IMAGE_SIZE], vector<Point> points, Point closestPair[2]) {
    for (int i = 0; i < NUM_POINTS; i++) {
        int x = (int) (points.at(i).getX() * IMAGE_SIZE);
        int y = (int) (points.at(i).getY() * IMAGE_SIZE);
        pixMap[y][x] = Color (false, false, false);
    }
    for (int i = 0; i < 2; i++) {
        int x = (int) (closestPair[i].getX() * IMAGE_SIZE);
        int y = (int) (closestPair[i].getY() * IMAGE_SIZE);
        pixMap[y][x] = Color (true, false, false);
    }
}

void generateImage(Color (&pixMap)[IMAGE_SIZE][IMAGE_SIZE]) {
    ofstream imageFile;
    imageFile.open("image.ppm");

    imageFile << "P3 " << IMAGE_SIZE << " " << IMAGE_SIZE << " 1" << endl;

    for (int y = IMAGE_SIZE - 1; y >= 0; y--) {
        for (int x = IMAGE_SIZE - 1; x >= 0; x--) 
            imageFile << (int) pixMap[y][x].r << " " << (int) pixMap[y][x].g << " " << (int) pixMap[y][x].b << " ";
        imageFile << endl;
   }
}

double distance(Point p1, Point p2) {
    return sqrt((p2.getX() - p1.getX()) * (p2.getX() - p1.getX()) + (p2.getY() - p1.getY()) * (p2.getY() - p1.getY()));
}

double min(double x, double y) {
    return x < y ? x : y;
}

Point* bruteForce(vector<Point> points, int left, int right) {
	auto start = std::chrono::high_resolution_clock::now();

    Point* closestPair[2];
    double min_distance = DBL_MAX;
    double temp;
    for (int i = left; i <= right; i++)
        for (int j = i + 1; j <= right; j++) {
            totalTimesToCalculateLength++;
            temp = distance(points[i], points[j]);
            if (temp < min_distance) {
                min_distance = temp;
                closestPair[0] = &points.at(i);
                closestPair[1] = &points.at(j);
            }
        }
    // Testing only
	auto finish = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double, std::milli> elapsed = finish - start;
	//elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(elapsed);

    return *closestPair;
}

Point* stripClosest(vector<Point> points) {
    Point* closestPair[2];
    double min_distance = DBL_MAX;
    double temp;
    for (int i = 0; i < points.size(); i++)
        for (int j = i + 1; j < points.size(); j++) {
            temp = distance(points[i], points[j]);
            if (temp < min_distance) {
                min_distance = temp;
                closestPair[0] = &points.at(i);
                closestPair[1] = &points.at(j);
            }
        }
    return *closestPair;
}

Point* closestPair(vector<Point> &points, int left, int right) {
    if (right - left <= 3)
        return bruteForce(points, left, right);
    int mid = left + (right - left) / 2;
    Point mid_point = points[mid];
    Point* pl = closestPair(points, left, mid);
    Point* pr = closestPair(points, mid + 1, right);
    double dist_pl = distance(pl[0], pl[1]);
    double dist_pr = distance(pr[0], pr[1]);
    Point* p = dist_pl < dist_pr ? pl : pr;
    double dist_p = min(dist_pl, dist_pr);

    vector<Point> strip;
    for (Point p : points)
        if (abs(p.getX() - mid_point.getX() < dist_p))
            strip.push_back(p);
    Point* ps = bruteForce(strip, 0, strip.size() - 1);
    double dist_ps = distance(ps[0], ps[1]);
    return dist_p < dist_ps ? p : ps;
}

Point* closest(vector<Point> &points) {
   	totalTimesToCalculateLength = 0;

   ofstream file; //creates file
	file.open("timesp2.txt");
    sort(points.begin(), points.end(), compareX);
    auto start = std::chrono::high_resolution_clock::now();

    Point* p= closestPair(points, 0, points.size() - 1);
    	auto finish = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double> elapsed = finish - start;
	 elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(elapsed);
    file << elapsed.count() << endl;
   	file << "part2() times to calculate length: " << totalTimesToCalculateLength << endl;
   file.close();
   return p;
    
}

int main() {
    srand((unsigned)time(NULL));

    vector<Point> points;
    for (int i = 0; i < NUM_POINTS; i++)
        points.push_back(Point());

    Color pixMap[IMAGE_SIZE][IMAGE_SIZE];

    Point* closestPair = closest(points);

    drawPoint(pixMap, points, closestPair);

    generateImage(pixMap);

    // for (auto p : points)
    //     cout << p << endl;
    // cout << "Median X at Index " << points.size() / 2 << ": " << points.at(points.size() / 2);

    return 0;
}