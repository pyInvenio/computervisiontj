//Forrest Meng Jurj PD 2 Canny Edge Part 1
#define _USE_MATH_DEFINES

#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <uchar.h>
#include <cmath> 
#include <iomanip> 

using namespace std;


int WIDTH, HEIGHT;

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
	vector<int> r, g, b, gray, gaussian, edge;
	double gFilter[3][3];
};
PPMImg loadAndGenerateGrayScale(const string& fileName) {
	ifstream img;
	img.open(fileName);
	PPMImg image;
	img >> image.identifyer >> WIDTH >> HEIGHT >> image.maxColVal;
	image.r.resize(WIDTH*HEIGHT);
	image.b.resize(WIDTH*HEIGHT);
	image.g.resize(WIDTH*HEIGHT);
	image.gray.resize(WIDTH*HEIGHT);
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

	// generating 5x5 kernel 
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

			double midleft = ppm.gray[WIDTH*(y)+(x - 1)] * ppm.gFilter[0][0];
			double midmid = ppm.gray[WIDTH*(y)+(x)] * ppm.gFilter[0][1];
			double midright = ppm.gray[WIDTH*(y)+(x + 1)] * ppm.gFilter[0][2];

			double botleft = ppm.gray[WIDTH*(y + 1) + (x - 1)] * ppm.gFilter[0][0];
			double botmid = ppm.gray[WIDTH*(y + 1) + (x)] * ppm.gFilter[0][1];
			double botright = ppm.gray[WIDTH*(y + 1) + (x + 1)] * ppm.gFilter[0][2];

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
		}
	}
	int threshold = 130;
	int bottomThreshold = 90;
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
int main() {
	PPMImg ppm = loadAndGenerateGrayScale("image.ppm");
	ppm = createGaussianFilter(ppm);
	ofstream imag("imageg.ppm");
	imag << "P3" << endl;
	imag << WIDTH << " " << HEIGHT << endl;
	imag << "255" << endl;
	int y = 0;
	while (y<ppm.r.size()) {//draws everything
		imag << ppm.gray[y] << " " << ppm.gray[y] << " " << ppm.gray[y] << endl;
		y++;
	}
	imag.close();
	ppm = applyGaussian(ppm);
	ppm = applySobelOperator(ppm);
	ofstream img("imagem.ppm");//creates ppm image
	img << "P3" << endl;
	img << WIDTH << " " << HEIGHT << endl;
	img << "1" << endl;
	 y = 0;
	while (y<ppm.r.size()) {//draws everything
		img << ppm.edge[y] << " " << ppm.edge[y] << " " << ppm.edge[y] << endl;
		y++;
	}
	img.close();

	return 0;
}
/*//Forrest Meng Jurj PD 2 Canny Edge Part 2
#define _USE_MATH_DEFINES
#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <uchar.h>
#include <cmath> 
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
	vector<int> r, g, b, gray, gaussian, edge, angles;
	double gFilter[3][3];
};
PPMImg loadAndGenerateGrayScale(const string& fileName) {
	ifstream img;
	img.open(fileName);
	PPMImg image;
	img >> image.identifyer >>  WIDTH>> HEIGHT >> image.maxColVal;
	image.r.resize(WIDTH*HEIGHT);
	image.b.resize(WIDTH*HEIGHT);
	image.g.resize(WIDTH*HEIGHT);
	image.angles.resize(WIDTH*HEIGHT);
	image.gray.resize(WIDTH*HEIGHT);
	image.gaussian.resize(WIDTH*HEIGHT);
	image.edge.resize(WIDTH*HEIGHT);
	int i = 0;
	
	if (image.identifyer.compare("P3") == 0) {
		while (i < image.r.size())
			if (!(img >> image.r[i-1] >> image.g[i-1] >> image.b[i++]))
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
	for (int y = 1; y < HEIGHT-2; y++) {
		for (int x = 1; x < WIDTH-2; x++) {
			int sum = 0;
			
			double topleft = ppm.gray[WIDTH*(y-1)+(x - 1)] * ppm.gFilter[0][0];
			double topmid = ppm.gray[WIDTH*(y-1)+(x )] * ppm.gFilter[0][1];
			double topright = ppm.gray[WIDTH*(y-1)+(x + 1)] * ppm.gFilter[0][2];

			double midleft = ppm.gray[WIDTH*(y) + (x - 1)] * ppm.gFilter[0][0];
			double midmid = ppm.gray[WIDTH*(y) + (x)] * ppm.gFilter[0][1];
			double midright = ppm.gray[WIDTH*(y) + (x + 1)] * ppm.gFilter[0][2];

			double botleft = ppm.gray[WIDTH*(y+1)+(x - 1)] * ppm.gFilter[0][0];
			double botmid = ppm.gray[WIDTH*(y+1)+(x)] * ppm.gFilter[0][1];
			double botright = ppm.gray[WIDTH*(y+1)+(x + 1)] * ppm.gFilter[0][2];

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
	int threshold = 110;
	int bottomThreshold = 70;
	for (int y = 1; y < HEIGHT - 2; y++) {
		for (int x = 1; x < WIDTH - 2; x++) {
			if (ppm.edge[WIDTH*y + x] > threshold)
				ppm.edge[WIDTH*y + x] = 255;
			else if (ppm.edge[WIDTH*y + x] < threshold && ppm.edge[WIDTH*y + x]>bottomThreshold) {
				vector<int> checksides;
				checksides.push_back(ppm.edge[WIDTH*(y - 1) + (x - 1)]);
				checksides.push_back(ppm.edge[WIDTH*(y - 1) + (x)]);
				checksides.push_back(ppm.edge[WIDTH*(y - 1) + (x + 1)]);
				checksides.push_back(ppm.edge[WIDTH*(y) + (x - 1)]);
				checksides.push_back(ppm.edge[WIDTH*(y ) + (x + 1)]);
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
				if(edgeYes)
					ppm.edge[WIDTH*y + x] = 255;
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
int main() {
	PPMImg ppm = loadAndGenerateGrayScale("image.ppm");
	ppm = createGaussianFilter(ppm);
	ppm = applyGaussian(ppm);
	ppm = applySobelOperator(ppm);
	ppm = applyHysteresis(ppm);
	ofstream img("imagef.ppm");//creates ppm image
	img << "P3" << endl;
	img << WIDTH << " " << HEIGHT << endl;
	img << "255" << endl;
	int y = 0;
	while (y<ppm.r.size()) {//draws everything
		img << ppm.edge[y] << " " << ppm.edge[y] << " " << ppm.edge[y] << endl;
		y++;
	}
	img.close();

	return 0;
}*/