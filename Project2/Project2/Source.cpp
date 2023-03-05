#define _USE_MATH_DEFINES
#include <iostream>
#include <fstream>
#include <sstream>
#include <time.h>
#include <cmath>
#include <string>
#include <cstring>
#include <cstdlib>
#include <math.h> 

using namespace std;
const int width = 800, height = 800; //sets dimensions of the image
int arr[width][height][3];
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
};
Point a, b, c, d;

void drawPoint(int x, int y, int val) {//draws point at an RGB value at (x,y)
	arr[x][y][0] = val;
	arr[x][y][1] = val;
	arr[x][y][2] = val;
}

class Line {//line class
	Point t, u;//end points
	double slope;//slope of the line
public:
	void setPoints(Point t1, Point u1) {//sets points
		t = t1;
		u = u1;
	};
	double findSlope() {//finds slope by m = (y2-y1)/(x2-x1)
		slope = (t.getY() - u.getY()) / (t.getX() - u.getX());
		return slope;
	}
	double getSlope(){ // returns slope;
		return findSlope();
	}
	Point getPoint1() { //returns the point with the smaller x value
		if(t.getX()<u.getX())
			return t;
		return u;
	}
	Point getPoint2() { // returns the point with the larger x value
		if (t.getX()<u.getX())
			return u;
		return t;
	}
	void drawline() {//draw line algorithm 
		int x1 = (int)(t.getX());
		int x2 = (int)(u.getX());
		int y1 = (int)(t.getY());
		int y2 = (int)(u.getY());
		//swap
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
					drawPoint(x1, i, 0);
				}
			}
		}
		else if (y1 == y2) {
			for (int i = x1; i <= x2; i++) {
				if (y1 >= 0 && y1 < height && i >= 0 && i < width) {
					drawPoint(i, y1, 0);
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
						drawPoint(j, i, 0);
				}
				else {
					if (j >= 0 && j < height && i >= 0 && i < width)

						drawPoint(i, j, 0);
				}

				while (e >= 0) {
					j += yd;
					e -= dx;
				}
				e += dy;
			}
		}
	}
};

