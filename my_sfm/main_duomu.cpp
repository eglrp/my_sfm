#include "sfm_h_duomu.h"

char *save_filename = "D:\\save.txt";
char *read_filename = "D:\\save.txt";

int main(int argc, char *argv[])
{
	//// TODO ��coutɾ��


	/*�ڲξ���				
			fx    s    x0
		K = 0    fy   y0
			0    0    1
	*/
	/*Mat K(Matx33d(
	2759.48, 0, 1520.69/5,
	0, 2764.16, 1006.81/5,
	0, 0, 1));*/
Mat K(Matx33d(
	6920, 0, 2080/5,
	0, 6920, 1560/5,
	0, 0, 1));

	string img_dir = "D:\\images";	// �����ع���ͼƬ����Ŀ¼
	vector<string> img_names;		// �����ع���ͼƬ�����б�

	vector<vector<KeyPoint>> key_points_for_all;	// �������ͼƬ��������
	vector<Mat> descriptors_for_all;					// �������ͼƬ��������
	vector<vector<Vec3b>> colors_for_all;			// ��������㴦��ɫ
	vector<vector<DMatch>> matches_for_all;			// ���ƥ����

	// ��ȡĿ¼�µ�ͼƬ
	get_img_names(img_dir, img_names);
	vector<Mat> images;

	vector<string>::iterator it = img_names.begin();
	for (; it != img_names.end(); it++) {
		Mat temp = imread(*it);
		if (temp.empty()) continue;
		resize(temp, temp, Size(), 0.2, 0.2);
		cout << "reading img " << *it << endl;
		images.push_back(temp);
	}

	// ��ȡ����ͼ���������
	extract_features(images, key_points_for_all, descriptors_for_all, colors_for_all);
	//������ͼ�����˳�ε�����ƥ��
	match_features(descriptors_for_all, matches_for_all);
	
	vector<Point3f> cps;
	vector<vector<int>> correspond_cps_idx; //�����i��ͼ���е�j���������Ӧ��cps�е������
	vector<Vec3b> colors;
	vector<Mat> rotations;
	vector<Mat> motions;
	// ����ǰ����ͼƬ���г�ʼ�Ĺ���
	init_construct(K,key_points_for_all,colors_for_all,matches_for_all,
		cps,correspond_cps_idx,colors,rotations,motions
		);
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
		//���任����
		solvePnPRansac(object_points, image_points, K, noArray(), r, T); 
		//����ת����ת��Ϊ��ת����
		Rodrigues(r, R);
		//����任����
		rotations.push_back(R);
		motions.push_back(T);

		vector<Point2f> p1, p2;
		vector<Vec3b> matched_colors;
		get_matched_points(key_points_for_all[i], key_points_for_all[i + 1], matches_for_all[i], p1, p2);
		get_matched_colors(colors_for_all[i], colors_for_all[i + 1], matches_for_all[i], matched_colors);

		//����֮ǰ��õ�R��T������ά�ؽ�
		vector<Point3f> next_cps;
		reconstruct(K, rotations[i], motions[i], R, T, p1, p2, next_cps);
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
	}

	// ����õ��ĵ�������
	cout << "save" << endl;
	save_cps(save_filename, cps, colors);

	vector<Point3f> pts3D;
	Point3f center3D;
	Vec3f size3D;
	float minX = 1e9, maxX = -1e9;
	float minY = 1e9, maxY = -1e9;
	float minZ = 1e9, maxZ = -1e9;
	for (size_t i = 0; i < cps.size(); ++i) {
		// opengl��������ͼ���õ���ά��������᲻ͬ��yz��ȡ��
		Point3f pt3D;
		pt3D.x = cps[i].x;
		pt3D.y = -cps[i].y;
		pt3D.z = -cps[i].z;
		minX = min(minX, cps[i].x); maxX = max(maxX, cps[i].x);
		minY = min(minY, -cps[i].y); maxY = max(maxY, -cps[i].y);
		minZ = min(minZ, -cps[i].z); maxZ = max(maxZ, -cps[i].z);
		pts3D.push_back(pt3D);
	}
	center3D.x = (minX + maxX) / 2;
	center3D.y = (minY + maxY) / 2;
	center3D.z = (minZ + maxZ) / 2;
	size3D[0] = maxX - minX;
	size3D[1] = maxY - minY;
	size3D[2] = maxZ - minZ;

	cout << "doing triangulation..." << endl;
	vector<Vec6f> tri;	// �洢ÿ��ͼ�������ʷֽ��
	vector<vector<Vec3i>> tri_no;	// �������ͼ�������ʷֽ����ÿ�����������ת���ɶ�Ӧ���������ţ�
	for (int i = 0; i < images.size()-1; i++) {
		vector<Vec3i> temp_tri;
		cout << i << endl;
		TriSubDiv(key_points_for_all[i], matches_for_all[i], images[i], tri);
		Vec3i temp;
		vector<Vec6f>::iterator it_tri = tri.begin();
		// ����ÿ��������
		for (; it_tri != tri.end(); it_tri++){
			int flag = 1;	// flagΪ0��ʾû�ҵ���Ӧ��
			// ����tri��ÿ������
			for (int j = 0; j < 3; j++){
				float tempx = (*it_tri)[j * 2];
				float tempy = (*it_tri)[j * 2 + 1];
				// ����matches,�ҳ���Ӧ�����������	// TODO ����ǲ��ǿ��Ըı�ѭ��˳�򣬴ﵽֻ����һ��match�����������Ŀ�ģ�
				vector<DMatch>::iterator it_match = matches_for_all[i].begin();
				for (; it_match != matches_for_all[i].end(); it_match++) {
					if (tempx == key_points_for_all[i][(*it_match).queryIdx].pt.x && tempy == key_points_for_all[i][(*it_match).queryIdx].pt.y) {
						temp[j] = (*it_match).queryIdx;
						break;
					}	
				}
				// ���û���ҵ���Ӧ�㣬�������������Σ���ʼ��һ����ͬʱ����Ǹ�0
				// �����Ӧ�ĵ�û�ж�Ӧ��ά��Ҳȥ��
				// TODO: �����Ļ���һ��ʼ�����ʷֵ�ʱ���Ӧ��ʹ����Щ���������ƥ���
				if (it_match == matches_for_all[i].end() || correspond_cps_idx[i][temp[j]] == -1) {
					flag = 0;
					break;
				}
			}
			if (flag != 0) {
				temp_tri.push_back(temp);
			} 
		}
		cout << temp_tri.size() << endl;
		tri_no.push_back(temp_tri);
	}
	cout << "last" << endl;
	// ���һ��ͼ�񵥶����������ʷ�
	tri.clear();
	Rect rect(0, 0, images[images.size() - 1].cols, images[images.size() - 1].rows); // Our outer bounding box������image.size().width==image.cols;
	Subdiv2D subdiv(rect);				// Create the initial subdivision
	Point2f fp;
	for (size_t i = 0; i < matches_for_all[images.size() - 2].size(); i++)
	{
		fp = key_points_for_all[images.size() - 1][matches_for_all[images.size() - 2][i].trainIdx].pt;
		subdiv.insert(fp);
	}
	subdiv.getTriangleList(tri);
	Vec3i temp;
	vector<Vec6f>::iterator it_tri = tri.begin();
	vector<Vec3i> temp_tri;
	// ����ÿ��������
	for (; it_tri != tri.end(); it_tri++){
		int flag = 1;	// flagΪ0��ʾû�ҵ���Ӧ��
		// ����tri��ÿ������
		for (int j = 0; j < 3; j++){
			float tempx = (*it_tri)[j * 2];
			float tempy = (*it_tri)[j * 2 + 1];
			// ����matches,�ҳ���Ӧ�����������	// TODO ����ǲ��ǿ��Ըı�ѭ��˳�򣬴ﵽֻ����һ��match�����������Ŀ�ģ�
			vector<DMatch>::iterator it_match = matches_for_all[images.size() - 2].begin();
			for (; it_match != matches_for_all[images.size() - 2].end(); it_match++) {
				if (tempx == key_points_for_all[images.size() - 1][(*it_match).trainIdx].pt.x && tempy == key_points_for_all[images.size() - 1][(*it_match).trainIdx].pt.y) {
					temp[j] = (*it_match).trainIdx;
					break;
				}
			}
			// ���û���ҵ���Ӧ�㣬�������������Σ���ʼ��һ����ͬʱ����Ǹ�0
			if (it_match == matches_for_all[images.size() - 2].end() || correspond_cps_idx[images.size() - 1][temp[j]] == -1) {
				flag = 0;
				break;
			}
		}
		if (flag != 0) {
			temp_tri.push_back(temp);
		}
	}
	cout << temp_tri.size() << endl;
	tri_no.push_back(temp_tri);

	/************************************************************************/
	/* Draw 3D scene using OpenGL                                           */
	/************************************************************************/
	glutInit(&argc, argv); // must be called first in a glut program
	InitGl(); // must be called first in a glut program
	
	cout << "creating 3D texture..." << endl;
	GLuint tex = Create3DTexture(images, tri_no, key_points_for_all, pts3D, center3D, size3D, correspond_cps_idx);
	Show(tex, center3D, size3D);

}