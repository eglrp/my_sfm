#ifndef HHH
#define HHH

#include <io.h>
#include <iostream>
#include <stdio.h>
#include <math.h>
// 文件读写
#include <fstream>
// opencv相关头文件
#include <opencv2/core/core.hpp>
#include <opencv2/opencv.hpp>  
#include <opencv2/xfeatures2d.hpp>
#include <opencv2\xfeatures2d\nonfree.hpp>
#include <opencv2\features2d\features2d.hpp>
#include <opencv2\highgui\highgui.hpp>
#include <opencv2\calib3d\calib3d.hpp>

// opengl相关头文件
#include <gl/glut.h>

// 解决error C3861: “_T”:  找不到标识符
#include <tchar.h>

using namespace cv;  //包含cv命名空间
using namespace std;

#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif


void init_construct(
	Mat K, vector<vector<KeyPoint>>& key_points_for_all,
	vector<vector<Vec3b>>& colors_for_all, vector<vector<DMatch>>& matches_for_all,
	vector<Point3f>& cps, vector<vector<int>>& correspond_struct_idx,
	vector<Vec3b>& matched_colors, vector<Mat>& rotations, vector<Mat>& motions
	);
void get_img_names(string dir_name, vector<string> & names);
void extract_features(vector<string>& image_names,
	vector<vector<KeyPoint>>& key_points_for_all,
	vector<Mat>& descriptor_for_all,
	vector<vector<Vec3b>>& colors_for_all
	);
void match_features(Mat& query, Mat& train, vector<DMatch>& matches);
void match_features(vector<Mat>& descriptor_for_all, vector<vector<DMatch>>& matches_for_all);
bool get_RT(Mat K, vector<Point2f>& p1, vector<Point2f>& p2, Mat &R, Mat &T, Mat &mask);
void get_matched_colors(vector<Vec3b>& c1, vector<Vec3b>& c2, vector<DMatch> matches, vector<Vec3b>& matched_colors);
void get_matched_points(vector<KeyPoint> keypoints_1, 
	vector<KeyPoint>keypoints_2, 
	vector<DMatch> matches, 
	vector<Point2f> &p1, 
	vector<Point2f> &p2
	);
void get_objpoints_and_imgpoints(
	vector<DMatch>& matches,
	vector<int>& correspond,
	vector<Point3f>& cps,
	vector<KeyPoint>& key_points,
	vector<Point3f>& object_points,
	vector<Point2f>& image_points
	);
void maskout_points(vector<Point2f>& p1, Mat& mask);
void maskout_colors(vector<Vec3b>& p1, Mat& mask);
void reconstruct(Mat& K, Mat& R1, Mat& T1, Mat& R2, Mat& T2, vector<Point2f>& p1, vector<Point2f>& p2, vector<Point3f>& cps);
void add_cps(
	vector<DMatch>& matches,
	vector<int>& correspond,
	vector<int>& next_correspond,
	vector<Point3f>& cps,
	vector<Point3f>& next_cps,
	vector<Vec3b>& colors,
	vector<Vec3b>& next_colors
	);
bool save_cps(const char *filename, vector<Point3f> cps, vector<Vec3b> colors);

// opengl相关显示函数
void show_cps(const char *cp_filename);
void gl_main(int argc, char *argv[]);








void TriSubDiv(vector<Point2f> &pts, Mat &img, vector<cv::Vec6f> &tri);
void MapTexTri(Mat & texImg, Point2f pt2D, Point3f pt3D);

GLuint Create3DTexture(Mat &img, vector<Vec6f> &tri,
	vector<Point2f> pts2DTex, vector<Point3f> &pts3D,
	Point3f center3D, Vec3f size3D);



/* OpenGL functions*/
void InitGl();

void Init_lightGl();

void displayGl();

void resizeGl(int w, int h);

void mouseGl(int button, int state, int x, int y);

void mouse_move_Gl(int x, int y);

void keyboard_control_Gl(unsigned char key, int a, int b);

void special_control_Gl(int key, int x, int y);

void Show(GLuint tex, Point3f center3D, Vec3i size3D);










extern char *save_filename;
extern char *read_filename;

#endif