#include <iostream>
#include <cstdlib>
#include <algorithm>
#include <time.h>
#include <fstream>
#include <string>
#include <list>
#include <vector>
#include <chrono> 
#include <random>
#include <stack>
#include <unordered_map>
#define POINT_NUM 50
using namespace std;
#pragma warning(disable:4996)
const int width = 500, height = 500; //sets dimensions of the image
int arr[width][height][3];
ofstream file; //log file
int totalTimesToCalculateLength = 0;
double totalElapsed = 0;


//point class
class Point {
	double x, y;
public:
	void setvalues(double x1, double y1) {//sets x and y values of the point
		x = x1;
		y = y1;
	};
	double getX() {//returns x value
		return x;
	}
	double getY() {//returns y value
		return y;
	}

	friend bool operator== (Point &p1, Point &p2);

	
};
typedef list<Point*> PointList; //allowing for list to be iterated  through
vector<Point> vectorPoint;
list<Point*> pointList;
Point pointsArray[POINT_NUM];
bool operator==(Point &p1, Point &p2)
{
	return p1.getX() == p2.getX() && p1.getY() == p2.getY();
}
bool operator<(Point &p1, Point &p2)
{
	return p1.getX() < p2.getX() ;
}
void generatePoints(int n) {//generating points
	Point p;//storing a point
	ofstream file; //creates file
	file.open("points.txt");
	for (int i = 0; i < n; i++) {
		double x1 = static_cast <double> (rand()) / static_cast <double> (RAND_MAX); //generate first set of points
		double y1 = static_cast <double> (rand()) / static_cast <double> (RAND_MAX);
		file << x1 << " " << y1 << endl;//write coordinates to file
		p.setvalues(x1, y1);//creates a point
		pointsArray[i] = p;
		pointList.push_back(&p);
		vectorPoint.push_back(p);
	}
	file.close();
}
void drawPoint(Point &p, int val) {//draws point at an RGB value at Point*'s components (x,y)
	int x = p.getX()*width;
	int y = p.getY()*height;
	arr[x][y][0] = val;
	arr[x][y][1] = val;
	arr[x][y][2] = val;
}
void drawPoint(int x, int y, int val) {//draws point at an RGB value at (x,y)
	arr[x][y][0] = val;
	arr[x][y][1] = val;
	arr[x][y][2] = val;
}
void drawPurpPoint(int x, int y) {//draws point at an RGB value at (x,y)
	arr[x][y][0] = 200;
	arr[x][y][1] = 20;
	arr[x][y][2] = 255;
}
void drawRedPoint(Point &p) {//draws point at an RGB value at Point*'s components (x,y)
	int x = p.getX()*width;
	int y = p.getY()*height;
	arr[x][y][0] = 255;
	arr[x][y][1] = 0;
	arr[x][y][2] = 0;
}

double getLength(Point &p1, Point &p2) { // finds the distance between two points
	double x1 = p1.getX();
	double y1 = p1.getY();
	double x2 = p2.getX();
	double y2 = p2.getY();
	return sqrt((x1 - x2)*(x1 - x2) + (y1 - y2)*(y1 - y2));
}

bool compareX(Point a, Point b) {
	return a.getX() < b.getX();
}

