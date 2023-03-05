#include <iostream>
#include <fstream>
#include <time.h>
#include <cstdlib>
#include <math.h>  
using namespace std;
const int width = 800, height = 800;
int a[width][height][3];
void drawl(int x1, int y1, int x2, int y2);
void drawc(float centerx, float centery, float r);
void setCircle(int x, int y, int centerx, int centery);
void drawEuler(float circumx, float circumy, float centroidx, float centroidy);
float findLength(float x1, float y1, float x2, float y2);
void drawPoint(int x, int y, int val);
int main() {
	srand(time(0));
	ofstream img("output.ppm");
	img << "P3" << endl;
	img << width << " " << height << endl;
	img << "255" << endl;

	//random points
	float x1 = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
	float y1 = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
	float x2 = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
	float y2 = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
	float x3 = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
	float y3 = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);

	//finding midpoints
	float midx1 = ((x1 + x2) / 2);
	float midy1 = ((y1 + y2) / 2);
	float midx2 = ((x2 + x3) / 2);
	float midy2 = ((y2 + y3) / 2);
	float midx3 = ((x1 + x3) / 2);
	float midy3 = ((y1 + y3) / 2);
	//draw white background
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			drawPoint(x, y, 255);
		}
	}
	//draw lines
	drawl((int)(x1*width), (int)(y1*height), (int)(x2*width), (int)(y2*height));
	drawl((int)(x2*width), (int)(y2*height), (int)(x3*width), (int)(y3*height));
	drawl((int)(x3*width), (int)(y3*height), (int)(x1*width), (int)(y1*height));

	//draw circles
	//finding circumcenter
	float d = 2 * (x1*(y2 - y3) + x2 * (y3 - y1) + x3 * (y1 - y2));
	float circumx = (1 / d)*((x1*x1 + y1 * y1)*(y2 - y3) + (x2*x2 + y2 * y2)*(y3 - y1) + (x3*x3 + y3 * y3)*(y1 - y2));
	float circumy = (1 / d)*((x1*x1 + y1 * y1)*(x3 - x2) + (x2*x2 + y2 * y2)*(x1 - x3) + (x3*x3 + y3 * y3)*(x2 - x1));
	//calculating side lengths
	float length1 = findLength(x1, y1, x2, y2);
	float length2 = findLength(x2, y2, x3, y3);
	float length3 = findLength(x1, y1, x3, y3);

	float midl1 = findLength(midx1, midy1, midx2, midy2);
	float midl2 = findLength(midx2, midy2, midx3, midy3);
	float midl3 = findLength(midx1, midy1, midx3, midy3);

	//finding incicle center
	float s = (0.5*(length1 + length2 + length3));
	float incx = (length2*x1 + length3 * x2 + length1 * x3) / (s * 2);
	float incy = (length2*y1 + length3 * y2 + length1 * y3) / (s * 2);

	//finding 9 point circle center
	float midd = 2 * (midx1*(midy2 - midy3) + midx2 * (midy3 - midy1) + midx3 * (midy1 - midy2));
	float ninex = (1 / midd)*((midx1*midx1 + midy1 * midy1)*(midy2 - midy3) + (midx2*midx2 + midy2 * midy2)*(midy3 - midy1) + (midx3*midx3 + midy3 * midy3)*(midy1 - midy2));
	float niney = (1 / midd)*((midx1*midx1 + midy1 * midy1)*(midx3 - midx2) + (midx2*midx2 + midy2 * midy2)*(midx1 - midx3) + (midx3*midx3 + midy3 * midy3)*(midx2 - midx1));
	//finding radii
	//incircle radius
	float r = sqrt((s - length1)*(s - length2)*(s - length3) / s);
	//circumcenter radius
	float circumr = (length1*length2*length3) / (4 * r*s);
	//nine point circle radius
	float ns = (0.5*(midl1 + midl2 + midl3));
	float inr = sqrt((ns - midl1)*(ns - midl2)*(ns - midl3) / ns);
	float niner = (midl1*midl2*midl3) / (4 * inr*ns);
	//draw circles
	drawc(circumx*width, circumy*height, circumr*width);
	drawc(incx*width, incy*height, r*width);
	drawc(ninex*width, niney*height, niner*width);
	//draw euler's line

	float centroidx = (x1 + x2 + x3) / 3;
	float centroidy = (y1 + y2 + y3) / 3;
	drawEuler(circumx*width, circumy*height, centroidx*width, centroidy*height);

	//draw image
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			img << a[x][y][0] << " " << a[x][y][1] << " " << a[x][y][2] << endl;
		}
	}
	return 0;
}
void drawl(int x1, int y1, int x2, int y2) {
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
void drawc(float centerx, float centery, float r) {

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
		setCircle((int)x, (int) y, (int)centerx, (int)centery);
		y2_new -= (2 * x) - 3;
	}
}
void drawEuler(float circumx, float circumy, float centroidx, float centroidy) {
	float x1 = circumx;
	float y1 = circumy;
	float x2 = centroidx;
	float y2 = centroidy;
	float m = (y2 - y1) / (x2 - x1);//slope
	float b=  circumy - circumx * m;//y intercept
	int startx = 0;
	float starty = b;
	int endx = width-1;
	float endy = m*(width-1)+b;
	drawl(startx, (int)starty, endx, (int)endy);
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
float findLength(float x1, float y1, float x2, float y2) {
	return sqrt((x1 - x2)*(x1 - x2) + (y1 - y2)*(y1 - y2));
}
void drawPoint(int x, int y, int val) {
	a[x][y][0] = val;
	a[x][y][1] = val;
	a[x][y][2] = val;
}