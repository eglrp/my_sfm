//#ifndef HHH
//#define HHH
//
//#include <iostream>
//#include <stdio.h>
//#include <math.h>
//// 文件读写
//#include <fstream>
//// opencv相关头文件
//#include <opencv2/core/core.hpp>
//#include <opencv2/opencv.hpp>  
//#include <opencv2/xfeatures2d.hpp>
//#include <opencv2\xfeatures2d\nonfree.hpp>
//#include <opencv2\features2d\features2d.hpp>
//#include <opencv2\highgui\highgui.hpp>
//#include <opencv2\calib3d\calib3d.hpp>
//
//// opengl相关头文件
//#include <gl/glut.h>
//
//// 解决error C3861: “_T”:  找不到标识符
//#include <tchar.h>
//
//using namespace cv;  //包含cv命名空间
//using namespace std;
//
//#ifndef max
//#define max(a,b)            (((a) > (b)) ? (a) : (b))
//#endif
//
//#ifndef min
//#define min(a,b)            (((a) < (b)) ? (a) : (b))
//#endif
//
//void extract_features(Mat &img, vector<KeyPoint> &key_points, Mat &descriptors, vector<Vec3b> &colors);
//void match_features(Mat& query, Mat& train, vector<DMatch>& matches);
//bool get_RT(Mat K, vector<Point2f>& p1, vector<Point2f>& p2, Mat &R, Mat &T, Mat &mask);
//void get_matched_colors(vector<Vec3b>& c1, vector<Vec3b>& c2, vector<DMatch> matches, vector<Vec3b>& matched_colors);
//void get_matched_points(vector<KeyPoint> keypoints_1, vector<KeyPoint>keypoints_2, vector<DMatch> matches, vector<Point2f> &p1, vector<Point2f> &p2);
//void maskout_points(vector<Point2f>& p1, Mat& mask);
//void reconstruct(Mat& K, Mat& R, Mat& T, vector<Point2f>& p1, vector<Point2f>& p2, Mat& structure);
//bool save_cps(const char *filename, Mat cps, vector<Vec3b> colors);
//
//// opengl相关显示函数
//void show_cps(const char *cp_filename);
//void gl_main(int argc, char *argv[]);
//
//
//
//
//
//
//
//
//void TriSubDiv(vector<Point2f> &pts, Mat &img, vector<cv::Vec6f> &tri);
//void MapTexTri(Mat & texImg, Point2f pt2D, Point3f pt3D);
//
//GLuint Create3DTexture(Mat &img, vector<Vec6f> &tri,
//	vector<Point2f> pts2DTex, vector<Point3f> &pts3D,
//	Point3f center3D, Vec3f size3D);
//
//
//
///* OpenGL functions*/
//void InitGl();
//
//void Init_lightGl();
//
//void displayGl();
//
//void resizeGl(int w, int h);
//
//void mouseGl(int button, int state, int x, int y);
//
//void mouse_move_Gl(int x, int y);
//
//void keyboard_control_Gl(unsigned char key, int a, int b);
//
//void special_control_Gl(int key, int x, int y);
//
//void Show(GLuint tex, Point3f center3D, Vec3i size3D);
//
//
//
//
//
//
//
//
//
//
//extern char *save_filename;
//extern char *read_filename;
//
//#endif