/*
Semestre 2023-1
Animación:
1.- Simple o básica:Por banderas y condicionales
2.- Compleja: Por medio de funciones y algoritmos,Textura Animada.
4.- Técnicas de Animación: Por Keyframes
*/
//para cargar imagen
#define STB_IMAGE_IMPLEMENTATION

#include <stdio.h>
#include <string.h>
#include <cmath>r
#include <vector>
#include <math.h>

#include <glew.h>
#include <glfw3.h>

#include <glm.hpp>
#include <gtc\matrix_transform.hpp>
#include <gtc\type_ptr.hpp>
//para probar el importer
//#include<assimp/Importer.hpp>

#include "Window.h"
#include "Mesh.h"
#include "Shader_light.h"
#include "Camera.h"
#include "Texture.h"
#include "Sphere.h"
#include"Model.h"
#include "Skybox.h"

//para iluminación
#include "CommonValues.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "Material.h"
const float toRadians = 3.14159265f / 180.0f;

//variables para animación

static float delay_daynight = 0.0f;
static float delay_helper = 0.0f;
static float light_changing = 1.0f;

bool day_flag = true;
int indexSkybox = 0.0f;


float toffsetu = 0.0f;
float toffsetv = 0.0f;
Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;

Camera camera;

//Texturas
Texture brickTexture;
Texture dirtTexture;
Texture plainTexture;
Texture pisoTexture;
Texture AgaveTexture;
Texture FlechaTexture;
Texture Anuncio1Texture;
Texture Anuncio2Texture;
Texture PantallaBack;
Texture PantallaWhite;

// Modelos
Model TrompoPastor_M;
Model PuestoComida1_M;
Model PuestoBebidas_M;
Model PuestoMariscos_M;
Model PuestoTortas_M;
Model Scenario_M;
Model FoodTruck1_M;
Model FoodTruck_Ramen_M;
Model FoodTruck_HotDogs_M;

//Skybox
Skybox skybox;

//materiales
Material Material_brillante;
Material Material_opaco;


//Sphere cabeza = Sphere(0.5, 20, 20);
GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;
static double limitFPS = 1.0 / 60.0;

// luz direccional
DirectionalLight mainLight;
//para declarar varias luces de tipo pointlight
PointLight pointLights[MAX_POINT_LIGHTS];
SpotLight spotLights[MAX_SPOT_LIGHTS];

// Vertex Shader
static const char* vShader = "shaders/shader_light.vert";

// Fragment Shader
static const char* fShader = "shaders/shader_light.frag";



//cálculo del promedio de las normales para sombreado de Phong
void calcAverageNormals(unsigned int* indices, unsigned int indiceCount, GLfloat* vertices, unsigned int verticeCount,
	unsigned int vLength, unsigned int normalOffset)
{
	for (size_t i = 0; i < indiceCount; i += 3)
	{
		unsigned int in0 = indices[i] * vLength;
		unsigned int in1 = indices[i + 1] * vLength;
		unsigned int in2 = indices[i + 2] * vLength;
		glm::vec3 v1(vertices[in1] - vertices[in0], vertices[in1 + 1] - vertices[in0 + 1], vertices[in1 + 2] - vertices[in0 + 2]);
		glm::vec3 v2(vertices[in2] - vertices[in0], vertices[in2 + 1] - vertices[in0 + 1], vertices[in2 + 2] - vertices[in0 + 2]);
		glm::vec3 normal = glm::cross(v1, v2);
		normal = glm::normalize(normal);

		in0 += normalOffset; in1 += normalOffset; in2 += normalOffset;
		vertices[in0] += normal.x; vertices[in0 + 1] += normal.y; vertices[in0 + 2] += normal.z;
		vertices[in1] += normal.x; vertices[in1 + 1] += normal.y; vertices[in1 + 2] += normal.z;
		vertices[in2] += normal.x; vertices[in2 + 1] += normal.y; vertices[in2 + 2] += normal.z;
	}

	for (size_t i = 0; i < verticeCount / vLength; i++)
	{
		unsigned int nOffset = i * vLength + normalOffset;
		glm::vec3 vec(vertices[nOffset], vertices[nOffset + 1], vertices[nOffset + 2]);
		vec = glm::normalize(vec);
		vertices[nOffset] = vec.x; vertices[nOffset + 1] = vec.y; vertices[nOffset + 2] = vec.z;
	}
}


