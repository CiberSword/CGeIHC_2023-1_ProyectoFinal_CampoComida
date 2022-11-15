#pragma once
#include<stdio.h>
#include<glew.h>
#include<glfw3.h>

class Window
{
public:
	Window();
	Window(GLint windowWidth, GLint windowHeight);
	int Initialise();
	GLfloat getBufferWidth() { return bufferWidth; }
	GLfloat getBufferHeight() { return bufferHeight; }
	GLfloat getXChange();
	GLfloat getYChange();
	GLfloat getelevacion_bh() { return elevacion_bh; }
	GLfloat getposx_bh() { return posx_bh; }
	GLfloat getposz_bh() { return posz_bh; }
	GLfloat getmuevecofre2() { return muevecofre2; }
	GLfloat getavance_carro2() { return avance_carro2; }
	GLfloat getposz_carro2() { return posz_carro2; }
	GLfloat getdirLuzFaros() { return dirLuzFaros; }
	GLfloat getluzTrasera_on() { return luzTrasera_on; }
	GLfloat getluzDelantera_on() { return luzDelantera_on; }

	GLfloat getblinking_led() { return blinking_led; }



	bool getShouldClose() {
		return  glfwWindowShouldClose(mainWindow);}
	bool* getsKeys() { return keys; }
	void swapBuffers() { return glfwSwapBuffers(mainWindow); }
	
	~Window();
private: 
	GLFWwindow *mainWindow;
	GLint width, height;
	bool keys[1024];
	GLint bufferWidth, bufferHeight;
	void createCallbacks();
	GLfloat lastX;
	GLfloat lastY;
	GLfloat xChange;
	GLfloat yChange;
	GLfloat elevacion_bh;
	GLfloat posx_bh;
	GLfloat posz_bh;
	GLfloat muevecofre2;
	GLfloat avance_carro2;
	GLfloat posz_carro2;
	GLfloat dirLuzFaros;
	GLfloat luzTrasera_on;
	GLfloat luzDelantera_on;
	GLfloat blinking_led;

	bool mouseFirstMoved;
	static void ManejaTeclado(GLFWwindow* window, int key, int code, int action, int mode);
	static void ManejaMouse(GLFWwindow* window, double xPos, double yPos);

};

