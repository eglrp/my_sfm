#include "sfm_h.h"



//GLuint drawcube, drawteapot;
//
//void reshape(int w, int h)
//{
//	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
//	glMatrixMode(GL_PROJECTION);		// ��ͶӰ����Ӧ�����ľ������
//	glLoadIdentity();					// �ָ�����ϵ����֮ǰ����任���±仯����ջ���������¹�λ����Ϊ��λ���󡪡�������֮ǰ�ľ���任������Ӱ�쵽��Ϊֹ��
//	gluPerspective(95, w / h, 1, 40);  // ͸��Ч��������������׵��Ƕȡ������������桢Զ����
//	glMatrixMode(GL_MODELVIEW);
//	glLoadIdentity();
//	gluLookAt(0, 6, 40, 0, 0, 0, 0, 10, 0);//���ù۲��
//}
//void init()
//{
//	glClearColor(0.8, 0.8, 0.8, 0.0);
//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//
//	//������������ʾ�б�
//	drawcube = glGenLists(1);
//	glNewList(drawcube, GL_COMPILE);
//	glutSolidCube(1);
//	glEndList();
//	//���������ʾ�б�
//	drawteapot = glGenLists(1);
//	glNewList(drawteapot, GL_COMPILE);
//	glutSolidTeapot(1);
//	glEndList();
//
//	//������ͨ�ƹ���0λ�ü�������
//	GLfloat position0[] = { 20, 20, 6, 1 };
//	GLfloat light0s[] = { 0.10, 0.10, 0.10, 0.0 };
//	GLfloat light0d[] = { 0.6, 0.7, 0.7 };
//	GLfloat light0a[] = { 0.6, 0.6, 0.6 };
//	GLfloat spot_direction[] = { -1.0, -0.3, -1.0 };	// ���շ��� glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, spot_direction);
//	glLightfv(GL_LIGHT0, GL_POSITION, position0);	// ��Դλ��
//	glLightfv(GL_LIGHT0, GL_SPECULAR, light0s);		// ��Դ�еľ��淴���ǿ��
//	glLightfv(GL_LIGHT0, GL_DIFFUSE, light0d);		// ��Դ�е�ɢ���ǿ��
//	glLightfv(GL_LIGHT0, GL_AMBIENT, light0a);		// ��Դ�еĻ�����ǿ�� 
//
//	glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, 30.0);		// ����۹��ֹ��
//	glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, spot_direction);	// �۹ⷽ��
//
//	glEnable(GL_LIGHTING);
//	glEnable(GL_DEPTH_TEST);
//	glEnable(GL_LIGHT0);
//}





/*
*	�ú�������opencv��ʾ����
*/
void show_cps(void) {
	//glEnable(GL_COLOR_MATERIAL);		// ����GL_COLOR_MATERIAL�����ڼ�����յ��������glColor������������ɫ����������ֳ�Ϊ��ɫ׷�١�
	//glColorMaterial(GL_FRONT, GL_AMBIENT);


	FILE *fp = NULL;

	fp = fopen(cp_filename, "r");//����cp_filename�������ǵ����ļ���λ��
	float	x, y, z;	// ����
	unsigned int b, g, r;	// ��ɫ
	float fb, fg, fr;

	while (!feof(fp))
	{
		fscanf(fp, "%f %f %f %u %u %u\n", &x, &y, &z, &b, &g, &r);//��ȡ6�����ݲ�����fx��fy��fz��x��y��z��
		// �ڶ�ȡ��bgr������ɫ������Ϊ0 - 255������������OpenGL�ж���ɫ�ı�ʾΪ0 - 1֮�䡣��Ϊbgr����255��
		fb = (float)b / 255;
		fg = (float)g / 255;
		fr = (float)r / 255;

		// ͨ��OpenGL��ʾ��
		glPointSize(1.0f);
		glColor3f(fr, fg, fb);	// glcolor3f��rgb��˳��
		glBegin(GL_POINTS);
			glVertex3f(x, y, z);
		glEnd();
	}

	//glDisable(GL_COLOR_MATERIAL);		// enable��disable
	glFlush();	// ����ִ��

	fclose(fp);
}

void gl_main() {
	glutInit(NULL, NULL);
	glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE | GLUT_DEPTH);	//���ô��ڳ�ʼ��ʾģʽ
	glutInitWindowSize(1200, 600);		//��ʾ���С
	glutInitWindowPosition(200, 400);	//ȷ����ʾ�����Ͻǵ�λ��
	glutCreateWindow("����");
	//init();
	glutDisplayFunc(show_cps);	//glutDisplayFunc����ʾ��ʱ����ã��ɼ��ı䴰�ں����glutReshapeFunc���ǻ����glutDisplayFunc������ʾ�ġ�
	//glutReshapeFunc(reshape);	//glutReshapeFunc�Ǵ��ڸı��ʱ����õĺ����������������Ը������ź�Ĵ�����������
	glutMainLoop();				//������GLUT�¼�����ѭ��
}