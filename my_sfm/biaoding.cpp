#include "sfm_h_duomu.h"

void InitCorners3D(CvMat *Corners3D, CvSize ChessBoardSize, int Nimages, float SquareSize);

int image_width;
int image_height;//���궨ͼƬ�Ĵ�С
const int ChessBoardSize_w = 7;
const int ChessBoardSize_h = 7;//ͼƬ�пɱ궨�ı�������ڽǵ�����

const CvSize  ChessBoardSize = cvSize(ChessBoardSize_w, ChessBoardSize_h);
const int NPoints = ChessBoardSize_w*ChessBoardSize_h;
const int NImages = 12;//���궨��ͼƬ��
int corner_count[NImages] = { 0 };
float SquareWidth = 20; //���̸��ӵı߳�����λ����

CvMat *intrinsics;
CvMat *distortion_coeff;
CvMat *rotation_vectors;
CvMat *translation_vectors;
CvMat *object_points;
CvMat *point_counts;
CvMat *image_points;

char *dir_name = "D:\\biaoding";	// ��ű궨ͼƬ���ļ���


int main(void) {
	IplImage     *current_frame_rgb;
	IplImage     *current_frame_rgb;
	IplImage     *current_frame_gray;
	IplImage     *chessBoard_Img;
	CvPoint2D32f corners[NPoints*NImages];

	chessBoard_Img = cvCreateImage(cvSize(image_width, image_height), IPL_DEPTH_8U, 3);
	current_frame_gray = cvCreateImage(cvSize(image_width, image_height), IPL_DEPTH_8U, 1);
	current_frame_rgb = cvCreateImage(cvSize(image_width, image_height), IPL_DEPTH_8U, 3);

	char *names;
	int image_no = 0;	// ������ڴ���ͼƬ�����
	// ���ζ�ȡ�궨ͼƬ�����нǵ���ȡ�ȹ���
	// �ļ����
	intptr_t hFile = 0;
	// �ļ���Ϣ
	struct _finddata_t fileinfo;
	string p;
	if ((hFile = _findfirst(p.assign(dir_name).append("\\*").c_str(), &fileinfo)) != -1) {
		do {
			if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0) {
				// string�������˼��һ����ֵ����:assign()
				p.assign(dir_name).append("\\").append(fileinfo.name);

				chessBoard_Img = cvLoadImage(names, CV_LOAD_IMAGE_COLOR);
				cvCvtColor(chessBoard_Img, current_frame_gray, CV_BGR2GRAY);
				cvCopy(chessBoard_Img, current_frame_rgb);

				/*
				����cvFindChessboardCorners��ͼȷ������ͼ���Ƿ�������ģʽ����ȷ���ǵ��λ��
				������нǵ㶼����⵽�����Ƕ�����һ��˳���Ų����������ط���ֵ
				*/
				int find_corners_result = cvFindChessboardCorners(current_frame_gray,	// ���������ͼ��������8λ�ĻҶȻ��߲�ɫͼ��
					ChessBoardSize,		// ����ͼ��ÿ�к�ÿ�нǵ�ĸ�����
					&corners[image_no * NPoints],	// ��⵽�Ľǵ�
					&corner_count[image_no],		// ������ǵ�ĸ���
					0);
				/*
				Flags:���ֲ�����־��������0��������ֵ����ϣ�
				CV_CALIB_CB_ADAPTIVE_THRESH - ʹ������Ӧ��ֵ��ͨ��ƽ��ͼ�����ȼ���õ�����ͼ��ת��Ϊ�ڰ�ͼ��������һ���̶�����ֵ��
				CV_CALIB_CB_NORMALIZE_IMAGE - �����ù̶���ֵ��������Ӧ����ֵ���ж�ֵ��֮ǰ����ʹ��cvNormalizeHist�����⻯ͼ�����ȡ�
				CV_CALIB_CB_FILTER_QUADS - ʹ��������׼��������������ܳ���������״����ȥ�����������׶μ�⵽�Ĵ��󷽿顣
				*/

				cvFindCornerSubPix(current_frame_gray,
					&corners[image_no*NPoints],
					NPoints, cvSize(2, 2), cvSize(-1, -1),
					cvTermCriteria(CV_TERMCRIT_ITER | CV_TERMCRIT_EPS, 20, 0.03));
				cvDrawChessboardCorners(current_frame_rgb, ChessBoardSize,
					&corners[image_no*NPoints],
					NPoints,
					find_corners_result);

				cvNamedWindow("Window 0", 0);
				cvNamedWindow("result", 0);
				cvShowImage("Window 0", chessBoard_Img);
				cvShowImage("result", current_frame_rgb);
				cvWaitKey(500);

				image_no++;
			}
		} while (_findnext(hFile, &fileinfo) == 0);  //Ѱ����һ�����ɹ�����0������-1
		/*��win10�У�ʹ���ļ���������_findnext�ᱨ0xC0000005����
		ԭ��
		_findnext()��һ��������·������������ص�����Ϊintptr_t��long long����
		�������Ϊlong����win7����û�����⣬
		������win10�о�Ҫ��Ϊlong long����intptr_t
		*/
		_findclose(hFile);

		intrinsics = cvCreateMat(3, 3, CV_32FC1);
		distortion_coeff = cvCreateMat(1, 4, CV_32FC1);
		rotation_vectors = cvCreateMat(NImages, 3, CV_32FC1);
		translation_vectors = cvCreateMat(NImages, 3, CV_32FC1);
		point_counts = cvCreateMat(NImages, 1, CV_32SC1);
		object_points = cvCreateMat(NImages*NPoints, 3, CV_32FC1);
		image_points = cvCreateMat(NImages*NPoints, 2, CV_32FC1);

		InitCorners3D(object_points, ChessBoardSize, NImages, SquareWidth);
		cvSetData(image_points, corners, sizeof(CvPoint2D32f));
		cvSetData(point_counts, &corner_count, sizeof(int));

		cvCalibrateCamera2(object_points,
			image_points,
			point_counts,
			cvSize(image_width, image_height),
			intrinsics,
			distortion_coeff,
			rotation_vectors,
			translation_vectors,
			0);

		float intr[3][3] = { 0.0 };
		float dist[4] = { 0.0 };
		float tranv[3] = { 0.0 };
		float rotv[3] = { 0.0 };

		for (int i = 0; i < 3; i++)
		{
			for (int j = 0; j < 3; j++)
			{
				intr[i][j] = ((float*)(intrinsics->data.ptr + intrinsics->step*i))[j];
			}
			dist[i] = ((float*)(distortion_coeff->data.ptr))[i];
			tranv[i] = ((float*)(translation_vectors->data.ptr))[i];
			rotv[i] = ((float*)(rotation_vectors->data.ptr))[i];
		}
		dist[3] = ((float*)(distortion_coeff->data.ptr))[3];

		printf("-----------------------------------------\n ");
		printf("INTRINSIC MATRIX:  \n");
		printf("[ %6.4f %6.4f %6.4f ]  \n", intr[0][0], intr[0][1], intr[0][2]);
		printf("[ %6.4f %6.4f %6.4f ]  \n", intr[1][0], intr[1][1], intr[1][2]);
		printf("[ %6.4f %6.4f %6.4f ]  \n", intr[2][0], intr[2][1], intr[2][2]);
		printf("----------------------------------------- \n");
		printf("DISTORTION VECTOR:  \n");
		printf("[ %6.4f %6.4f %6.4f %6.4f ]  \n", dist[0], dist[1], dist[2], dist[3]);
		printf("----------------------------------------- \n");
		printf("ROTATION VECTOR:  \n");
		printf("[ %6.4f %6.4f %6.4f ]  \n", rotv[0], rotv[1], rotv[2]);
		printf("TRANSLATION VECTOR:  \n");
		printf("[ %6.4f %6.4f %6.4f ]  \n", tranv[0], tranv[1], tranv[2]);
		printf("----------------------------------------- \n");

		cvReleaseMat(&intrinsics);
		cvReleaseMat(&distortion_coeff);
		cvReleaseMat(&rotation_vectors);
		cvReleaseMat(&translation_vectors);
		cvReleaseMat(&point_counts);
		cvReleaseMat(&object_points);
		cvReleaseMat(&image_points);
		cvDestroyAllWindows();
	}

	
}

