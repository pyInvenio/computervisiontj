//Name: Evan Williams
//Date: April 24, 2020
//Computer Vision, Period 5, Mr. Jurj
//Lab 6 - Coin Detection

#define _USE_MATH_DEFINES
#define PI 3.14159265

#include <cmath>
#include <fstream>
#include <string>
#include <vector>
#include <iostream>
#include <sstream>

using namespace std;
int WIDTH;
int HEIGHT;

double minThreshold = 0.25;
double maxThreshold = 0.35;

vector<vector<double>> image;	//Instantiate all of the vectors I will need
vector<vector<vector<int>>> originalImage;
vector<vector<int>> edges;
vector<vector<double>> strong;
vector<vector<double>> weak;
vector<vector<double>> angle;
vector<vector<double>> gradient;
vector<vector<double>> circles;
vector<vector<double>> radii;
vector<vector<double>> votes;

int circle_threshold = 16;	//Create global variables for the coins
int quarters = 0;
int pennies = 0;
int dimes = 0;
int nickels = 0;
double sum = 0;

void input(string s)	//Method to input the image chosen (Coins1, Coins2, Coins3, etc)
{

    ifstream infile(s, ios::binary);
    string type;
    int clrSize;

    infile >> type;
    infile >> WIDTH;
    infile >> HEIGHT;
    infile >> clrSize;

    image.resize(WIDTH);
    for (int i = 0; i < WIDTH; i++)
        image[i].resize(HEIGHT);
    edges.resize(WIDTH);
    for (int i = 0; i < WIDTH; i++)
        edges[i].resize(HEIGHT);
    strong.resize(WIDTH);
    for (int i = 0; i < WIDTH; i++)
        strong[i].resize(HEIGHT);
    weak.resize(WIDTH);
    for (int i = 0; i < WIDTH; i++)
        weak[i].resize(HEIGHT);
    gradient.resize(WIDTH);
    for (int i = 0; i < WIDTH; i++)
        gradient[i].resize(HEIGHT);
    angle.resize(WIDTH);
    for (int i = 0; i < WIDTH; i++)
        angle[i].resize(HEIGHT);
    originalImage.resize(WIDTH);
    for (int i = 0; i < WIDTH; i++)
        originalImage[i].resize(HEIGHT);
    votes.resize(WIDTH);
    for (int i = 0; i < WIDTH; i++)
        votes[i].resize(HEIGHT);
    circles.resize(WIDTH);
    for (int i = 0; i < WIDTH; i++)
        circles[i].resize(HEIGHT);
    radii.resize(WIDTH);
    for (int i = 0; i < WIDTH; i++)
        radii[i].resize(HEIGHT);
    for (int i = 0; i < WIDTH; i++)
        for (int j = 0; j < HEIGHT; j++)
            votes[i][j] = 0;

    if (type == "P3")
    {
        int red;
        int green;
        int blue;
        for (int y = 0; y < HEIGHT; y++)
        {
            for (int x = 0; x < WIDTH; x++)
            {
                infile >> red;
                infile >> green;
                infile >> blue;
                if (red + green + blue > 500)
                    image[x][y] = (red + green + blue) / 3; //intensity
                originalImage[x][y] = { red, green, blue };
            }
        }
    }
    infile.close();
}

int filter[] = { 2, 4,  5,  4,  2, 4, 9,  12, 9,  4, 5, 12, 15, 12, 5, 4, 9,  12, 9,  4, 2, 4,  5,  4,  2 };

void sobel()	//Sobel
{
    double gradMax = 0;
    for (int i = 1; i < WIDTH - 1; i++)
    {
        for (int j = 1; j < HEIGHT - 1; j++)
        {
            double a = -1 * image[i - 1][j - 1];
            double b = 1 * image[i + 1][j - 1];
            double c = -2 * image[i - 1][j];
            double d = 2 * image[i + 1][j];
            double e = -1 * image[i - 1][j + 1];
            double f = 1 * image[i + 1][j + 1];
            double avgX = (a + b + c + d + e + f) / 6;

            double g = -1 * image[i - 1][j - 1];
            double h = -1 * image[i + 1][j - 1];
            double k = -2 * image[i][j - 1];
            double l = 2 * image[i][j + 1];
            double m = 1 * image[i - 1][j + 1];
            double n = 1 * image[i + 1][j + 1];
            double avgY = (g + h + k + l + m + n) / 6;

            gradient[i][j] = sqrt((avgX * avgX) + (avgY * avgY));
            if (gradient[i][j] > gradMax)
                gradMax = gradient[i][j];
            angle[i][j] = atan2(avgY, avgX) * 180 / PI;
        }
    }
}

