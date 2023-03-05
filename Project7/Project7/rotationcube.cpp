//Project 8 
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/core.hpp>
#include <math.h>
#include <fstream>
#define w 400
#define h 600
using namespace cv;
using namespace std;

class PVector{
public:
  float x;
  float y;
  float z;
  PVector(float x_, float y_, float z__) {
	x = x_;
	y = y_;
	z = z__;
  }
  float get(int i) {
	  if (i == 0)
		  return x;
	  else if (i == 1)
		  return y;
	  else
		  return z;
  }
  void setValues(PVector newP) {
	  x = newP.x;
	  y = newP.y;
	  z = newP.z;
  }
};

vector<Point> points;
float projection[2][3]{
	{1,0,0},
	{0,1,0}
};
float angle;
PVector multMatX(float angle, PVector p) {

	float rotationX[3][3]{
		{1,0,0},
		{0, cos(angle),-sin(angle)},
		{0, sin(angle),cos(angle)} };
	float point[3][1]{
		p.x, p.y, p.z
	};
	float mult[3][1]{
		0,0,0
	};
	for (int i = 0; i < 3; ++i)
		for (int j = 0; j < 1; ++j)
			for (int k = 0; k < 3; ++k)
			{
				mult[i][j] += rotationX[i][k] * point[k][j];
			}
	PVector ret = PVector(mult[0][0], mult[1][0], mult[2][0]);
	return ret;
};
PVector multMatY(float angle, PVector p) {

	float rotationY[3][3]{
		{cos(angle),0,sin(angle)},
		{0,1,0},
		{-sin(angle),0,cos(angle)} };
	float point[3][1]{
		p.x, p.y, p.z
	};
	float mult[3][1]{
		0,0,0
	};
	for (int i = 0; i < 3; ++i)
		for (int j = 0; j < 1; ++j)
			for (int k = 0; k < 3; ++k)
			{
				mult[i][j] += rotationY[i][k] * point[k][j];
			}
	PVector ret = PVector(mult[0][0], mult[1][0], mult[2][0]);
	return ret;
};
PVector multMatZ(float angle, PVector p) {
	float rotationZ[3][3]{
	{cos(angle),-sin(angle), 0},
	{sin(angle),cos(angle), 0},
	{0,0,1} };
	float point[3][1]{
		p.x, p.y, p.z
	};
	float mult[3][1]{
		0,0,0
	};
	for (int i = 0; i < 3; ++i)
		for (int j = 0; j < 1; ++j)
			for (int k = 0; k < 3; ++k)
			{
				mult[i][j] += rotationZ[i][k] * point[k][j];
			}
	PVector ret = PVector(mult[0][0], mult[1][0], mult[2][0]);
	return ret;
};

