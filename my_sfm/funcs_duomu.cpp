#include "sfm_h_duomu.h"

/*
*	�ú������ڻ�ȡĿ¼�µ�ͼƬ���ƣ���ŵ�vector�б���
*/
void get_img_names(string dir_name, vector<string> & names)
{
	names.clear();
	// �ļ����
	
	intptr_t hFile = 0;
	// �ļ���Ϣ
	struct _finddata_t fileinfo;
	string p;
	if ((hFile = _findfirst(p.assign(dir_name).append("\\*").c_str(), &fileinfo)) != -1) {
		do {
			if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0) {
				// string�������˼��һ����ֵ����:assign()
				names.push_back(p.assign(dir_name).append("\\").append(fileinfo.name));
			}
		} while (_findnext(hFile, &fileinfo) == 0);  //Ѱ����һ�����ɹ�����0������-1
		/*��win10�У�ʹ���ļ���������_findnext�ᱨ0xC0000005���� 
			ԭ�� 
			_findnext()��һ��������·������������ص�����Ϊintptr_t��long long����
			�������Ϊlong����win7����û�����⣬
			������win10�о�Ҫ��Ϊlong long����intptr_t
		*/
		_findclose(hFile);
	}
}

/*
*	�ú���������ȡ������,������������Ϣ����������Ϣ���Լ���ɫ��Ϣ
*/
void extract_features(vector<Mat>& images,
	vector<vector<KeyPoint>>& key_points_for_all,
	vector<Mat>& descriptor_for_all,
	vector<vector<Vec3b>>& colors_for_all) {

	Ptr<Feature2D> sift = xfeatures2d::SIFT::create();	// ����ֱ�Ӱ�SIFT����SURF��
	Mat img;
	key_points_for_all.clear();
	descriptor_for_all.clear();
	int img_no = 0;
	for (auto it = images.begin(); it != images.end(); ++it)
	{
		img = *it;

		cout << "Extracing features from img-" << ++img_no << endl;

		// ��ȡ�����㲢������������
		vector<KeyPoint> key_points;
		Mat descriptor;
		sift->detectAndCompute(img, noArray(), key_points, descriptor);

		key_points_for_all.push_back(key_points);
		descriptor_for_all.push_back(descriptor);

		// ��ɫ
		vector<Vec3b> colors(key_points.size());
		for (int i = 0; i < key_points.size(); ++i)
		{
			Point2f& p = key_points[i].pt;
			colors[i] = img.at<Vec3b>(p.y, p.x);
			/* Note the ordering of x and y.
			Since in OpenCV images are represented by the same structure as matrices,
			we use the same convention for both cases -
			the 0-based row index (or y-coordinate) goes first and the 0-based column index (or x-coordinate) follows it.
			Alternatively, you can use the following notation: Scalar intensity = img.at<uchar>(Point(x, y));
			*/
		}
		colors_for_all.push_back(colors);
	}
}