void InitCorners3D(CvMat *Corners3D, CvSize ChessBoardSize, int NImages, float SquareSize)
{
	int CurrentImage = 0;
	int CurrentRow = 0;
	int CurrentColumn = 0;
	int NPoints = ChessBoardSize.height*ChessBoardSize.width;
	float * temppoints = new float[NImages*NPoints * 3];

	// for now, assuming we're row-scanning
	for (CurrentImage = 0; CurrentImage < NImages; CurrentImage++)
	{
		for (CurrentRow = 0; CurrentRow < ChessBoardSize.height; CurrentRow++)
		{
			for (CurrentColumn = 0; CurrentColumn < ChessBoardSize.width; CurrentColumn++)
			{
				temppoints[(CurrentImage*NPoints * 3) + (CurrentRow*ChessBoardSize.width + CurrentColumn) * 3] = (float)CurrentRow*SquareSize;
				temppoints[(CurrentImage*NPoints * 3) + (CurrentRow*ChessBoardSize.width + CurrentColumn) * 3 + 1] = (float)CurrentColumn*SquareSize;
				temppoints[(CurrentImage*NPoints * 3) + (CurrentRow*ChessBoardSize.width + CurrentColumn) * 3 + 2] = 0.f;
			}
		}
	}
	(*Corners3D) = cvMat(NImages*NPoints, 3, CV_32FC1, temppoints);
}





