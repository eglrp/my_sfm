//#include "sfm_h.h"
//
//char *save_filename = "D:\\save.txt";
//char *read_filename = "D:\\save.txt";
//
//int main(int argc, char *argv[])
//{
//	// TODO ��coutɾ��
//
//
//	/*�ڲξ���				
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
//	//namedWindow("��matchͼ��", 0);
//
//	// ����ͼ��
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
//	//// ѹ��һ�£�����ٶȣ�sift̫��
//	//resize(img_1_raw, img_1, Size(), 0.4, 0.4);
//	//resize(img_2_raw, img_2, Size(), 0.4, 0.4);
//
//	// ���������	
//	cout << "detect" << endl;
//	vector<KeyPoint> keypoints_1, keypoints_2;
//	Mat descriptors_1, descriptors_2;
//	extract_features(img_1, keypoints_1, descriptors_1, colors1);
//	extract_features(img_2, keypoints_2, descriptors_2, colors2);
//
//	// ƥ��������
//	cout << "match" << endl;
//	vector<DMatch> matches;	// �洢ƥ�䵽�ĵ��
//	match_features(descriptors_1, descriptors_2, matches);
//
//	// ����任����R��T��
//	cout << "calc R T" << endl;
//	vector<Point2f> p1, p2;
//	vector<Vec3b> matched_colors;
//	Mat R, T;	//��ת�����ƽ������
//	Mat mask;	//mask�д�����ĵ����ƥ��㣬���������ʧ���
//	get_matched_points(keypoints_1, keypoints_2, matches, p1, p2);
//	get_matched_colors(colors1, colors2, matches, matched_colors);
//	get_RT(K, p1, p2, R, T, mask);
//
//	// ��ά�ؽ�
//	cout << "r" << endl;
//	Mat cps;	//4��N�еľ���ÿһ�д���ռ��е�һ���㣨������꣩
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
//		c /= c(3);	//������꣬��Ҫ�������һ��Ԫ�ز�������������ֵ
//		// ����������y���z�����������opengl��y���z��
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
//	// ����õ��ĵ�������
//	cout << "save" << endl;
//	save_cps(save_filename, cps, matched_colors);
//
//	////����ƥ����Ĺؼ���
//	//Mat img_matches;
//	//drawMatches(img_1, keypoints_1, img_2, keypoints_2, matches, img_matches);
//	//imshow("��matchͼ��", img_matches);
//	////�ȴ����ⰴ������
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