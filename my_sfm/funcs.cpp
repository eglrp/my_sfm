#include "sfm_h.h"

/*
*	�ú���������ȡ������,������������Ϣ����������Ϣ���Լ���ɫ��Ϣ
*/ 
void extract_features(Mat &img, vector<KeyPoint> &key_points, Mat &descriptors, vector<Vec3b> colors) {

	Ptr<Feature2D> sift = xfeatures2d::SIFT::create();	// ����ֱ�Ӱ�SIFT����SURF��

	// ��ȡ�����㲢������������
	sift->detectAndCompute(img, noArray(), key_points, descriptors);

	
	// ��ɫ
	for (int i = 0; i < key_points.size(); ++i)
	{
		Point2f& p = key_points[i].pt;
		colors.push_back(img.at<Vec3b>(p.y, p.x));
		/* Note the ordering of x and y. 
		Since in OpenCV images are represented by the same structure as matrices, 
		we use the same convention for both cases - 
		the 0-based row index (or y-coordinate) goes first and the 0-based column index (or x-coordinate) follows it. 
		Alternatively, you can use the following notation: Scalar intensity = img.at<uchar>(Point(x, y));
		*/
	}

}

/*
*	�ú������ڽ����������ƥ��
*	DMatch��Ҫ��������ƥ����Ϣ�Ľṹ�壬
*	query��Ҫƥ��������ӣ�train�Ǳ�ƥ���������
*/
void match_features(Mat& query, Mat& train, vector<DMatch>& matches) {
	vector<vector<DMatch>> knn_matches;
	BFMatcher matcher(NORM_L2);
	matcher.knnMatch(query, train, knn_matches, 2);  // kȡ2�������������ƥ���Ȼ��radio test

	//��ȡ����Ratio Test����Сƥ��ľ���
	float min_dist = FLT_MAX;
	for (int r = 0; r < knn_matches.size(); ++r)
	{
		//Ratio Test
		if (knn_matches[r][0].distance > 0.6*knn_matches[r][1].distance)
			continue;

		float dist = knn_matches[r][0].distance;
		if (dist < min_dist) min_dist = dist;
	}

	matches.clear();
	for (size_t r = 0; r < knn_matches.size(); ++r)
	{
		//�ų�������Ratio Test�ĵ��ƥ��������ĵ�
		if (
			knn_matches[r][0].distance > 0.6*knn_matches[r][1].distance ||
			knn_matches[r][0].distance > 5 * max(min_dist, 10.0f)
			)
			continue;

		//����ƥ���
		matches.push_back(knn_matches[r][0]);
	}
}

/*
*	�ú������ڼ������������R��T��R, T �ֱ���������ͷ�����������ͷ����ת����3*3����ƽ��������3*1��
*				fx    s    x0
    �ڲξ���K =  0    fy   y0
				 0    0    1
*	mask�д�����ĵ����ƥ��㣬���������ʧ���
*/
bool get_RT(Mat K, vector<Point2f>& p1, vector<Point2f>& p2, Mat &R, Mat &T, Mat &mask) {
	double focal_length = 0.5*(K.at<double>(0) + K.at<double>(4));		// (fx+fy)/2,����
	Point2d center_point(K.at<double>(2), K.at<double>(5));				// ��������

	//����ƥ�����ȡ��������ʹ��RANSAC����һ���ų�ʧ���
	Mat e_Mat = findEssentialMat(p1, p2, focal_length, center_point, RANSAC, 0.999, 1.0);
	if (e_Mat.empty()) {
		return false;
	}

	//double feasible_count = countNonZero(mask);
	//cout << (int)feasible_count << " -in- " << p1.size() << endl;
	////����RANSAC���ԣ�outlier��������50%ʱ������ǲ��ɿ���
	//if (feasible_count <= 15 || (feasible_count / p1.size()) < 0.6)
	//	return false;

	//�ֽⱾ�����󣬻�ȡ��Ա任
	int pass_count = recoverPose(e_Mat, p1, p2, R, T, focal_length, center_point, mask);

	////ͬʱλ���������ǰ���ĵ������Ҫ�㹻��
	//if (((double)pass_count) / feasible_count < 0.7)
	//	return false;

	return true;
}



/*
*	�ú���������ά�ؽ����ؽ���ʽΪ���ǻ�
*	OpenCV�ṩ�˸÷���������ֱ��ʹ��
*/
void reconstruct(Mat& K, Mat& R, Mat& T, vector<Point2f>& p1, vector<Point2f>& p2, Mat& structure)
{
	//���������ͶӰ����[R T]��triangulatePointsֻ֧��float��
	Mat proj1(3, 4, CV_32FC1);
	Mat proj2(3, 4, CV_32FC1);

	proj1(Range(0, 3), Range(0, 3)) = Mat::eye(3, 3, CV_32FC1); // 3��3�жԽǾ���
	proj1.col(3) = Mat::zeros(3, 1, CV_32FC1);	// �����

	R.convertTo(proj2(Range(0, 3), Range(0, 3)), CV_32FC1);
	T.convertTo(proj2.col(3), CV_32FC1);

	// ת���ڲξ���Ϊfloat��
	Mat fK;
	K.convertTo(fK, CV_32FC1);	
	proj1 = fK*proj1;
	proj2 = fK*proj2;

	//���ǻ��ؽ�
	triangulatePoints(proj1, proj2, p1, p2, structure);
}