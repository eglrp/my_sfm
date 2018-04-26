#include "sfm_h.h"

//#include <iostream>
//#include <opencv2/opencv.hpp>  //ͷ�ļ�
//#include <opencv2/xfeatures2d.hpp>
//using namespace cv;  //����cv�����ռ�
//using namespace std;

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


	cp_filename = " ";

	namedWindow("��matchͼ��", 0);
	// ����ͼ��
	Mat img_1_raw = imread("D:\\v1.jpg");
	Mat img_2_raw = imread("D:\\V2.jpg");
	Mat img_1;
	Mat img_2;
	vector<Vec3b> colors1;
	vector<Vec3b> colors2;
	// ѹ��һ�£�����ٶȣ�sift̫��
	resize(img_1_raw, img_1, Size(), 0.4, 0.4);
	resize(img_2_raw, img_2, Size(), 0.4, 0.4);
	// ���������	
	vector<KeyPoint> keypoints_1, keypoints_2;
	Mat descriptors_1, descriptors_2;
	extract_features(img_1, keypoints_1, descriptors_1, colors1);
	extract_features(img_2, keypoints_2, descriptors_2, colors2);
	// ƥ��������
	vector<DMatch> matches;	// �洢ƥ�䵽�ĵ��
	match_features(descriptors_1, descriptors_2, matches);



	//����ƥ����Ĺؼ���
	Mat img_matches;
	drawMatches(img_1, keypoints_1, img_2, keypoints_2, matches, img_matches);
	imshow("��matchͼ��", img_matches);
	//�ȴ����ⰴ������
	waitKey(0);


}