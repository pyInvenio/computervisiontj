// CV Lab 3.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include<fstream>
using namespace std;
#include <stdlib.h>     // srand, rand 
#include <time.h>       // time
#include <math.h>		//operations
#include <list>
#include <iomanip>
#include <vector>
#include <algorithm>

const int height = 800;
const int width = 800;
int pixels[height][width][3]; //global

class Point {
	double x, y;
public:
	Point() {
		x = y = 0;
	}
	Point(double px, double py) {
		x = px;
		y = py;
	}
	double getX() { return x; }
	double getY() { return y; }
	void setXY(double px, double py) { x = px; y = py; }

};
double distance(Point a, Point b) { //calculate side length of a triangle
	return sqrt((b.getX() - a.getX()) * (b.getX() - a.getX()) + (b.getY() - a.getY()) * (b.getY() - a.getY()));
}

void set_pixel(int x, int y, int r = 0, int g = 0, int b = 0) { //parameters for color
	if (x < 0 || x >= 800 || y < 0 || y >= 800)//out of bounds
		return; //don't illuminate
	pixels[x][y][0] = r;
	pixels[x][y][1] = g;
	pixels[x][y][2] = b; 
}
void drawCircle(int radius, Point center, int r = 0, int g = 0, int b = 0) {
	int centerX = center.getX();
	int centerY = center.getY();
	int x, y, xmax, y2, y2_new, ty;
	xmax = (int)(radius * 0.70710678); 
	y = radius;
	y2 = y * y;
	ty = (2 * y) - 1;
	y2_new = y2;
	for (x = 0; x <= xmax; x++) {
		if ((y2 - y2_new) >= ty) {
			y2 -= ty;
			y -= 1;
			ty -= 2;
		}
		set_pixel(x + centerX, y + centerY, r, g, b);
		set_pixel(x + centerX, -y + centerY, r, g, b);
		set_pixel(-x + centerX, y + centerY, r, g, b);
		set_pixel(-x + centerX, -y + centerY, r, g, b);
		set_pixel(y + centerX, x + centerY, r, g, b);
		set_pixel(y + centerX, -x + centerY, r, g, b);
		set_pixel(-y + centerX, x + centerY, r, g, b);
		set_pixel(-y + centerX, -x + centerY, r, g, b);
		y2_new -= (2 * x) - 3;
	}
}
list<Point> points; //GLOBAL list of points
const int numPoints = 1000;
void generatePoints1() {
	Point p;
	double x, y;

	//adds 50 Point objects to the list
	for (int i = 0; i < numPoints; i++) {
		x = ((double)rand() / (RAND_MAX));
		y = ((double)rand() / (RAND_MAX));
		p.setXY(x, y);
		points.push_back(p); //points is a global list
	}
	ofstream file;
	file.open("points.txt");
	file.precision(24);
	list<Point>::iterator iter;
	//iterator through the list of points and write them in the file
	for (iter = points.begin(); iter != points.end(); ++iter) {
		Point p = *iter;
		file << p.getX() << "  " << p.getY();
		file << endl;
	}
	file.close();
}
Point A1, B1; //these are the points that will be colored red. these are the closets points.
void part1() {
	
	
	
	list<Point>::iterator iter;
	//calculate the distance between every possible pair of points (WITHOUT REPEATING)
	list<Point>::iterator iter2; // new iterator
	double shortestDistance = RAND_MAX; 
	
	for (iter = points.begin(); iter != points.end(); ++iter) {
		Point p = *iter;
		//position iter2 so that it starts on the point directly AFTER where iter is
		iter2 = iter;
		++iter2;
		while (iter2 != points.end()) {
			Point q = *iter2;
			double dist = distance(p, q);
			if (dist < shortestDistance) {
				//if the distance is shorter than shortestDistance, set A to p and B to q
				A1.setXY(p.getX(), p.getY());
				B1.setXY(q.getX(), q.getY());
				shortestDistance = dist;
			}
			++iter2;
		}
	}
	//for convenience, just some extra information
	cout << A1.getX() << ", " << A1.getY();
	cout << endl;
	cout << B1.getX() << ", " << B1.getY();
	cout << endl;
	cout << "short distance by p1: " << shortestDistance << endl;
	

	
}
void drawPPM() {
	for (int x = 0; x < width; x++) //whiteout
		for (int y = 0; y < height; y++) {
			pixels[x][y][0] = 1;
			pixels[x][y][1] = 1;
			pixels[x][y][2] = 1;
		}
	list<Point>::iterator iter;
	//redden the two points
	for (iter = points.begin(); iter != points.end(); ++iter) {
		Point p = *iter;
		Point q(p.getX() * 800, p.getY() * 800);
		if ((p.getX() == A1.getX() && p.getY() == A1.getY()) || (p.getX() == B1.getX() && p.getY() == B1.getY())) {
			drawCircle(2, q, 1);
		}
		else {
			drawCircle(2, q);
		}
	}
	ofstream img("output.ppm");
	img << "P3" << endl;
	img << width << " " << height << endl;
	img << "1" << endl;
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			img << pixels[x][y][0] << " " << pixels[x][y][1] << " " << pixels[x][y][2] << endl;
		}
	}
}
class ClosestPair {
	Point A, B;
	double distance = INT8_MAX;
public:
	ClosestPair() {}
	ClosestPair(Point p, Point q, double d) {
		A = p;
		B = q;
		distance = d;
	}
	Point getPointA() { return A; }
	Point getPointB() { return B; }
	double getDistance() { return distance; }
	void setAB(Point pA, Point pB, double pdist) {
		A.setXY(pA.getX(), pA.getY());
		B.setXY(pB.getX(), pB.getY());
		distance = pdist;
	}
	

};
vector<Point> vectorOfPoints;
void listToVector2() {
	//transfer the points in the list to the vector
	//reminder: "points" is my global list, "vectorOfPoints" is my global vector
	list<Point>::iterator iter;
	for (iter = points.begin(); iter != points.end(); ++iter) {
		Point p = *iter;
		vectorOfPoints.push_back(p);
	}
}
bool compareX(Point p, Point q)
{
	return (p.getX() < q.getX());
}
bool compareY(Point p, Point q)
{
	return (p.getY() < q.getY());
}
ClosestPair bruteForce(vector<Point> vect) {
	double shortestDistance = INT8_MAX;
	ClosestPair cp;
	for (int i = 0; i < vect.size()-1; i++) {
		Point p = vect[i];
		for (int j = i + 1; j < vect.size(); j++) {
			Point q = vect[j];
			double dist = distance(p, q);
			if (dist < shortestDistance) {
				shortestDistance = dist;
				cp.setAB(p, q, shortestDistance);
			}
		}
	}
	return cp;
}
ClosestPair closestDistRecursive(vector<Point> vect) {
	int size = vect.size();

	if (size <= 3) {
		return bruteForce(vect);
	}
	int mid = size / 2;
	Point midPoint = vect[mid];
	vector<Point> left, right;
	for (int i = 0; i < mid; i++) {
		left.push_back(vect[i]);
	}
	for (int i = mid; i < size; i++) {
		right.push_back(vect[i]);
	}
	ClosestPair d1 = closestDistRecursive(left);
	ClosestPair d2 = closestDistRecursive(right);

	double d = min(d1.getDistance(), d2.getDistance());

	//strip
	vector<Point> leftStrip, rightStrip;
	for (int i = left.size() - 1; i >= 0 && (midPoint.getX() - left[i].getX() < d); i--) {
		leftStrip.push_back(left[i]);
	}
	for (int i = 0; i < right.size() && (right[i].getX() - midPoint.getX() < d); i++) {
		rightStrip.push_back(right[i]);
	}
	vector<Point>::iterator leftIter;
	vector<Point>::iterator rightIter; // new iterator
	ClosestPair d3;
	for (leftIter = leftStrip.begin(); leftIter != leftStrip.end(); ++leftIter) {
		Point p = *leftIter;
		//position iter2 so that it starts on the point directly AFTER where iter is
		
		for (rightIter = rightStrip.begin(); rightIter != rightStrip.end(); ++rightIter) {
			Point q = *rightIter;
			double dist = distance(p, q);
			if (dist < d3.getDistance()) {
				d3.setAB(p, q, dist);
			}
		}
	}
	if (d3.getDistance() < d)
		return d3;
	else if (d1.getDistance() < d2.getDistance())
		return d1;
	else
		return d2;

}
void part2() {

	//sort the array by x coord
	//qsort(vectorOfPoints.data(), vectorOfPoints.size(), sizeof(Point), compareX);
	sort(vectorOfPoints.begin(), vectorOfPoints.end(), compareX);

	

	ClosestPair pair = closestDistRecursive(vectorOfPoints);
	cout << pair.getPointA().getX() << ", " << pair.getPointA().getY() << endl;
	cout << pair.getPointB().getX() << ", " << pair.getPointB().getY() << endl;
	cout << "shortest distance by part2: " << pair.getDistance() << endl;

}