

#define _USE_MATH_DEFINES
#include <opencv2/opencv.hpp>
using namespace cv;
using namespace std;

vector<Point3f> objPts{
	Point3f(0, 0, 0),  // 0
	Point3f(1, 0, 0),  // 1
	Point3f(0, 1, 0),  // 2
	Point3f(1, 1, 0),  // 3
	Point3f(0, 0, -1), // 4
	Point3f(1, 0, -1), // 5
	Point3f(0, 1, -1), // 6
	Point3f(1, 1, -1), // 7
};

// Set vector for the 4 points in the base
vector<Point3f> basepoints{
	Point3f(0, 0, 0),  // 0
	Point3f(1, 0, 0),  // 1
	Point3f(0, 1, 0),  // 2
	Point3f(1, 1, 0),  // 3
};

// Set edge lines of the object
vector<pair<int, int>> linePairs{
	pair<int, int>(0, 1),
	pair<int, int>(1, 3),
	pair<int, int>(3, 2),
	pair<int, int>(0, 2),
	pair<int, int>(4, 5),
	pair<int, int>(5, 7),
	pair<int, int>(7, 6),
	pair<int, int>(4, 6),
	pair<int, int>(0, 4),
	pair<int, int>(1, 5),
	pair<int, int>(2, 6),
	pair<int, int>(3, 7),
};

int main(int argc, char** argv) {
	VideoCapture capture("withChessBoard.mov");

	Size videoSize(capture.get(CAP_PROP_FRAME_WIDTH), capture.get(CAP_PROP_FRAME_HEIGHT));
	VideoWriter outputVideo("result.avi", VideoWriter::fourcc('M', 'J', 'P', 'G'),
		capture.get(CAP_PROP_FPS), videoSize);

	Size chessboardSize(7, 7);
	vector<vector<Point3f>> checkerptsMat;
	vector<vector<Point2f>> fMatrix;

	int numFrames = capture.get(CAP_PROP_FRAME_COUNT);

	Mat cameraMatrix, distCoeffs;
	vector<Mat> rot, tran;
	vector<Point3f> checkerobjPts, checkerobjpts;
	for (int i = -chessboardSize.width / 2; i <= chessboardSize.width / 2; i++)
		for (int j = -chessboardSize.height / 2; j <= chessboardSize.height / 2; j++)
			checkerobjpts.push_back(Point3f(i, j, 0));

	Mat frame, greyImg, emp;

	Mat prevFrame, prevImg;
	vector<Point2f> origPts;
	vector<Point2f> corners;

	capture >> prevFrame;
	cvtColor(prevFrame, prevImg, COLOR_BGR2GRAY);
	findChessboardCorners(prevFrame, chessboardSize, corners,CALIB_CB_FAST_CHECK);

	for (int i = 1; i < numFrames; i++) {
		if (i % 30 != 0) continue;

		checkerobjPts = checkerobjpts;
		capture >> frame;
		if (frame.empty()) break;
		flip(frame, frame, 0);
		cvtColor(frame, greyImg, COLOR_BGR2GRAY);
		bool found = findChessboardCorners(frame, chessboardSize, corners,CALIB_CB_FAST_CHECK);
		if (found) { 
			cv::cornerSubPix(greyImg, corners, cv::Size(11, 11), cv::Size(-1, -1), TermCriteria(TermCriteria::EPS + TermCriteria::COUNT, 30, 0.0001));
			checkerptsMat.push_back(checkerobjPts);
			fMatrix.push_back(corners); 
		}
	}
	calibrateCamera(checkerptsMat,
		fMatrix, cv::Size(greyImg.rows, greyImg.cols),
		cameraMatrix, distCoeffs, rot, tran);

	VideoCapture capture2("withChessBoard.mov");
	Mat rvec, tvec;
	capture2 >> prevFrame;
	cvtColor(prevFrame, prevImg, COLOR_BGR2GRAY); 
	for (int i = 1; i < numFrames; i++) { 

		capture2 >> frame;
		if (frame.empty()) break; 
		flip(frame, frame, 0); 
		cvtColor(frame, greyImg, COLOR_BGR2GRAY); 
		vector<Point2f> outputPts;

		bool found = findChessboardCorners(frame, chessboardSize, corners,CALIB_CB_NORMALIZE_IMAGE + CALIB_CB_FAST_CHECK);
		if (found) {
			if (i > 3) {
				origPts = corners;
				vector<uchar> status;
				vector<float> err;
				TermCriteria crit = TermCriteria((TermCriteria::COUNT) + (TermCriteria::EPS), 10, 0.03);
				vector<Point2f> newPts;
				calcOpticalFlowPyrLK(prevImg, greyImg, origPts, corners, status, err, Size(11, 11), 2, crit);
			}
			vector<Point2f> imgbasepts = { corners[16], corners[18], corners[30], corners[32] };

			solvePnP(basepoints, imgbasepts, cameraMatrix, distCoeffs, rvec, tvec, false, SOLVEPNP_ITERATIVE);

			prevImg = greyImg.clone();
			projectPoints(objPts, rvec, tvec, cameraMatrix, distCoeffs, outputPts);	
			int counter = 0;
			for (const pair<int, int>& lines : linePairs) {
					line(frame, outputPts[lines.first], outputPts[lines.second],Scalar(0, 0, 255), 5);
				counter++;
			}
		}
		else {
			vector<uchar> status;
			vector<float> err;
			TermCriteria crit = TermCriteria((TermCriteria::COUNT) + (TermCriteria::EPS), 10, 0.03);
			vector<Point2f> newPts;
			calcOpticalFlowPyrLK(prevImg, greyImg, origPts, newPts, status, err, Size(11, 11), 6, crit);

			prevImg = greyImg.clone();
			origPts = newPts;
			
			cv::TermCriteria criteria(TermCriteria::EPS | TermCriteria::MAX_ITER, 30, 0.001);
			cv::cornerSubPix(greyImg, origPts, cv::Size(11, 11), cv::Size(-1, -1), criteria);

			vector<Point2f> imgbasepts = { origPts[16],origPts[18] , origPts[30], origPts[32] };
			solvePnP(basepoints, imgbasepts, cameraMatrix, distCoeffs, rvec, tvec, false, SOLVEPNP_ITERATIVE);
			projectPoints(objPts, rvec, tvec, cameraMatrix, distCoeffs, outputPts);

			int counter = 0;
			for (const pair<int, int>& lines : linePairs) {
				line(frame, outputPts[lines.first], outputPts[lines.second],Scalar(0, 0, 255), 5);
				counter++;
			}
			
		}
		imshow("Frame", frame);
		char c = (char)waitKey(25);
		if (c == 27) break;
		outputVideo << frame;
	}

	capture.release();
	outputVideo.release();
	cv::destroyAllWindows();

	return 0;
}
