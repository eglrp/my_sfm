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
//	glMatrixMode(GL_PROJECTION);		// ��ͶӰ����Ӧ�����ľ������
//	glLoadIdentity();					// �ָ�����ϵ����֮ǰ����任���±仯����ջ���������¹�λ����Ϊ��λ���󡪡�������֮ǰ�ľ���任������Ӱ�쵽��Ϊֹ��
//	gluPerspective(95, w / h, 1, 40);  // ͸��Ч��������������׵��Ƕȡ������������桢Զ����
//	glMatrixMode(GL_MODELVIEW);
//	glLoadIdentity();					//replace the current matrix with the identity matrix  
//	//gluLookAt(0, 6, 40, 0, 0, 0, 0, 10, 0);//���ù۲��
//}
//void init()
//{
//	glClearColor(1, 1, 1, 0.0);	// ��ɫ����
//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//
//	////������������ʾ�б�
//	//drawcube = glGenLists(1);
//	//glNewList(drawcube, GL_COMPILE);
//	//glutSolidCube(1);
//	//glEndList();
//	////���������ʾ�б�
//	//drawteapot = glGenLists(1);
//	//glNewList(drawteapot, GL_COMPILE);
//	//glutSolidTeapot(1);
//	//glEndList();
//
//	////������ͨ�ƹ���0λ�ü�������
//	//GLfloat position0[] = { 20, 20, 6, 1 };
//	//GLfloat light0s[] = { 0.10, 0.10, 0.10, 0.0 };
//	//GLfloat light0d[] = { 0.6, 0.7, 0.7 };
//	//GLfloat light0a[] = { 0.6, 0.6, 0.6 };
//	//GLfloat spot_direction[] = { -1.0, -0.3, -1.0 };	// ���շ��� glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, spot_direction);
//	//glLightfv(GL_LIGHT0, GL_POSITION, position0);	// ��Դλ��
//	//glLightfv(GL_LIGHT0, GL_SPECULAR, light0s);		// ��Դ�еľ��淴���ǿ��
//	//glLightfv(GL_LIGHT0, GL_DIFFUSE, light0d);		// ��Դ�е�ɢ���ǿ��
//	//glLightfv(GL_LIGHT0, GL_AMBIENT, light0a);		// ��Դ�еĻ�����ǿ�� 
//
//	//glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, 30.0);		// ����۹��ֹ��
//	//glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, spot_direction);	// �۹ⷽ��
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
//*	�ú�������opencv��ʾ����
//*/
//void show_cps(void) {
//	//glEnable(GL_COLOR_MATERIAL);		// ����GL_COLOR_MATERIAL�����ڼ�����յ��������glColor������������ɫ����������ֳ�Ϊ��ɫ׷�١�
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
//	err = fopen_s(&fp, read_filename, "r"); //fp = fopen(read_filename, "r");����read_filename�������ǵ����ļ���λ��
//	float	x, y, z;	// ����
//	unsigned int b, g, r;	// ��ɫ
//	float fb, fg, fr;
//
//	int level = 10;
//
//	// TODO
//	glPointSize(2.0f);		// ����ÿ����Ĵ�СΪ��������
//	glBegin(GL_POINTS);
//	while (!feof(fp))
//	{
//		fscanf_s(fp, "%f %f %f %u %u %u\n", &x, &y, &z, &b, &g, &r);//��ȡ6�����ݲ�����fx��fy��fz��x��y��z��
//		cout << x << " " << y << " " << z << " " << r << " " << g << " " << b << endl;
// 		// �ڶ�ȡ��bgr������ɫ������Ϊ0 - 255������������OpenGL�ж���ɫ�ı�ʾΪ0 - 1֮�䡣��Ϊbgr����255��
//		fb = (float)b / 255;
//		fg = (float)g / 255;
//		fr = (float)r / 255;
//
//		// ͨ��OpenGL��ʾ��
//		
//		glColor3f(fr, fg, fb);	// glcolor3f��rgb��˳��
//		glVertex3f(x*level, y*level, z*level);
//	}
//	glEnd();
//
//
//
//	//glDisable(GL_COLOR_MATERIAL);		// enable��disable
//	glFlush();	// ����ִ��
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
//opengl������	
//*/
//void gl_main(int argc, char *argv[]) {
//	glutInit(&argc, argv);
//	glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE | GLUT_DEPTH);	//���ô��ڳ�ʼ��ʾģʽ
//	glutInitWindowSize(800, 400);		//��ʾ���С
//	glutInitWindowPosition(50, 100);	//ȷ����ʾ�����Ͻǵ�λ��
//	glutCreateWindow("����");
//	init();		// ��ʼ��
//	glutDisplayFunc(show_cps);	//glutDisplayFunc����ʾ��ʱ����ã��ɼ��ı䴰�ں����glutReshapeFunc���ǻ����glutDisplayFunc������ʾ�ġ�
//	glutReshapeFunc(reshape);	//glutReshapeFunc�Ǵ��ڸı��ʱ����õĺ����������������Ը������ź�Ĵ�����������
//
//	glutMouseFunc(mouse);
//	glutMotionFunc(motion);
//	glutSpecialFunc(special);
//
//	glutMainLoop();				//������GLUT�¼�����ѭ��
//}