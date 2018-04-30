#include "sfm_h.h"

/*
*	����Delaunay�ʷ�
*/
void TriSubDiv(vector<Point2f> &pts, Mat &img, vector<Vec3i> &tri)
{
	// ��Щ���봴���˳�ʼ���ʷ֣�һ�������ΰ���һ���ض��ľ��ο�
	Rect rect(0, 0, img.cols, img.rows); // Our outer bounding box
	Subdiv2D subdiv(rect);				// Create the initial subdivision

	// ����㣬��Щ�������32λfloat���͵ģ������Ǵ�����������ֵ�ĵ㣨cv::Point��






















	Subdiv2D* subdiv;//The subdivision itself // ϸ�� 
	CvMemStorage* storage = cvCreateMemStorage(0);;//Storage for the Delaunay subdivsion //�����洢�����ʷ� 
	Rect rc = Rect(0, 0, img.cols, img.rows); //Our outer bounding box //���ǵ���ӱ߽���� 

	subdiv = CreateSubdiv2D(CV_SEQ_KIND_SUBDIV2D, sizeof(*subdiv),
		sizeof(Subdiv2DPoint),
		sizeof(QuadEdge2D),
		storage);//Ϊ��������ռ�  

	InitSubdivDelaunay2D(subdiv, rc);//rect sets the bounds 

	//������ǵĵ㼯����32λ�ģ����������ǽ���תΪCvPoint2D32f���������ַ�����
	for (size_t i = 0; i < pts.size(); i++)
	{
		CvSubdiv2DPoint *pt = cvSubdivDelaunay2DInsert(subdiv, pts[i]);
		pt->id = i;
	}

	CvSeqReader reader;
	int total = subdiv->edges->total;
	int elem_size = subdiv->edges->elem_size;

	cvStartReadSeq((CvSeq*)(subdiv->edges), &reader, 0);
	Point buf[3];
	const Point *pBuf = buf;
	Vec3i verticesIdx;
	Mat imgShow = img.clone();

	srand((unsigned)time(NULL));
	for (int i = 0; i < total; i++)
	{
		CvQuadEdge2D* edge = (CvQuadEdge2D*)(reader.ptr);

		if (CV_IS_SET_ELEM(edge))
		{
			CvSubdiv2DEdge t = (CvSubdiv2DEdge)edge;
			int iPointNum = 3;
			Scalar color = CV_RGB(rand() & 255, rand() & 255, rand() & 255);

			//bool isNeg = false;
			int j;
			for (j = 0; j < iPointNum; j++)
			{
				CvSubdiv2DPoint* pt = cvSubdiv2DEdgeOrg(t);
				if (!pt) break;
				buf[j] = pt->pt;
				//if (pt->id == -1) isNeg = true;
				verticesIdx[j] = pt->id;
				t = cvSubdiv2DGetEdge(t, CV_NEXT_AROUND_LEFT);
			}
			if (j != iPointNum) continue;
			if (isGoodTri(verticesIdx, tri))
			{
				//tri.push_back(verticesIdx);
				polylines(imgShow, &pBuf, &iPointNum,
					1, true, color,
					1, CV_AA, 0);
				//printf("(%d, %d)-(%d, %d)-(%d, %d)\n", buf[0].x, buf[0].y, buf[1].x, buf[1].y, buf[2].x, buf[2].y);
				//printf("%d\t%d\t%d\n", verticesIdx[0], verticesIdx[1], verticesIdx[2]);
				//imshow("Delaunay", imgShow);
				//waitKey();
			}

			t = (CvSubdiv2DEdge)edge + 2;

			for (j = 0; j < iPointNum; j++)
			{
				CvSubdiv2DPoint* pt = cvSubdiv2DEdgeOrg(t);
				if (!pt) break;
				buf[j] = pt->pt;
				verticesIdx[j] = pt->id;
				t = cvSubdiv2DGetEdge(t, CV_NEXT_AROUND_LEFT);
			}
			if (j != iPointNum) continue;
			if (isGoodTri(verticesIdx, tri))
			{
				//tri.push_back(verticesIdx);
				polylines(imgShow, &pBuf, &iPointNum,
					1, true, color,
					1, CV_AA, 0);
				//printf("(%d, %d)-(%d, %d)-(%d, %d)\n", buf[0].x, buf[0].y, buf[1].x, buf[1].y, buf[2].x, buf[2].y);
				//printf("%d\t%d\t%d\n", verticesIdx[0], verticesIdx[1], verticesIdx[2]);
				//imshow("Delaunay", imgShow);
				//waitKey();
			}
		}

		CV_NEXT_SEQ_ELEM(elem_size, reader);

	}

	//RemoveDuplicate(tri);
	char title[100];
	sprintf_s(title, 100, "Delaunay: %d Triangles", tri.size());
	imshow(title, imgShow);
	waitKey();
}