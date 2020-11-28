/*
*        Computer Graphics Course - Shenzhen University
*      Week 6 - Camera Position and Control Skeleton Code
* ============================================================
*
* - ����������ǲο����룬����Ҫ����ο���ҵ˵��������˳������ɡ�
* - ��������OpenGL�����������������У���ο���һ��ʵ��γ�����ĵ���
*/

#include "include/Angel.h"
#include "include/TriMesh.h"

//#pragma comment(lib, "glew32.lib")

#include <cstdlib>
#include <cmath>
#include <iostream>

using namespace std;

GLuint programID;
GLuint vertexArrayID;
GLuint vertexBufferID;
GLuint vertexIndexBuffer;

GLuint vPositionID;
GLuint vColorID;
GLuint modelViewMatrixID;

TriMesh* mesh = new TriMesh();
const int NUM_VERTICES = 8;

std::vector<vec3> points;   //������ɫ���Ļ��Ƶ�
std::vector<vec3> colors;   //������ɫ������ɫ

const vec3 vertex_colors[NUM_VERTICES] = {
	vec3(1.0, 1.0, 1.0),  // White
	vec3(1.0, 1.0, 0.0),  // Yellow
	vec3(0.0, 1.0, 0.0),  // Green
	vec3(0.0, 1.0, 1.0),  // Cyan
	vec3(1.0, 0.0, 1.0),  // Magenta
	vec3(1.0, 0.0, 0.0),  // Red
	vec3(0.0, 0.0, 0.0),  // Black
	vec3(0.0, 0.0, 1.0)   // Blue
};

// TODO �밴��ʵ������ݲ�ȫ�������
// �������
double Delta = M_PI / 2.0;
double Theta = M_PI / 2.0;
double R = 1.0;

namespace Camera
{
    mat4 modelMatrix;
    mat4 viewMatrix;

	mat4 lookAt( const vec4& eye, const vec4& at, const vec4& up )
	{
		// TODO �밴��ʵ������ݲ�ȫ����۲����ļ���
		vec4  n;
		vec4  u;
		vec4  v;

		n = eye - at;
		n = normalize(n);
		u = vec4(cross(up, n), 0.0);
		u = normalize(u);
		v = vec4(cross(n, u), 0.0);
		v = normalize(v);

		return mat4(u, v, n, vec4(0.0, 0.0, 0.0, 1.0)) * 
			mat4(1.0, 0, 0, 0,
				0, 1.0, 0, 0,
				0, 0, 1.0, 0,
				-eye.x, -eye.y, -eye.z, 1.0);
	}
}

//////////////////////////////////////////////////////////////////////////
// OpenGL ��ʼ��

void init()
{
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	// ���ⲿ��ȡ��άģ���ļ�
	mesh->read_off("cube.off");

	// ��������Ϣ����vs����Ƭ��Ϣ����fs
	std::vector<vec3f> vs = mesh->v();
	std::vector<vec3i> fs = mesh->f();

	points.clear();
	colors.clear();

	// ����vs��fs�ڵ����ݣ������������ڻ���
	for (int i = 0; i < fs.size(); i++)
	{
		points.push_back(vs[fs[i].a]);
		colors.push_back(vertex_colors[fs[i].a]);

		points.push_back(vs[fs[i].b]);
		colors.push_back(vertex_colors[fs[i].b]);

		points.push_back(vs[fs[i].c]);
		colors.push_back(vertex_colors[fs[i].c]);

	}

	// ����shader���һ�ȡ������λ��
	programID = InitShader("vshader.glsl", "fshader.glsl");
	vPositionID = glGetAttribLocation(programID, "vPosition");
	vColorID = glGetAttribLocation(programID, "vColor");
	modelViewMatrixID = glGetUniformLocation(programID, "modelViewMatrix");

	// ����VAO
	glGenVertexArrays(1, &vertexArrayID);
	glBindVertexArray(vertexArrayID);

	// ��������ʼ�����㻺�����
	GLuint buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, points.size() * sizeof(vec3) + colors.size() * sizeof(vec3), NULL, GL_STATIC_DRAW);

	// �ֱ��ȡ����
	glBufferSubData(GL_ARRAY_BUFFER, 0, points.size() * sizeof(vec3), &points[0]);
	glBufferSubData(GL_ARRAY_BUFFER, points.size() * sizeof(vec3), colors.size() * sizeof(vec3), &colors[0]);

	// ��ȡ��ɫ����ʹ��
	glUseProgram(programID);

	// �Ӷ�����ɫ���г�ʼ�������λ��
	GLuint pLocation = glGetAttribLocation(programID, "vPosition");
	glEnableVertexAttribArray(pLocation);
	glVertexAttribPointer(pLocation, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	// ��ƬԪ��ɫ���г�ʼ���������ɫ
	GLuint cLocation = glGetAttribLocation(programID, "vColor");
	glEnableVertexAttribArray(cLocation);
	glVertexAttribPointer(cLocation, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(points.size() * sizeof(vec3)));

	// ��þ���λ��
	modelViewMatrixID = glGetUniformLocation(programID, "modelViewMatrix");

	// OpenGL��Ӧ״̬����
	glEnable(GL_LIGHTING);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
}

