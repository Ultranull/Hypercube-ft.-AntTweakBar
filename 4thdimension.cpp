//built in includes
#include<iostream>
#include <stdio.h>
#include <string>
#include <sstream>
#include <fstream>
#include <vector>
//windows include
#include<windows.h>
//openGL includes
#include<GL/glew.h>
#include<GLFW/glfw3.h>
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtx/transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include<glm/gtc/quaternion.hpp>
#include<AntTweakBar.h>


using namespace std;
using namespace glm;

struct {
	float xz = 0,
		xy = 0,
		yz = 0,
		xw = 0,
		yw = 0,
		zw = 0;
	float fixed = 0;
	bool dfc = false;
	quat rot3D;
	quat rot4D;
}options;


GLuint loadshaders(const char *vertexfile, const char *fragmentfile);

inline void TwEventMouseButtonGLFW3(GLFWwindow* window, int button, int action, int mods) { TwEventMouseButtonGLFW(button, action); }
inline void TwEventMousePosGLFW3(GLFWwindow* window, double xpos, double ypos) { TwMouseMotion(int(xpos), int(ypos)); }
inline void TwEventMouseWheelGLFW3(GLFWwindow* window, double xoffset, double yoffset) { TwEventMouseWheelGLFW(yoffset); }
inline void TwEventKeyGLFW3(GLFWwindow* window, int key, int scancode, int action, int mods) { TwEventKeyGLFW(key, action); }
inline void TwEventCharGLFW3(GLFWwindow* window, int codepoint) { TwEventCharGLFW(codepoint, GLFW_PRESS); }
void TW_CALL buttonCallback(void *clientData) {
	string action((char*)clientData);
	if (action.compare("reset") == 0) {
		options.rot3D = quat(0, 0, 0, 0);
		options.rot4D = quat(0, 0, 0, 0);
	}

	if (action.compare("exit") == 0) {
		TwTerminate();
		glfwTerminate();
		exit(0);
	}

	if (action.compare("dfc") == 0) {
		options.dfc = !options.dfc;
	}

}
void antTweakInit(GLFWwindow *window) {
	TwInit(TW_OPENGL, NULL);

	glfwSetMouseButtonCallback(window, (GLFWmousebuttonfun)TwEventMouseButtonGLFW3);
	glfwSetCursorPosCallback(window, (GLFWcursorposfun)TwEventMousePosGLFW3);
	glfwSetScrollCallback(window, (GLFWscrollfun)TwEventMouseWheelGLFW3);
	glfwSetKeyCallback(window, (GLFWkeyfun)TwEventKeyGLFW3);
	glfwSetCharCallback(window, (GLFWcharfun)TwEventCharGLFW3);

	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	TwWindowSize(width, height);

	TwBar *main;
	main = TwNewBar("Options");
	TwAddVarRW(main, "3D rotation", TW_TYPE_QUAT4F, &options.rot3D, "opened=true help='rotations in the x,y,z axis'");
	TwAddVarRW(main, "4D rotation", TW_TYPE_QUAT4F, &options.rot4D, "opened=true help='rotations across the xw,yw,zw planes'");
	TwAddButton(main, "dump 4D component", buttonCallback, (void*)"dfc", "");
	TwAddButton(main, "reset", buttonCallback, (void*)"reset", "");
	TwAddButton(main, "exit", buttonCallback, (void*)"exit", "");
	TwDefine(" GLOBAL fontsize=5 ");
	
}
void OnReshape(GLFWwindow *window,int width, int height) {
	glViewport(0, 0, width, height);
	TwWindowSize(width, height);
}