void CreateObjects()
{
	unsigned int indices[] = {
		0, 3, 1,
		1, 3, 2,
		2, 3, 0,
		0, 1, 2
	};

	unsigned int pantallaIndices[] = {
		0, 1, 2, 2, 3, 0, 1, 5, 6, 6, 2, 1, 7, 6, 5,
		5, 4, 7, 4, 0, 3, 3, 7, 4, 4, 5, 1, 1, 0, 4
	};

	unsigned int imagenIndices[] = { //Plano top
		3, 2, 6, 6, 7, 3
	};

	GLfloat pantallaVertices[] = {
		-0.5f, -0.5f,  0.5f,	0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.5f, -0.5f,  0.5f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.5f,  0.5f,  0.5f,		1.0f, 1.0f,		0.0f, 0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,	0.0f, 1.0f,		0.0f, 0.0f, 0.0f,

		-0.5f, -0.5f, -0.5f,	0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.5f, -0.5f, -0.5f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.5f,  0.5f, -0.5f,		1.0f, 1.0f,		0.0f, 0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f, 	0.0f, 1.0f,		0.0f, 0.0f, 0.0f,
	};

	GLfloat vertices[] = {
		//	x      y      z			u	  v			nx	  ny    nz
			-1.0f, -1.0f, -0.6f,	0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			0.0f, -1.0f, 1.0f,		0.5f, 0.0f,		0.0f, 0.0f, 0.0f,
			1.0f, -1.0f, -0.6f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f,		0.5f, 1.0f,		0.0f, 0.0f, 0.0f
	};

	unsigned int floorIndices[] = {
		0, 2, 1,
		1, 2, 3
	};

	GLfloat floorVertices[] = {
		-15.0f, 0.0f, -15.0f,	0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		15.0f, 0.0f, -15.0f,	15.0f, 0.0f,	0.0f, -1.0f, 0.0f,
		-15.0f, 0.0f, 15.0f,	0.0f, 15.0f,	0.0f, -1.0f, 0.0f,
		15.0f, 0.0f, 15.0f,		15.0f, 15.0f,	0.0f, -1.0f, 0.0f
	};

	unsigned int flechaIndices[] = {
	   0, 1, 2,
	   0, 2, 3,
	};

	GLfloat flechaVertices[] = {
		-0.5f, 0.0f, 0.5f,		0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, 0.5f,		1.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, -0.5f,		1.0f, 1.0f,		0.0f, -1.0f, 0.0f,
		-0.5f, 0.0f, -0.5f,		0.0f, 1.0f,		0.0f, -1.0f, 0.0f,

	};

	Mesh *obj1 = new Mesh();
	obj1->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj1);

	Mesh *obj2 = new Mesh();
	obj2->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj2);

	//textura piso
	Mesh *obj3 = new Mesh();
	obj3->CreateMesh(floorVertices, floorIndices, 32, 6);
	meshList.push_back(obj3);

	//textura anuncio 2
	Mesh* obj4 = new Mesh();
	obj4->CreateMesh(flechaVertices, flechaIndices, 32, 6);
	meshList.push_back(obj4);

	//textura de pantalla cubo
	Mesh* obj5 = new Mesh();
	obj5->CreateMesh(pantallaVertices, pantallaIndices, 64, 30);
	meshList.push_back(obj5);

	//textura de pantalla
	Mesh* obj6 = new Mesh();
	obj6->CreateMesh(pantallaVertices, imagenIndices, 64, 6);
	meshList.push_back(obj6);
	
}


void CreateShaders()
{
	Shader *shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}




