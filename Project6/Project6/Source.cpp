#define _USE_MATH_DEFINES
#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <uchar.h>
#include <cmath> 
#include <map>
#include <iomanip> 

using namespace std;


long WIDTH, HEIGHT;

struct RGB {
	int R, B, G;
};
class PPMImg {
public:
	PPMImg() { };

	friend std::ifstream& operator >>(std::ifstream& inputStream, PPMImg& img) {
		inputStream >> img;
	};
	friend std::ofstream& operator <<(std::ofstream& outputStream, const PPMImg& img) {
		outputStream << img;
	};
	void grayscale();
	string identifyer;
	int maxColVal;
	vector<int> r, g, b, gray, gaussian, edge, angles, acc;
	double gFilter[3][3];
};
PPMImg picture;
PPMImg loadAndGenerateGrayScale(const string& fileName) {
	ifstream img;
	img.open(fileName);
	PPMImg image;
	img >> image.identifyer >> WIDTH >> HEIGHT >> image.maxColVal;
	image.r.resize(WIDTH*HEIGHT);
	image.b.resize(WIDTH*HEIGHT);
	image.g.resize(WIDTH*HEIGHT);
	image.angles.resize(WIDTH*HEIGHT);
	image.gray.resize(WIDTH*HEIGHT);
	image.acc.resize(WIDTH*HEIGHT);
	image.gaussian.resize(WIDTH*HEIGHT);
	image.edge.resize(WIDTH*HEIGHT);
	int i = 1;

	if (image.identifyer.compare("P3") == 0) {
		while (i < image.r.size())
			if (!(img >> image.r[i - 1] >> image.g[i - 1] >> image.b[i++]))
				cerr << "Cannot read file";
	}
	else if (image.identifyer.compare("P6") == 0) {
		unsigned char buffer[3];
		while (i < image.r.size())
			if (img.read((char *)buffer, 3)) {
				image.r[i] = (int)buffer[0];
				image.g[i] = (int)buffer[1];
				image.b[i++] = (int)buffer[2];
			}
			else {
				cerr << "Cannot Read File\n";
			}
	}

	if (image.maxColVal != 256)
		for (int i = 0; i < image.r.size(); i++) {
			image.r[i] = (image.r[i] + 1) * (256 / image.maxColVal) - 1;
			image.g[i] = (image.g[i] + 1) * (256 / image.maxColVal) - 1;
			image.b[i] = (image.b[i] + 1) * (256 / image.maxColVal) - 1;
		}
	for (int i = 0; i < image.r.size(); i++) {
		int a = (int)((image.r[i] + image.g[i] + image.b[i]) / 3);
		image.gray[i] = a;
	}
	img.close();
	
	return image;
}

PPMImg createGaussianFilter(PPMImg &ppm) {
	// intialising standard deviation to 1.0 
	double sigma = 4.0;
	double r, s = 2.0 * sigma * sigma;
	// sum is for normalization 
	double sum = 0.0;

	// generating 3x3 kernel 
	for (int x = -1; x <= 1; x++) {
		for (int y = -1; y <= 1; y++) {
			r = sqrt(x * x + y * y);
			ppm.gFilter[x + 1][y + 1] = (exp(-(r * r) / s)) / (M_PI * s);
			sum += ppm.gFilter[x + 1][y + 1];
		}
	}

	// normalising the Kernel 
	for (int i = 0; i < 3; ++i)
		for (int j = 0; j < 3; ++j)
			ppm.gFilter[i][j] /= sum;
	return ppm;
}
PPMImg applyGaussian(PPMImg &ppm) {
	for (int y = 1; y < HEIGHT - 2; y++) {
		for (int x = 1; x < WIDTH - 2; x++) {
			int sum = 0;

			double topleft = ppm.gray[WIDTH*(y - 1) + (x - 1)] * ppm.gFilter[0][0];
			double topmid = ppm.gray[WIDTH*(y - 1) + (x)] * ppm.gFilter[0][1];
			double topright = ppm.gray[WIDTH*(y - 1) + (x + 1)] * ppm.gFilter[0][2];

			double midleft = ppm.gray[WIDTH*(y)+(x - 1)] * ppm.gFilter[1][0];
			double midmid = ppm.gray[WIDTH*(y)+(x)] * ppm.gFilter[1][1];
			double midright = ppm.gray[WIDTH*(y)+(x + 1)] * ppm.gFilter[1][2];

			double botleft = ppm.gray[WIDTH*(y + 1) + (x - 1)] * ppm.gFilter[2][0];
			double botmid = ppm.gray[WIDTH*(y + 1) + (x)] * ppm.gFilter[2][1];
			double botright = ppm.gray[WIDTH*(y + 1) + (x + 1)] * ppm.gFilter[2][2];

			sum = topleft + topmid + topright + midleft + midmid + midright + botleft + botmid + botright;
			ppm.gaussian[y*WIDTH + x] = sum;
		}
	}
	return ppm;
}
int sobelX[] = {
	-1, 0, 1,
	-2, 0, 2,
	-1, 0, 1
};

