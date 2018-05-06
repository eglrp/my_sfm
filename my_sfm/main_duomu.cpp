#include "sfm_h_duomu.h"

char *save_filename = "D:\\save.txt";
char *read_filename = "D:\\save.txt";

int main(int argc, char *argv[])
{
	//// TODO ��coutɾ��









	//vector<Point3f> pts3D;
	//Point3f center3D;
	//Vec3f size3D;
	//float minX = 1e9, maxX = -1e9;
	//float minY = 1e9, maxY = -1e9;
	//float minZ = 1e9, maxZ = -1e9;
	//for (size_t i = 0; i < cps.cols; ++i) {
	//	Point3f pt3D;
	//	Mat_<float> c = cps.col(i);
	//	c /= c(3);	//������꣬��Ҫ�������һ��Ԫ�ز�������������ֵ
	//	// ����������y���z�����������opengl��y���z��
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




	/*�ڲξ���				
			fx    s    x0
		K = 0    fy   y0
			0    0    1
	*/
	Mat K(Matx33d(
	2759.48, 0, 1520.69,
	0, 2764.16, 1006.81,
	0, 0, 1));

	string img_dir = "D:\\images";	// �����ع���ͼƬ����Ŀ¼
	vector<string> img_names;		// �����ع���ͼƬ�����б�

	vector<vector<KeyPoint>> key_points_for_all;	// �������ͼƬ��������
	vector<Mat> descriptor_for_all;					// �������ͼƬ��������
	vector<vector<Vec3b>> colors_for_all;			// ��������㴦��ɫ
	vector<vector<DMatch>> matches_for_all;			// ���ƥ����

	// ��ȡĿ¼�µ�ͼƬ
	get_img_names(img_dir, img_names);


	//vector<string>::iterator it = img_names.begin();
	//for (; it != img_names.end(); it++) {
	//	cout << *it << endl;
	//}

	// ��ȡ����ͼ���������
	extract_features(img_names, key_points_for_all, descriptor_for_all, colors_for_all);
	//������ͼ�����˳�ε�����ƥ��
	match_features(descriptor_for_all, matches_for_all);
	
	vector<Point3f> cps;
	vector<vector<int>> correspond_cps_idx; //�����i��ͼ���е�j���������Ӧ��cps�е������
	vector<Vec3b> colors;
	vector<Mat> rotations;
	vector<Mat> motions;
	cout << "a" << endl;
	// ����ǰ����ͼƬ���г�ʼ�Ĺ���
	init_construct(K,key_points_for_all,colors_for_all,matches_for_all,
		cps,correspond_cps_idx,colors,rotations,motions
		);
	cout << "b" << endl;
	//������ʽ�ؽ�ʣ���ͼ��
	for (int i = 1; i < matches_for_all.size(); ++i)
	{
		vector<Point3f> object_points;
		vector<Point2f> image_points;
		Mat r, R, T;
		//Mat mask; 

		//��ȡ��i��ͼ����ƥ����Ӧ����ά�㣬�Լ��ڵ�i+1��ͼ���ж�Ӧ�����ص�
		get_objpoints_and_imgpoints(
			matches_for_all[i],
			correspond_cps_idx[i],
			cps,
			key_points_for_all[i + 1],
			object_points,
			image_points
			);
		cout << "c" << endl;
		//���任����
		solvePnPRansac(object_points, image_points, K, noArray(), r, T); 
		cout << "d" << endl;
		//����ת����ת��Ϊ��ת����
		Rodrigues(r, R);
		cout << "e" << endl;
		//����任����
		rotations.push_back(R);
		motions.push_back(T);

		vector<Point2f> p1, p2;
		vector<Vec3b> matched_colors;
		cout << "f" << endl;
		get_matched_points(key_points_for_all[i], key_points_for_all[i + 1], matches_for_all[i], p1, p2);
		get_matched_colors(colors_for_all[i], colors_for_all[i + 1], matches_for_all[i], matched_colors);

		//����֮ǰ��õ�R��T������ά�ؽ�
		vector<Point3f> next_cps;
		cout << "g" << endl;
		reconstruct(K, rotations[i], motions[i], R, T, p1, p2, next_cps);
		cout << "h" << endl;
		//���µõ��ĵ��Ƽ���֮ǰ���е�
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

	// ����õ��ĵ�������
	cout << "save" << endl;
	save_cps(save_filename, cps, colors);

	return 0;
}