void setCircle(int x, int y, int centerx, int centery) { // this method finds and draws a circle around a point 
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

void drawc(double centerx, double centery, double r) { //draws a circle around a point with radius r

	double y = r;
	double ysq = y * y;
	double ty = (2 * y) - 1;
	double y2_new = ysq;
	double xmax = (r*0.70710678);
	int x;
	for (x = 0; x <= xmax; x++) {
		if ((ysq - y2_new) >= ty) {
			ysq -= ty;
			y -= 1;
			ty -= 2;
		}
		setCircle((int)x, (int)y, (int)centerx, (int)centery);
		y2_new -= (2 * x) - 3;
	}
}
double findLength(double x1, double y1, double x2, double y2) { // finds the distance between two points
	return sqrt((x1 - x2)*(x1 - x2) + (y1 - y2)*(y1 - y2));
}
double findArea(Point a, Point b, Point c) { // finds the area of a triangle using heron's formula
	
	double  length1 = findLength(a.getX(), a.getY(), b.getX(), b.getY());
	double length2 = findLength(a.getX(), a.getY(), c.getX(), c.getY());
	double length3 = findLength(c.getX(), c.getY(), b.getX(), b.getY());
	double s = (length1 + length2 + length3) / 2;
	double area = sqrt(s*(s - length1)*(s - length2)*(s - length3));
	return area;
}
Point midPoint(Point a, Point b) { //finds midpoint of two points, returns it.
	double x = (a.getX() + b.getX()) / 2;
	double y = (a.getY() + b.getY()) / 2;
	Point p;
	p.setvalues(x, y);
	return p;
}
bool collinear(Point a, Point b, Point c) { //checks if three points are collinear
	double length1 = findLength(a.getX(), a.getY(), b.getX(), b.getY());
	double length2 = findLength(a.getX(), a.getY(), c.getX(), c.getY());
	double length3 = findLength(c.getX(), c.getY(), b.getX(), b.getY());
	//if the sums of two lengths are equal to the third, the three points are collinear
	if ((int)(length1*width) == (int)(width*length2 + width*length3) || (int)(length2*width) == (int)(width*length1 + width * length3) || (int)(length3*width) == (int)(width*length2 + width * length1))
		return false;
	return true;
}
bool poss(Point a, Point b, Point c, Point d) { // checks the possibility of four points to form a convex quadrilateral
	double tarea = findArea(a, b, c);
	double area1 = findArea(a, b, d);
	double area2 = findArea(d, b, c);
	double area3 = findArea(a, d, c);
	//checks possibility by summing areas and collinearity. If the sum of three triangles in the quadrilateral equal the sum of the other triangle, the quadrilateral is concave
	if (abs(tarea - (area1 + area2 + area3)) > 0.01 && abs(area1 - (tarea + area2 + area3)) > 0.01 && abs(area2 - (area1 + tarea + area3)) > 0.01 && abs(area3 - (area1 + area2 + tarea)) > 0.01)
		if (collinear(a, b, c) && collinear(a, b, d) && collinear(a, c, d) && collinear(d, b, c))
			return true;
	return false;
}

bool checkInter(Line l1, Line l2) {//checks if two lines intersect
	double mL1 = l1.getSlope();
	int bL1= (l1.getPoint1().getY()*height - l1.getPoint1().getX()*mL1*width);
	double mL2 = l2.getSlope();
	int bL2= (l2.getPoint1().getY()*height - l2.getPoint1().getX()*mL2*width);

	// Get the X of intersection of L1 and L2
	double x = (bL2 - bL1) / (mL1 - mL2);

	if (x > l2.getPoint1().getX() * width && x < l2.getPoint2().getX() * width)
	{
		return true;
	} 
	else {
		return false;
	}
}
Point findInter(Line l1, Line l2) {//finds the point of intersection between two lines
	double mL1 = l1.getSlope();
	int bL1 = (l1.getPoint1().getY()*height - l1.getPoint1().getX()*mL1*width);
	double mL2 = l2.getSlope();
	int bL2 = (l2.getPoint1().getY()*height - l2.getPoint1().getX()*mL2*width);

	// Get the X of intersection of L1 and L2
	double x = (bL2 - bL1) / (mL1 - mL2);
	double y = x * mL1 + bL1;
	Point ret;
	ret.setvalues(x, y);
	return ret;
}
Line drawPerpLine(Line z, Point p) { //finds a line perpendicular to line z that goes through one point p
		double x1 = p.getX();
		double y1 = p.getY();
		double m = -1/z.getSlope();//slope
		double b = y1 - x1 * m;//y intercept
		//allows for the line to draw through the whole canvas
		double startx = 0;
		double starty = b;
		Point p1;
		p1.setvalues(startx, starty);
		double endx = width - 1;
		double endy = m * (width - 1) + b;
		Point p2;
		p2.setvalues(endx, endy);
		Line n;
		n.setPoints(p1, p2);
		return n;
}
Line fullLine(Point p1, Point p2) {//finds the line that goes through the whole canvas and two points
	double x1 = p1.getX();
	double y1 = p1.getY();
	double x2 = p2.getX();
	double y2 = p2.getY();
	double m = (y2 - y1) / (x2 - x1);//slope
	double b = y1 - x1 * m;//y intercept found by y = mx + b
	int startx = 0; 
	double starty = b;
	Point start;
	start.setvalues(startx, starty);
	int endx = width - 1;
	double endy = m * (width - 1) + b;
	Point end;
	end.setvalues(endx, endy);
	Line ret;
	ret.setPoints(start, end);
	return ret;
}
Point findE1(Line l, double d, Point b) { //finds the E point using the + in front of the square root. 
	//E is a point on a perpendicular line that goes through B to the line that goes through points A and C. It is the same distance from B as A is from C
	Point e;
	double pX = b.getX() + d / sqrt(1 + l.getSlope()*l.getSlope());
	double pY = b.getY() + l.getSlope()*d / sqrt(1 + l.getSlope()*l.getSlope());
	e.setvalues(pX, pY);
	return e;
}
Point findE2(Line l, double d, Point b) {//finds the E point using the - in front of the square root.
	//E is a point on a perpendicular line that goes through B to the line that goes through points A and C. It is the same distance from B as A is from C
	Point e;
	double pX = b.getX() - d / sqrt(1 + l.getSlope()*l.getSlope());
	double pY = b.getY() - l.getSlope()*d / sqrt(1 + l.getSlope()*l.getSlope());
	e.setvalues(pX, pY);
	return e;
}
Point *findSquare(Point e, Point a, Point b, Point c, Point d) {//finds the vertices of the square
	static Point vert[4];
	Line l1 = fullLine(e, d);//sides found by constructing perpendicular lines through the points that are perpendicular to the other line
	Line l2 = drawPerpLine(l1, a);
	Line l3 = drawPerpLine(l2, b);
	Line l4 = drawPerpLine(l3, c);
	vert[0] = findInter(l1, l2);
	vert[1] = findInter(l2, l3);
	vert[2] = findInter(l3, l4);
	vert[3] = findInter(l4, l1);
	return vert;//returns array of points
}
double findArea(Point p1, Point p2) {//finds the area of a square
	double len = findLength(p1.getX(), p1.getY(), p2.getX(), p2.getY());
	return (len)*(len);
}
void part1() {
	
	double x1 = static_cast <double> (rand()) / static_cast <double> (RAND_MAX); //generate first set of points
	double y1 = static_cast <double> (rand()) / static_cast <double> (RAND_MAX);
	a.setvalues(x1, y1);
	double x2 = static_cast <double> (rand()) / static_cast <double> (RAND_MAX);
	double y2 = static_cast <double> (rand()) / static_cast <double> (RAND_MAX);
	b.setvalues(x2, y2);
	double x3 = static_cast <double> (rand()) / static_cast <double> (RAND_MAX);
	double y3 = static_cast <double> (rand()) / static_cast <double> (RAND_MAX);
	c.setvalues(x3, y3);
	while (!collinear(a, b, c)) {//makes sure the three points are not collinear, otherwise choose another third point
		double x3 = static_cast <double> (rand()) / static_cast <double> (RAND_MAX);
		double y3 = static_cast <double> (rand()) / static_cast <double> (RAND_MAX);
		c.setvalues(x3, y3);
	}
	double x4 = static_cast <double> (rand()) / static_cast <double> (RAND_MAX);
	double y4 = static_cast <double> (rand()) / static_cast <double> (RAND_MAX);
	d.setvalues(x4, y4);
	while (poss(a, b, c, d) != true) {//checks if the points create a convex shape, otherwise choose another fourth point
		x4 = static_cast <double> (rand()) / static_cast <double> (RAND_MAX);
		y4 = static_cast <double> (rand()) / static_cast <double> (RAND_MAX);
		d.setvalues(x4, y4);
	}
	//draws points and a circle with r = 2 around them
	drawPoint((int)(a.getX()*width), (int)(a.getY()*width), 0);
	drawc((a.getX()*width),(a.getY()*width), 2);
	drawPoint((int)(b.getX()*width), (int)(b.getY()*width), 0);
	drawc((b.getX()*width), (b.getY()*width), 2);
	drawPoint((int)(c.getX()*width), (int)(c.getY()*width), 0);
	drawc((c.getX()*width), (c.getY()*width), 2);
	drawPoint((int)(d.getX()*width), (int)(d.getY()*width), 0);
	drawc((d.getX()*width), (d.getY()*width), 2);
	ofstream file; //creates file
	file.open("points.txt"); //writes the points to the file
	file << "(" << a.getX() << "," << a.getY() << ")" << " , " << "(" << b.getX() << "," << b.getY() << ")" << " , " << "(" << c.getX() << "," << c.getY() << ")" << " , " << "(" << d.getX() << "," << d.getY() << ")" << endl; //writes the points to file
	file.close();
}
void part2() {
	ifstream inFile;
	inFile.open("points.txt");//opens point file
	if (!inFile) {
		cerr << "Unable to open file datafile.txt";
		exit(1);   
	}
	string points;
		getline(inFile, points);
	
	inFile.close();
	string arr[4];
	int i = 0;
	stringstream ssin(points);//puts the point string to a string array
	while (ssin.good() && i < 4) {
		if(ssin.str() !=",")
			ssin >> arr[i];
		if(arr[i]!= ",")
			++i;
	}
	Point a, b, c, d;//creates the four points and four booleans to see if they are filled with their x and y values
	bool afill = false;
	bool bfill = false;
	bool cfill = false;
	bool dfill = false;
	for (int x = 0; x < 4; x++) {
		string str = arr[x];//changes the string in string array to char array
		char cstr[100];
		strcpy_s(cstr, str.c_str());
		string f = "";//first part of the point
		string s = "";//second part of the point
		bool first = true;
		for (int y = 0; y < strlen(cstr); y++) {//checks what part of the point (x,y) the char is
			
			if (cstr[y] == ',') //if the char is a comma, then it signifies the x coordinate is finished
				first = false;
			if (cstr[y] != '(' && cstr[y] != ',' && first == true) // adds the chars to the first coord string
				f += cstr[y];
			
			if (first == false && cstr[y] != ')'&& cstr[y] != ',')//adds the chars to the second coord string
				s += cstr[y];
		}
		const char *fi = f.c_str();//changes string to chars
		const char *si = s.c_str();
		
		if (!afill) {//goes through each point in order, if it is not filled, then fill it. changes chars to doubles
			a.setvalues(strtof(fi, NULL), strtof(si, NULL));
			afill = true;
		}
		else if (!bfill) {
			b.setvalues(strtof(fi, NULL), strtof(si, NULL));
			bfill = true;
		}
		else if (!cfill) {
			c.setvalues(strtof(fi, NULL), strtof(si, NULL));
			cfill = true;
		}
		else if (!dfill) {
			d.setvalues(strtof(fi, NULL), strtof(si, NULL));
			dfill = true;
		}
	}
	//draws the points from the file as well as a circle of radius 2 around the point
	drawPoint((int)(a.getX()*width), (int)(a.getY()*width), 0);
	drawc((a.getX()*width), (a.getY()*width), 2);
	drawPoint((int)(b.getX()*width), (int)(b.getY()*width), 0);
	drawc((b.getX()*width), (b.getY()*width), 2);
	drawPoint((int)(c.getX()*width), (int)(c.getY()*width), 0);
	drawc((c.getX()*width), (c.getY()*width), 2);
	drawPoint((int)(d.getX()*width), (int)(d.getY()*width), 0);
	drawc((d.getX()*width), (d.getY()*width), 2);

	ofstream file; //creates file
	file.open("output.txt");
	//writes the points to file
	file << "(" << a.getX() << "," << a.getY() << ")" << " , " << "(" << b.getX() << "," << b.getY() << ")" << " , " << "(" << c.getX() << "," << c.getY() << ")" << " , " << "(" << d.getX() << "," << d.getY() << ")" << endl; 
	//creation of lines between the points
	Line ab, cd, bc, ad, ac;
	ab.setPoints(a, b);
	cd.setPoints(c, d);
	bc.setPoints(b, c);
	ad.setPoints(a, d);
	//swaps the points if they are not in a clockwise/counterclockwise orientation - easier manipulation later
	if (checkInter(ab, cd)) { //if two lines are intersecting, the endpoints of two lines are swapped
		swap(b, d);
		ab.setPoints(a, b);
		cd.setPoints(c, d);
	}
	if (checkInter(ad, bc)) {//checks twice due to two possible ways the lines could be intersecting
		swap(a, b);
		bc.setPoints(b, c);
		ad.setPoints(a, d);
	}
	
	ac.setPoints(a, c); //sets the initial AC line 
	Line n;
	Point min[4];
	Point* v;
	double minA = 1000000;
	Point p1;
	Point p2;
	Point p3;
	Point p4;
	double printA ;

	for (int i = 0; i < 6; i++) {//goes through all six combinations and square possibilities
		//when i == 0, order is normal ABCD
		//when i == 1, order is ACBD cw
		if (i == 1) {
			swap(b, c);
		}
		//i == 2, order is DBCA cw
		if (i == 2) {
			swap(b, c);
			swap(a, d);
		}
		//i == 3, order is BACD cw
		if (i == 3) {
			swap(a, d);
			swap(a, b);
		}
		//i == 4, order is DCBA cw
		if (i == 4) {
			swap(a, d);
			swap(c, d);
			swap(b, d);
		}
		//i == 5 order is CDBA cw
		if (i == 5) {
			swap(d, c);
		
		}
		ac.setPoints(a, c);//resets the AC line

		n = drawPerpLine(ac, b);//finds the perpendicular line through B relative to AC
		double acLen = findLength(a.getX(), a.getY(), c.getX(), c.getY()); //finds AC length
		Point e;
		Point e1 = findE1(n, acLen, b);
		Point e2 = findE2(n, acLen, b);
		//checks which e to use. Whichever one is closer to AC, we use that one
		double e1a = findLength(e1.getX(), e1.getY(), a.getX(), a.getY());
		double e2a = findLength(e2.getX(), e2.getY(), a.getX(), a.getY());
		double e1c = findLength(e1.getX(), e1.getY(), c.getX(), c.getY());
		double e2c = findLength(e2.getX(), e2.getY(), c.getX(), c.getY());
		if ((e1a + e1c) > (e2a + e2c))
			e = e2;
		else
			e = e1;
		//finds the vertices of the square using the e and the points
		Point* v = findSquare(e, a, b, c, d);
		file << "AC: " << findLength(a.getX(), a.getY(), c.getX(), c.getY());
		file << "BE: " << findLength(b.getX(), b.getY(), e.getX(), e.getY());


		double a = findArea(v[0], v[1]);//finds the area of the square
		if (minA > a) {//if the area is smaller than the previous min area value, update the minimum square vertices array
			minA = a;
			min[0] = v[0];
			min[1] = v[1];
			min[2] = v[2];
			min[3] = v[3];

		}
		//sets values of the vertices found to record into the file.
		p1.setvalues(v[0].getX() / width, v[0].getY() / height);
		p2.setvalues(v[1].getX() / width, v[1].getY() / height);
		p3.setvalues(v[2].getX() / width, v[2].getY() / height);
		p4.setvalues(v[3].getX() / width, v[3].getY() / height);
		a = a / (width*height);
		file << "(" << p1.getX() << "," << p1.getY() << ")" << " " << "(" << p2.getX() << "," << p2.getY() << ")" << " " << "(" << p3.getX() << "," << p3.getY() << ")" << " " << "(" << p4.getX() << "," << p4.getY() << ")" << " " << "Area=" << a << endl; //writes the points to file

	}
	//creates full extension lines of the square
	Line l1 = fullLine(min[0], min[1]);
	Line l2 = fullLine(min[1], min[2]);
	Line l3 = fullLine(min[2], min[3]);
	Line l4 = fullLine(min[3], min[0]);
	//draws the extension lines of the square
	l1.drawline();
	l2.drawline();
	l3.drawline();
	l4.drawline();
	//closes file
	file.close();
}

int main() {
	srand(time(0));
	ofstream img("output.ppm");
	img << "P3" << endl;
	img << width << " " << height << endl;
	img << "255" << endl;
	//makes image background white
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			drawPoint(x, y, 255);
		}
	}
	part1(); //finding the convex points
	part2(); //finding the minimum square
	//drawing image
	drawc(0, 0, 5);
	drawc(100, 10, 5);
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			img << arr[x][y][0] << " " << arr[x][y][1] << " " << arr[x][y][2] << endl;
		}
	}
	return 0;

}