int sobelY[] = {
	1,  2,  1,
	0,  0,  0,
	-1, -2, -1
};

PPMImg applyNonMaxSupp(PPMImg ppm) {
	vector<int> edges(ppm.edge.size());
	vector<float> dir(ppm.angles.size());
	int i, tc, tl, tr, cl, cr, bc, bl, br;

	for (int i = 0; i < ppm.angles.size(); i++)
		dir[i] = fmod(ppm.angles[i] + M_PI, M_PI) / M_PI * 8;


	for (int y = 1; y < HEIGHT - 1; y++)

		for (int x = 1; x < WIDTH - 1; x++) {

			i = y * WIDTH + x;
			tc = i - WIDTH;  // top-center
			tl = tc - 1;     // top-left
			tr = tc + 1;     // top-right
			cl = i - 1;      // center-left
			cr = i + 1;      // center-right
			bc = i + WIDTH;  // bottom-center
			bl = bc - 1;     // bottom-left
			br = bc + 1;     // bottom-right

			if (((dir[i] <= 1 || dir[i] > 7) && ppm.edge[i] > ppm.edge[cr] &&
				ppm.edge[i] > ppm.edge[cl]) ||  // 0 deg
				((dir[i] <= 3 && dir[i] > 1) && ppm.edge[i] > ppm.edge[tr] &&
					ppm.edge[i] > ppm.edge[bl]) ||  // 45 deg
					((dir[i] <= 5 && dir[i] > 3) && ppm.edge[i] > ppm.edge[tc] &&
						ppm.edge[i] > ppm.edge[bc]) ||  // 90 deg
						((dir[i] <= 7 && dir[i] > 5) && ppm.edge[i] > ppm.edge[tl] &&
							ppm.edge[i] > ppm.edge[br]))  // 135 deg
				edges[i] = ppm.edge[i];
			else
				edges[i] = 0;
		}
	ppm.edge = edges;
	return ppm;
}
PPMImg applySobelOperator(PPMImg ppm) {
	for (int y = 1; y < HEIGHT - 2; y++) {
		for (int x = 1; x < WIDTH - 2; x++) {
			double gxSum = 0;
			double gySum = 0;
			double topleftx = ppm.gray[WIDTH*(y - 1) + (x - 1)] * sobelX[0];
			double topmidx = ppm.gray[WIDTH*(y - 1) + (x)] * sobelX[1];
			double toprightx = ppm.gray[WIDTH*(y - 1) + (x + 1)] * sobelX[2];

			double midleftx = ppm.gray[WIDTH*(y)+(x - 1)] * sobelX[3];
			double midmidx = ppm.gray[WIDTH*(y)+(x)] * sobelX[4];
			double midrightx = ppm.gray[WIDTH*(y)+(x + 1)] * sobelX[5];

			double botleftx = ppm.gray[WIDTH*(y + 1) + (x - 1)] * sobelX[6];
			double botmidx = ppm.gray[WIDTH*(y + 1) + (x)] * sobelX[7];
			double botrightx = ppm.gray[WIDTH*(y + 1) + (x + 1)] * sobelX[8];

			gxSum = topleftx + topmidx + toprightx + midleftx + midmidx + midrightx + botleftx + botmidx + botrightx;

			double toplefty = ppm.gray[WIDTH*(y - 1) + (x - 1)] * sobelY[0];
			double topmidy = ppm.gray[WIDTH*(y - 1) + (x)] * sobelY[1];
			double toprighty = ppm.gray[WIDTH*(y - 1) + (x + 1)] * sobelY[2];

			double midlefty = ppm.gray[WIDTH*(y)+(x - 1)] * sobelY[3];
			double midmidy = ppm.gray[WIDTH*(y)+(x)] * sobelY[4];
			double midrighty = ppm.gray[WIDTH*(y)+(x + 1)] * sobelY[5];

			double botlefty = ppm.gray[WIDTH*(y + 1) + (x - 1)] * sobelY[6];
			double botmidy = ppm.gray[WIDTH*(y + 1) + (x)] * sobelY[7];
			double botrighty = ppm.gray[WIDTH*(y + 1) + (x + 1)] * sobelY[8];

			gySum = toplefty + topmidy + toprighty + midlefty + midmidy + midrighty + botlefty + botmidy + botrighty;

			int sum = sqrt(gxSum*gxSum + gySum * gySum);
			ppm.edge[WIDTH*y + x] = sum;
			ppm.angles[WIDTH*y + x] = atan2(gySum, gxSum);
		}
	}
	ppm = applyNonMaxSupp(ppm);
	int threshold = 150;
	int bottomThreshold = 120;
	for (int y = 1; y < HEIGHT - 2; y++) {
		for (int x = 1; x < WIDTH - 2; x++) {
			if (ppm.edge[WIDTH*y + x] > threshold)
				ppm.edge[WIDTH*y + x] = 1;
			else if (ppm.edge[WIDTH*y + x] < threshold && ppm.edge[WIDTH*y + x]>bottomThreshold) {
				vector<int> checksides;
				checksides.push_back(ppm.edge[WIDTH*(y - 1) + (x - 1)]);
				checksides.push_back(ppm.edge[WIDTH*(y - 1) + (x)]);
				checksides.push_back(ppm.edge[WIDTH*(y - 1) + (x + 1)]);
				checksides.push_back(ppm.edge[WIDTH*(y)+(x - 1)]);
				checksides.push_back(ppm.edge[WIDTH*(y)+(x + 1)]);
				checksides.push_back(ppm.edge[WIDTH*(y + 1) + (x - 1)]);
				checksides.push_back(ppm.edge[WIDTH*(y + 1) + (x)]);
				checksides.push_back(ppm.edge[WIDTH*(y + 1) + (x + 1)]);
				bool edgeYes = false;
				for (int i = 0; i < checksides.size(); i++) {
					if (checksides[i] > threshold) {
						edgeYes = true;
						break;
					}
				}
				if (edgeYes)
					ppm.edge[WIDTH*y + x] = 1;
				else
					ppm.edge[WIDTH*y + x] = 0;

				checksides.clear();
			}
			else
				ppm.edge[WIDTH*y + x] = 0;

		}
	}
	return ppm;
}
PPMImg applyHysteresis(PPMImg ppm) {

	return ppm;
}


