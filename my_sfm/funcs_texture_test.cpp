#include "sfm_h.h"

/*
*	����Delaunay�ʷ�
*/
void TriSubDiv(vector<Point2f> &pts, Mat &img, vector<cv::Vec6f> &tri)
{
	// ��Щ���봴���˳�ʼ���ʷ֣�һ�������ΰ���һ���ض��ľ��ο�
	Rect rect(0, 0, img.cols, img.rows); // Our outer bounding box������image.size().width==image.cols;
	Subdiv2D subdiv(rect);				// Create the initial subdivision

	// ����㣬��Щ�������32λfloat���͵ģ������Ǵ�����������ֵ�ĵ㣨cv::Point��
	// typedef Point_<float> Point2f; �����float���͵ģ����ԾͲ�ת��
	Point2f fp;
	for (size_t i = 0; i < pts.size(); i++)
	{
		fp = pts[i];
		subdiv.insert(fp);
	}

	// ��Delaunay�����ʷ��м���������
	// ����֮�󣬣����������е�ÿ��Vec6f�����������㣺��x1,y1,x2,y2,x3,y3,����
	//vector<cv::Vec6f> triangles;
	subdiv.getTriangleList(tri);

	// �õ���Ӧ��Voronoiͼ��
	// facets����VoronoiС��飨����ĵ�����ֻ��������εĶ��㣩��centers������Ӧ���������ġ�
	vector<vector<cv::Point2f> > facets;
	vector<cv::Point2f> centers;
	subdiv.getVoronoiFacetList(vector<int>(), facets, centers);

	


	
	/*char title[100];
	sprintf_s(title, 100, "Delaunay: %d Triangles", tri.size());
	imshow(title, imgShow);
	waitKey();*/
}