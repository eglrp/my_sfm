#include "sfm_h.h"



//GLuint drawcube, drawteapot;
//
//void reshape(int w, int h)
//{
//	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
//	glMatrixMode(GL_PROJECTION);		// 对投影矩阵应用随后的矩阵操作
//	glLoadIdentity();					// 恢复坐标系，将之前矩阵变换导致变化过的栈顶矩阵重新归位，置为单位矩阵——等于是之前的矩阵变换带来的影响到此为止了
//	gluPerspective(95, w / h, 1, 40);  // 透视效果，参数——视椎体角度、比例、近裁面、远裁面
//	glMatrixMode(GL_MODELVIEW);
//	glLoadIdentity();
//	gluLookAt(0, 6, 40, 0, 0, 0, 0, 10, 0);//设置观察点
//}
//void init()
//{
//	glClearColor(0.8, 0.8, 0.8, 0.0);
//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//
//	//画出正方体显示列表
//	drawcube = glGenLists(1);
//	glNewList(drawcube, GL_COMPILE);
//	glutSolidCube(1);
//	glEndList();
//	//画出茶壶显示列表
//	drawteapot = glGenLists(1);
//	glNewList(drawteapot, GL_COMPILE);
//	glutSolidTeapot(1);
//	glEndList();
//
//	//设置普通灯光照0位置及参数；
//	GLfloat position0[] = { 20, 20, 6, 1 };
//	GLfloat light0s[] = { 0.10, 0.10, 0.10, 0.0 };
//	GLfloat light0d[] = { 0.6, 0.7, 0.7 };
//	GLfloat light0a[] = { 0.6, 0.6, 0.6 };
//	GLfloat spot_direction[] = { -1.0, -0.3, -1.0 };	// 光照方向 glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, spot_direction);
//	glLightfv(GL_LIGHT0, GL_POSITION, position0);	// 光源位置
//	glLightfv(GL_LIGHT0, GL_SPECULAR, light0s);		// 光源中的镜面反射光强度
//	glLightfv(GL_LIGHT0, GL_DIFFUSE, light0d);		// 光源中的散射光强度
//	glLightfv(GL_LIGHT0, GL_AMBIENT, light0a);		// 光源中的环境光强度 
//
//	glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, 30.0);		// 定义聚光截止角
//	glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, spot_direction);	// 聚光方向
//
//	glEnable(GL_LIGHTING);
//	glEnable(GL_DEPTH_TEST);
//	glEnable(GL_LIGHT0);
//}





/*
*	该函数利用opencv显示点云
*/
void show_cps(void) {
	//glEnable(GL_COLOR_MATERIAL);		// 激活GL_COLOR_MATERIAL可以在激活光照的情况下用glColor函数给物体上色。这个方法又称为颜色追踪。
	//glColorMaterial(GL_FRONT, GL_AMBIENT);


	FILE *fp = NULL;

	fp = fopen(cp_filename, "r");//其中cp_filename代表我们点云文件的位置
	float	x, y, z;	// 坐标
	unsigned int b, g, r;	// 颜色
	float fb, fg, fr;

	while (!feof(fp))
	{
		fscanf(fp, "%f %f %f %u %u %u\n", &x, &y, &z, &b, &g, &r);//读取6个数据并赋予fx，fy，fz，x，y，z。
		// 在读取的bgr代表颜色的数据为0 - 255的整数，而在OpenGL中对颜色的表示为0 - 1之间。故为bgr除以255。
		fb = (float)b / 255;
		fg = (float)g / 255;
		fr = (float)r / 255;

		// 通过OpenGL显示。
		glPointSize(1.0f);
		glColor3f(fr, fg, fb);	// glcolor3f是rgb的顺序
		glBegin(GL_POINTS);
			glVertex3f(x, y, z);
		glEnd();
	}

	//glDisable(GL_COLOR_MATERIAL);		// enable后disable
	glFlush();	// 立即执行

	fclose(fp);
}

void gl_main() {
	glutInit(NULL, NULL);
	glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE | GLUT_DEPTH);	//设置窗口初始显示模式
	glutInitWindowSize(1200, 600);		//显示框大小
	glutInitWindowPosition(200, 400);	//确定显示框左上角的位置
	glutCreateWindow("点云");
	//init();
	glutDisplayFunc(show_cps);	//glutDisplayFunc是显示的时候调用，可见改变窗口后调用glutReshapeFunc后还是会调用glutDisplayFunc进行显示的。
	//glutReshapeFunc(reshape);	//glutReshapeFunc是窗口改变的时候调用的函数，在这个里面可以根据缩放后的窗口重新设置
	glutMainLoop();				//启动主GLUT事件处理循环
}