/*
*	�ú������ڽ��������������ƥ��
*/
void match_features(vector<Mat>& descriptor_for_all, vector<vector<DMatch>>& matches_for_all) {
	matches_for_all.clear();
	// n��ͼ������˳���� n-1 ��ƥ��
	// 1��2ƥ�䣬2��3ƥ�䣬3��4ƥ�䣬�Դ�����
	for (int i = 0; i < descriptor_for_all.size() - 1; ++i)
	{
		cout << "Matching images " << i << " - " << i + 1 << endl;
		vector<DMatch> matches;
		match_features(descriptor_for_all[i], descriptor_for_all[i + 1], matches);
		matches_for_all.push_back(matches);
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
*	�ú������ڻ�ȡ��i��ͼ����ƥ����Ӧ����ά�㣬�Լ��ڵ�i+1��ͼ���ж�Ӧ�����ص�
*/
void get_objpoints_and_imgpoints(
	vector<DMatch>& matches,
	vector<int>& correspond,
	vector<Point3f>& cps,
	vector<KeyPoint>& key_points,
	vector<Point3f>& object_points,
	vector<Point2f>& image_points)
{
	object_points.clear();
	image_points.clear();

	for (int i = 0; i < matches.size(); ++i)
	{
		int query_idx = matches[i].queryIdx;
		int train_idx = matches[i].trainIdx;

		int cps_idx = correspond[query_idx];
		if (cps_idx < 0) continue;	// ֮ǰcorrespond��resize��ʱ�����ȫ����-1

		object_points.push_back(cps[cps_idx]);
		image_points.push_back(key_points[train_idx].pt);
	}
}





/*
*	��ʼ������
*/
void init_construct(
	Mat K,
	vector<vector<KeyPoint>>& key_points_for_all,
	vector<vector<Vec3b>>& colors_for_all,
	vector<vector<DMatch>>& matches_for_all,
	vector<Point3f>& cps,
	vector<vector<int>>& correspond_cps_idx,
	vector<Vec3b>& matched_colors,
	vector<Mat>& rotations,
	vector<Mat>& motions
	)
{
	//����ͷ����ͼ��֮��ı任����
	vector<Point2f> p1, p2;
	Mat R, T;	//��ת�����ƽ������
	Mat mask;	//mask�д�����ĵ����ƥ��㣬���������ʧ���
	get_matched_points(key_points_for_all[0], key_points_for_all[1], matches_for_all[0], p1, p2);
	get_matched_colors(colors_for_all[0], colors_for_all[1], matches_for_all[0], matched_colors);
	get_RT(K, p1, p2, R, T, mask);

	//��ͷ����ͼ�������ά�ؽ�
	maskout_points(p1, mask);
	maskout_points(p2, mask);
	maskout_colors(matched_colors, mask);


	Mat R0 = Mat::eye(3, 3, CV_64FC1);		// 3��3�жԽǾ���
	Mat T0 = Mat::zeros(3, 1, CV_64FC1);	// �����
	reconstruct(K, R0, T0, R, T, p1, p2, cps);
	//����任����
	rotations = { R0, R };
	motions = { T0, T };

	//��correspond_cps_idx�Ĵ�С��ʼ��Ϊ��key_points_for_all��ȫһ��
	correspond_cps_idx.clear();
	correspond_cps_idx.resize(key_points_for_all.size());
	for (int i = 0; i < key_points_for_all.size(); ++i)
	{
		correspond_cps_idx[i].resize(key_points_for_all[i].size(), -1);	// ȫ������ֵΪ-1��֮��û�ж�Ӧ�ļ��� 
	}

	//��дͷ����ͼ��Ľṹ����
	int idx = 0;
	vector<DMatch>& matches = matches_for_all[0];
	for (int i = 0; i < matches.size(); ++i)
	{
		if (mask.at<uchar>(i) == 0)
			continue;

		correspond_cps_idx[0][matches[i].queryIdx] = idx;
		correspond_cps_idx[1][matches[i].trainIdx] = idx;
		++idx;
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
void reconstruct(Mat& K, Mat& R1, Mat& T1, Mat& R2, Mat& T2, vector<Point2f>& p1, vector<Point2f>& p2, vector<Point3f>& cps)
{
	//���������ͶӰ����[R T]��triangulatePointsֻ֧��float��
	Mat proj1(3, 4, CV_32FC1);
	Mat proj2(3, 4, CV_32FC1);

	R1.convertTo(proj1(Range(0, 3), Range(0, 3)), CV_32FC1);	
	T1.convertTo(proj1.col(3), CV_32FC1);	// �����

	R2.convertTo(proj2(Range(0, 3), Range(0, 3)), CV_32FC1);
	T2.convertTo(proj2.col(3), CV_32FC1);

	// ת���ڲξ���Ϊfloat��
	Mat fK;
	K.convertTo(fK, CV_32FC1);
	proj1 = fK*proj1;
	proj2 = fK*proj2;

	//���ǻ��ؽ�
	Mat s;
	triangulatePoints(proj1, proj2, p1, p2, s);

	cps.clear();
	cps.reserve(s.cols);	// �ı�����������reallocate memory�Ĵ�������߳��������
	for (int i = 0; i < s.cols; ++i)
	{
		Mat_<float> col = s.col(i);
		col /= col(3);	//������꣬��Ҫ�������һ��Ԫ�ز�������������ֵ
		cps.push_back(Point3f(col(0), col(1), col(2)));
	}
}

/*
*	�ú������ڻ�ȡƥ�䵽�ĵ�
*/
void get_matched_points(
	vector<KeyPoint> keypoints_1,
	vector<KeyPoint>keypoints_2,
	vector<DMatch> matches,
	vector<Point2f> &p1,
	vector<Point2f> &p2) {
	// ���
	p1.clear();
	p2.clear();
	for (int i = 0; i < matches.size(); ++i)
	{
		p1.push_back(keypoints_1[matches[i].queryIdx].pt);
		p2.push_back(keypoints_2[matches[i].trainIdx].pt);
	}
}

/*
*	�ú������ڻ�ȡƥ������ɫ
*/
void get_matched_colors(
	vector<Vec3b>& c1,
	vector<Vec3b>& c2,
	vector<DMatch> matches,
	vector<Vec3b>& matched_colors
	)
{
	matched_colors.clear();

	for (int i = 0; i < matches.size(); ++i)
	{
		Vec3b temp;
		//temp = c1[matches[i].queryIdx];
		// ȡ�������ƽ��ֵ
		temp.val[0] = (c1[matches[i].queryIdx].val[0] + c2[matches[i].trainIdx].val[0]) / 2;
		temp.val[1] = (c1[matches[i].queryIdx].val[1] + c2[matches[i].trainIdx].val[1]) / 2;
		temp.val[2] = (c1[matches[i].queryIdx].val[2] + c2[matches[i].trainIdx].val[2]) / 2;
		matched_colors.push_back(temp);
	}
}


/*
*	�ú��������ٴ�ȥ��ʧ���
*/
void maskout_points(vector<Point2f>& p1, Mat& mask)
{
	vector<Point2f> p1_copy = p1;
	p1.clear();

	for (int i = 0; i < mask.rows; ++i)
	{
		if (mask.at<uchar>(i) > 0)
			p1.push_back(p1_copy[i]);
	}
}

/*
*	�ú�������ȥ��ʧ����Ӧ����ɫ
*/
void maskout_colors(vector<Vec3b>& p1, Mat& mask)
{
	vector<Vec3b> p1_copy = p1;
	p1.clear();

	for (int i = 0; i < mask.rows; ++i)
	{
		if (mask.at<uchar>(i) > 0)
			p1.push_back(p1_copy[i]);
	}
}

/*
*	�ú������ڽ��µõ��ĵ������ݼ��뵽֮ǰ����
*/
void add_cps(
	vector<DMatch>& matches,
	vector<int>& correspond,
	vector<int>& next_correspond,
	vector<Point3f>& cps,
	vector<Point3f>& next_cps,
	vector<Vec3b>& colors,
	vector<Vec3b>& next_colors
	)
{
	for (int i = 0; i < matches.size(); ++i)
	{
		int query_idx = matches[i].queryIdx;
		int train_idx = matches[i].trainIdx;

		int cps_idx = correspond[query_idx];
		if (cps_idx >= 0) //���õ��ڿռ����Ѿ����ڣ������ƥ����Ӧ�Ŀռ��Ӧ����ͬһ��������Ҫ��ͬ
		{
			next_correspond[train_idx] = cps_idx;
			continue;
		}

		//���õ��ڿռ���δ���ڣ����õ���뵽�ṹ�У������ƥ���Ŀռ��������Ϊ�¼���ĵ������
		cps.push_back(next_cps[i]);
		colors.push_back(next_colors[i]);
		// �¼���ĵ��Ǽ���cps�����ģ������Ϊ���-1
		correspond[query_idx] = next_correspond[train_idx] = cps.size() - 1;	
	}
}



/*
*	�ú������ڴ洢��õĵ������ݵ�txt�ļ���
*	��ʽΪx y z b g r
*	����true/false��ָʾ�Ƿ�ɹ�����
*	cps������ǵ������ݣ�4��N�еľ���ÿһ�д���ռ��е�һ���㣨������꣩
*	colors���δ����ɫ
*/
bool save_cps(const char *filename, vector<Point3f> cps, vector<Vec3b> colors) {
	ofstream outfile(filename);	// ��Ҫ���浽���ļ���û����ᴴ��
	// ��ʧ������ֹ
	if (!outfile){
		cout << filename << "��ʧ�ܣ�";	// TODO ��Ҫ����ʾ�ڿ���̨
		return false;
	}

	for (size_t i = 0; i < cps.size(); ++i)
	{
		Point3f c = cps[i];
		outfile << c.x << " " << c.y << " " << c.z << " " << (unsigned int)colors[i].val[0] << " " << (unsigned int)colors[i].val[1] << " " << (unsigned int)colors[i].val[2] << endl;
	}

	outfile.close();
}