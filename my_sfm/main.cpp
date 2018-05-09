//#include "sfm_h.h"
//
//char *save_filename = "D:\\save.txt";
//char *read_filename = "D:\\save.txt";
//
//int main(int argc, char *argv[])
//{
//	// TODO 把cout删掉
//
//
//	/*内参矩阵				
//			fx    s    x0
//		K = 0    fy   y0		fx = f/dx
//			0    0    1
//	*/
//	/*Mat K(Matx33d(
//	2759.48, 0, 1520.69,
//	0, 2764.16, 1006.81,
//	0, 0, 1));*/
//	Mat K(Matx33d(
//		6920, 0, 2080,
//		0, 6920, 1560,
//		0, 0, 1));
//
//	//namedWindow("【match图】", 0);
//
//	// 读入图像
//	cout << "loading..." << endl;
//	/*Mat img_1_raw = imread("D:\\0004.png");
//	Mat img_2_raw = imread("D:\\0006.png");
//	Mat img_1;
//	Mat img_2;*/
//	Mat img_1 = imread("D:\\4.jpg");
//	Mat img_2 = imread("D:\\6.jpg");
//	/*Mat img_1 = imread("D:\\004.jpg");
//	Mat img_2 = imread("D:\\006.jpg");*/
//	vector<Vec3b> colors1;
//	vector<Vec3b> colors2;
//	
//	//// 压缩一下，提高速度，sift太慢
//	//resize(img_1_raw, img_1, Size(), 0.4, 0.4);
//	//resize(img_2_raw, img_2, Size(), 0.4, 0.4);
//
//	// 检测特征点	
//	cout << "detect" << endl;
//	vector<KeyPoint> keypoints_1, keypoints_2;
//	Mat descriptors_1, descriptors_2;
//	extract_features(img_1, keypoints_1, descriptors_1, colors1);
//	extract_features(img_2, keypoints_2, descriptors_2, colors2);
//
//	// 匹配特征点
//	cout << "match" << endl;
//	vector<DMatch> matches;	// 存储匹配到的点对
//	match_features(descriptors_1, descriptors_2, matches);
//
//	// 计算变换矩阵（R和T）
//	cout << "calc R T" << endl;
//	vector<Point2f> p1, p2;
//	vector<Vec3b> matched_colors;
//	Mat R, T;	//旋转矩阵和平移向量
//	Mat mask;	//mask中大于零的点代表匹配点，等于零代表失配点
//	get_matched_points(keypoints_1, keypoints_2, matches, p1, p2);
//	get_matched_colors(colors1, colors2, matches, matched_colors);
//	get_RT(K, p1, p2, R, T, mask);
//
//	// 三维重建
//	cout << "r" << endl;
//	Mat cps;	//4行N列的矩阵，每一列代表空间中的一个点（齐次坐标）
//	maskout_points(p1, mask);
//	maskout_points(p2, mask);
//	reconstruct(K, R, T, p1, p2, cps);
//
//
//
//
//
//
//
//
//
//	vector<Point3f> pts3D;
//	Point3f center3D;
//	Vec3f size3D;
//	float minX = 1e9, maxX = -1e9;
//	float minY = 1e9, maxY = -1e9;
//	float minZ = 1e9, maxZ = -1e9;
//	for (size_t i = 0; i < cps.cols; ++i) {
//		Point3f pt3D;
//		Mat_<float> c = cps.col(i);
//		c /= c(3);	//齐次坐标，需要除以最后一个元素才是真正的坐标值
//		// 这里计算出的y轴和z轴的正方向与opengl中y轴和z轴
//		pt3D.x = c(0);
//		pt3D.y = -c(1);
//		pt3D.z = -c(2);
//		minX = min(minX, c(0)); maxX = max(maxX, c(0));
//		minY = min(minY, -c(1)); maxY = max(maxY, -c(1));
//		minZ = min(minZ, -c(2)); maxZ = max(maxZ, -c(2));
//		pts3D.push_back(pt3D);
//	}
//	center3D.x = (minX + maxX) / 2;
//	center3D.y = (minY + maxY) / 2;
//	center3D.z = (minZ + maxZ) / 2;
//	size3D[0] = maxX - minX;
//	size3D[1] = maxY - minY;
//	size3D[2] = maxZ - minZ;
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
//
//	// 保存得到的点云数据
//	cout << "save" << endl;
//	save_cps(save_filename, cps, matched_colors);
//
//	////绘制匹配出的关键点
//	//Mat img_matches;
//	//drawMatches(img_1, keypoints_1, img_2, keypoints_2, matches, img_matches);
//	//imshow("【match图】", img_matches);
//	////等待任意按键按下
//	//waitKey(0);
//
//	//gl_main(argc, argv);
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
//
//	cout << "doing triangulation..." << endl;
//	vector<Vec6f> tri;
//	TriSubDiv(p1, img_1, tri);
//
//	/************************************************************************/
//	/* Draw 3D scene using OpenGL                                           */
//	/************************************************************************/
//	glutInit(&argc, argv); // must be called first in a glut program
//	InitGl(); // must be called first in a glut program
//
//	cout << "creating 3D texture..." << endl;
//	cout << p1.size() << " **** " << pts3D.size();
//	GLuint tex = Create3DTexture(img_1, tri, p1, pts3D, center3D, size3D);
//	Show(tex, center3D, size3D);
//}