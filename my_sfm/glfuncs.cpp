//#include "sfm_h.h"
//
//
//#define pi 3.1415926  
//bool mouseisdown = false;
//bool loopr = false;
//int mx=0, my=0;
//int ry = 10;
//int rx = 10;
//
//
//
//
////GLuint drawcube, drawteapot;
////
//void reshape(int w, int h)
//{
//	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
//	glMatrixMode(GL_PROJECTION);		// 对投影矩阵应用随后的矩阵操作
//	glLoadIdentity();					// 恢复坐标系，将之前矩阵变换导致变化过的栈顶矩阵重新归位，置为单位矩阵——等于是之前的矩阵变换带来的影响到此为止了
//	gluPerspective(95, w / h, 1, 40);  // 透视效果，参数——视椎体角度、比例、近裁面、远裁面
//	glMatrixMode(GL_MODELVIEW);
//	glLoadIdentity();					//replace the current matrix with the identity matrix  
//	//gluLookAt(0, 6, 40, 0, 0, 0, 0, 10, 0);//设置观察点
//}
//void init()
//{
//	glClearColor(1, 1, 1, 0.0);	// 白色背景
//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//
//	////画出正方体显示列表
//	//drawcube = glGenLists(1);
//	//glNewList(drawcube, GL_COMPILE);
//	//glutSolidCube(1);
//	//glEndList();
//	////画出茶壶显示列表
//	//drawteapot = glGenLists(1);
//	//glNewList(drawteapot, GL_COMPILE);
//	//glutSolidTeapot(1);
//	//glEndList();
//
//	////设置普通灯光照0位置及参数；
//	//GLfloat position0[] = { 20, 20, 6, 1 };
//	//GLfloat light0s[] = { 0.10, 0.10, 0.10, 0.0 };
//	//GLfloat light0d[] = { 0.6, 0.7, 0.7 };
//	//GLfloat light0a[] = { 0.6, 0.6, 0.6 };
//	//GLfloat spot_direction[] = { -1.0, -0.3, -1.0 };	// 光照方向 glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, spot_direction);
//	//glLightfv(GL_LIGHT0, GL_POSITION, position0);	// 光源位置
//	//glLightfv(GL_LIGHT0, GL_SPECULAR, light0s);		// 光源中的镜面反射光强度
//	//glLightfv(GL_LIGHT0, GL_DIFFUSE, light0d);		// 光源中的散射光强度
//	//glLightfv(GL_LIGHT0, GL_AMBIENT, light0a);		// 光源中的环境光强度 
//
//	//glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, 30.0);		// 定义聚光截止角
//	//glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, spot_direction);	// 聚光方向
//
//	//glEnable(GL_LIGHTING);
//	//glEnable(GL_DEPTH_TEST);
//	//glEnable(GL_LIGHT0);
//}
//
//
//
//
//
///*
//*	该函数利用opencv显示点云
//*/
//void show_cps(void) {
//	//glEnable(GL_COLOR_MATERIAL);		// 激活GL_COLOR_MATERIAL可以在激活光照的情况下用glColor函数给物体上色。这个方法又称为颜色追踪。
//	//glColorMaterial(GL_FRONT, GL_AMBIENT);
//
//	glClear(GL_COLOR_BUFFER_BIT);
//	glLoadIdentity();// Reset the coordinate system before modifying  
//	gluLookAt(0.0, 0.0, 7.0, 0.0, 0.0, 1.0, 0.0, 1.0, 0.0);
//	glRotatef(ry, 0, 1, 0);
//	glRotatef(rx - 180, 1, 0, 0);
//
//
//
//
//	errno_t err;
//	FILE *fp = NULL;
//
//	err = fopen_s(&fp, read_filename, "r"); //fp = fopen(read_filename, "r");其中read_filename代表我们点云文件的位置
//	float	x, y, z;	// 坐标
//	unsigned int b, g, r;	// 颜色
//	float fb, fg, fr;
//
//	int level = 10;
//
//	// TODO
//	glPointSize(2.0f);		// 设置每个点的大小为多少像素
//	glBegin(GL_POINTS);
//	while (!feof(fp))
//	{
//		fscanf_s(fp, "%f %f %f %u %u %u\n", &x, &y, &z, &b, &g, &r);//读取6个数据并赋予fx，fy，fz，x，y，z。
//		cout << x << " " << y << " " << z << " " << r << " " << g << " " << b << endl;
// 		// 在读取的bgr代表颜色的数据为0 - 255的整数，而在OpenGL中对颜色的表示为0 - 1之间。故为bgr除以255。
//		fb = (float)b / 255;
//		fg = (float)g / 255;
//		fr = (float)r / 255;
//
//		// 通过OpenGL显示。
//		
//		glColor3f(fr, fg, fb);	// glcolor3f是rgb的顺序
//		glVertex3f(x*level, y*level, z*level);
//	}
//	glEnd();
//
//
//
//	//glDisable(GL_COLOR_MATERIAL);		// enable后disable
//	glFlush();	// 立即执行
//
//	fclose(fp);
//}
//
//
//void test_display() {
//	
//}
//
//
//
//
//
//
//
//
//void timer(int p)
//{
//	ry -= 5;
//	//marks the current window as needing to be redisplayed.  
//	glutPostRedisplay();
//	if (loopr)
//		glutTimerFunc(100, timer, 0);
//}
//
//
//void mouse(int button, int state, int x, int y)
//{
//	if (button == GLUT_LEFT_BUTTON)
//	{
//		if (state == GLUT_DOWN)
//		{
//			mouseisdown = true;
//			loopr = false;
//		}
//		else
//		{
//			mouseisdown = false;
//		}
//	}
//
//	if (button == GLUT_RIGHT_BUTTON)
//	if (state == GLUT_DOWN)
//	{
//		loopr = true; glutTimerFunc(200, timer, 0);
//	}
//}
//
//void motion(int x, int y)
//{
//	if (mouseisdown == true)
//	{
//		ry += x - mx;
//		rx += y - my;
//		mx = x;
//		my = y;
//		glutPostRedisplay();
//	}
//}
//
//void special(int key, int x, int y)
//{
//	switch (key)
//	{
//	case GLUT_KEY_LEFT:
//		ry -= 5;
//		glutPostRedisplay();
//		break;
//	case GLUT_KEY_RIGHT:
//		ry += 5;
//		glutPostRedisplay();
//		break;
//	case GLUT_KEY_UP:
//		rx += 5;
//		glutPostRedisplay();
//		break;
//	case GLUT_KEY_DOWN:
//		rx -= 5;
//		glutPostRedisplay();
//		break;
//	}
//}
//
//
//
//
//
//
//
//
//
//
//
//
//
//
///*
//opengl主函数	
//*/
//void gl_main(int argc, char *argv[]) {
//	glutInit(&argc, argv);
//	glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE | GLUT_DEPTH);	//设置窗口初始显示模式
//	glutInitWindowSize(800, 400);		//显示框大小
//	glutInitWindowPosition(50, 100);	//确定显示框左上角的位置
//	glutCreateWindow("点云");
//	init();		// 初始化
//	glutDisplayFunc(show_cps);	//glutDisplayFunc是显示的时候调用，可见改变窗口后调用glutReshapeFunc后还是会调用glutDisplayFunc进行显示的。
//	glutReshapeFunc(reshape);	//glutReshapeFunc是窗口改变的时候调用的函数，在这个里面可以根据缩放后的窗口重新设置
//
//	glutMouseFunc(mouse);
//	glutMotionFunc(motion);
//	glutSpecialFunc(special);
//
//	glutMainLoop();				//启动主GLUT事件处理循环
//}