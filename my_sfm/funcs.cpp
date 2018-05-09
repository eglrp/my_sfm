//#include "sfm_h.h"
//
///*
//*	该函数用于提取特征点,保存特征点信息，描述子信息，以及颜色信息
//*/ 
//void extract_features(Mat &img, vector<KeyPoint> &key_points, Mat &descriptors, vector<Vec3b> &colors) {
//
//	Ptr<Feature2D> sift = xfeatures2d::SIFT::create();	// 可以直接把SIFT换成SURF等
//
//	// 提取特征点并生成特征向量
//	sift->detectAndCompute(img, noArray(), key_points, descriptors);
//
//	
//	// 颜色
//	for (int i = 0; i < key_points.size(); ++i)
//	{
//		Point2f& p = key_points[i].pt;
//		colors.push_back(img.at<Vec3b>(p.y, p.x));
//		/* Note the ordering of x and y. 
//		Since in OpenCV images are represented by the same structure as matrices, 
//		we use the same convention for both cases - 
//		the 0-based row index (or y-coordinate) goes first and the 0-based column index (or x-coordinate) follows it. 
//		Alternatively, you can use the following notation: Scalar intensity = img.at<uchar>(Point(x, y));
//		*/
//	}
//
//}
//
///*
//*	该函数用于进行特征点的匹配
//*	DMatch主要用来储存匹配信息的结构体，
//*	query是要匹配的描述子，train是被匹配的描述子
//*/
//void match_features(Mat& query, Mat& train, vector<DMatch>& matches) {
//	vector<vector<DMatch>> knn_matches;
//	BFMatcher matcher(NORM_L2);
//	matcher.knnMatch(query, train, knn_matches, 2);  // k取2，着最近的两个匹配点然后radio test
//
//	//获取满足Ratio Test的最小匹配的距离
//	float min_dist = FLT_MAX;
//	for (int r = 0; r < knn_matches.size(); ++r)
//	{
//		//Ratio Test
//		if (knn_matches[r][0].distance > 0.8*knn_matches[r][1].distance)
//			continue;
//		
//		float dist = knn_matches[r][0].distance;
//		if (dist < min_dist) min_dist = dist;
//	}
//
//	matches.clear();
//	for (size_t r = 0; r < knn_matches.size(); ++r)
//	{
//		//排除不满足Ratio Test的点和匹配距离过大的点
//		if (
//			knn_matches[r][0].distance > 0.8*knn_matches[r][1].distance ||
//			knn_matches[r][0].distance > 5 * max(min_dist, 10.0f)
//			)
//			continue;
//
//		//保存匹配点
//		matches.push_back(knn_matches[r][0]);
//	}
//}
//
///*
//*	该函数用于计算外参数矩阵R和T，R, T 分别是右摄像头相对于左摄像头的旋转矩阵（3*3）和平移向量（3*1）
//*				fx    s    x0
//    内参矩阵K =  0    fy   y0
//				 0    0    1
//*	mask中大于零的点代表匹配点，等于零代表失配点
//*/
//bool get_RT(Mat K, vector<Point2f>& p1, vector<Point2f>& p2, Mat &R, Mat &T, Mat &mask) {
//	double focal_length = 0.5*(K.at<double>(0) + K.at<double>(4));		// (fx+fy)/2,焦距
//	Point2d center_point(K.at<double>(2), K.at<double>(5));				// 光心坐标
//
//	//根据匹配点求取本征矩阵，使用RANSAC，进一步排除失配点
//	Mat e_Mat = findEssentialMat(p1, p2, focal_length, center_point, RANSAC, 0.999, 1.0);
//	if (e_Mat.empty()) {
//		return false;
//	}
//
//	//double feasible_count = countNonZero(mask);
//	//cout << (int)feasible_count << " -in- " << p1.size() << endl;
//	////对于RANSAC而言，outlier数量大于50%时，结果是不可靠的
//	//if (feasible_count <= 15 || (feasible_count / p1.size()) < 0.6)
//	//	return false;
//
//	//分解本征矩阵，获取相对变换
//	int pass_count = recoverPose(e_Mat, p1, p2, R, T, focal_length, center_point, mask);
//
//	////同时位于两个相机前方的点的数量要足够大
//	//if (((double)pass_count) / feasible_count < 0.7)
//	//	return false;
//
//	return true;
//}
//
//
///*
//*	该函数用于三维重建，重建方式为三角化
//*	OpenCV提供了该方法，可以直接使用
//*/
//void reconstruct(Mat& K, Mat& R, Mat& T, vector<Point2f>& p1, vector<Point2f>& p2, Mat& cps)
//{
//	//两个相机的投影矩阵[R T]，triangulatePoints只支持float型
//	Mat proj1(3, 4, CV_32FC1);
//	Mat proj2(3, 4, CV_32FC1);
//
//	proj1(Range(0, 3), Range(0, 3)) = Mat::eye(3, 3, CV_32FC1); // 3行3列对角矩阵
//	proj1.col(3) = Mat::zeros(3, 1, CV_32FC1);	// 零矩阵
//
//	R.convertTo(proj2(Range(0, 3), Range(0, 3)), CV_32FC1);
//	T.convertTo(proj2.col(3), CV_32FC1);
//
//	// 转换内参矩阵为float型
//	Mat fK;
//	K.convertTo(fK, CV_32FC1);	
//	proj1 = fK*proj1;
//	proj2 = fK*proj2;
//
//	//三角化重建
//	triangulatePoints(proj1, proj2, p1, p2, cps);
//}
//
///*
//*	该函数用于获取匹配到的点
//*/
//void get_matched_points(
//	vector<KeyPoint> keypoints_1, 
//	vector<KeyPoint>keypoints_2, 
//	vector<DMatch> matches, 
//	vector<Point2f> &p1, 
//	vector<Point2f> &p2) {
//	// 清空
//	p1.clear();
//	p2.clear();
//	for (int i = 0; i < matches.size(); ++i)
//	{
//		p1.push_back(keypoints_1[matches[i].queryIdx].pt);
//		p2.push_back(keypoints_2[matches[i].trainIdx].pt);
//	}
//}
//
///*
//*	该函数用于获取匹配点的颜色
//*/
//void get_matched_colors(
//	vector<Vec3b>& c1,
//	vector<Vec3b>& c2,
//	vector<DMatch> matches,
//	vector<Vec3b>& matched_colors
//	)
//{
//	matched_colors.clear();
//	
//	for (int i = 0; i < matches.size(); ++i)
//	{
//		Vec3b temp;
//		//temp = c1[matches[i].queryIdx];
//		// 取两个点的平均值
//		temp.val[0] = (c1[matches[i].queryIdx].val[0] + c2[matches[i].trainIdx].val[0]) / 2;
//		temp.val[1] = (c1[matches[i].queryIdx].val[1] + c2[matches[i].trainIdx].val[1]) / 2;
//		temp.val[2] = (c1[matches[i].queryIdx].val[2] + c2[matches[i].trainIdx].val[2]) / 2;
//		matched_colors.push_back(temp);
//	}
//}
//
//
///*
//*	该函数用于再次去除失配点
//*/
//void maskout_points(vector<Point2f>& p1, Mat& mask)
//{
//	vector<Point2f> p1_copy = p1;
//	p1.clear();
//
//	for (int i = 0; i < mask.rows; ++i)
//	{
//		if (mask.at<uchar>(i) > 0)
//			p1.push_back(p1_copy[i]);
//	}
//}
//
//
///*
//*	该函数用于存储获得的点云数据到txt文件中
//*	格式为x y z b g r
//*	返回true/false，指示是否成功保存
//*	cps保存的是点云数据，4行N列的矩阵，每一列代表空间中的一个点（齐次坐标）
//*	colors依次存放颜色
//*/
//bool save_cps(const char *filename, Mat cps, vector<Vec3b> colors) {
//	ofstream outfile(filename);	// 打开要保存到的文件，没有则会创建
//	// 打开失败则终止
//	if (!outfile){
//		cout << filename << "打开失败！";	// TODO 不要光显示在控制台
//		return false;
//	}
//
//	for (size_t i = 0; i < cps.cols; ++i)
//	{
//		Mat_<float> c = cps.col(i);
//		c /= c(3);	//齐次坐标，需要除以最后一个元素才是真正的坐标值
//		outfile << c(0) << " " << c(1) << " " << c(2) << " " << (unsigned int)colors[i].val[0] << " " << (unsigned int)colors[i].val[1] << " " << (unsigned int)colors[i].val[2] << endl;
//	}
//
//	outfile.close();
//}