int main()
{
	mainWindow = Window(1366, 768); // 1280, 1024 or 1024, 768
	mainWindow.Initialise();

	CreateObjects();
	CreateShaders();
	camera = Camera(glm::vec3(0.0f, 15.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 2.5f, 0.5f);

	//Carga de texturas
	brickTexture = Texture("Textures/brick.png");
	brickTexture.LoadTextureA();
	dirtTexture = Texture("Textures/dirt.png");
	dirtTexture.LoadTextureA();
	plainTexture = Texture("Textures/plain.png");
	plainTexture.LoadTextureA();
	pisoTexture = Texture("Textures/piso_adoquin.tga");
	pisoTexture.LoadTextureA();
	FlechaTexture = Texture("Textures/flechas.tga");
	FlechaTexture.LoadTextureA();
	Anuncio1Texture = Texture("Textures/negro.tga");
	Anuncio1Texture.LoadTextureA();
	Anuncio2Texture = Texture("Textures/cafe.tga");
	Anuncio2Texture.LoadTextureA();
	PantallaBack = Texture("Textures/fondonegro.tga");
	PantallaBack.LoadTextureA();
	PantallaWhite = Texture("Textures/fondoblanco.tga");
	PantallaWhite.LoadTextureA();

	//Carga de modelos
	TrompoPastor_M = Model();
	TrompoPastor_M.LoadModel("Models/TrompoPastor_M.obj");
	PuestoComida1_M = Model();
	PuestoComida1_M.LoadModel("Models/PuestoComida1_M.obj");
	PuestoBebidas_M = Model();
	PuestoBebidas_M.LoadModel("Models/PuestoBebidas_M.obj");
	PuestoMariscos_M = Model();
	PuestoMariscos_M.LoadModel("Models/PuestoMariscos_M.obj");
	PuestoTortas_M = Model();
	PuestoTortas_M.LoadModel("Models/PuestoTortas_M.obj");
	Scenario_M = Model();
	Scenario_M.LoadModel("Models/Escenario_M.obj");
	FoodTruck_Ramen_M = Model();
	FoodTruck_Ramen_M.LoadModel("Models/FoodTruck_Ramen_M.obj");
	FoodTruck_HotDogs_M = Model();
	FoodTruck_HotDogs_M.LoadModel("Models/FoodTruckHotDog_M.obj");



	std::vector<std::string> skyboxFaces;
	std::vector<std::string> nowSkybox;

	//light_1.0
	skyboxFaces.push_back("Textures/Skybox/town_light_1_0/town_px.png");
	skyboxFaces.push_back("Textures/Skybox/town_light_1_0/town_nx.png");
	skyboxFaces.push_back("Textures/Skybox/town_light_1_0/town_ny.png");
	skyboxFaces.push_back("Textures/Skybox/town_light_1_0/town_py.png");
	skyboxFaces.push_back("Textures/Skybox/town_light_1_0/town_pz.png");
	skyboxFaces.push_back("Textures/Skybox/town_light_1_0/town_nz.png");
	//light_0.9
	skyboxFaces.push_back("Textures/Skybox/town_light_0_9/town_px.png");
	skyboxFaces.push_back("Textures/Skybox/town_light_0_9/town_nx.png");
	skyboxFaces.push_back("Textures/Skybox/town_light_0_9/town_ny.png");
	skyboxFaces.push_back("Textures/Skybox/town_light_0_9/town_py.png");
	skyboxFaces.push_back("Textures/Skybox/town_light_0_9/town_pz.png");
	skyboxFaces.push_back("Textures/Skybox/town_light_0_9/town_nz.png");
	//light_0.8
	skyboxFaces.push_back("Textures/Skybox/town_light_0_8/town_px.png");
	skyboxFaces.push_back("Textures/Skybox/town_light_0_8/town_nx.png");
	skyboxFaces.push_back("Textures/Skybox/town_light_0_8/town_ny.png");
	skyboxFaces.push_back("Textures/Skybox/town_light_0_8/town_py.png");
	skyboxFaces.push_back("Textures/Skybox/town_light_0_8/town_pz.png");
	skyboxFaces.push_back("Textures/Skybox/town_light_0_8/town_nz.png");
	//light_0.7
	skyboxFaces.push_back("Textures/Skybox/town_light_0_7/town_px.png");
	skyboxFaces.push_back("Textures/Skybox/town_light_0_7/town_nx.png");
	skyboxFaces.push_back("Textures/Skybox/town_light_0_7/town_ny.png");
	skyboxFaces.push_back("Textures/Skybox/town_light_0_7/town_py.png");
	skyboxFaces.push_back("Textures/Skybox/town_light_0_7/town_pz.png");
	skyboxFaces.push_back("Textures/Skybox/town_light_0_7/town_nz.png");
	//light_0.6
	skyboxFaces.push_back("Textures/Skybox/town_light_0_6/town_px.png");
	skyboxFaces.push_back("Textures/Skybox/town_light_0_6/town_nx.png");
	skyboxFaces.push_back("Textures/Skybox/town_light_0_6/town_ny.png");
	skyboxFaces.push_back("Textures/Skybox/town_light_0_6/town_py.png");
	skyboxFaces.push_back("Textures/Skybox/town_light_0_6/town_pz.png");
	skyboxFaces.push_back("Textures/Skybox/town_light_0_6/town_nz.png");
	//light_0.5
	skyboxFaces.push_back("Textures/Skybox/town_light_0_5/town_px.png");
	skyboxFaces.push_back("Textures/Skybox/town_light_0_5/town_nx.png");
	skyboxFaces.push_back("Textures/Skybox/town_light_0_5/town_ny.png");
	skyboxFaces.push_back("Textures/Skybox/town_light_0_5/town_py.png");
	skyboxFaces.push_back("Textures/Skybox/town_light_0_5/town_pz.png");
	skyboxFaces.push_back("Textures/Skybox/town_light_0_5/town_nz.png");
	//light_0.4
	skyboxFaces.push_back("Textures/Skybox/town_light_0_4/town_px.png");
	skyboxFaces.push_back("Textures/Skybox/town_light_0_4/town_nx.png");
	skyboxFaces.push_back("Textures/Skybox/town_light_0_4/town_ny.png");
	skyboxFaces.push_back("Textures/Skybox/town_light_0_4/town_py.png");
	skyboxFaces.push_back("Textures/Skybox/town_light_0_4/town_pz.png");
	skyboxFaces.push_back("Textures/Skybox/town_light_0_4/town_nz.png");
	//light_0.3
	skyboxFaces.push_back("Textures/Skybox/town_light_0_3/town_px.png");
	skyboxFaces.push_back("Textures/Skybox/town_light_0_3/town_nx.png");
	skyboxFaces.push_back("Textures/Skybox/town_light_0_3/town_ny.png");
	skyboxFaces.push_back("Textures/Skybox/town_light_0_3/town_py.png");
	skyboxFaces.push_back("Textures/Skybox/town_light_0_3/town_pz.png");
	skyboxFaces.push_back("Textures/Skybox/town_light_0_3/town_nz.png");
	//light_0.2
	skyboxFaces.push_back("Textures/Skybox/town_light_0_2/town_px.png");
	skyboxFaces.push_back("Textures/Skybox/town_light_0_2/town_nx.png");
	skyboxFaces.push_back("Textures/Skybox/town_light_0_2/town_ny.png");
	skyboxFaces.push_back("Textures/Skybox/town_light_0_2/town_py.png");
	skyboxFaces.push_back("Textures/Skybox/town_light_0_2/town_pz.png");
	skyboxFaces.push_back("Textures/Skybox/town_light_0_2/town_nz.png");
	//light_0.1
	skyboxFaces.push_back("Textures/Skybox/town_light_0_1/town_px.png");
	skyboxFaces.push_back("Textures/Skybox/town_light_0_1/town_nx.png");
	skyboxFaces.push_back("Textures/Skybox/town_light_0_1/town_ny.png");
	skyboxFaces.push_back("Textures/Skybox/town_light_0_1/town_py.png");
	skyboxFaces.push_back("Textures/Skybox/town_light_0_1/town_pz.png");
	skyboxFaces.push_back("Textures/Skybox/town_light_0_1/town_nz.png");

	skybox = Skybox(skyboxFaces);

	Material_brillante = Material(4.0f, 256);
	Material_opaco = Material(0.3f, 4);

	//contador de luces puntuales
	unsigned int pointLightCount = 0;
	unsigned int spotLightCount = 0;
	//linterna
	spotLights[0] = SpotLight(1.0f, 1.0f, 1.0f,
		0.0f, 2.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		5.0f);
	spotLightCount++;

	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
		uniformSpecularIntensity = 0, uniformShininess = 0, uniformTextureOffset=0;
	GLuint uniformColor = 0;
	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 1000.0f);
	
	
	////Loop mientras no se cierra la ventana
	while (!mainWindow.getShouldClose())
	{
		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		deltaTime += (now - lastTime) / limitFPS;
		lastTime = now;

		//Recibir eventos del usuario
		glfwPollEvents();
		camera.keyControl(mainWindow.getsKeys(), deltaTime);
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());

		// Clear the window
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		skybox.DrawSkybox(camera.calculateViewMatrix(), projection);
		shaderList[0].UseShader();
		uniformModel = shaderList[0].GetModelLocation();
		uniformProjection = shaderList[0].GetProjectionLocation();
		uniformView = shaderList[0].GetViewLocation();
		uniformEyePosition = shaderList[0].GetEyePositionLocation();
		uniformColor = shaderList[0].getColorLocation();
		uniformTextureOffset = shaderList[0].getOffsetLocation();

		//información en el shader de intensidad especular y brillo
		uniformSpecularIntensity = shaderList[0].GetSpecularIntensityLocation();
		uniformShininess = shaderList[0].GetShininessLocation();

		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);

		// luz ligada a la cámara de tipo flash
		glm::vec3 lowerLight = camera.getCameraPosition();
		lowerLight.y -= 0.3f;
		spotLights[0].SetFlash(lowerLight, camera.getCameraDirection());

		//información al shader de fuentes de iluminación
		shaderList[0].SetPointLights(pointLights, pointLightCount);
		shaderList[0].SetSpotLights(spotLights, spotLightCount);

		glm::mat4 model(1.0);
		glm::mat4 modelaux(1.0);
		glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);
		glm::vec2 toffset = glm::vec2(0.0f, 0.0f);


		//modelo de piso
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -2.0f, 0.0f));
		model = glm::scale(model, glm::vec3(30.0f, 1.0f, 30.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
		pisoTexture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[2]->RenderMesh();
		shaderList[0].SetDirectionalLight(&mainLight);

		// RENDER TROMPOS DE PASTOR
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-318.0f, 0.0f, -348.0f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		TrompoPastor_M.RenderModel();

		// RENDER PUESTO COMIDA 1
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-264.0f, 0.0f, -321.0f));
		model = glm::scale(model, glm::vec3(0.7f, 0.7f, 0.7f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		PuestoComida1_M.RenderModel();

		// RENDER PUESTO BEBIDAS
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(200.0f, 0.0f, 100.0f));
		//model = glm::translate(model, glm::vec3(mainWindow.getposx_bh(), mainWindow.getelevacion_bh(), mainWindow.getposz_bh()));
		// Para facilitar el acomodo de objetos, preguntar dudas
		model = glm::scale(model, glm::vec3(0.7f, 0.7f, 0.7f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		PuestoBebidas_M.RenderModel();

		// RENDER PUESTO MARISCOS
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -3.0f, 100.0f));
		model = glm::translate(model, glm::vec3(mainWindow.getposx_bh(), mainWindow.getelevacion_bh(), mainWindow.getposz_bh()));
		// Para facilitar el acomodo de objetos, preguntar dudas
		model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		PuestoMariscos_M.RenderModel();

		// RENDER PUESTO TORTAS
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -3.0f, -200.0f));
		//model = glm::translate(model, glm::vec3(mainWindow.getposx_bh(), mainWindow.getelevacion_bh(), mainWindow.getposz_bh()));
		// Para facilitar el acomodo de objetos, preguntar dudas
		model = glm::scale(model, glm::vec3(6.0f, 6.0f, 6.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		PuestoTortas_M.RenderModel();

		// RENDER FOOD TRUCK RAMEN
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(200.0f, 0.0f, 0.0f));
		//model = glm::translate(model, glm::vec3(mainWindow.getposx_bh(), mainWindow.getelevacion_bh(), mainWindow.getposz_bh()));
		// Para facilitar el acomodo de objetos, preguntar dudas
		model = glm::scale(model, glm::vec3(0.35f, 0.35f, 0.35f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		FoodTruck_Ramen_M.RenderModel();

		//RENDER FOOD TRUCK HOTDOGS y hamburguesaS
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-200.0f, 0.0f, -200.0f));
		model = glm::translate(model, glm::vec3(mainWindow.getposx_bh(), mainWindow.getelevacion_bh(), mainWindow.getposz_bh()));
		model = glm::scale(model, glm::vec3(2.5f, 2.5f, 2.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		FoodTruck_HotDogs_M.RenderModel();

		//RENDER SCENARIO
		/*model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-200.0f, 0.0f, 0.0f));
		model = glm::translate(model, glm::vec3(mainWindow.getposx_bh(), mainWindow.getelevacion_bh(), mainWindow.getposz_bh()));
		model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Scenario_M.RenderModel();*/


		//Modelo textura con openGL pantalla
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 10.0f, 0.0f));
		model = glm::translate(model, glm::vec3(mainWindow.getposx_bh(), mainWindow.getelevacion_bh(), mainWindow.getposz_bh()));
		model = glm::scale(model, glm::vec3(10.0f, 2.0f, 5.0f));
		PantallaBack.UseTexture();
		meshList[4]->RenderMesh();

		//imagen
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 10.0f, 2.25f));
		model = glm::translate(model, glm::vec3(mainWindow.getposx_bh(), mainWindow.getelevacion_bh(), mainWindow.getposz_bh()));
		model = glm::scale(model, glm::vec3(10.0f, 0.5f, 5.0f));
		PantallaWhite.UseTexture();
		meshList[5]->RenderMesh();


		//Acá todo lo de GL_BLEND (Texturas)
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		//Textura de anuncio 1
		toffsetu += 0.0001;
		if (toffsetu > 1.0) toffsetu = 0.0;
		toffset = glm::vec2(toffsetu, 0.0f);
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 0.2f, -6.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(10.0f, 1.0f, 2.0f));
		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Anuncio1Texture.UseTexture();
		meshList[3]->RenderMesh();

		////Textura de anuncio 2
		toffsetv += 0.0001;
		if (toffsetv > 1.0) toffsetv = 0.0;
		toffset = glm::vec2(0.0f, toffsetv);
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(15.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 10.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Anuncio2Texture.UseTexture();
		meshList[3]->RenderMesh();


		glDisable(GL_BLEND);

		//HELPER PARA COLOCAR OBJETOS
		if (delay_helper >= 50.0f) {

			printf("\nEl objeto está en X: %f , Y: %f , Z: %f", mainWindow.getposx_bh(), mainWindow.getelevacion_bh(), mainWindow.getposz_bh());
			delay_helper = 0.0f;
		}

		delay_helper += deltaTime;

		// SECCION PARA ANIMACIÓN



		// Ciclado Dia-Noche de la luz direccional
			if (delay_daynight >= 500.0f && day_flag == true) {

				mainLight = DirectionalLight(1.0f, 1.0f, 1.0f,
					light_changing, light_changing,
					0.0f, 0.0f, -1.0f);
				shaderList[0].SetDirectionalLight(&mainLight);
				light_changing -= 0.1;
				delay_daynight = 0.0f;
				
				nowSkybox = { skyboxFaces.begin() + indexSkybox, skyboxFaces.begin() + indexSkybox + 6 };
				skybox = Skybox(nowSkybox);
				indexSkybox += 6;


				if (light_changing <= 0.1)
					day_flag = false;

			}

			if (delay_daynight >= 500.0f && day_flag == false) {

				mainLight = DirectionalLight(1.0f, 1.0f, 1.0f,
					light_changing, light_changing,
					0.0f, 0.0f, -1.0f);
				shaderList[0].SetDirectionalLight(&mainLight);
				light_changing += 0.1;
				delay_daynight = 0.0f;

				nowSkybox = { skyboxFaces.begin() + indexSkybox, skyboxFaces.begin() + indexSkybox + 6 };
				skybox = Skybox(nowSkybox);
				indexSkybox -= 6;


				if (light_changing >= 1.0f)
					day_flag = true;
			}

			delay_daynight += deltaTime;

		

		glUseProgram(0);

		mainWindow.swapBuffers();
	}

	return 0;
}
