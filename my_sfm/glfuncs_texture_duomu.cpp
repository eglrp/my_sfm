#include "sfm_h_duomu.h"
#include <atltrace.h>


#if !defined(GLUT_WHEEL_UP)
#  define GLUT_WHEEL_UP   3
#  define GLUT_WHEEL_DOWN 4
#endif


// draw a triangle texture
void MapTexTri(Mat & texImg, Point2f pt2D[3], Point3f pt3D[3])
{
	//glEnable(GL_TEXTURE_2D);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, texImg.cols, texImg.rows, 0,
		GL_RGB, GL_UNSIGNED_BYTE, texImg.data);

	//glDepthMask(GL_FALSE);
	//glDisable(GL_LIGHTING);
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	// TODO:这个纹理映射是只要指定边角的对应点就可以把这块纹理直接贴过去吗？
	glBegin(GL_TRIANGLES);
	//glColor4f(1.0, 1.0, 1.0, 1.0);

	glTexCoord2f(pt2D[0].x, pt2D[0].y); glVertex3f(pt3D[0].x, pt3D[0].y, pt3D[0].z);
	glTexCoord2f(pt2D[1].x, pt2D[1].y); glVertex3f(pt3D[1].x, pt3D[1].y, pt3D[1].z);
	glTexCoord2f(pt2D[2].x, pt2D[2].y); glVertex3f(pt3D[2].x, pt3D[2].y, pt3D[2].z);
	
	glEnd();

	//glDisable(GL_TEXTURE_2D);
	//glDisable(GL_BLEND);
}

GLuint Create3DTexture(vector<Mat> &images,
	vector<vector<Vec3i>> &tri,
	vector<vector<KeyPoint>> keypoints_for_all,
	vector<Point3f> &cps,
	Point3f center3D, Vec3f size3D,
	vector<vector<int>> correspond)
{
	GLuint tex = glGenLists(1);
	int error = glGetError();
	if (error != GL_NO_ERROR)
		cout << "An OpenGL error has occured: " << gluErrorString(error) << endl;
	if (tex == 0) return 0;

	glNewList(tex, GL_COMPILE);

	glDisable(GL_BLEND);	// 禁用混合，即无透明度
	glEnable(GL_TEXTURE_2D);
	for (int order = 0; order < images.size(); order++) {

		if (order != 0 && order != 2 && order != 4 && order != 6 && order != 8 && order != 10) continue;	// 测试时只取2幅图
		cout << endl << "***" << order << "***" << endl;
		Point2f pt2D[3];
		Point3f pt3D[3];
		Mat texImg;
		Mat img = images[order];
		cvtColor(img, img, CV_BGR2RGB);
		resize(img, texImg, Size(256, 256)); // seems no need to do this
		int j = 0;
		vector<Vec3i>::iterator iterTri = tri[order].begin();
		for (; iterTri != tri[order].end(); iterTri++)
		{
			cout << ++j << "  ";
			//int flags = 0;
			
			Vec3i it_tri = *iterTri;

			for (int i = 0; i < 3; i++)
			{
				pt2D[i].x = keypoints_for_all[order][it_tri[i]].pt.x / img.cols;
				pt2D[i].y = keypoints_for_all[order][it_tri[i]].pt.y / img.rows;

				pt3D[i] = (cps[correspond[order][it_tri[i]]] - center3D) * (1.f / max(size3D[0], size3D[1]));
			}

//			if (!flags) {
				MapTexTri(texImg, pt2D, pt3D);
//			}
		}

		system("pause");
	}
	
	glDisable(GL_TEXTURE_2D);

	glEndList();
	return tex;

}

#define PI_180			(CV_PI/180)
#define ROTATE_STEP		5
#define TRANSLATE_STEP	.3

static float	g_rx, g_ry;
static float	/*g_tx, g_ty, */g_tz;
//static float	g_txBuf, g_tyBuf;
//static int		g_mouse_x, g_mouse_y;
//static bool		g_bLeftDown, g_bRightDown;
static GLuint	g_tex;


void InitGl()
{
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(600, 600);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("3D reconstruction");
	//	glEnable(GL_POINT_SMOOTH);
	//	glHint(GL_POINT_SMOOTH_HINT,GL_DONT_CARE);
	//	glPointSize(2.5);
	glClearColor(0, 0, 0, 1);
	//	glShadeModel(GL_FLAT);
	glutDisplayFunc(displayGl);
	glutReshapeFunc(resizeGl);
	glutKeyboardFunc(keyboard_control_Gl);
	glutSpecialFunc(special_control_Gl);
	glutMouseFunc(mouseGl);
	glutMotionFunc(mouse_move_Gl);

	//Init_lightGl();
	wglUseFontBitmaps(wglGetCurrentDC(), 0, 256, 1000);
	glEnable(GL_DEPTH_TEST);
}

