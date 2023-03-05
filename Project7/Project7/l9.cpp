#define _USE_MATH_DEFINES
#include <opencv2/opencv.hpp>
using namespace cv;
using namespace std;

int main(int argc, char** argv) {
	// Open video file
	VideoCapture cap("withChessBoard.mov");
	if (!cap.isOpened()) {
		cout << "Error opening video stream or file" << endl;
		return -1;
	}

	Size videoSize(cap.get(CAP_PROP_FRAME_WIDTH), cap.get(CAP_PROP_FRAME_HEIGHT));
	VideoWriter outputVideo("result.avi", VideoWriter::fourcc('M', 'J', 'P', 'G'),
		cap.get(CAP_PROP_FPS), videoSize);

	if (!outputVideo.isOpened()) {
		cout << "Could not open the output video for write: "
			<< "output.avi" << endl;
		return -1;
	}

	// Set vertices for 3D object
	vector<Point3f> object_points{
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
	vector<Point3f> base_points{
		Point3f(0, 0, 0),  // 0
		Point3f(1, 0, 0),  // 1
		Point3f(0, 1, 0),  // 2
		Point3f(1, 1, 0),  // 3
	};

	// Set edge lines of the object
	vector<pair<int, int>> object_lines{
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

	Size chessboardSize(7, 7);
	vector<vector<Point3f>> checkerptsMat;
	vector<vector<Point2f>> framesMat;

	// Get number of frames
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

	// Calibrate camera
	for (int i = 1; i < frames; i++) {
		// Skip some frames for chess borad in different directions
		if (i % 20 != 0) continue;

		checker_object_pts = checkerobjpts;
		printf_s("\rGenerating frame %d of %d...", i, frames);
		cap >> frame;
		if (frame.empty()) break;//takes frames
		flip(frame, frame, 0);//flips so its rightside up
		cvtColor(frame, gray, COLOR_BGR2GRAY);//gray image
		bool found = findChessboardCorners(frame, chessboardSize, corners,
			CALIB_CB_FAST_CHECK); //finds chessboard corners
		if (found) {//if found chessboardcorners
			cv::cornerSubPix(gray, corners, cv::Size(11, 11), cv::Size(-1, -1), TermCriteria(TermCriteria::EPS + TermCriteria::COUNT, 30, 0.0001));
			checkerptsMat.push_back(checker_object_pts);
			framesMat.push_back(corners);//framesMat pushback the corners found
		}
	}
	calibrateCamera(checkerptsMat,
		framesMat, cv::Size(gray.rows, gray.cols),
		cameraMatrix, distCoeffs, R, T);

	VideoCapture cap2("withChessBoard.mov");//new video input
	Mat rot, tran;
	cap2 >> old_frame;
	cvtColor(old_frame, old_gray, COLOR_BGR2GRAY);//THIS LOOP FOR PROJECTION AND SOLVEPNP
	for (int i = 1; i < frames; i++) {//draws the projections
		printf_s("\rDrawing Frame %d of %d...", i, frames);

		cap2 >> frame;
		if (frame.empty()) break;//takes frames
		flip(frame, frame, 0);//flips so its rightside up
		cvtColor(frame, gray, COLOR_BGR2GRAY);//gray image
		vector<Point2f> output_points;

		//ejurjsince we use solvepnp we do not need the rot and trans from prev and we use findchessbardcorners
		bool found = findChessboardCorners(frame, chessboardSize, corners,
			CALIB_CB_NORMALIZE_IMAGE + CALIB_CB_FAST_CHECK); //finds chessboard corners
		if (found) {
			if (i > 3) {
				p0 = corners;
				vector<uchar> status;
				vector<float> err;
				TermCriteria crit = TermCriteria((TermCriteria::COUNT) + (TermCriteria::EPS), 10, 0.03);
				vector<Point2f> good_new;
				calcOpticalFlowPyrLK(old_gray, gray, p0, corners, status, err, Size(11, 11), 3, crit);//optical flow with 6 pyramids
			}
			vector<Point2f> image_base__pts = { corners[16], corners[18], corners[30], corners[32] };

			// Ignore lens distortion
			distCoeffs = (Mat1d(4, 1) << 0, 0, 0, 0);

			solvePnP(base_points, image_base__pts, cameraMatrix, distCoeffs, rot, tran, false, SOLVEPNP_ITERATIVE);

			old_gray = gray.clone();//updates old gray images and corners
			projectPoints(object_points, rot, tran, cameraMatrix, distCoeffs, output_points);//projects points
			cout << output_points << endl;
		//	drawChessboardCorners(frame, Size(7, 7), corners, true);

			//ejurj lets draw a different color the first 4 since they are the base to visually see that the base is correct
			//we will use a counter variable
			int counter = 0;
			for (const pair<int, int>& lines : object_lines) {//draws cube lines
				if (counter < 4)
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
		else {//using opticalflow to find chessboard corner when chessboard is blocked
			vector<uchar> status;
			vector<float> err;
			TermCriteria crit = TermCriteria((TermCriteria::COUNT) + (TermCriteria::EPS), 10, 0.03);
			vector<Point2f> good_new;
			calcOpticalFlowPyrLK(old_gray, gray, p0, good_new, status, err, Size(11, 11), 8, crit);//optical flow with 6 pyramids

			old_gray = gray.clone();//updates old gray images and corners
			p0 = good_new;
			//framesMat.push_back(good_new);//framesMat pushback the corners found
			cv::TermCriteria criteria(TermCriteria::EPS | TermCriteria::MAX_ITER, 30, 0.001);
			cv::cornerSubPix(gray, p0, cv::Size(11, 11), cv::Size(-1, -1), criteria);
		//	drawChessboardCorners(frame, Size(7, 7), p0, true);

			vector<Point2f> image_base__pts = { p0[16],p0[18] , p0[30], p0[32] };

			// Ignore lens distortion
			distCoeffs = (Mat1d(4, 1) << 0, 0, 0, 0);
			solvePnP(base_points, image_base__pts, cameraMatrix, distCoeffs, rot, tran, false, SOLVEPNP_ITERATIVE);

			projectPoints(object_points, rot, tran, cameraMatrix, distCoeffs, output_points);//projects points
			cout << output_points << endl;

			// Draw a different color the first 4 since they are the base to visually see that the base is correct
			int counter = 0;
			for (const pair<int, int>& lines : object_lines) {//draws cube lines
				if (counter < 4)
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