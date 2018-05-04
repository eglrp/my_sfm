#include "sfm_h.h"

/*
*	创建Delaunay剖分
*/
void TriSubDiv(vector<Point2f> &pts, Mat &img, vector<cv::Vec6f> &tri)
{
	// 这些代码创建了初始的剖分，一个三角形包含一个特定的矩形框。
	Rect rect(0, 0, img.cols, img.rows); // Our outer bounding box，其中image.size().width==image.cols;
	Subdiv2D subdiv(rect);				// Create the initial subdivision

	// 插入点，这些点必须是32位float类型的，或者是带有整数坐标值的点（cv::Point）
	// typedef Point_<float> Point2f; 这个是float类型的，所以就不转了
	Point2f fp;
	for (size_t i = 0; i < pts.size(); i++)
	{
		fp = pts[i];
		subdiv.insert(fp);
	}

	// 从Delaunay三角剖分中计算三角形
	// 调用之后，，在三角形中的每个Vec6f包含三个顶点：（x1,y1,x2,y2,x3,y3,）。
	//vector<cv::Vec6f> triangles;
	subdiv.getTriangleList(tri);

	// 得到对应的Voronoi图。
	// facets包含Voronoi小面块（里面的点数据只包括多边形的顶点），centers包含对应的区域中心。
	vector<vector<cv::Point2f> > facets;
	vector<cv::Point2f> centers;
	subdiv.getVoronoiFacetList(vector<int>(), facets, centers);

	


	
	/*char title[100];
	sprintf_s(title, 100, "Delaunay: %d Triangles", tri.size());
	imshow(title, imgShow);
	waitKey();*/
}