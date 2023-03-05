// Project 8
#include <math.h>

#include <fstream>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>
#define h 400
#define w 600
using namespace cv;
using namespace std;
// create a vector storing x, y, z

vector<Point> points;  // create global vector to store all points
float side = 1;        // constant for the side length
int scale = 250;       // scale factor

float angle;  // angle used to calculate rotation matrixes
// shapes
float cube[8][3]{ {1, 1, 1},  {1, 1, -1},  {1, -1, 1},  {1, -1, -1},
                 {-1, 1, 1}, {-1, 1, -1}, {-1, -1, 1}, {-1, -1, -1} };

float dodec[20][3]{ {0, 0.6180339887498948, 1.618033988749895},
                   {0, 0.6180339887498948, -1.618033988749895},
                   {0, -0.6180339887498948, 1.618033988749895},
                   {0, -0.6180339887498948, -1.618033988749895},
                   {1.618033988749895, 0, 0.6180339887498948},
                   {1.618033988749895, 0, -0.6180339887498948},
                   {-1.618033988749895, 0, 0.6180339887498948},
                   {-1.618033988749895, 0, -0.6180339887498948},
                   {0.6180339887498948, 1.618033988749895, 0},
                   {0.6180339887498948, -1.618033988749895, 0},
                   {-0.6180339887498948, 1.618033988749895, 0},
                   {-0.6180339887498948, -1.618033988749895, 0},
                   {1, 1, 1},
                   {1, 1, -1},
                   {1, -1, 1},
                   {1, -1, -1},
                   {-1, 1, 1},
                   {-1, 1, -1},
                   {-1, -1, 1},
                   {-1, -1, -1} };

float icosa[12][3]{
    {0, 1.618033988749895, 1},   {0, 1.618033988749895, -1},  {0, -1.618033988749895, 1},
    {0, -1.618033988749895, -1}, {1, 0, 1.618033988749895},   {1, 0, -1.618033988749895},
    {-1, 0, 1.618033988749895},  {-1, 0, -1.618033988749895}, {1.618033988749895, 1, 0},
    {1.618033988749895, -1, 0},  {-1.618033988749895, 1, 0},  {-1.618033988749895, -1, 0} };

float octo[6][3]{
    {1, 0, 0}, {-1, 0, 0}, {0, 1, 0}, {0, -1, 0}, {0, 0, 1}, {0, 0, -1},
};

float tetra[4][3]{ {0.5773502691896258, 0, 0},
                  {0, 0, 0.8164965809277261},
                  {-0.28867513459481287, 0.5, 0},
                  {-0.28867513459481287, -0.5, 0} };
Point3f multMatX(float angle, Point3f p) {  // multiply matrix function relative to X axis

    float rotationX[3][3]{// x rotation matrix
                          {1, 0, 0},
                          {0, cos(angle), -sin(angle)},
                          {0, sin(angle), cos(angle)} };
    float point[3][1]{// makes PVector into a matrix
                      p.x, p.y, p.z };
    float mult[3][1]{// resulting multiplied matrix
                     0, 0, 0 };
    for (int i = 0; i < 3; ++i)  // multiply the matrices
        for (int j = 0; j < 1; ++j)
            for (int k = 0; k < 3; ++k) {
                mult[i][j] += rotationX[i][k] * point[k][j];
            }
    Point3f ret =
        Point3f(mult[0][0], mult[1][0], mult[2][0]);  // makes a PVector from the resulted matrix
    return ret;                                       // returns matrix
};
Point3f multMatY(float angle, Point3f p) {  // multiply matrix function relative to Y axis

    float rotationY[3][3]{// y rotation matrix
                          {cos(angle), 0, sin(angle)},
                          {0, 1, 0},
                          {-sin(angle), 0, cos(angle)} };
    float point[3][1]{// makes PVector into a matrix
                      p.x, p.y, p.z };
    float mult[3][1]{// resulting multiplied matrix
                     0, 0, 0 };
    for (int i = 0; i < 3; ++i)  // multiply the matrices
        for (int j = 0; j < 1; ++j)
            for (int k = 0; k < 3; ++k) {
                mult[i][j] += rotationY[i][k] * point[k][j];
            }
    Point3f ret =
        Point3f(mult[0][0], mult[1][0], mult[2][0]);  // makes a PVector from the resulted matrix
    return ret;                                       // returns matrix
};
Point3f multMatZ(float angle, Point3f p) {  // multiply matrix function relative to Z axis
    float rotationZ[3][3]{                  // z rotation matrix
                          {cos(angle), -sin(angle), 0},
                          {sin(angle), cos(angle), 0},
                          {0, 0, 1} };
    float point[3][1]{// makes PVector into a matrix
                      p.x, p.y, p.z };
    float mult[3][1]{// resulting multiplied matrix
                     0, 0, 0 };
    for (int i = 0; i < 3; ++i)  // multiply the matrices
        for (int j = 0; j < 1; ++j)
            for (int k = 0; k < 3; ++k) {
                mult[i][j] += rotationZ[i][k] * point[k][j];
            }
    Point3f ret =
        Point3f(mult[0][0], mult[1][0], mult[2][0]);  // makes a PVector from the resulted matrix
    return ret;                                       // returns matrix
};

