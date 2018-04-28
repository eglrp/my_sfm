#include "sfm_h.h"

//#include <iostream>
//#include <opencv2/opencv.hpp>  //ͷ�ļ�
//#include <opencv2/xfeatures2d.hpp>
//using namespace cv;  //����cv�����ռ�
//using namespace std;

char *save_filename = "D:\\save.txt";
char *read_filename = "D:\\save.txt";

int main()
{
	////Create SIFT class pointer
	//Ptr<Feature2D> f2d = xfeatures2d::SURF::create();
	//cvNamedWindow("��matchͼ��");
	////����ͼƬ
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
	////����ƥ����Ĺؼ���
	//Mat img_matches;
	//drawMatches(img_1, keypoints_1, img_2, keypoints_2, matches, img_matches);
	//imshow("��matchͼ��", img_matches);
	////�ȴ����ⰴ������
	//waitKey(0);



	// TODO ��coutɾ��


	/*�ڲξ���				
			fx    s    x0
		K = 0    fy   y0
			0    0    1
	*/
	Mat K(Matx33d(
		2759.48, 0, 1520.69,
		0, 2764.16, 1006.81,
		0, 0, 1));

	namedWindow("��matchͼ��", 0);

	// ����ͼ��
	cout << "loading..." << endl;
	/*Mat img_1_raw = imread("D:\\v1.jpg");
	Mat img_2_raw = imread("D:\\V2.jpg");
	Mat img_1;
	Mat img_2;*/
	Mat img_1 = imread("D:\\0004.png");
	Mat img_2 = imread("D:\\0006.png");
	vector<Vec3b> colors1;
	vector<Vec3b> colors2;
	

	//// ѹ��һ�£�����ٶȣ�sift̫��
	//resize(img_1_raw, img_1, Size(), 0.4, 0.4);
	//resize(img_2_raw, img_2, Size(), 0.4, 0.4);

	// ���������	
	cout << "detect" << endl;
	vector<KeyPoint> keypoints_1, keypoints_2;
	Mat descriptors_1, descriptors_2;
	extract_features(img_1, keypoints_1, descriptors_1, colors1);
	extract_features(img_2, keypoints_2, descriptors_2, colors2);

	// ƥ��������
	cout << "match" << endl;
	vector<DMatch> matches;	// �洢ƥ�䵽�ĵ��
	match_features(descriptors_1, descriptors_2, matches);

	// ����任����R��T��
	cout << "calc R T" << endl;
	vector<Point2f> p1, p2;
	vector<Vec3b> matched_colors;
	Mat R, T;	//��ת�����ƽ������
	Mat mask;	//mask�д�����ĵ����ƥ��㣬���������ʧ���
	get_matched_points(keypoints_1, keypoints_2, matches, p1, p2);
	get_matched_colors(colors1, colors2, matches, matched_colors);
	get_RT(K, p1, p2, R, T, mask);

	// ��ά�ؽ�
	cout << "r" << endl;
	Mat cps;	//4��N�еľ���ÿһ�д���ռ��е�һ���㣨������꣩
	maskout_points(p1, mask);
	maskout_points(p2, mask);
	reconstruct(K, R, T, p1, p2, cps);

	// ����õ��ĵ�������
	cout << "save" << endl;
	save_cps(save_filename, cps, matched_colors);

	//����ƥ����Ĺؼ���
	Mat img_matches;
	drawMatches(img_1, keypoints_1, img_2, keypoints_2, matches, img_matches);
	imshow("��matchͼ��", img_matches);
	//�ȴ����ⰴ������
	waitKey(0);


}