struct Edge {
	int x, y;
	Edge() { x = 0; y = 0; }
	Edge(const int& x, const int& y) {
		this->x = x;
		this->y = y;
	}
};

vector<Edge> findAllEdges(PPMImg& ppm, vector<Edge>& edges) {
	for (int y = 0; y < HEIGHT; y++)
		for (int x = 0; x < WIDTH; x++)
			if (picture.edge[y * WIDTH + x] == 1) 
				edges.emplace_back(Edge(x, y));
	return edges;
}

inline double lineDistance(const Edge& a, const Edge& b, const Edge& c) {
	return ((b.y - a.y) * c.x - (b.x - a.x) * c.y + b.x * a.y - b.y * a.x) / sqrt((b.y - a.y) * (b.y - a.y) + (b.x - a.x) * (b.x - a.x));
}
void drawPixel(PPMImg& ppm, int x,	int y) {
	if (x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT) 
		return;
	picture.r[x+y*WIDTH]=255;
	picture.b[x+y*WIDTH]=0;
	picture.g[x+y*WIDTH]=0;
}
void drawCircle(PPMImg& ppm,int cx, int cy, int rad) {
	int x = rad;
	int y = 0;
	int e = 0;
	while (x >= y) {
		drawPixel(picture, cx + x, cy + y);
		drawPixel(picture, cx + y, cy + x);
		drawPixel(picture, cx - y, cy + x);
		drawPixel(picture, cx - x, cy + y);
		drawPixel(picture, cx - x, cy - y);
		drawPixel(picture, cx - y, cy - x);
		drawPixel(picture, cx + y, cy - x);
		drawPixel(picture, cx + x, cy - y);
		if (e <= 0) {
			y++;
			e += 2 * y + 1;
		}
		else {
			x--;
			e -= 2 * x + 1;
		}
	}
}


