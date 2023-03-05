
//Project 7
#include <opencv2/opencv.hpp>
#include <fstream>
using namespace cv;
using namespace std;

int main(int argc, char** argv) {
	const char* file = argc >= 2 ? argv[1] : "coins3.jpg";
	Mat src = imread(samples::findFile(file), IMREAD_COLOR);
	Mat dst; 
	cv::resize(src, dst, cv::Size(), .25, .25);
	Mat gray;
	cvtColor(dst, gray, COLOR_BGR2GRAY);
	GaussianBlur(gray, gray, Size(5, 5), 20);

	vector<Vec3f> circles;
	HoughCircles(gray, circles, HOUGH_GRADIENT, 1, gray.cols / 50, 100, 30, gray.cols / 80,	gray.cols / 16);

	Mat radii(circles.size(), 1, CV_32F), coin_labels, centers;
	for (int i = 0; i < circles.size(); i++) {
		radii.at<float>(i) = circles[i][2];
	}
	kmeans(radii, 3, coin_labels,TermCriteria(TermCriteria::MAX_ITER | TermCriteria::EPS, 10, .01), 10,	KMEANS_PP_CENTERS, centers);
	
	
	float total = 0.0;
	int p = 0.0;
	int n = 0.0;
	int q = 0.0;
	int hd = 0.0;
	for (int i = 0; i < circles.size(); i++) {
		Point center = Point(circles[i][0], circles[i][1]);
		circle(dst, center, 1, Scalar(255, 255, 255), 1, LINE_4);
		Scalar color;
		switch (coin_labels.at<int>(i)) {
		case 0:
			color = Scalar(0, 0, 255);
			p++;
			total += 0.01f;
			break;
		case 1:
			color = Scalar(255, 215, 0);
			n++;
			total += 0.05f;
			break;
		case 2:
			color = Scalar(255, 0, 0);
			q++;
			total += 0.25f;
			break;
		case 3:
			color = Scalar(0, 0, 0);
			hd++;
			total += 0.50f;
		}
		
		circle(dst, center, circles[i][2], color, 3, LINE_AA);
	}
	ofstream fileout("results.txt");
	//fileout << coin_labels << endl;
	vector<int> compression_params;
	compression_params.push_back(95);
	cout << "Pennies: " << p << endl;
	cout << "Nickles: " << n << endl;
	cout << "Quarters: " << q << endl;
	cout << "Half Dollars: " << hd << endl;
	cout << "Total: $" << setprecision(2) << fixed << total;
	fileout << "Pennies: " << p << endl;
	fileout << "Nickles: " << n << endl;
	fileout << "Quarters: " << q << endl;
	fileout << "Half Dollars: " << hd << endl;
	fileout << "Total: $" <<setprecision(2)<<fixed<< total;
	fileout.close();
	// Show results
	imshow("Detected Circles", dst);
	imwrite("imagec.jpg", dst, compression_params);
	// Wait and Exit
	waitKey();
	return 0;
}