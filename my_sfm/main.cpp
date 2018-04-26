#include "sfm_h.h"

//#include <iostream>
//#include <opencv2/opencv.hpp>  //头文件
//#include <opencv2/xfeatures2d.hpp>
//using namespace cv;  //包含cv命名空间
//using namespace std;

int main()
{
	////Create SIFT class pointer
	//Ptr<Feature2D> f2d = xfeatures2d::SURF::create();
	//cvNamedWindow("【match图】");
	////读入图片
	//cout << "a";
	//Mat img_1_raw = imread("D:\\v1.jpg");
	//Mat img_2_raw = imread("D:\\V2.jpg");
	//Mat img_1;
	//Mat img_2;
	//resize(img_1_raw, img_1, Size(), 0.2, 0.2);
	//resize(img_2_raw, img_2, Size(), 0.2, 0.2);

	//cout << "b";
	////Detect the keypoints
	//vector<KeyPoint> keypoints_1, keypoints_2;
	//f2d->detect(img_1, keypoints_1);
	//f2d->detect(img_2, keypoints_2);
	//cout << "c";
	////Calculate descriptors (feature vectors)
	//Mat descriptors_1, descriptors_2;
	//f2d->compute(img_1, keypoints_1, descriptors_1);
	//f2d->compute(img_2, keypoints_2, descriptors_2);
	////Matching descriptor vector using BFMatcher
	//cout << "d";
	//BFMatcher matcher;
	//vector<DMatch> matches;
	//matcher.match(descriptors_1, descriptors_2, matches);
	////绘制匹配出的关键点
	//Mat img_matches;
	//drawMatches(img_1, keypoints_1, img_2, keypoints_2, matches, img_matches);
	//imshow("【match图】", img_matches);
	////等待任意按键按下
	//waitKey(0);


	cp_filename = " ";

	namedWindow("【match图】", 0);
	// 读入图像
	Mat img_1_raw = imread("D:\\v1.jpg");
	Mat img_2_raw = imread("D:\\V2.jpg");
	Mat img_1;
	Mat img_2;
	vector<Vec3b> colors1;
	vector<Vec3b> colors2;
	// 压缩一下，提高速度，sift太慢
	resize(img_1_raw, img_1, Size(), 0.4, 0.4);
	resize(img_2_raw, img_2, Size(), 0.4, 0.4);
	// 检测特征点	
	vector<KeyPoint> keypoints_1, keypoints_2;
	Mat descriptors_1, descriptors_2;
	extract_features(img_1, keypoints_1, descriptors_1, colors1);
	extract_features(img_2, keypoints_2, descriptors_2, colors2);
	// 匹配特征点
	vector<DMatch> matches;	// 存储匹配到的点对
	match_features(descriptors_1, descriptors_2, matches);



	//绘制匹配出的关键点
	Mat img_matches;
	drawMatches(img_1, keypoints_1, img_2, keypoints_2, matches, img_matches);
	imshow("【match图】", img_matches);
	//等待任意按键按下
	waitKey(0);


}