bool compareY(Point a, Point b) {
	return a.getY() < b.getY();
}
void drawl(Point a, Point b) {
	//swap
	int x1=a.getX()*width;
	int y1 = a.getY()*height;
	int x2 = b.getX()*width;
	int y2 = b.getY()*height;
	if (x1 > x2 && y1 > y2) {
		swap(x1, x2);
		swap(y1, y2);
	}
	//x and y differences
	int dx = abs(x2 - x1);
	int dy = abs(y2 - y1);
	//error
	int e = dy - dx;

	//vertical or horizontal
	if (x1 == x2) {
		for (int i = y1; i <= y2; i++) {
			if (x1 >= 0 && x1 < width && i >= 0 && i < height) {
				drawPurpPoint(x1, i);
			}
		}
	}
	else if (y1 == y2) {
		for (int i = x1; i <= x2; i++) {
			if (y1 >= 0 && y1 < height && i >= 0 && i < width) {
				drawPurpPoint(i, y1);
			}
		}
	}
	//actual drawing line algorithm
	else {
		int m = abs(dy / dx);
		bool steep = false;
		if (m > 1) {
			swap(x1, y1);
			swap(x2, y2);
			steep = true;
			dx = abs(x2 - x1);
			dy = abs(y2 - y1);
		}
		int xd = (x2 > x1) ? 1 : -1;
		int yd = (y2 > y1) ? 1 : -1;
		int i = x1;
		int j = y1;
		for (; i != x2 + xd; i += xd) {
			if (steep == true) {
				if (j >= 0 && j < width && i >= 0 && i < height)
					drawPurpPoint(j, i);
			}
			else {
				if (j >= 0 && j < height && i >= 0 && i < width)

					drawPurpPoint(i, j);
			}

			while (e >= 0) {
				j += yd;
				e -= dx;
			}
			e += dy;
		}
	}
}

void setCircle(int x, int y, int centerx, int centery) {
	int xCx = x + centerx;
	int xCy = x + centery;
	int nxCx = -x + centerx;
	int nxCy = -x + centery;

	int yCx = y + centerx;
	int yCy = y + centery;
	int nyCx = -y + centerx;
	int nyCy = -y + centery;

	if (xCx >= 0 && xCx < width && yCy >= 0 && yCy < height)
		drawPoint(xCx, yCy, 0);
	if (nxCx >= 0 && nxCx < width && yCy >= 0 && yCy < height)
		drawPoint(nxCx, yCy, 0);
	if (xCx >= 0 && xCx < width && nyCy >= 0 && nyCy < height)
		drawPoint(xCx, nyCy, 0);
	if (nxCx >= 0 && nxCx < width && nyCy >= 0 && nyCy < height)
		drawPoint(nxCx, nyCy, 0);
	if (xCy >= 0 && xCy < height && yCx >= 0 && yCx < width)
		drawPoint(yCx, xCy, 0);
	if (xCy >= 0 && xCy < height && nyCx >= 0 && nyCx < width)
		drawPoint(nyCx, xCy, 0);
	if (nxCy >= 0 && nxCy < height && nyCx >= 0 && nyCx < width)
		drawPoint(nyCx, nxCy, 0);
	if (nxCy >= 0 && nxCy < height && yCx >= 0 && yCx < width)
		drawPoint(yCx, nxCy, 0);

}
void drawc(Point p, float r) {

	float y = r;
	float ysq = y * y;
	float ty = (2 * y) - 1;
	float y2_new = ysq;
	float xmax = (r*0.70710678);
	int x;
	for (x = 0; x <= xmax; x++) {
		if ((ysq - y2_new) >= ty) {
			ysq -= ty;
			y -= 1;
			ty -= 2;
		}
		setCircle((int)x, (int)y, (int)(p.getX()*width), (int)(p.getY()*height));
		y2_new -= (2 * x) - 3;
	}
}

int findSide(Point p1, Point p2, Point p)
{
	double val = (p.getY() - p1.getY()) * (p2.getX() - p1.getX()) -
		(p2.getY() - p1.getY()) * (p.getX() - p1.getX());
	if (val > 0)
		return 1;
	if (val < 0)
		return -1;
	return 0;
}
Point findLeft() {
	Point l = vectorPoint[0];
	for (int i = 1; i < POINT_NUM; i++ ) {
		if (l.getX() > vectorPoint[i].getX())
			l = vectorPoint[i];
	}
	return l;
}