int main() {
	GLFWwindow *window;
	if (!glfwInit()) {
		fprintf(stderr, "Failed to initialize glfw\n");
		getchar();
		exit(-1);
	}
	window = glfwCreateWindow(900, 900, "test", NULL, NULL);
	glfwWindowHint(GLFW_SAMPLES, 4);
	glEnable(GL_MULTISAMPLE);
	if (window == nullptr) {
		fprintf(stderr, "Failed to init window\n");
		getchar();
		glfwTerminate();
		exit(-1);
	}
	glfwMakeContextCurrent(window);
	glewExperimental = true;
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to load glew\n");
		getchar();
		glfwTerminate();
		exit(-1);
	}
	glfwSetWindowSizeCallback(window, OnReshape);
	antTweakInit(window);
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	GLuint program = loadshaders("vertex.vert", "fragment.frag");
	glUseProgram(program);

	float w = .5f;
	vector<vec4> vertexBuffer = {
		{-w,-w,-w, w },
		{ w,-w,-w, w },
		{ w, w,-w, w },
		{-w, w,-w, w },
		{-w,-w, w, w },
		{ w,-w, w, w },
		{ w, w, w, w },
		{-w, w, w, w },
		{-w,-w,-w,-w },
		{ w,-w,-w,-w },
		{ w, w,-w,-w },
		{-w, w,-w,-w },
		{-w,-w, w,-w },
		{ w,-w, w,-w },
		{ w, w, w,-w },
		{-w, w, w,-w }
	};
	vector<vec3> colorBuffer = {
		{ 1,0,0 },
		{ 1,0,0 },
		{ 1,0,0 },
		{ 1,0,0 },
		{ 1,0,0 },
		{ 1,0,0 },
		{ 1,0,0 },
		{ 1,0,0 },
		{ 0,1,0 },
		{ 0,1,0 },
		{ 0,1,0 },
		{ 0,1,0 },
		{ 0,1,0 },
		{ 0,1,0 },
		{ 0,1,0 },
		{ 0,1,0 }
	};
	vector<unsigned int> elementBuffer;
	for (int i = 0;i < vertexBuffer.size();i++)
		for (int j = 0;j < vertexBuffer.size();j++) {
			if (i == j)continue;
			if (distance(vertexBuffer[i], vertexBuffer[j]) == 1) {
				elementBuffer.push_back(i);
				elementBuffer.push_back(j);
			}
		}

	GLuint vao, vertexVBO,colorVBO,ebo;
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vertexVBO);
	glGenBuffers(1, &colorVBO);
	glGenBuffers(1, &ebo);

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vertexVBO);
	glBufferData(GL_ARRAY_BUFFER, vertexBuffer.size() * sizeof(vec4), &vertexBuffer[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glBindBuffer(GL_ARRAY_BUFFER, colorVBO);
	glBufferData(GL_ARRAY_BUFFER, colorBuffer.size() * sizeof(vec3), &colorBuffer[0], GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, elementBuffer.size() * sizeof(unsigned int), &elementBuffer[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);


	float ticks = 0;
	do {
		int width, height;
		glfwGetFramebufferSize(window, &width, &height);
		glViewport(0, 0, width, height);
		vec3 eulers = eulerAngles(options.rot3D)*180.f/3.14159265f;
		options.xz = eulers.y;
		options.xy = eulers.z;
		options.yz = eulers.x;
		eulers = eulerAngles(options.rot4D)*180.f / 3.14159265f;
		options.yw = eulers.y;
		options.zw = eulers.z;
		options.xw = eulers.x;
		
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		mat4 to3D = mat4(perspective(radians(90.f), width / (float)height, .1f, 100.f));
		mat4 view = lookAt(vec3(0,0,3),vec3(0),vec3(0,1,0));
		glUniformMatrix4fv(glGetUniformLocation(program, "to3D"), 1, GL_FALSE, &to3D[0][0]);
		glUniform1i(glGetUniformLocation(program, "dfc"), options.dfc);
		glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_FALSE, &view[0][0]);
		glUniform1f(glGetUniformLocation(program, "time"), ticks);
		glUniform1f(glGetUniformLocation(program, "xzAngle"), options.xz);
		glUniform1f(glGetUniformLocation(program, "xyAngle"), options.xy);
		glUniform1f(glGetUniformLocation(program, "yzAngle"), options.yz);
		glUniform1f(glGetUniformLocation(program, "xwAngle"), options.xw);
		glUniform1f(glGetUniformLocation(program, "ywAngle"), options.yw);
		glUniform1f(glGetUniformLocation(program, "zwAngle"), options.zw);
		glDrawElements(GL_LINES, elementBuffer.size(), GL_UNSIGNED_INT, 0);

		TwDraw();
		glfwPollEvents();
		glfwSwapBuffers(window);
		ticks += 0.0005f;
	} while (glfwWindowShouldClose(window) == 0);
	glfwTerminate();
	TwTerminate();
	return 0;
}

string readFile(const char *file) {
	string content;
	ifstream stream(file);
	if (stream.is_open()) {
		stringstream ss;
		ss << stream.rdbuf();
		content = ss.str();
		stream.close();
	} else {
		printf("Failed to open %s\n", file);
		return "";
	}
	return content;
}
bool compileshader(const char* file, GLuint id) {
	GLint result = GL_FALSE;
	int infoLogLength;
	printf("Compiling shader: %s\n", file);
	string content = readFile(file);
	if (content.compare("") == 0) { return NULL; }
	char const * src = content.c_str();
	const GLint length = content.size();
	glShaderSource(id, 1, &src, &length);
	glCompileShader(id);

	glGetShaderiv(id, GL_COMPILE_STATUS, &result);
	glGetShaderiv(id, GL_INFO_LOG_LENGTH, &infoLogLength);
	if (infoLogLength > 1) {
		vector<char> errormessage(infoLogLength + 1);
		glGetShaderInfoLog(id, infoLogLength, NULL, &errormessage[0]);
		printf("%s compile error:\n\t%s\n", file, &errormessage[0]);
		return false;
	}
	return true;
}

GLuint loadshaders(const char *vertexfile, const char *fragmentfile) {
	GLuint vertexID = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragmentID = glCreateShader(GL_FRAGMENT_SHADER);

	GLint result = GL_FALSE;
	int infoLogLength;

	if (!compileshader(vertexfile, vertexID)) { return NULL; }
	if (!compileshader(fragmentfile, fragmentID)) { return NULL; }

	printf("linking program\n");
	GLuint programID = glCreateProgram();
	glAttachShader(programID, vertexID);
	glAttachShader(programID, fragmentID);
	glLinkProgram(programID);

	glGetProgramiv(programID, GL_LINK_STATUS, &result);
	glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &infoLogLength);
	if (infoLogLength > 1) {
		vector<char> errormessage(infoLogLength + 1);
		glGetProgramInfoLog(programID, infoLogLength, NULL, &errormessage[0]);
		printf("link error:\n%s\n", &errormessage[0]);
		return NULL;
	}

	glDetachShader(programID, vertexID);
	glDetachShader(programID, fragmentID);

	glDeleteShader(vertexID);
	glDeleteShader(fragmentID);

	return programID;
}