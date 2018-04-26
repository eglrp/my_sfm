#include <iostream>
#include<stdio.h>
#include<math.h>
// opencv���ͷ�ļ�
#include <opencv2/opencv.hpp>  
#include <opencv2/xfeatures2d.hpp>
#include <opencv2\xfeatures2d\nonfree.hpp>
#include <opencv2\features2d\features2d.hpp>
#include <opencv2\highgui\highgui.hpp>
#include <opencv2\calib3d\calib3d.hpp>
// opengl���ͷ�ļ�
#include <gl/glut.h>

using namespace cv;  //����cv�����ռ�
using namespace std;

void extract_features(Mat &img, vector<KeyPoint> &key_points, Mat &descriptors, vector<Vec3b> colors);
void match_features(Mat& query, Mat& train, vector<DMatch>& matches);
bool get_RT(Mat K, vector<Point2f>& p1, vector<Point2f>& p2, Mat &R, Mat &T, Mat &mask);
void reconstruct(Mat& K, Mat& R, Mat& T, vector<Point2f>& p1, vector<Point2f>& p2, Mat& structure);

// opengl�����ʾ����
void show_cps(const char *cp_filename);

extern char *cp_filename;