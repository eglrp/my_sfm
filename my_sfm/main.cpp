#include "sfm_h.h"

//#include <iostream>
//#include <opencv2/opencv.hpp>  //头文件
//#include <opencv2/xfeatures2d.hpp>
//using namespace cv;  //包含cv命名空间
//using namespace std;

char *save_filename = "D:\\save.txt";
char *read_filename = "D:\\save.txt";

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



	// TODO 把cout删掉


	/*内参矩阵				
			fx    s    x0
		K = 0    fy   y0
			0    0    1
	*/
	Mat K(Matx33d(
		2759.48, 0, 1520.69,
		0, 2764.16, 1006.81,
		0, 0, 1));

	namedWindow("【match图】", 0);

	// 读入图像
	cout << "loading..." << endl;
	/*Mat img_1_raw = imread("D:\\v1.jpg");
	Mat img_2_raw = imread("D:\\V2.jpg");
	Mat img_1;
	Mat img_2;*/
	Mat img_1 = imread("D:\\0004.png");
	Mat img_2 = imread("D:\\0006.png");
	vector<Vec3b> colors1;
	vector<Vec3b> colors2;
	

	//// 压缩一下，提高速度，sift太慢
	//resize(img_1_raw, img_1, Size(), 0.4, 0.4);
	//resize(img_2_raw, img_2, Size(), 0.4, 0.4);

	// 检测特征点	
	cout << "detect" << endl;
	vector<KeyPoint> keypoints_1, keypoints_2;
	Mat descriptors_1, descriptors_2;
	extract_features(img_1, keypoints_1, descriptors_1, colors1);
	extract_features(img_2, keypoints_2, descriptors_2, colors2);

	// 匹配特征点
	cout << "match" << endl;
	vector<DMatch> matches;	// 存储匹配到的点对
	match_features(descriptors_1, descriptors_2, matches);

	// 计算变换矩阵（R和T）
	cout << "calc R T" << endl;
	vector<Point2f> p1, p2;
	vector<Vec3b> matched_colors;
	Mat R, T;	//旋转矩阵和平移向量
	Mat mask;	//mask中大于零的点代表匹配点，等于零代表失配点
	get_matched_points(keypoints_1, keypoints_2, matches, p1, p2);
	get_matched_colors(colors1, colors2, matches, matched_colors);
	get_RT(K, p1, p2, R, T, mask);

	// 三维重建
	cout << "r" << endl;
	Mat cps;	//4行N列的矩阵，每一列代表空间中的一个点（齐次坐标）
	maskout_points(p1, mask);
	maskout_points(p2, mask);
	reconstruct(K, R, T, p1, p2, cps);

	// 保存得到的点云数据
	cout << "save" << endl;
	save_cps(save_filename, cps, matched_colors);

	//绘制匹配出的关键点
	Mat img_matches;
	drawMatches(img_1, keypoints_1, img_2, keypoints_2, matches, img_matches);
	imshow("【match图】", img_matches);
	//等待任意按键按下
	waitKey(0);


}