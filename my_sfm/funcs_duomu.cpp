#include "sfm_h_duomu.h"

/*
*	该函数用于获取目录下的图片名称，存放到vector中备用
*/
void get_img_names(string dir_name, vector<string> & names)
{
	names.clear();
	// 文件句柄
	
	intptr_t hFile = 0;
	// 文件信息
	struct _finddata_t fileinfo;
	string p;
	if ((hFile = _findfirst(p.assign(dir_name).append("\\*").c_str(), &fileinfo)) != -1) {
		do {
			if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0) {
				// string类很有意思的一个赋值函数:assign()
				names.push_back(p.assign(dir_name).append("\\").append(fileinfo.name));
			}
		} while (_findnext(hFile, &fileinfo) == 0);  //寻找下一个，成功返回0，否则-1
		/*在win10中，使用文件遍历函数_findnext会报0xC0000005错误 
			原因： 
			_findnext()第一个参数”路径句柄”，返回的类型为intptr_t（long long），
			如果定义为long，在win7中是没有问题，
			但是在win10中就要改为long long或者intptr_t
		*/
		_findclose(hFile);
	}
}

/*
*	该函数用于提取特征点,保存特征点信息，描述子信息，以及颜色信息
*/
void extract_features(vector<Mat>& images,
	vector<vector<KeyPoint>>& key_points_for_all,
	vector<Mat>& descriptor_for_all,
	vector<vector<Vec3b>>& colors_for_all) {

	Ptr<Feature2D> sift = xfeatures2d::SIFT::create();	// 可以直接把SIFT换成SURF等
	Mat img;
	key_points_for_all.clear();
	descriptor_for_all.clear();
	int img_no = 0;
	for (auto it = images.begin(); it != images.end(); ++it)
	{
		img = *it;

		cout << "Extracing features from img-" << ++img_no << endl;

		// 提取特征点并生成特征向量
		vector<KeyPoint> key_points;
		Mat descriptor;
		sift->detectAndCompute(img, noArray(), key_points, descriptor);

		key_points_for_all.push_back(key_points);
		descriptor_for_all.push_back(descriptor);

		// 颜色
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
*	该函数用于进行所有特征点的匹配
*/
void match_features(vector<Mat>& descriptor_for_all, vector<vector<DMatch>>& matches_for_all) {
	matches_for_all.clear();
	// n个图像，两两顺次有 n-1 对匹配
	// 1与2匹配，2与3匹配，3与4匹配，以此类推
	for (int i = 0; i < descriptor_for_all.size() - 1; ++i)
	{
		cout << "Matching images " << i << " - " << i + 1 << endl;
		vector<DMatch> matches;
		match_features(descriptor_for_all[i], descriptor_for_all[i + 1], matches);
		matches_for_all.push_back(matches);
	}
}

/*
*	该函数用于进行特征点的匹配
*	DMatch主要用来储存匹配信息的结构体，
*	query是要匹配的描述子，train是被匹配的描述子
*/
void match_features(Mat& query, Mat& train, vector<DMatch>& matches) {
	vector<vector<DMatch>> knn_matches;
	BFMatcher matcher(NORM_L2);
	matcher.knnMatch(query, train, knn_matches, 2);  // k取2，着最近的两个匹配点然后radio test

	//获取满足Ratio Test的最小匹配的距离
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
		//排除不满足Ratio Test的点和匹配距离过大的点
		if (
			knn_matches[r][0].distance > 0.6*knn_matches[r][1].distance ||
			knn_matches[r][0].distance > 5 * max(min_dist, 10.0f)
			)
			continue;

		//保存匹配点
		matches.push_back(knn_matches[r][0]);
	}
}



/*
*	该函数用于获取第i幅图像中匹配点对应的三维点，以及在第i+1幅图像中对应的像素点
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
		if (cps_idx < 0) continue;	// 之前correspond在resize的时候就是全部赋-1

		object_points.push_back(cps[cps_idx]);
		image_points.push_back(key_points[train_idx].pt);
	}
}





/*
*	初始化构建
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
	//计算头两幅图像之间的变换矩阵
	vector<Point2f> p1, p2;
	Mat R, T;	//旋转矩阵和平移向量
	Mat mask;	//mask中大于零的点代表匹配点，等于零代表失配点
	get_matched_points(key_points_for_all[0], key_points_for_all[1], matches_for_all[0], p1, p2);
	get_matched_colors(colors_for_all[0], colors_for_all[1], matches_for_all[0], matched_colors);
	get_RT(K, p1, p2, R, T, mask);

	//对头两幅图像进行三维重建
	maskout_points(p1, mask);
	maskout_points(p2, mask);
	maskout_colors(matched_colors, mask);


	Mat R0 = Mat::eye(3, 3, CV_64FC1);		// 3行3列对角矩阵
	Mat T0 = Mat::zeros(3, 1, CV_64FC1);	// 零矩阵
	reconstruct(K, R0, T0, R, T, p1, p2, cps);
	//保存变换矩阵
	rotations = { R0, R };
	motions = { T0, T };

	//将correspond_cps_idx的大小初始化为与key_points_for_all完全一致
	correspond_cps_idx.clear();
	correspond_cps_idx.resize(key_points_for_all.size());
	for (int i = 0; i < key_points_for_all.size(); ++i)
	{
		correspond_cps_idx[i].resize(key_points_for_all[i].size(), -1);	// 全部赋初值为-1，之后没有对应的即是 
	}

	//填写头两幅图像的结构索引
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
*	该函数用于计算外参数矩阵R和T，R, T 分别是右摄像头相对于左摄像头的旋转矩阵（3*3）和平移向量（3*1）
*				fx    s    x0
内参矩阵K =  0    fy   y0
0    0    1
*	mask中大于零的点代表匹配点，等于零代表失配点
*/
bool get_RT(Mat K, vector<Point2f>& p1, vector<Point2f>& p2, Mat &R, Mat &T, Mat &mask) {
	double focal_length = 0.5*(K.at<double>(0) + K.at<double>(4));		// (fx+fy)/2,焦距
	Point2d center_point(K.at<double>(2), K.at<double>(5));				// 光心坐标

	//根据匹配点求取本征矩阵，使用RANSAC，进一步排除失配点
	Mat e_Mat = findEssentialMat(p1, p2, focal_length, center_point, RANSAC, 0.999, 1.0);
	if (e_Mat.empty()) {
		return false;
	}

	//double feasible_count = countNonZero(mask);
	//cout << (int)feasible_count << " -in- " << p1.size() << endl;
	////对于RANSAC而言，outlier数量大于50%时，结果是不可靠的
	//if (feasible_count <= 15 || (feasible_count / p1.size()) < 0.6)
	//	return false;

	//分解本征矩阵，获取相对变换
	int pass_count = recoverPose(e_Mat, p1, p2, R, T, focal_length, center_point, mask);

	////同时位于两个相机前方的点的数量要足够大
	//if (((double)pass_count) / feasible_count < 0.7)
	//	return false;

	return true;
}


/*
*	该函数用于三维重建，重建方式为三角化
*	OpenCV提供了该方法，可以直接使用
*/
void reconstruct(Mat& K, Mat& R1, Mat& T1, Mat& R2, Mat& T2, vector<Point2f>& p1, vector<Point2f>& p2, vector<Point3f>& cps)
{
	//两个相机的投影矩阵[R T]，triangulatePoints只支持float型
	Mat proj1(3, 4, CV_32FC1);
	Mat proj2(3, 4, CV_32FC1);

	R1.convertTo(proj1(Range(0, 3), Range(0, 3)), CV_32FC1);	
	T1.convertTo(proj1.col(3), CV_32FC1);	// 零矩阵

	R2.convertTo(proj2(Range(0, 3), Range(0, 3)), CV_32FC1);
	T2.convertTo(proj2.col(3), CV_32FC1);

	// 转换内参矩阵为float型
	Mat fK;
	K.convertTo(fK, CV_32FC1);
	proj1 = fK*proj1;
	proj2 = fK*proj2;

	//三角化重建
	Mat s;
	triangulatePoints(proj1, proj2, p1, p2, s);

	cps.clear();
	cps.reserve(s.cols);	// 改变容量，减少reallocate memory的次数，提高程序的性能
	for (int i = 0; i < s.cols; ++i)
	{
		Mat_<float> col = s.col(i);
		col /= col(3);	//齐次坐标，需要除以最后一个元素才是真正的坐标值
		cps.push_back(Point3f(col(0), col(1), col(2)));
	}
}

/*
*	该函数用于获取匹配到的点
*/
void get_matched_points(
	vector<KeyPoint> keypoints_1,
	vector<KeyPoint>keypoints_2,
	vector<DMatch> matches,
	vector<Point2f> &p1,
	vector<Point2f> &p2) {
	// 清空
	p1.clear();
	p2.clear();
	for (int i = 0; i < matches.size(); ++i)
	{
		p1.push_back(keypoints_1[matches[i].queryIdx].pt);
		p2.push_back(keypoints_2[matches[i].trainIdx].pt);
	}
}

/*
*	该函数用于获取匹配点的颜色
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
		// 取两个点的平均值
		temp.val[0] = (c1[matches[i].queryIdx].val[0] + c2[matches[i].trainIdx].val[0]) / 2;
		temp.val[1] = (c1[matches[i].queryIdx].val[1] + c2[matches[i].trainIdx].val[1]) / 2;
		temp.val[2] = (c1[matches[i].queryIdx].val[2] + c2[matches[i].trainIdx].val[2]) / 2;
		matched_colors.push_back(temp);
	}
}


/*
*	该函数用于再次去除失配点
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
*	该函数用于去除失配点对应的颜色
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
*	该函数用于将新得到的点云数据加入到之前的中
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
		if (cps_idx >= 0) //若该点在空间中已经存在，则这对匹配点对应的空间点应该是同一个，索引要相同
		{
			next_correspond[train_idx] = cps_idx;
			continue;
		}

		//若该点在空间中未存在，将该点加入到结构中，且这对匹配点的空间点索引都为新加入的点的索引
		cps.push_back(next_cps[i]);
		colors.push_back(next_colors[i]);
		// 新加入的点是加在cps的最后的，故序号为最后-1
		correspond[query_idx] = next_correspond[train_idx] = cps.size() - 1;	
	}
}



/*
*	该函数用于存储获得的点云数据到txt文件中
*	格式为x y z b g r
*	返回true/false，指示是否成功保存
*	cps保存的是点云数据，4行N列的矩阵，每一列代表空间中的一个点（齐次坐标）
*	colors依次存放颜色
*/
bool save_cps(const char *filename, vector<Point3f> cps, vector<Vec3b> colors) {
	ofstream outfile(filename);	// 打开要保存到的文件，没有则会创建
	// 打开失败则终止
	if (!outfile){
		cout << filename << "打开失败！";	// TODO 不要光显示在控制台
		return false;
	}

	for (size_t i = 0; i < cps.size(); ++i)
	{
		Point3f c = cps[i];
		outfile << c.x << " " << c.y << " " << c.z << " " << (unsigned int)colors[i].val[0] << " " << (unsigned int)colors[i].val[1] << " " << (unsigned int)colors[i].val[2] << endl;
	}

	outfile.close();
}