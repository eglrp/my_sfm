#include "sfm_h_duomu.h"

char *save_filename = "D:\\save.txt";
char *read_filename = "D:\\save.txt";

int main(int argc, char *argv[])
{
	//// TODO 把cout删掉


	/*内参矩阵				
			fx    s    x0
		K = 0    fy   y0
			0    0    1
	*/
	Mat K(Matx33d(
	2759.48, 0, 1520.69,
	0, 2764.16, 1006.81,
	0, 0, 1));

	string img_dir = "D:\\images";	// 用于重构的图片所在目录
	vector<string> img_names;		// 用于重构的图片名字列表

	vector<vector<KeyPoint>> key_points_for_all;	// 存放所有图片的特征点
	vector<Mat> descriptors_for_all;					// 存放所有图片的描述子
	vector<vector<Vec3b>> colors_for_all;			// 存放特征点处颜色
	vector<vector<DMatch>> matches_for_all;			// 存放匹配点对

	// 获取目录下的图片
	get_img_names(img_dir, img_names);
	vector<Mat> images;

	vector<string>::iterator it = img_names.begin();
	for (; it != img_names.end(); it++) {
		Mat temp = imread(*it);
		if (temp.empty()) continue;
		cout << "reading img " << *it << endl;
		images.push_back(temp);
	}

	// 提取所有图像的特征点
	extract_features(images, key_points_for_all, descriptors_for_all, colors_for_all);
	//对所有图像进行顺次的特征匹配
	match_features(descriptors_for_all, matches_for_all);
	
	vector<Point3f> cps;
	vector<vector<int>> correspond_cps_idx; //保存第i副图像中第j个特征点对应的cps中点的索引
	vector<Vec3b> colors;
	vector<Mat> rotations;
	vector<Mat> motions;
	cout << "a" << endl;
	// 利用前两幅图片进行初始的构建
	init_construct(K,key_points_for_all,colors_for_all,matches_for_all,
		cps,correspond_cps_idx,colors,rotations,motions
		);
	cout << "b" << endl;
	//增量方式重建剩余的图像
	for (int i = 1; i < matches_for_all.size(); ++i)
	{
		vector<Point3f> object_points;
		vector<Point2f> image_points;
		Mat r, R, T;
		//Mat mask; 

		//获取第i幅图像中匹配点对应的三维点，以及在第i+1幅图像中对应的像素点
		get_objpoints_and_imgpoints(
			matches_for_all[i],
			correspond_cps_idx[i],
			cps,
			key_points_for_all[i + 1],
			object_points,
			image_points
			);
		cout << "c" << endl;
		//求解变换矩阵
		solvePnPRansac(object_points, image_points, K, noArray(), r, T); 
		cout << "d" << endl;
		//将旋转向量转换为旋转矩阵
		Rodrigues(r, R);
		cout << "e" << endl;
		//保存变换矩阵
		rotations.push_back(R);
		motions.push_back(T);

		vector<Point2f> p1, p2;
		vector<Vec3b> matched_colors;
		cout << "f" << endl;
		get_matched_points(key_points_for_all[i], key_points_for_all[i + 1], matches_for_all[i], p1, p2);
		get_matched_colors(colors_for_all[i], colors_for_all[i + 1], matches_for_all[i], matched_colors);

		//根据之前求得的R，T进行三维重建
		vector<Point3f> next_cps;
		cout << "g" << endl;
		reconstruct(K, rotations[i], motions[i], R, T, p1, p2, next_cps);
		cout << "h" << endl;
		//将新得到的点云加入之前已有的
		add_cps(
			matches_for_all[i],
			correspond_cps_idx[i],
			correspond_cps_idx[i + 1],
			cps,
			next_cps,
			colors,
			matched_colors
			);
		cout << "i" << endl;
	}

	// 保存得到的点云数据
	cout << "save" << endl;
	save_cps(save_filename, cps, colors);

	//vector<Point3f> pts3D;
	Point3f center3D;
	Vec3f size3D;
	float minX = 1e9, maxX = -1e9;
	float minY = 1e9, maxY = -1e9;
	float minZ = 1e9, maxZ = -1e9;
	for (size_t i = 0; i < cps.size(); ++i) {
	/*	Point3f pt3D;
		pt3D.x = cps[i].x;
		pt3D.y = cps[i].y;
		pt3D.z = cps[i].z;*/
		minX = min(minX, cps[i].x); maxX = max(maxX, cps[i].x);
		minY = min(minY, cps[i].y); maxY = max(maxY, cps[i].y);
		minZ = min(minZ, cps[i].z); maxZ = max(maxZ, cps[i].z);
		//pts3D.push_back(pt3D);
	}
	center3D.x = (minX + maxX) / 2;
	center3D.y = (minY + maxY) / 2;
	center3D.z = (minZ + maxZ) / 2;
	size3D[0] = maxX - minX;
	size3D[1] = maxY - minY;
	size3D[2] = maxZ - minZ;

	cout << "doing triangulation..." << endl;
	vector<Vec6f> tri;	// 存储每幅图的三角剖分结果
	vector<vector<Vec3i>> tri_no;	// 存放所有图像三角剖分结果（每个顶点从坐标转换成对应特征点的序号）
	for (int i = 0; i < images.size()-1; i++) {
		TriSubDiv(key_points_for_all[i], matches_for_all[i], images[i], tri);
		Vec3i temp;
		vector<Vec6f>::iterator it_tri = tri.begin();
		// 遍历每个三角形
		for (; it_tri != tri.end(); it_tri++){
			int flag = 1;	// flag为0表示没找到对应点
			// 遍历tri中每个顶点
			for (int j = 0; j < 3; j++){
				float tempx = (*it_tri)[j * 2];
				float tempy = (*it_tri)[j * 2 + 1];
				// 遍历matches,找出对应的特征点序号	// TODO 这边是不是可以改变循环顺序，达到只遍历一次match而找三个点的目的？
				vector<DMatch>::iterator it_match = matches_for_all[i].begin();
				for (; it_match != matches_for_all[i].end(); it_match++) {
					if (tempx == key_points_for_all[i][(*it_match).queryIdx].pt.x && tempy == key_points_for_all[i][(*it_match).queryIdx].pt.y) {
						temp[j] = (*it_match).queryIdx;
						break;
					}	
				}
				// 如果没有找到对应点，则结束这个三角形，开始下一个，同时给标记赋0
				if (it_match != matches_for_all[i].end()) {
					flag = 0;
					break;
				}
			}
			if (flag != 0) {
				tri_no[i].push_back(temp);
			} 
		}
	}
	// 最后一幅图像单独进行三角剖分
	tri.clear();
	Rect rect(0, 0, images[images.size() - 1].cols, images[images.size() - 1].rows); // Our outer bounding box，其中image.size().width==image.cols;
	Subdiv2D subdiv(rect);				// Create the initial subdivision
	Point2f fp;
	for (size_t i = 0; i < matches_for_all[images.size() - 2].size(); i++)
	{
		fp = key_points_for_all[images.size() - 2][matches_for_all[images.size() - 2][i].trainIdx].pt;
		subdiv.insert(fp);
	}
	subdiv.getTriangleList(tri);
	Vec3i temp;
	vector<Vec6f>::iterator it_tri = tri.begin();
	int i = images.size() - 2;
	// 遍历每个三角形
	for (; it_tri != tri.end(); it_tri++){
		int flag = 1;	// flag为0表示没找到对应点
		// 遍历tri中每个顶点
		for (int j = 0; j < 3; j++){
			float tempx = (*it_tri)[j * 2];
			float tempy = (*it_tri)[j * 2 + 1];
			// 遍历matches,找出对应的特征点序号	// TODO 这边是不是可以改变循环顺序，达到只遍历一次match而找三个点的目的？
			vector<DMatch>::iterator it_match = matches_for_all[i].begin();
			for (; it_match != matches_for_all[i].end(); it_match++) {
				if (tempx == key_points_for_all[i][(*it_match).trainIdx].pt.x && tempy == key_points_for_all[i][(*it_match).trainIdx].pt.y) {
					temp[j] = (*it_match).trainIdx;
					break;
				}
			}
			// 如果没有找到对应点，则结束这个三角形，开始下一个，同时给标记赋0
			if (it_match != matches_for_all[i].end()) {
				flag = 0;
				break;
			}
		}
		if (flag != 0) {
			tri_no[i+1].push_back(temp);
		}
	}


	/************************************************************************/
	/* Draw 3D scene using OpenGL                                           */
	/************************************************************************/
	glutInit(&argc, argv); // must be called first in a glut program
	InitGl(); // must be called first in a glut program
	
	cout << "creating 3D texture..." << endl;
	GLuint tex = Create3DTexture(images, tri_no, key_points_for_all, cps, center3D, size3D);
	Show(tex, center3D, size3D);

}