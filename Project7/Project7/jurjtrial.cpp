
#define _USE_MATH_DEFINES
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

int main(int argc, char** argv) {
	VideoCapture cap("withChessBoard.mov");
	if (!cap.isOpened()) {
		cout << "Error opening video stream or file" << endl;
		return -1;
	}

	Size videoSize(cap.get(CAP_PROP_FRAME_WIDTH), cap.get(CAP_PROP_FRAME_HEIGHT));
	VideoWriter outputVideo("output.avi", VideoWriter::fourcc('M', 'J', 'P', 'G'),
		cap.get(CAP_PROP_FPS), videoSize);

	if (!outputVideo.isOpened()) {
		cout << "Could not open the output video for write: "
			<< "output.avi" << endl;
		return -1;
	}

	//ejurj corrected the vertices
	vector<Point3f> object_points{
		Point3f(0, 0, 0),  // 0
		Point3f(1, 0, 0),   // 1
		Point3f(0, 1, 0),   // 2
		Point3f(1, 1, 0),    // 3
		Point3f(0, 0, -1),  // 4
		Point3f(1, 0, -1),   // 5
		Point3f(0, 1, -1),   // 6
		Point3f(1, 1, -1),    // 7
	};	
	//ejurj added a vector for the 4 points in the base
	vector<Point3f> base_points{
		Point3f(0, 0, 0),  // 0
		Point3f(1, 0, 0),   // 1
		Point3f(0, 1, 0),   // 2
		Point3f(1, 1, 0),    // 3
	};


	vector<pair<int, int>> object_lines{
		pair<int, int>(0, 1),  //
		pair<int, int>(1, 3),  //
		pair<int, int>(3, 2),  //
		pair<int, int>(0, 2),  //
		pair<int, int>(4, 5),  //
		pair<int, int>(5, 7),  //
		pair<int, int>(7, 6),  //
		pair<int, int>(4, 6),  //
		pair<int, int>(0, 4),  //
		pair<int, int>(1, 5),  //
		pair<int, int>(2, 6),  //
		pair<int, int>(3, 7),
	};

	Size chessboardSize(7, 7);
	vector<vector<Point3f>> checkerptsMat;
	vector<vector<Point2f>> framesMat;
	int frames = cap.get(CAP_PROP_FRAME_COUNT);
	Mat cameraMatrix, distCoeffs;
	vector<Mat> R, T;
	vector<Point3f> checker_object_pts, checkerobjpts;
	vector<Mat> rvec, tvec, camMatMat, distMat;
	for (int i = -chessboardSize.width / 2; i <= chessboardSize.width / 2; i++)
		for (int j = -chessboardSize.height / 2; j <= chessboardSize.height / 2; j++)
			checkerobjpts.push_back(Point3f(i, j, 0));
	Mat frame, gray, emp;

	Mat old_frame, old_gray;
	vector<Point2f> p0, p1;
	vector<Point2f> corners;


	cap >> old_frame;
	cvtColor(old_frame, old_gray, COLOR_BGR2GRAY);
	findChessboardCorners(old_frame, chessboardSize, corners,
		CALIB_CB_FAST_CHECK); //finds chessboard corners

	for (int i = 1; i <= frames / 3; i++) {
		checker_object_pts = checkerobjpts;
		checkerptsMat.push_back(checker_object_pts);
		printf_s("\rGenerating frame %d of %d...", i, frames);
		cap >> frame;
		if (frame.empty()) break;//takes frames
		flip(frame, frame, 0);//flips so its rightside up
		cvtColor(frame, gray, COLOR_BGR2GRAY);//gray image
		bool found = findChessboardCorners(frame, chessboardSize, corners,
			CALIB_CB_FAST_CHECK); //finds chessboard corners
		if (found) {//if found chessboardcorners
		 //   drawChessboardCorners(frame, Size(7, 7), corners, found);
			framesMat.push_back(corners);//framesMat pushback the corners found
			p0 = corners;//sets previous corners to corners found
			cv::TermCriteria criteria(TermCriteria::EPS | TermCriteria::MAX_ITER, 30, 0.001);
			cv::cornerSubPix(gray, corners, cv::Size(11, 11), cv::Size(-1, -1), criteria);
		}
		else {//using opticalflow to find chessboard corner when chessboard is blocked
			vector<uchar> status;
			vector<float> err;
			TermCriteria crit = TermCriteria((TermCriteria::COUNT) + (TermCriteria::EPS), 10, 0.03);
			vector<Point2f> good_new;
			calcOpticalFlowPyrLK(old_gray, gray, p0, good_new, status, err, Size(11, 11), 6, crit);//optical flow with 6 pyramids

			old_gray = gray.clone();//updates old gray images and corners
			p0 = good_new;
			framesMat.push_back(good_new);//framesMat pushback the corners found
			cv::TermCriteria criteria(TermCriteria::EPS | TermCriteria::MAX_ITER, 30, 0.001);
			cv::cornerSubPix(gray, good_new, cv::Size(11, 11), cv::Size(-1, -1), criteria);
			//  drawChessboardCorners(frame, Size(7, 7), good_new, true);
		}
		if (i % 15 == 0) {
			calibrateCamera(checkerptsMat,
				framesMat, cv::Size(gray.rows, gray.cols),
				cameraMatrix, distCoeffs, R, T);//calibrates camera every 15 frames
		//    camMatMat.push_back(cameraMatrix);
			rvec.insert(rvec.end(), R.begin(), R.end());//updates the video rotation and translation vector of vectors
			tvec.insert(tvec.end(), T.begin(), T.end());
			checkerptsMat.clear();//clears all the inputarrays
			framesMat.clear();
			R.clear();
			T.clear();
		}


	}

	VideoCapture cap2("withChessBoard.mov");//new video input
	Mat rot, tran;
	for (int i = 1; i < frames / 3; i++) {//draws the projections
		printf_s("\rDrawing Frame %d of %d...", i, frames);

		cap2 >> frame;
		if (frame.empty()) break;//takes frames
		flip(frame, frame, 0);//flips so its rightside up
		cvtColor(frame, gray, COLOR_BGR2GRAY);//gray image
		vector<Point2f> output_points;
		rot = rvec[i - 1];//gets rot and trans matrices
		tran = tvec[i - 1];
		//ejurjsince we use solvepnp we do not need the rot and trans from prev and we use findchessbardcorners
		bool found = findChessboardCorners(frame, chessboardSize, corners,
			CALIB_CB_FAST_CHECK); //finds chessboard corners
		if(found) {
		//if (!rot.empty()) {
			//      cameraMatrix = camMatMat[i - 1];
				 // distCoeffs = distMat[i - 1];

			//ejurj added solvePnP
			vector<Point2f> image_base__pts = { corners[16], corners[18], corners[30], corners[32] };
			distCoeffs = (Mat1d(4, 1) << 0, 0, 0, 0);
			cameraMatrix = (Mat_<double>(3, 3) << frame.cols, 0, frame.cols / 2, 0, frame.rows, frame.rows / 2, 0, 0, 1);
			solvePnP(base_points, image_base__pts, cameraMatrix, distCoeffs, rot, tran, false, SOLVEPNP_P3P);


			projectPoints(object_points, rot, tran, cameraMatrix, distCoeffs, output_points);//projects points
			cout << output_points << endl;

			//ejurj lets draw a different color the first 4 since they are the base to visually see that the base is correct
			//we will use a counter variable
			int counter = 0;
			for (const pair<int, int>& lines : object_lines) {//draws cube lines
				if(counter<4)
				  line(frame, output_points[lines.first], output_points[lines.second],
					Scalar(0, 255, 0), 10);
				else
					line(frame, output_points[lines.first], output_points[lines.second],
						Scalar(255, 0, 0), 10);
				counter++;
			}
			for (Point2f pt : output_points)
				circle(frame, pt, 5, Scalar(0, 255, 0), 2);
		}
		imshow("Frame", frame);
		char c = (char)waitKey(25);
		if (c == 27) break;
		outputVideo << frame;
	}

	cap.release();
	outputVideo.release();
	cv::destroyAllWindows();

	return 0;
}