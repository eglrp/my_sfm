#include "sfm_h_duomu.h"

char *save_filename = "D:\\save.txt";
char *read_filename = "D:\\save.txt";

int main(int argc, char *argv[])
{
	//// TODO 把cout删掉









	//vector<Point3f> pts3D;
	//Point3f center3D;
	//Vec3f size3D;
	//float minX = 1e9, maxX = -1e9;
	//float minY = 1e9, maxY = -1e9;
	//float minZ = 1e9, maxZ = -1e9;
	//for (size_t i = 0; i < cps.cols; ++i) {
	//	Point3f pt3D;
	//	Mat_<float> c = cps.col(i);
	//	c /= c(3);	//齐次坐标，需要除以最后一个元素才是真正的坐标值
	//	// 这里计算出的y轴和z轴的正方向与opengl中y轴和z轴
	//	pt3D.x = c(0);
	//	pt3D.y = -c(1);
	//	pt3D.z = -c(2);
	//	minX = min(minX, c(0)); maxX = max(maxX, c(0));
	//	minY = min(minY, -c(1)); maxY = max(maxY, -c(1));
	//	minZ = min(minZ, -c(2)); maxZ = max(maxZ, -c(2));
	//	pts3D.push_back(pt3D);
	//}
	//center3D.x = (minX + maxX) / 2;
	//center3D.y = (minY + maxY) / 2;
	//center3D.z = (minZ + maxZ) / 2;
	//size3D[0] = maxX - minX;
	//size3D[1] = maxY - minY;
	//size3D[2] = maxZ - minZ;






	//cout << "doing triangulation..." << endl;
	//vector<Vec6f> tri;
	//TriSubDiv(p1, img_1, tri);

	///************************************************************************/
	///* Draw 3D scene using OpenGL                                           */
	///************************************************************************/
	//glutInit(&argc, argv); // must be called first in a glut program
	//InitGl(); // must be called first in a glut program

	//cout << "creating 3D texture..." << endl;
	//cout << p1.size() << " **** " << pts3D.size();
	//GLuint tex = Create3DTexture(img_1, tri, p1, pts3D, center3D, size3D);
	//Show(tex, center3D, size3D);




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
	vector<Mat> descriptor_for_all;					// 存放所有图片的描述子
	vector<vector<Vec3b>> colors_for_all;			// 存放特征点处颜色
	vector<vector<DMatch>> matches_for_all;			// 存放匹配点对

	// 获取目录下的图片
	get_img_names(img_dir, img_names);


	//vector<string>::iterator it = img_names.begin();
	//for (; it != img_names.end(); it++) {
	//	cout << *it << endl;
	//}

	// 提取所有图像的特征点
	extract_features(img_names, key_points_for_all, descriptor_for_all, colors_for_all);
	//对所有图像进行顺次的特征匹配
	match_features(descriptor_for_all, matches_for_all);
	
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

	return 0;
}