void Init_lightGl()
{
	GLfloat light_position1[] = { 0.0, -3.0, -3.0, 0.0 };
	GLfloat light_ambient1[] = { 1, 1, 1, 1 };
	GLfloat light_diffuse1[] = { 0.80, 0.8, 0.8, 1 };
	GLfloat light_specular1[] = { 0.75, 0.75, 0.75, 1 };
	GLfloat light_shine1[] = { 50 };

	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient1);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse1);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular1);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position1);
	glLightfv(GL_LIGHT0, GL_SHININESS, light_shine1);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_AUTO_NORMAL);
	glEnable(GL_NORMALIZE);
}

void Show(GLuint tex, Point3f center3D, Vec3i size3D)
{
	//g_tx = 0;
	//g_ty = 0;
	g_tz = 2; // adjust camera position

	//g_txBuf = g_tyBuf = 0;
	g_rx = 90;
	g_ry = 0;
	//g_mouse_x = g_mouse_y = 0;
	//g_bLeftDown = g_bRightDown = false;

	g_tex = tex;

	glutMainLoop();
}

void displayGl()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	//glRotatef(g_ry, 1,0,0); // angles should be in degrees
	//glRotatef(g_rx, 0,1,0);
	//glTranslatef(-g_tx, -g_ty, -g_tz);
	//float len = abs(g_tz);
	float eyey = g_tz*sin(g_ry*PI_180),
		eyex = g_tz*cos(g_ry*PI_180)*cos(g_rx*PI_180),
		eyez = g_tz*cos(g_ry*PI_180)*sin(g_rx*PI_180);
	gluLookAt(eyex, eyey, eyez, 0, 0, 0, 0, 1, 0);
	ATLTRACE("%.1f,%.1f,%.1f,%.1f,%.1f\n", g_rx, g_ry, eyex, eyey, eyez);	// Reports warnings to an output device

	glColor3f(1, 1, 1);
	glCallList(g_tex);

	glPopMatrix();
	glPushMatrix();
	glColor3f(0, 1, 0);
	glTranslatef(-0.08, .08, -0.2);
	glListBase(1000);
	glRasterPos3f(0, 0, 0);
	string help = "use arrow keys to rotate, mouse wheel to zoom";
	glCallLists(help.size(), GL_UNSIGNED_BYTE, help.c_str());

	glPopMatrix();
	glFlush();
	glutSwapBuffers();
}

void resizeGl(int w, int h)
{
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45, GLdouble(w) / GLdouble(h), 0.01, 10000.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void mouseGl(int button, int state, int x, int y)
{
	switch (button)
	{
		//case GLUT_LEFT_BUTTON:
		//	if (state == GLUT_DOWN) 
		//	{
		//		g_bLeftDown = true;
		//		g_mouse_x = x;
		//		g_mouse_y = y;
		//	}
		//	else if (state == GLUT_UP) 
		//	{
		//		g_bLeftDown = false;
		//		
		//	}
		//	break;

		//case GLUT_RIGHT_BUTTON:
		//	if (state == GLUT_DOWN) 
		//	{
		//		g_bRightDown = true;
		//		g_mouse_x = x;
		//		g_mouse_y = y;
		//	}
		//	else if (state == GLUT_UP)
		//	{
		//		g_tx += g_txBuf;
		//		g_ty += g_tyBuf;
		//		g_txBuf = g_tyBuf = 0;
		//		g_bRightDown = false;
		//	}
		//	break;

	case GLUT_WHEEL_UP:
		g_tz -= TRANSLATE_STEP;
		break;

	case  GLUT_WHEEL_DOWN:
		g_tz += TRANSLATE_STEP;
		break;

	default:
		break;
	}
	if (g_tz < 0) g_tz = 0;
	glutPostRedisplay();
}

void mouse_move_Gl(int x, int y)
{
	/*if (g_bLeftDown)
	{

	}
	else if (g_bRightDown)
	{
	g_txBuf = (x-g_mouse_x)*TRANSLATE_STEP;
	g_tyBuf = (y-g_mouse_y)*TRANSLATE_STEP;
	}*/
	glutPostRedisplay();
}

void keyboard_control_Gl(unsigned char key, int a, int b)
{
	if (key == 0x1B)
		exit(1);
	glutPostRedisplay();
}

// 上下左右键同样可以控制视角
void special_control_Gl(int key, int x, int y)
{
	if (key == GLUT_KEY_LEFT)
	{
		g_rx -= ROTATE_STEP;
		//if (g_rx<1) g_rx = 1;
	}
	else if (key == GLUT_KEY_RIGHT)
	{
		g_rx += ROTATE_STEP;
		//if (g_rx >= 179) g_rx = 179;
	}
	else if (key == GLUT_KEY_UP)
	{
		g_ry -= ROTATE_STEP;
		//if (g_ry<-89) g_ry = -89;
	}
	else if (key == GLUT_KEY_DOWN)
	{
		g_ry += ROTATE_STEP;
		//if (g_ry >= 89) g_ry = 89;
	}
	glutPostRedisplay();
}
