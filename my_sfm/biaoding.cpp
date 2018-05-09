#include "sfm_h_duomu.h"

void InitCorners3D(CvMat *Corners3D, CvSize ChessBoardSize, int Nimages, float SquareSize);

int image_width;
int image_height;//待标定图片的大小
const int ChessBoardSize_w = 7;
const int ChessBoardSize_h = 7;//图片中可标定的标角数（内角点数）

const CvSize  ChessBoardSize = cvSize(ChessBoardSize_w, ChessBoardSize_h);
const int NPoints = ChessBoardSize_w*ChessBoardSize_h;
const int NImages = 12;//待标定的图片数
int corner_count[NImages] = { 0 };
float SquareWidth = 20; //棋盘格子的边长，单位毫米

CvMat *intrinsics;
CvMat *distortion_coeff;
CvMat *rotation_vectors;
CvMat *translation_vectors;
CvMat *object_points;
CvMat *point_counts;
CvMat *image_points;

char *dir_name = "D:\\biaoding";	// 存放标定图片的文件夹


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
	int image_no = 0;	// 标记现在处理图片的序号
	// 依次读取标定图片并进行角点提取等工作
	// 文件句柄
	intptr_t hFile = 0;
	// 文件信息
	struct _finddata_t fileinfo;
	string p;
	if ((hFile = _findfirst(p.assign(dir_name).append("\\*").c_str(), &fileinfo)) != -1) {
		do {
			if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0) {
				// string类很有意思的一个赋值函数:assign()
				p.assign(dir_name).append("\\").append(fileinfo.name);

				chessBoard_Img = cvLoadImage(names, CV_LOAD_IMAGE_COLOR);
				cvCvtColor(chessBoard_Img, current_frame_gray, CV_BGR2GRAY);
				cvCopy(chessBoard_Img, current_frame_rgb);

				/*
				函数cvFindChessboardCorners试图确定输入图像是否是棋盘模式，并确定角点的位置
				如果所有角点都被检测到且它们都被以一定顺序排布，函数返回非零值
				*/
				int find_corners_result = cvFindChessboardCorners(current_frame_gray,	// 输入的棋盘图，必须是8位的灰度或者彩色图像
					ChessBoardSize,		// 棋盘图中每行和每列角点的个数。
					&corners[image_no * NPoints],	// 检测到的角点
					&corner_count[image_no],		// 输出，角点的个数
					0);
				/*
				Flags:各种操作标志，可以是0或者下面值的组合：
				CV_CALIB_CB_ADAPTIVE_THRESH - 使用自适应阈值（通过平均图像亮度计算得到）将图像转换为黑白图，而不是一个固定的阈值。
				CV_CALIB_CB_NORMALIZE_IMAGE - 在利用固定阈值或者自适应的阈值进行二值化之前，先使用cvNormalizeHist来均衡化图像亮度。
				CV_CALIB_CB_FILTER_QUADS - 使用其他的准则（如轮廓面积，周长，方形形状）来去除在轮廓检测阶段检测到的错误方块。
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
		} while (_findnext(hFile, &fileinfo) == 0);  //寻找下一个，成功返回0，否则-1
		/*在win10中，使用文件遍历函数_findnext会报0xC0000005错误
		原因：
		_findnext()第一个参数”路径句柄”，返回的类型为intptr_t（long long），
		如果定义为long，在win7中是没有问题，
		但是在win10中就要改为long long或者intptr_t
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