//////////////////////////////////////////////////////////////////////////
// ��Ⱦ

void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(programID);

	// TODO �����������

	vec4 eye = vec4(R * sin(Delta) * cos(Theta), R * cos(Delta), R * sin(Delta) * sin(Theta), 0);
	vec4 at = vec4(0, 0, 0, 0);
	vec4 up = vec4(0, 1, 0, 0);
	
	Camera::modelMatrix = mat4(1.0);
	Camera::viewMatrix = Camera::lookAt(eye, at, up);
	
	mat4 modelViewMatrix = Camera::viewMatrix * Camera::modelMatrix;
	glUniformMatrix4fv(modelViewMatrixID, 1, GL_TRUE, &modelViewMatrix[0][0]);

	glDrawArrays(GL_TRIANGLES, 0, points.size());
	glutSwapBuffers();
}

//////////////////////////////////////////////////////////////////////////
// �������ô���

void reshape(GLsizei w, GLsizei h)
{
	glViewport(0, 0, w, h);
}

//////////////////////////////////////////////////////////////////////////
// �����Ӧ����

void mouse(int button, int state, int x, int y)
{
	return;
}

//////////////////////////////////////////////////////////////////////////
// ������Ӧ����

// ����Delta
void updateDelta(int sign, double step) {
	Delta += sign * step;
	
}

// ����Theta
void updateTheta(int sign, double step) {
	Theta += sign * step;
}

void keyboard(unsigned char key, int x, int y)
{
	switch(key) 
	{
	case 033:	// ESC�� �� 'q' ���˳���Ϸ
		exit(EXIT_SUCCESS);
		break;
	case 'q':
		exit (EXIT_SUCCESS);
		break;
	// Todo�����̿��������λ�úͳ���
	// w, s --> R����
	// e, d --> Delta
	// r, f --> Theta
	case 'w':
		R += 0.1;
		break;
	case 's':
		R -= 0.1;
		break;
	case 'e':
		updateDelta(1, 0.1);
		break;
	case 'd':
		updateDelta(-1, 0.1);
		break;
	case 'r':
		updateTheta(1, 0.1);
		// ����Ϊ2 * M_PI
		break;
	case 'f':
		updateTheta(-1, 0.1);
		break;
	}
	glutPostRedisplay();
}

//////////////////////////////////////////////////////////////////////////

void idle(void)
{
	glutPostRedisplay();
}

//////////////////////////////////////////////////////////////////////////

void clean()
{
	glDeleteBuffers(1, &vertexBufferID);
	glDeleteProgram(programID);
	glDeleteVertexArrays(1, &vertexArrayID);

	if (mesh) {
		delete mesh;
		mesh = NULL;
	}
}

int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	glutInitWindowSize(500, 500);
	glutCreateWindow("OpenGL-Tutorial");

	glewInit();
	init();

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutMouseFunc(mouse);
	glutKeyboardFunc(keyboard);
	glutIdleFunc(idle);

	glutMainLoop();

	clean();

	return 0;
}