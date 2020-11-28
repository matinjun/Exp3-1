/*
*        Computer Graphics Course - Shenzhen University
*      Week 6 - Camera Position and Control Skeleton Code
* ============================================================
*
* - 本代码仅仅是参考代码，具体要求请参考作业说明，按照顺序逐步完成。
* - 关于配置OpenGL开发环境、编译运行，请参考第一周实验课程相关文档。
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

std::vector<vec3> points;   //传入着色器的绘制点
std::vector<vec3> colors;   //传入着色器的颜色

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

// TODO 请按照实验课内容补全相机参数
// 相机参数
double Delta = M_PI / 2.0;
double Theta = M_PI / 2.0;
double R = 1.0;

namespace Camera
{
    mat4 modelMatrix;
    mat4 viewMatrix;

	mat4 lookAt( const vec4& eye, const vec4& at, const vec4& up )
	{
		// TODO 请按照实验课内容补全相机观察矩阵的计算
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
// OpenGL 初始化

void init()
{
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	// 从外部读取三维模型文件
	mesh->read_off("cube.off");

	// 将顶点信息存入vs，面片信息存入fs
	std::vector<vec3f> vs = mesh->v();
	std::vector<vec3i> fs = mesh->f();

	points.clear();
	colors.clear();

	// 解析vs与fs内的内容，存入容器便于绘制
	for (int i = 0; i < fs.size(); i++)
	{
		points.push_back(vs[fs[i].a]);
		colors.push_back(vertex_colors[fs[i].a]);

		points.push_back(vs[fs[i].b]);
		colors.push_back(vertex_colors[fs[i].b]);

		points.push_back(vs[fs[i].c]);
		colors.push_back(vertex_colors[fs[i].c]);

	}

	// 加载shader并且获取变量的位置
	programID = InitShader("vshader.glsl", "fshader.glsl");
	vPositionID = glGetAttribLocation(programID, "vPosition");
	vColorID = glGetAttribLocation(programID, "vColor");
	modelViewMatrixID = glGetUniformLocation(programID, "modelViewMatrix");

	// 生成VAO
	glGenVertexArrays(1, &vertexArrayID);
	glBindVertexArray(vertexArrayID);

	// 创建并初始化顶点缓存对象
	GLuint buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, points.size() * sizeof(vec3) + colors.size() * sizeof(vec3), NULL, GL_STATIC_DRAW);

	// 分别读取数据
	glBufferSubData(GL_ARRAY_BUFFER, 0, points.size() * sizeof(vec3), &points[0]);
	glBufferSubData(GL_ARRAY_BUFFER, points.size() * sizeof(vec3), colors.size() * sizeof(vec3), &colors[0]);

	// 读取着色器并使用
	glUseProgram(programID);

	// 从顶点着色器中初始化顶点的位置
	GLuint pLocation = glGetAttribLocation(programID, "vPosition");
	glEnableVertexAttribArray(pLocation);
	glVertexAttribPointer(pLocation, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	// 从片元着色器中初始化顶点的颜色
	GLuint cLocation = glGetAttribLocation(programID, "vColor");
	glEnableVertexAttribArray(cLocation);
	glVertexAttribPointer(cLocation, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(points.size() * sizeof(vec3)));

	// 获得矩阵位置
	modelViewMatrixID = glGetUniformLocation(programID, "modelViewMatrix");

	// OpenGL相应状态设置
	glEnable(GL_LIGHTING);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
}

//////////////////////////////////////////////////////////////////////////
// 渲染

void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(programID);

	// TODO 设置相机参数

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
// 重新设置窗口

void reshape(GLsizei w, GLsizei h)
{
	glViewport(0, 0, w, h);
}

//////////////////////////////////////////////////////////////////////////
// 鼠标响应函数

void mouse(int button, int state, int x, int y)
{
	return;
}

//////////////////////////////////////////////////////////////////////////
// 键盘响应函数

// 更新Delta
void updateDelta(int sign, double step) {
	Delta += sign * step;
	
}

// 更新Theta
void updateTheta(int sign, double step) {
	Theta += sign * step;
}

void keyboard(unsigned char key, int x, int y)
{
	switch(key) 
	{
	case 033:	// ESC键 和 'q' 键退出游戏
		exit(EXIT_SUCCESS);
		break;
	case 'q':
		exit (EXIT_SUCCESS);
		break;
	// Todo：键盘控制相机的位置和朝向
	// w, s --> R距离
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
		// 周期为2 * M_PI
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