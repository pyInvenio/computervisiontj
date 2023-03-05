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
#include <unordered_map>
#pragma warning(disable:4996)
using namespace std;
const int width = 800, height = 800; //sets dimensions of the image
int arr[width][height][3];
ofstream fileresults, filerandomized; //log file
int totalTimesToCalculateLength = 0;
double totalElapsed = 0;

//point class
class Point {
public:
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
	
	friend bool operator== (const Point &p1, const Point &p2)
	{
		return p1.x == p2.x && p1.y == p2.y;
	}
	
	Point(ifstream& f) {
		f >> this->x >> this->y;
	}
	Point() {

	}
};


struct hash_pair {
	size_t operator()(const pair<Point, Point>& p) const
	{
		auto hash1 = hash<double>()(p.first.x);
		auto hash2 = hash<double>()(p.first.y);
		auto hash3 = hash<double>()(p.second.x);
		auto hash4 = hash<double>()(p.second.y);
		return (hash1 ^ hash2) ^ (hash3 ^ hash4);
	}
};

double 	minLen = 10000000; //min length between two points
Point minP1, minP2;//two points that are minimum from each otherlist <Point> l1, l2; //having two lists to store points
typedef list<Point> PointList; //allowing for list to be iterated  through
typedef vector<Point> PointVector; //allowing for list to be iterated  through
list<Point> pointList, l2;
vector<Point> vPoint1, vPoint2;
void generatePoints(int n) {//generating points
	Point p;//storing a point
	ofstream file; //creates file
	file.open("points.txt");
	for (int i = 0; i < n; i++) {
		double x1 = static_cast <double> (rand()) / static_cast <double> (RAND_MAX); //generate first set of points
		double y1 = static_cast <double> (rand()) / static_cast <double> (RAND_MAX);
		file << x1 << " " << y1 << endl;//write coordinates to file
		p.setvalues(x1, y1);//creates a point
		
	}
	file.close();
}
void drawPoint(Point &p, int val) {//draws point at an RGB value at Point's components (x,y)
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
void drawRedPoint(Point &p) {//draws point at an RGB value at Point's components (x,y)
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
double compareX(Point &a, Point &b) {
	return a.getX() < b.getX();
}

double compareY(Point &a, Point &b) {
	return a.getY() < b.getY();
}
void readFile(string fileName) {
	ifstream file(fileName);
	string line;

	int nPoints;
	while (getline(file, line)) {
		vPoint1.emplace_back(Point(file));

	}
	vPoint2 = vPoint1;
	std::copy(vPoint2.begin(), vPoint2.end(), std::back_inserter(pointList));
	std::copy(vPoint2.begin(), vPoint2.end(), std::back_inserter(l2));
}
list<Point> bruteForce(list<Point> &l1, list<Point> &l2) {
	auto start = std::chrono::high_resolution_clock::now();

	for (PointList::iterator i1 = l1.begin(); i1 != l1.end(); ++i1) {//iterates through the first copy of the list
		l2.pop_front();// pops the first one of the second list to create easier comparisons
		for (PointList::iterator i2 = l2.begin(); i2 != l2.end(); ++i2) {//iterates through the rest of the second list
			Point p1 = *i1;//takes the two points
			Point p2 = *i2;
			double length = getLength(p1, p2);
			totalTimesToCalculateLength++;
			if (minLen > length) {//finds length, compares with minLen, if it is smaller, then replace minLen and replace the minPoints
				minLen = length;
				minP1.setvalues(p1.getX(), p1.getY());
				minP2.setvalues(p2.getX(), p2.getY());
			}
		}
	}
	list<Point> ret;
	ret.push_back(minP1);
	ret.push_back(minP2);

	// Testing only
	auto finish = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double, std::milli> elapsed = finish - start;
	//elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(elapsed);
	//file << "bruteForce() milliseconds: " << elapsed.count() << endl;

	return ret;
}
vector<Point> bruteForceVector(vector<Point> &v1, int left, int right) {
	auto start = std::chrono::high_resolution_clock::now();

	double min_distance = DBL_MAX;
	double temp;
	Point minpoint1;
	Point minpoint2;
	for (int i = left; i <= right; i++)
		for (int j = i + 1; j <= right; j++) {
			temp = getLength(v1[i], v1[j]);
			totalTimesToCalculateLength++;
			if (temp < min_distance) {
				min_distance = temp;
				minpoint1 = v1.at(i);
				minpoint2 = v1.at(j);
			}
		}
	vector<Point> ret;
	ret.push_back(minpoint1);
	ret.push_back(minpoint2);

	// Testing only
	auto finish = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double, std::milli> elapsed = finish - start;
	//elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(elapsed);

	totalElapsed += elapsed.count();

	return ret;
}

double min(double x, double y) {
	return (x < y) ? x : y;
}
vector<Point> stripClosest(vector<Point>&p, Point &mid) {
	vector<Point> left;
	vector<Point> right;
	vector<Point> ret;
	Point min1, min2;
	bool leftfinish = false;
	double minD = 10000;
	for (PointVector::iterator i = p.begin(); i!=p.end(); ++i) {
		if(!leftfinish)
			left.push_back(*i);
		if (*i == mid)
			leftfinish = true;
		if (leftfinish)
			right.push_back(*i);
	}
	for (PointVector::iterator i1 = left.begin(); i1 != left.end(); ++i1) {
		for (PointVector::iterator i2 = right.begin(); i2 != right.end(); ++i2) {
			double d = getLength(*i1, *i2);
			if (d < minD) {
				min1 = *i1;
				min2 = *i2;
			}
		}
	}
	ret.push_back(min1);
	ret.push_back(min2);
	return ret;
}
vector<Point> closestPair(vector<Point> &p, int left, int right) {
	if (right - left <= 3) {
		return bruteForceVector(p, left, right);
	}

	int mid = (left + right) / 2;
	

	vector<Point> pl = closestPair(p, left, mid);
	vector<Point> pr = closestPair(p, mid + 1, right);

	double dist_pl = getLength(pl[0], pl[1]);
	double dist_pr = getLength(pr[0], pr[1]);
	double d = min(dist_pl, dist_pr);
	vector<Point> smallerSide = (dist_pl < dist_pr) ? pl : pr;
	return smallerSide;
}

vector<Point> closestSplit(vector<Point> &p, Point &mid) {
	sort(p.begin(), p.end(), compareY);
	double minD = 100000;
	Point min1, min2;
	vector<Point> ret;
	for (PointVector::iterator i1 = p.begin(); i1 != p.end(); ++i1) {
		int count = 0;
		for (PointVector::iterator i2 = i1+1; i2 != p.end(); ++i2) {
			if (count > 7)
				break;
			count++;
			double d = getLength(*i1, *i2);
			if (d < minD) {
				min1 = *i1;
				min2 = *i2;
				minD = d;
			}
		}
	}
	ret.push_back(min1);
	ret.push_back(min2);
	return ret;
}
vector<Point> closestPair3(vector<Point> &p, int left, int right) {
	if (right - left <= 3) {
		return bruteForceVector(p, left, right);
	}

	int mid = (left + right) / 2;
	Point mid_point = p[mid];

	vector<Point> pl = closestPair3(p, left, mid);
	vector<Point> pr = closestPair3(p, mid + 1, right);

	double dist_pl = getLength(pl[0], pl[1]);
	double dist_pr = getLength(pr[0], pr[1]);
	double d = min(dist_pl, dist_pr);
	vector<Point> smallerSide = (dist_pl < dist_pr) ? pl : pr;
	return smallerSide;
}

vector<Point> closest(vector<Point> &p) {

	vector<Point> smallerSide =  closestPair(p, 0, p.size() - 1);
	int mid = p.size() / 2;
	vector<Point> strip;
	Point mid_point = p[mid];
	// Add mid point
	strip.push_back(mid_point);
	double d = getLength(smallerSide[0], smallerSide[1]);
	// Check left side
	for (int i = mid - 1; i >= 0; i--)
	{
		if (abs(p[i].getX() - mid_point.getX()) < d) {
			strip.push_back(p[i]);
		}
		else {
			// As points have already been sorted based on X, now it is safe to break the loop
			break;
		}
	}

	// Check right side
	for (int i = mid + 1; i < p.size(); i++)
	{
		if (abs(p[i].getX() - mid_point.getX()) < d) {
			strip.push_back(p[i]);
		}
		else {
			// As points have already been sorted based on X, now it is safe to break the loop
			break;
		}
	}

	if (strip.size() > 1) {
		vector<Point> psplit = stripClosest(strip, mid_point);
		double dist_ps = getLength(psplit[0], psplit[1]);
		return (d < dist_ps) ? smallerSide : psplit;
	}
	else {
		return smallerSide;
	}
}
vector<Point> closest3(vector<Point> &p) {
	vector<Point> smallerSide =  closestPair3(p, 0, p.size() - 1);
	vector<Point> strip;
	double d = getLength(smallerSide[0], smallerSide[1]);
	int mid = p.size() / 2;
	Point mid_point = p[p.size()/2];
	// Add mid point
	strip.push_back(mid_point);

	// Check left side
	for (int i = mid - 1; i >= 0; i--)
	{
		if (abs(p[i].getX() - mid_point.getX()) < d) {
			strip.push_back(p[i]);
		}
		else {
			// As points have already been sorted based on X, now it is safe to break the loop
			break;
		}
	}

	// Check right side
	for (int i = mid + 1; i < p.size(); i++)
	{
		if (abs(p[i].getX() - mid_point.getX()) < d) {
			strip.push_back(p[i]);
		}
		else {
			// As points have already been sorted based on X, now it is safe to break the loop
			break;
		}
	}

	if (strip.size() > 1) {
		vector<Point> psplit = closestSplit(strip, mid_point);
		double dist_ps = getLength(psplit[0], psplit[1]);
		return (d < dist_ps) ? smallerSide : psplit;
	}
	else {
		return smallerSide;
	}
}


void part1() {//brute force method
	totalTimesToCalculateLength = 0;
	fileresults.open("timesp1.txt");
	auto start = std::chrono::high_resolution_clock::now();

	bruteForce(pointList, l2);
	auto finish = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double, std::milli> elapsed = finish - start;
	//elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(elapsed);
	for (PointList::iterator i1 = pointList.begin(); i1 != pointList.end(); ++i1) {//draws the points	
		drawPoint(*i1, 0);
	}
	drawRedPoint(minP1);//draws the closest points in red
	drawRedPoint(minP2);

	fileresults << "part1() times to calculate length: " << totalTimesToCalculateLength << endl;
	fileresults << "part1() milliseconds: " << elapsed.count() << endl;
	fileresults.close();
}

vector<Point> part2() {
	sort(vPoint1.begin(), vPoint1.end(), compareX);


	// Find the closed pair of points
	vector<Point> closestv = closest(vPoint1);

	
	// Draw the points
	for (PointVector::iterator i1 = vPoint1.begin(); i1 != vPoint1.end(); ++i1) {
		drawPoint(*i1, 0);
	}
	drawRedPoint(closestv[0]);//draws the closest points in red
	drawRedPoint(closestv[1]);
	return closestv;
}

vector<Point> part3() {
	sort(vPoint1.begin(), vPoint1.end(), compareX);

	vector<Point> closestv = closest3(vPoint1);

	for (PointVector::iterator i1 = vPoint1.begin(); i1 != vPoint1.end(); ++i1) {//draws the points	
		drawPoint(*i1, 0);
	}
	drawRedPoint(closestv[0]);//draws the closest points in red
	drawRedPoint(closestv[1]);
	return closestv;
}
unordered_map<int, Point> makeDict(unordered_map<int, Point> dict, vector<Point> p, int n, int numsquares, double sideLength) {
	for (PointVector::iterator i1 = p.begin(); i1 != p.begin() + n ; ++i1) {
		Point z = *i1;
		dict[(int)(z.getX() / sideLength) * numsquares + (int)(z.getY() / sideLength)] = z;
	}
	return dict;
}
void shuffle() {
	for (int i = 0; i < vPoint2.size(); i++) {
		int j = rand() % (i + 1);

		// Swap arr[i] with the element  
		// at random index  
		swap(vPoint2[i], vPoint2[j]);
	}
}
vector<Point> closestMap(vector<Point> &pArray) {
	double d = getLength(pArray[0], pArray[1]);
	double sideLength = d / 2;
	int numSquares = 1 / sideLength;
	unordered_map<int, Point>* dict = new unordered_map<int, Point>();
	*dict = makeDict(*dict, pArray, 1, numSquares, sideLength);
	vector<Point> closestPair;
	int x, y, pos;
	double temp_distance;
	closestPair.push_back(pArray[0]);
	closestPair.push_back(pArray[1]);
	Point temp;

	bool update = false;
	for (int i = 2; i < vPoint1.size(); i++) {
		x = (int)(pArray[i].getX() / sideLength);
		y = (int)(pArray[i].getY() / sideLength);
		for (int ty = y - 2; ty <= y + 2; ty++) {
			pos = ty * numSquares + x;
			for (int loc = pos - 2; loc <= pos + 2; loc++) {
				if (dict->find(loc) != dict->end()) {
					 temp = dict->at(loc);
					temp_distance = getLength(pArray[i], temp);
					if (temp_distance < d) {
						d = temp_distance;
						update = true;
					}
				}
			}
		}
		if (update) {
			update = false;
			sideLength = d / 2;
			numSquares = (int)(1 / sideLength);
			closestPair[0] = pArray[i];
			closestPair[1] = temp;
			dict->clear();
			*dict = makeDict(*dict, pArray, i, numSquares, sideLength);
		}
		else {
			Point t = pArray[i];
			(*dict)[y * numSquares + x] = t;
		}
	}
	delete dict;
	return closestPair;
}


vector<Point> part4() {
	return closestMap(vPoint2);
}
int main() {
	srand(time(0));

	generatePoints(100000);//creates random points
	ofstream img("output.ppm");//creates ppm image
	img << "P3" << endl;
	img << width << " " << height << endl;
	img << "255" << endl;
	//makes image background white
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			drawPoint(x, y, 255);
		}
	}
	filerandomized.open("randomized.txt");
	fileresults.open("results.txt");
	//part1(); //finding the 50 points and two points that are close
	fileresults.precision(24);
	filerandomized.precision(24);
	readFile("points.txt");
	/*
	auto start = std::chrono::high_resolution_clock::now();
	vector<Point> vp2=  part2(); //finding the two points that are close through preliminary recursive and bruteforce methods
	auto finish = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double, std::milli> elapsed = finish - start;
	fileresults << "part2() milliseconds: " << elapsed.count() << endl;
	fileresults << "Point1: " << vp2[0].getX()<<" "<<vp2[0].getY()<< endl;
	fileresults << "Point2: " << vp2[1].getX()<<" "<<vp2[1].getY() <<endl;
	fileresults << "Distance: " << getLength(vp2[0], vp2[1])<<endl;
	*/
	auto start = std::chrono::high_resolution_clock::now();
	vector<Point> vp3 = part3();
	auto finish = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double, std::milli> elapsed = finish - start;
	filerandomized << "part3() total milliseconds: " << elapsed.count() << endl;
	filerandomized << "Point1: " << vp3[0].getX()  << " " << vp3[0].getY() << endl;
	filerandomized << "Point2: " << vp3[1].getX() << " " << vp3[1].getY() << endl;
	filerandomized << "Distance: " << getLength(vp3[0], vp3[1])<<endl;

	shuffle();
	start = std::chrono::high_resolution_clock::now();
	vector<Point> vp4 = part4();
	finish = std::chrono::high_resolution_clock::now();
	elapsed = finish - start;
	filerandomized << "part4() total milliseconds: " << elapsed.count() << endl;
	filerandomized << "Point1: " << vp4[0].getX() << " " << vp4[0].getY() << endl;
	filerandomized << "Point2: " << vp4[1].getX() << " " << vp4[1].getY() << endl;
	filerandomized << "Distance: " << getLength(vp4[0], vp4[1]);
	filerandomized.close();

	/*
			 for (int y = 0; y < height; y++) {//draws everything
			 	for (int x = 0; x < width; x++) {
			 		img << arr[x][y][0] << " " << arr[x][y][1] << " " << arr[x][y][2] << endl;
			 	}
			 }*/

	return 0;
}