PVector multMatrix(Mat M, PVector p) {
	float point[3][1]{
		p.x, p.y, p.z
	};
	Mat pointMat = Mat(3, 1, CV_64F, point);
	Mat resultmat = Mat(3, 1, CV_64F);
		resultmat= M * pointMat;
	PVector result = PVector(resultmat.at<float>(0, 0), resultmat.at<float>(1, 0), resultmat.at<float>(2, 0));//error here
	return result;
}
int main() {
	angle = 0;
	PVector p1 = PVector(-50, -50, -50);
	PVector p2 = PVector(50, -50, -50);
	PVector p3 = PVector(50, 50, -50);
	PVector p4 = PVector(-50, 50, -50);
	PVector p5 = PVector(-50, -50, 50);
	PVector p6 = PVector(50, -50, 50);
	PVector p7 = PVector(50, 50, 50);
	PVector p8 = PVector(-50, 50, 50);
	points.push_back(Point(p1.x, p1.y));
	points.push_back(Point(p2.x, p2.y));
	points.push_back(Point(p3.x, p3.y));
	points.push_back(Point(p4.x, p4.y));
	points.push_back(Point(p5.x, p5.y));
	points.push_back(Point(p6.x, p6.y));
	points.push_back(Point(p7.x, p7.y));
	points.push_back(Point(p8.x, p8.y));
	Mat image = Mat::zeros(w, h, CV_8UC3);
	VideoWriter video("outcpp.avi", VideoWriter::fourcc('M', 'J', 'P', 'G'), 10, Size(w, h));
	int addConst = 200;
	while (1) {
		//face1
		line(image, points[0], points[1], Scalar(255, 255, 255), 1);
		line(image, points[1], points[2], Scalar(255, 255, 255), 1);
		line(image, points[2], points[3], Scalar(255, 255, 255), 1);
		line(image, points[0], points[3], Scalar(255, 255, 255), 1);
		//face 2
		line(image, points[4], points[5], Scalar(255, 255, 255), 1);
		line(image, points[5], points[6], Scalar(255, 255, 255), 1);
		line(image, points[6], points[7], Scalar(255, 255, 255), 1);
		line(image, points[4], points[7], Scalar(255, 255, 255), 1);
		//connecting lines
		line(image, points[3], points[7], Scalar(255, 255, 255), 1);
		line(image, points[2], points[6], Scalar(255, 255, 255), 1);
		line(image, points[1], points[5], Scalar(255, 255, 255), 1);
		line(image, points[4], points[0], Scalar(255, 255, 255), 1);
		video.write(image);
		imshow("image", image);
		angle += .1;
		
	 PVector newp1 = multMatY(angle, multMatZ(angle, multMatX(angle, p1)));
	 PVector newp2 = multMatY(angle, multMatZ(angle, multMatX(angle, p2)));
	 PVector newp3 = multMatY(angle, multMatZ(angle, multMatX(angle, p3)));
	 PVector newp4 = multMatY(angle, multMatZ(angle, multMatX(angle, p4)));
	 PVector newp5 = multMatY(angle, multMatZ(angle, multMatX(angle, p5)));
	 PVector newp6 = multMatY(angle, multMatZ(angle, multMatX(angle, p6)));
	 PVector newp7 = multMatY(angle, multMatZ(angle, multMatX(angle, p7)));
	 PVector newp8 = multMatY(angle, multMatZ(angle, multMatX(angle, p8)));
		 points.clear();
		points.push_back(Point(newp1.x+ addConst, newp1.y + addConst));
		points.push_back(Point(newp2.x + addConst, newp2.y + addConst));
		points.push_back(Point(newp3.x + addConst, newp3.y + addConst));
		points.push_back(Point(newp4.x + addConst, newp4.y + addConst));
		points.push_back(Point(newp5.x + addConst, newp5.y + addConst));
		points.push_back(Point(newp6.x + addConst, newp6.y + addConst));
		points.push_back(Point(newp7.x + addConst, newp7.y + addConst));
		points.push_back(Point(newp8.x + addConst, newp8.y + addConst));
		waitKey(50);

		image = Mat::zeros(w, h, CV_8UC3);

		cout << p1.x << " " << p1.y << " " << p1.z <<"| " <<cos(angle)<<endl;
	}
	waitKey(0);
	
	return(0);
}
//Project 7
/**#include <opencv2/opencv.hpp>
#include <fstream>
using namespace cv;
using namespace std;

int main(int argc, char** argv) {
	const char* file = argc >= 2 ? argv[1] : "coins3.jpg";
	Mat src = imread(samples::findFile(file), IMREAD_COLOR);
	Mat gray;
	cvtColor(src, gray, COLOR_BGR2GRAY);
	GaussianBlur(gray, gray, Size(5, 5), 20);

	vector<Vec3f> circles;
	HoughCircles(gray, circles, HOUGH_GRADIENT, 1, gray.cols / 50, 100, 30, gray.cols / 80,	gray.cols / 16);

	Mat radii(circles.size(), 1, CV_32F), coin_labels, centers;
	for (int i = 0; i < circles.size(); i++) {
		radii.at<float>(i) = circles[i][2];
	}
	kmeans(radii, 3, coin_labels,TermCriteria(TermCriteria::MAX_ITER | TermCriteria::EPS, 10, .01), 10,	KMEANS_PP_CENTERS, centers);

	float total = 0.0;
	for (int i = 0; i < circles.size(); i++) {
		Point center = Point(circles[i][0], circles[i][1]);
		circle(src, center, 1, Scalar(255, 255, 255), 1, LINE_4);
		Scalar color;
		switch (coin_labels.at<int>(i)) {
		case 0:
			color = Scalar(0, 0, 255);
			total += 0.01f;
			break;
		case 1:
			color = Scalar(255, 215, 0);
			total += 0.05f;
			break;
		case 2:
			color = Scalar(255, 0, 0);
			total += 0.25f;
			break;
		case 3:
			color = Scalar(0, 0, 0);
			total += 0.50f;
		}
		
		circle(src, center, circles[i][2], color, 3, LINE_AA);
	}
	ofstream fileout("results.txt");

	vector<int> compression_params;
	compression_params.push_back(95);
	fileout << "Total: $" <<setprecision(2)<<fixed<< total;
	fileout.close();
	// Show results
	//imshow("Detected Circles", src);
	imwrite("imagec.jpg", src, compression_params);
	// Wait and Exit
	waitKey();
	return 0;
}**/