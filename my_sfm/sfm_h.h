#ifndef HHH
#define HHH

#include <iostream>
#include <stdio.h>
#include <math.h>
// �ļ���д
#include <fstream>
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

void extract_features(Mat &img, vector<KeyPoint> &key_points, Mat &descriptors, vector<Vec3b> &colors);
void match_features(Mat& query, Mat& train, vector<DMatch>& matches);
bool get_RT(Mat K, vector<Point2f>& p1, vector<Point2f>& p2, Mat &R, Mat &T, Mat &mask);
void get_matched_colors(vector<Vec3b>& c1, vector<Vec3b>& c2, vector<DMatch> matches, vector<Vec3b>& matched_colors);
void get_matched_points(vector<KeyPoint> keypoints_1, vector<KeyPoint>keypoints_2, vector<DMatch> matches, vector<Point2f> &p1, vector<Point2f> &p2);
void maskout_points(vector<Point2f>& p1, Mat& mask);
void reconstruct(Mat& K, Mat& R, Mat& T, vector<Point2f>& p1, vector<Point2f>& p2, Mat& structure);
bool save_cps(const char *filename, Mat cps, vector<Vec3b> colors);

// opengl�����ʾ����
void show_cps(const char *cp_filename);

extern char *save_filename;
extern char *read_filename;

#endif