void accPixels(PPMImg& ppm, int x, int y) {
	if (x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT) 
		return;
	if(picture.acc[y*WIDTH+x]<255)
		picture.acc[y * WIDTH + x]++;
}

void accumCircle(PPMImg& ppm, int cx, int cy, int rad) {

	int x = rad;
	int y = 0;
	int e = 0;
	while (x >= y) {
		//accumulates circle pixels, similar to draw circle
		accPixels(picture, cx + x, cy + y);
		accPixels(picture, cx + y, cy + x);
		accPixels(picture, cx - y, cy + x);
		accPixels(picture, cx - x, cy + y);
		accPixels(picture, cx - x, cy - y);
		accPixels(picture, cx - y, cy - x);
		accPixels(picture, cx + y, cy - x);
		accPixels(picture, cx + x, cy - y);
		if (e <= 0) {
			y++;
			e += 2 * y + 1;
		}
		else {
			x--;
			e -= 2 * x + 1;
		}
		if (x < y) {
			return;
		}
	}
	
}
int min(int i, int j) {
	if (i < j)
		return i;
	return j;
}
bool isInCicles(const map<int, vector<pair<int, int>>>& allradius, int x, int y, int r) {
	for (const pair<int, vector<pair<int, int>>>& cc : allradius) {
		for (const pair<int, int>& p : cc.second) {
			int distSq =sqrt((p.first - x) * (p.first - x) + (p.second - y) * (p.second - y));
			if (distSq <= r+r*.3)
				return true;
		}
	}
	return false;
}
PPMImg houghTransform(PPMImg& ppm) {
	vector<Edge> edges;
	edges = findAllEdges(picture, edges);
	
	int tHeight = sqrt(WIDTH * WIDTH + HEIGHT * HEIGHT) / 2;
	int tWidth = 360;
	int rMin = min(WIDTH, HEIGHT)/40;
	int rMax = min(WIDTH, HEIGHT) / 14;
	// cout << rMin << " " << rMax << endl;
	int rDepth = rMax - rMin;

	vector<int> out(ppm.r.size(), 0);
	map<int, vector<pair<int, int>>> coinCollection;

	for (int rad = rMax; rad >= rMin; --rad) {
		vector<int> accum(WIDTH * HEIGHT, 0);
		picture.acc = accum;
		cout << rad << '\n';
		for (Edge edge : edges) {
			accumCircle(picture, edge.x, edge.y, rad);
		}
		// unsigned int i, t, l, r, b;
		int threshold = 10.0/3.0*rad;
		accum = picture.acc;
		for (int y = 1; y < HEIGHT - 1; y++) {
			for (int x = 1; x < WIDTH - 1; x++) {
				if (picture.acc[y * WIDTH + x] > threshold) {
					int index = y * WIDTH + x;
					// Check if it is a local max
					if (accum[index] > accum[index + 1] && accum[index] > accum[index - 1] && accum[index] > accum[index + WIDTH] && accum[index] > accum[index - WIDTH])
						if (!isInCicles(coinCollection, x, y, rad)) {
							coinCollection[rad].emplace_back(make_pair(x, y));
						}
				}
			}
		}
	}
	
	std::map<int, vector<pair<int, int>>>::iterator it = coinCollection.begin();
	while (it != coinCollection.end()) {
		int r = it->first;
		vector<pair<int, int>> c = it->second;
		for (int i = 0; i < c.size(); i++) {
			drawCircle(picture, c[i].first, c[i].second, r);
		}
		it++;
	}
	
	int sum = 0, numCircles = 0;
	for (const pair<int, vector<pair<int, int>>>& cc : coinCollection) {
		sum += cc.first;
		numCircles += cc.second.size();
	}
	double avg = (double)sum / coinCollection.size();
	double var = 0;
	for (const pair<int, vector<pair<int, int>>>& cc : coinCollection)
		var += (cc.first - avg) * (cc.first - avg) * cc.second.size();
	var /= numCircles;
	double sd = sqrt(var);
	double dim = avg - sd*.72;
	double pen = avg - sd*.4;
	double nic = avg + sd*.4;
	double qua = avg + sd*2;
	int dCount = 0;
	int dolCount = 0;
	int pCount = 0;
	int nCount = 0;
	int qCount = 0;
	for (const pair<int, vector<pair<int, int>>>& cc : coinCollection) {
		if (cc.first < pen) {
			for (int i = 0; i < cc.second.size(); i++) {
				int index = cc.second[i].first + cc.second[i].second*WIDTH;
				if ((float)picture.r[index] > (float)picture.g[index] * 1.15 &&(float)picture.r[index] > (float)picture.b[index] * 1.15)
					pCount += 1;
				else
					dCount += 1;
			}
		}
		else if (cc.first >= pen && cc.first < nic)
			nCount += cc.second.size();
		else if (cc.first >=nic && cc.first < qua)
			qCount += cc.second.size();
		else
			dolCount += cc.second.size();
	}
	ofstream file("results.txt");
	file << "Dimes: " << dCount << endl;
	file << "Pennies: " << pCount << endl;
	file << "Nickles: " <<  nCount<< endl;
	file << "Quarters: " << qCount << endl;
	file << "Dollars: " << dolCount << endl;
	double total = pCount * .01 + nCount * 0.05 + qCount * .25 + .1*dCount + dolCount;
	file << "Total: $" << total << endl;
	file.close();
	//for easy coin image, 40 pennies, 10 nickles, 18 quarters
	
	cout << "Pennies: " << pCount << endl;
	cout << "Nickles: " << nCount << endl;
	cout << "Quarters: " << qCount << endl;
	cout << "Total: $" << total << endl;
return picture;
}


int main() {
	picture = loadAndGenerateGrayScale("image.ppm");
	picture = createGaussianFilter(picture);
	picture = applyGaussian(picture);
	picture = applySobelOperator(picture);
	
	ofstream img("imagef.ppm");//creates ppm image
	img << "P3" << endl;
	img << WIDTH << " " << HEIGHT << endl;
	img << "1" << endl;
	int y = 0;
	while (y<picture.r.size()) {//draws everything
		img << picture.edge[y] << " " << picture.edge[y] << " " << picture.edge[y] << endl;
		y++;
	}
	img.close();
	picture = houghTransform(picture);

	ofstream img2("imagec.ppm");//creates ppm image
	img2 << "P3" << endl;
	img2 << WIDTH << " " << HEIGHT << endl;
	img2 << "255" << endl;
	y = 0;
	while (y<picture.r.size()) {//draws everything
		img2 << picture.r[y] << " " << picture.g[y] << " " << picture.b[y] << endl;
		y++;
	}
	img2.close();
	return 0;
}