double lineDist(Point p1, Point p2, Point p)
{
	return abs((p.getY() - p1.getY()) * (p2.getX() - p1.getX()) -
		(p2.getY() - p1.getY()) * (p.getX() - p1.getX()));
}
Point findRight() {
	Point l = vectorPoint[0];
	for (int i = 1; i < POINT_NUM; i++) {
		if (l.getX() < vectorPoint[i].getX())
			l = vectorPoint[i];
	}
	return l;
}
void quickHull(vector<Point> pArray, int n, Point l, Point r, int side) {
	int ind = -1;
	double max_d = 0;
	for (int i = 0; i < n; i++)
	{
		double temp = lineDist(l, r, pArray[i]);
		if (findSide(l, r, pArray[i]) == side && temp > max_d)
		{
			ind = i;
			max_d = temp;
		}
	}
		if (ind == -1)
		{
			drawl(l, r);
			return;
		}

		// Recur for the two parts divided by a[ind] 
		quickHull(pArray, n, pArray[ind], l, -findSide(pArray[ind], l, r));
		quickHull(pArray, n, pArray[ind], r, -findSide(pArray[ind], r, l));
	
}
void hullQuickHull(vector<Point> pArray, int n) {
	Point l = findLeft();
	Point r = findRight();

	quickHull(pArray, n, l, r, 1);
	quickHull(pArray, n, l, r, -1);
}

Point secondToTop(stack<Point> &stack)
{
	Point p = stack.top();
	stack.pop();
	Point ret = stack.top();
	stack.push(p);
	return ret;
}
int orientation(Point p1, Point p2, Point p3)
{
	double val = (p2.getY() - p1.getY()) * (p3.getX() - p2.getX()) -
		(p2.getX() - p1.getX()) * (p3.getY() - p2.getY());

	if (val == 0) return 0;  // colinear 

	return (val > 0) ? 1 : 2; // cw or ccw 
}
Point p0;
int compare(const void *vp1, const void *vp2)
{
	Point *p1 = (Point *)vp1;
	Point *p2 = (Point *)vp2;

	// Find orientation 
	int o = orientation(p0, *p1, *p2);
	if (o == 0)
		return (getLength(p0, *p2)*getLength(p0, *p2) >= getLength(p0, *p1)*getLength(p0, *p1)) ? -1 : 1;

	return (o == 2) ? -1 : 1;
}
void grahamScan(vector<Point> points, int n)
{
	int ymin = points[0].getY(), min = 0;
	for (int i = 1; i < n; i++)
	{
		int y = points[i].getY();
		if ((y < ymin) || (ymin == y &&
			points[i].getX() < points[min].getX()))
			ymin = points[i].getY(), min = i;
	}
	swap(points[0], points[min]);

	p0 = points[0];
	qsort(&points[1], n - 1, sizeof(Point), compare);

	int m = 1; 
	for (int i = 1; i<n; i++)
	{
		while (i < n - 1 && orientation(p0, points[i],
			points[i + 1]) == 0)
			i++;


		points[m] = points[i];
		m++;  
	}

	if (m < 3) return;

	stack<Point> s;
	s.push(points[0]);
	s.push(points[1]);
	s.push(points[2]);

	for (int i = 3; i < m; i++)
	{

		while (orientation(secondToTop(s), s.top(), points[i]) != 2)
			s.pop();
		s.push(points[i]);
	}
	Point firstP = s.top();
	while (s.size() > 1) {
		drawl(s.top(), secondToTop(s));
		s.pop();
	}
	drawl(s.top(), firstP);

}
int main() {
	srand(time(0));
	generatePoints(POINT_NUM);//creates random points
	ofstream img("cv.ppm");//creates ppm image
	img << "P3" << endl;
	img << width << " " << height << endl;
	img << "255" << endl;
	//makes image background white
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			drawPoint(x, y, 255);
			
		}
	}
	int seed = (unsigned)time(NULL);
	//hullQuickHull(vectorPoint, POINT_NUM);
	grahamScan(vectorPoint, POINT_NUM);

	file.open("results.txt");
	


	file.close();
	for (int i = 0; i < POINT_NUM; i++) {
		drawPoint(vectorPoint[i], 0);
		drawc(vectorPoint[i], 2);
	}
	//drawHull(hull);
	for (int y = 0; y < height; y++) {//draws everything
	for (int x = 0; x < width; x++) {
	img << arr[x][y][0] << " " << arr[x][y][1] << " " << arr[x][y][2] << endl;
	}
	}
	return 0;
}