Point3f addProjection(Point3f p) {   // multiplies the rotated PVector by the projection matrix
    float distance = 3.2;              // distance from camera to the cube
    float z = 1 / (distance - p.z);  // calculate the relative projection matrix multiplier
    float projection[2][3]{          // create projection matrix
                           {z, 0, 0},
                           {0, z, 0} };
    float point[3][1]{// makes PVector point into matrix
                      p.x, p.y, p.z };
    float mult[3][1]{// resulting matrix
                     0, 0, 0 };
    for (int i = 0; i < 2; ++i)  // do matrix multiplication
        for (int j = 0; j < 1; ++j)
            for (int k = 0; k < 3; ++k) {
                mult[i][j] += projection[i][k] * point[k][j];
            }
    Point3f ret = Point3f(mult[0][0], mult[1][0], mult[2][0]);  // makes PVector from mult matrix
    return ret;                                                 // returns resulting PVector
};
float findminDistance(vector<Point3f> pv) {
    float min = 10000000;
    for (int i = 0; i < pv.size(); i++) {
        for (int j = i + 1; j < pv.size(); j++) {
            float x = sqrt(pow(pv[i].x - pv[j].x, 2) + pow(pv[i].y - pv[j].y, 2) +
                pow(pv[i].z - pv[j].z, 2));
            if (x < min) min = x;
        }
    }
    return min;
}
vector<pair<int, int>> findEdges(vector<Point3f> pv) {
    float d = findminDistance(pv);
    vector<pair<int, int>> edges;
    for (int i = 0; i < pv.size(); i++) {
        for (int j = i + 1; j < pv.size(); j++) {
            float x = sqrt(pow(pv[i].x - pv[j].x, 2) + pow(pv[i].y - pv[j].y, 2) +
                pow(pv[i].z - pv[j].z, 2));
            if (abs(x - d) < 0.001) {
                edges.push_back(pair<int, int>(i, j));
            }
        }
    }
    return edges;
}
void createprojection(float shape[][3], int row, vector<String> axis) {
    bool axisx = false;
    bool axisy = false;
    bool axisz = false;
    for (int i = 0; i < axis.size(); i++){
    if (axis[i] == "x")
        axisx = true;
    else if (axis[i] == "y")
        axisy = true;
    else if (axis[i] == "z")
        axisz = true; }
    
    angle = 0;
    vector<Point3f> pvVector;
    for (int i = 0; i < row; i++) {
        Point3f point = Point3f(shape[i][0], shape[i][1], shape[i][2]);
        pvVector.push_back(point);
    }
    // creates image Mat
    Mat image = Mat::zeros(h, w, CV_8UC3);
    // starts videowriter object
    VideoWriter video("movie.avi", VideoWriter::fourcc('M', 'J', 'P', 'G'), 24, Size(w, h));
    // constants to add to the coordinates to center the cube
    float addConstX = w / 2;
    float addConstY = h / 2;
    int count = 0;

    vector<pair<int, int>> edges = findEdges(pvVector);

    while (count < 500) {
        vector<Point> pv;
        for (const Point3f& p : pvVector) {
            Point3f point = p;
            if(axisx==true)
                point = multMatX(angle, point);
            if(axisy==true)
                point = multMatY(angle, point);
            if(axisz==true)
                point = multMatZ(angle, point);
            point = addProjection(point);
            pv.push_back(Point(point.x * scale  + addConstX, point.y * scale  + addConstY));
        }
        // pvVector = pv;

       // cout << edges.size() << " ";

        for (const auto& p : edges) {
            line(image, pv[p.first], pv[p.second], Scalar(255, 255, 255), 2);
        }

        video << image;
        imshow("image", image);
        waitKey(16);
       

        angle += .05;

        // clears the image
        image = Mat::zeros(h, w, CV_8UC3);
        count++;
    }
    video.release();  // releases the video
}
void chooseprojection(int selection,vector<String> v) {
    if (selection == 1)
        createprojection(cube, 8, v);
    else if (selection == 2)
        createprojection(dodec, 20, v);
    else if (selection == 3)
        createprojection(icosa, 12, v);
    else if (selection == 4)
        createprojection(tetra, 4, v);
    else if (selection == 5)
        createprojection(octo, 6, v);
}
int main() {

    int selection;
    vector<String> v;
    String axis1, axis2, axis3;
    cout << "Shape Projection Project" << endl;
    cout << "Type 1 for cube, 2 for dodecahedron, 3 for icosahedron, 4 for tetrahedron, or 5 for octohedron" << endl;
    cout << "Type a number for 500 frames of rotation: ";
    cin >> selection;
    while (selection != 1 && selection != 2 && selection != 3 && selection != 4 && selection != 5) {
        cout << "Please type the number for which shape you want: ";
        cin >> selection;
    }
    cout << "Type which axis you want to rotate around (lower case, 'x', 'y', or 'z'): ";
    cin >> axis1;
    while (axis1 != "x" && axis1 != "y" && axis1 != "z") {
        cout << "Please type which axis you want to rotate around (lower case, 'x', 'y', or 'z'): ";
        cin >> axis1;
    }
    v.push_back(axis1);
    cout << "Type which second axis you want to rotate around (type '0' if you do not want a second axis): ";
    cin >> axis2;
    while (axis2 != "x" && axis2 != "y" && axis2 != "z" && axis2!="0") {
        cout << "Please type which axis you want to rotate around or '0' if you do not want a second axis: ";
        cin >> axis2;
    }
    if (axis2 != "0") {
        v.push_back(axis2);
        cout << "Type which third axis you want to rotate around (type '0' if you do not want a third axis): ";
        cin >> axis3;
        while (axis3 != "x" && axis3 != "y" && axis3 != "z" && axis3 != "0") {
            cout << "Please type which axis you want to rotate around or '0' if you do not want a third axis: ";
            cin >> axis3;
        }
        if (axis3 != "0")
            v.push_back(axis2);
    }
    chooseprojection(selection, v);


    return (0);
}
// Project 7
/**
#include <fstream>
#include <opencv2/opencv.hpp>
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
        HoughCircles(gray, circles, HOUGH_GRADIENT, 1, gray.cols / 50, 100, 30, gray.cols / 80,
gray.cols / 16);

        Mat radii(circles.size(), 1, CV_32F), coin_labels, centers;
        for (int i = 0; i < circles.size(); i++) {
                radii.at<float>(i) = circles[i][2];
        }
        kmeans(radii, 3, coin_labels,TermCriteria(TermCriteria::MAX_ITER | TermCriteria::EPS, 10,
.01), 10,	KMEANS_PP_CENTERS, centers);


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
}**/