void suppression()   //Non maximum suppression
{
    for (int i = 1; i < WIDTH - 1; i++)
    {
        for (int j = 1; j < HEIGHT - 1; j++)
        {
            if (edges[i][j] == 1)
            {
                int theta = angle[i][j];
                if (theta <= 22.5 || theta >= 157.5)
                    theta = 0;
                else if (theta > 22.5 && theta <= 67.5)
                    theta = 45;
                else if (theta > 67.5 && theta <= 112.5)
                    theta = 90;
                else if (theta > 112.5 && theta <= 157.5)
                    theta = 135;

                if (theta == 0)
                {
                    double m1 = gradient[i][j - 1];
                    double m2 = gradient[i][j + 1];
                    double m = gradient[i][j];
                    int max = 2;

                    if (m1 > m)
                    {
                        max = 1;
                        m = m1;
                    }
                    if (m2 > m)
                        max = 3;

                    if (max == 1)
                    {
                        edges[i][j - 1] = 1;
                        if (strong[i][j] != 1)
                            edges[i][j] = 0;
                        if (strong[i][j + 1] != 1)
                            edges[i][j + 1] = 0;
                    }
                    else if (max == 2)
                    {
                        edges[i][j - 1] = 0;
                        edges[i][j] = 1;
                        edges[i][j + 1] = 0;
                    }
                    else
                    {
                        edges[i][j - 1] = 0;
                        edges[i][j] = 0;
                        edges[i][j + 1] = 1;
                    }
                }

                else if (theta == 45)
                {
                    double m1 = gradient[i - 1][j - 1];
                    double m2 = gradient[i + 1][j + 1];
                    double m = gradient[i][j];
                    int max = 2;
                    if (m1 > m)
                    {
                        max = 1;
                        m = m1;
                    }
                    if (m2 > m)
                        max = 3;

                    if (max == 1)
                    {
                        edges[i - 1][j - 1] = 1;
                        edges[i][j] = 0;
                        edges[i + 1][j + 1] = 0;
                    }
                    else if (max == 2)
                    {
                        edges[i - 1][j - 1] = 0;
                        edges[i][j] = 1;
                        edges[i + 1][j + 1] = 0;
                    }
                    else
                    {
                        edges[i - 1][j - 1] = 0;
                        edges[i][j] = 0;
                        edges[i + 1][j + 1] = 1;
                    }
                }
                else if (theta == 90)
                {
                    double m1 = gradient[i - 1][j];
                    double m2 = gradient[i + 1][j];
                    double m = gradient[i][j];
                    int max = 2;

                    if (m1 > m)
                    {
                        max = 1;
                        m = m1;
                    }
                    if (m2 > m)
                        max = 3;

                    if (max == 1)
                    {
                        edges[i - 1][j] = 1;
                        edges[i][j] = 0;
                        edges[i + 1][j] = 0;
                    }
                    else if (max == 2)
                    {
                        edges[i - 1][j] = 0;
                        edges[i][j] = 1;
                        edges[i + 1][j] = 0;
                    }
                    else
                    {
                        edges[i - 1][j] = 0;
                        edges[i][j] = 0;
                        edges[i + 1][j] = 1;
                    }
                }
                else
                {
                    double m1 = gradient[i - 1][j + 1];
                    double m2 = gradient[i + 1][j - 1];
                    double m = gradient[i][j];
                    int max = 2;
                    if (m1 > m)
                    {
                        max = 1;
                        m = m1;
                    }
                    if (m2 > m)
                        max = 3;

                    if (max == 1)
                    {
                        edges[i - 1][j + 1] = 1;
                        edges[i][j] = 0;
                        edges[i + 1][j - 1] = 0;
                    }
                    else if (max == 2)
                    {
                        edges[i - 1][j + 1] = 0;
                        edges[i][j] = 1;
                        edges[i + 1][j - 1] = 0;
                    }
                    else
                    {
                        edges[i - 1][j + 1] = 0;
                        edges[i][j] = 0;
                        edges[i + 1][j - 1] = 1;
                    }
                }
            }
        }
    }
}

void threshold()    //double threshold
{
    double maximum = gradient[0][0];
    for (int i = 1; i < WIDTH - 1; i++)
    {
        for (int j = 1; j < HEIGHT - 1; j++)
        {
            if (gradient[i][j] > maximum)
                maximum = gradient[i][j];
        }
    }

    double maxTest = maxThreshold * maximum;
    double minTest = minThreshold * maximum;

    for (int i = 1; i < WIDTH - 1; i++)
    {
        for (int j = 1; j < HEIGHT - 1; j++)
        {
            if (gradient[i][j] > maxTest)
            {
                edges[i][j] = 1;
                strong[i][j] = 1;
            }
            else if (gradient[i][j] > minTest)
            {
                edges[i][j] = 1;
                weak[i][j] = 1;
            }
            else
            {
                edges[i][j] = 0;
            }
        }
    }
}

void hysteresisHelper(int x, int y)	//Helper method for recursive hysteresis
{
    if (x < WIDTH - 1 && x >= 1 && y < HEIGHT - 1 && y >= 1)
    {
        if (edges[x][y] == 1 || edges[x][y] == 2)
        {
            edges[x][y] = 3;
            hysteresisHelper(x + 1, y);
            hysteresisHelper(x - 1, y);
            hysteresisHelper(x, y + 1);
            hysteresisHelper(x, y - 1);
            hysteresisHelper(x + 1, y + 1);
            hysteresisHelper(x - 1, y + 1);
            hysteresisHelper(x + 1, y - 1);
            hysteresisHelper(x - 1, y - 1);
        }
        else
            return;
    }
    else
        return;
}

void hysteresis()	//hysteresis
{
    int count = 0;
    int strongCount = 0;
    for (int i = 1; i < WIDTH - 1; i++)
    {
        for (int j = 1; j < HEIGHT - 1; j++)
        {
            if (edges[i][j] == 2)
                hysteresisHelper(i, j);
        }
    }
}

int main()
{
    input("image.ppm");

    sobel();
    suppression();
    threshold();
    hysteresis();

    ofstream output("imagef.ppm");
    output << "P3 " << WIDTH << " " << HEIGHT << " 1\n";
    for (int j = 0; j < HEIGHT; j++)
    {
        for (int i = 0; i < WIDTH; i++)
        {
            if (edges[i][j] > 0)
                output << 1 << " " << 1 << " " << 1 << " ";
            else
                output << 0 << " " << 0 << " " << 0 << " ";
        }
        output << endl;
    }
    output.close();

    return 0;
}