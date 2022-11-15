/*
Semestre 2023-1
Práctica 8: Iluminación 2
*/
//para cargar imagen
#define STB_IMAGE_IMPLEMENTATION

#include <stdio.h>
#include <string.h>
#include <cmath>
#include <vector>
#include <math.h>
#include <Windows.h>

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
	
Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;

Camera camera;

Texture brickTexture;
Texture dirtTexture;
Texture plainTexture;
Texture pisoTexture;
Texture AgaveTexture;
Texture dodecaedroTexture;

Model Kitt_M;
Model Llanta_M;
Model Camino_M;
Model Blackhawk_M;
Model Dado_M;
Model Pista_carreras;

// Carro
Model CarroPrevio_SinCofre_M;
Model Cofre_CarroPrevio_M;
Model LlantaEstrella_M;

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
		-10.0f, 0.0f, -10.0f,	0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, -10.0f,	10.0f, 0.0f,	0.0f, -1.0f, 0.0f,
		-10.0f, 0.0f, 10.0f,	0.0f, 10.0f,	0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, 10.0f,		10.0f, 10.0f,	0.0f, -1.0f, 0.0f
	};

	unsigned int vegetacionIndices[] = {
	   0, 1, 2,
	   0, 2, 3,
	   4,5,6,
	   4,6,7
	};

	GLfloat vegetacionVertices[] = {
		-0.5f, -0.5f, 0.0f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.5f, -0.5f, 0.0f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.5f, 0.5f, 0.0f,		1.0f, 1.0f,		0.0f, 0.0f, 0.0f,
		-0.5f, 0.5f, 0.0f,		0.0f, 1.0f,		0.0f, 0.0f, 0.0f,

		0.0f, -0.5f, -0.5f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.5f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, 0.5f,		1.0f, 1.0f,		0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, -0.5f,		0.0f, 1.0f,		0.0f, 0.0f, 0.0f,

	};

	unsigned int dode_indices[] = {

		//Cara1

		0, 1, 2,
		2, 3, 4,
		2, 4 ,0,

		//Cara2

		5, 6, 7,
		7, 8, 9,
		7, 9 ,5,

		//Cara3

		10, 11, 12,
		12, 13, 14,
		12, 14 ,10,

		//Cara4

		15, 16, 17,
		17, 18, 19,
		17, 19 ,15,

		//Cara5

		20, 21, 22,
		22, 23, 24,
		22, 24 ,20,

		//Cara6

		25, 26, 27,
		27, 28, 29,
		27, 29 ,25,

		//Cara7

		30, 31, 32,
		32, 33, 34,
		32, 34 ,30,

		//Cara8

		35, 36, 37,
		37, 38, 39,
		37, 39 ,35,

		//Cara9

		40, 41, 42,
		42, 43, 44,
		42, 44 ,40,

		//Cara10

		45, 46, 47,
		47, 48, 49,
		47, 49 ,45,

		//Cara11

		50, 51, 52,
		52, 53, 54,
		52, 54 ,50,

		//Cara12

		55, 56, 57,
		57, 58, 59,
		57, 59 ,55,

	};

	GLfloat dode_vertices[] = {

		// Cara1 Carmin
		//x		y		z		       S		T			NX		NY		NZ
		0.577f, 0.577f, -0.577f,	0.039f,  0.189f,	0.0f,	0.0f,	0.0f,	//0
		0.0f, 0.934f, -0.357f,		0.036f,	0.334f,		0.0f,	0.0f,	0.0f,	//1
		-0.577f, 0.577f, -0.577f,	0.115f,	0.370f,		0.0f,	0.0f,	0.0f,	//2
		-0.357f,  0.0f,  -0.934f,	0.172f,	0.260f,		0.0f,	0.0f,	0.0f,	//3
		0.357f,  0.0f,  -0.934f,	0.117f,	0.156f,		0.0f,	0.0f,	0.0f,	//4
		// Cara2 Verde
		//x		y		z		       S		T			NX		NY		NZ
		0.0f, 0.934f, -0.357f,		0.350f,  0.086f,	0.0f,	0.0f,	0.0f,	//0
		0.0f, 0.934f, 0.357f,		0.253f,	0.232f,		0.0f,	0.0f,	0.0f,	//1
		-0.577f, 0.577f, 0.577f,	0.333f,	0.268f,		0.0f,	0.0f,	0.0f,	//2
		-0.934f, 0.357f, 0.0f,		0.384f,	0.160f,		0.0f,	0.0f,	0.0f,	//3
		-0.577f, 0.577f, -0.577f,	0.331f,	0.057f,		0.0f,	0.0f,	0.0f,	//4

		// Cara3 Azul
		//x		y		z		       S		T			NX		NY		NZ
		0.577f, 0.577f, -0.577f,	0.176f,  0.420f,		0.0f,	0.0f,	0.0f,	//0
		0.934f, 0.357f, -0.0f,		0.230f,	0.532f,		0.0f,	0.0f,	0.0f,	//1
		0.577f, 0.577f, 0.577f,		0.313f,	0.496f,		0.0f,	0.0f,	0.0f,	//2
		-0.0f,  0.934f,  0.357f,	0.311f,	0.350f,		0.0f,	0.0f,	0.0f,	//3
		-0.0f,  0.934f,  -0.357f,	0.233f,	0.319f,		0.0f,	0.0f,	0.0f,	//4

		// Cara4 Rojo fuerte
		//x		y		z		       S		T			NX		NY		NZ
		-0.0f, 0.934f, 0.357f,			0.018f,  0.496f,		0.0f,	0.0f,	0.0f,	//0
		0.577f, 0.577f, 0.577f,			0.014f,	0.672f,		0.0f,	0.0f,	0.0f,	//1
		0.357f, 0.0f, 0.934f,			0.129f,	0.727f,		0.0f,	0.0f,	0.0f,	//2
		-0.357f,  0.0f,  0.934f,		0.203f,	0.581f,		0.0f,	0.0f,	0.0f,	//3
		-0.577f,  0.577f,  0.577f,		0.130f,	0.435f,		0.0f,	0.0f,	0.0f,	//4

		// Cara5 Naranja
		//x		y		z		       S		T			NX		NY		NZ
		-0.934f, 0.357f, 0.0f,			0.236f,  0.763f,		0.0f,	0.0f,	0.0f,	//0
		-0.577f, 0.577f, 0.577f,		0.341f,	0.813f,		0.0f,	0.0f,	0.0f,	//1
		-0.357f, 0.0f, 0.934f,			0.411f,	0.679f,		0.0f,	0.0f,	0.0f,	//2
		-0.577f,  -0.577f,  0.577f,		0.344f,	0.545f,		0.0f,	0.0f,	0.0f,	//3
		-0.934f,  -0.357f,  0.0f,		0.235f,	0.594f,		0.0f,	0.0f,	0.0f,	//4

		// Cara6 Amarillo
		//x		y		z		       S		T			NX		NY		NZ
		-0.0f, -0.934f, 0.357f,			0.378f,  0.499f,		0.0f,	0.0f,	0.0f,	//0
		-0.577f, -0.577f, 0.577f,		0.468f,	0.541f,		0.0f,	0.0f,	0.0f,	//1
		-0.357f, 0.0f, 0.934f,			0.530f,	0.421f,		0.0f,	0.0f,	0.0f,	//2
		0.357f, 0.0f,  0.934f,			0.469f,	0.302f,		0.0f,	0.0f,	0.0f,	//3
		0.577f,  -0.577f,  0.577f,		0.380f,	0.343f,		0.0f,	0.0f,	0.0f,	//4

		// Cara7 Azul fuerte
		//x		y		z		       S		T			NX		NY		NZ
		0.934f, -0.357f, -0.0f,			0.521f,  0.580f,		0.0f,	0.0f,	0.0f,	//0
		0.577f, -0.577f, 0.577f,		0.578f,	0.696f,		0.0f,	0.0f,	0.0f,	//1
		0.357f, 0.0f, 0.934f,			0.661f,	0.664f,		0.0f,	0.0f,	0.0f,	//2
		0.577f, 0.577f,  0.577f,		0.658f,	0.509f,		0.0f,	0.0f,	0.0f,	//3
		0.934f,  0.357f,  -0.0f,		0.580f,	0.468f,		0.0f,	0.0f,	0.0f,	//4

		// Cara8 Azul claro
		//x		y		z		       S		T			NX		NY		NZ
		-0.577f, -0.577f, -0.577f,	0.669f,  0.321f,		0.0f,	0.0f,	0.0f,	//0
		-0.934f, -0.357f, 0.0f,		0.713f,	0.433f,		0.0f,	0.0f,	0.0f,	//1
		-0.577f,-0.577f, 0.577f,	0.792f,	0.394f,		0.0f,	0.0f,	0.0f,	//2
		0.0f, -0.934f,  0.357f,		0.791f,	0.253f,		0.0f,	0.0f,	0.0f,	//3
		-0.0f,  -0.934f,  -0.357f,	0.715f,	0.223f,		0.0f,	0.0f,	0.0f,	//4

		// Cara9 Morado claro
		//x		y		z		       S		T			NX		NY		NZ
		-0.0f, -0.934f, -0.357f,	0.742f,  0.516f,		0.0f,	0.0f,	0.0f,	//0
		-0.0f, -0.934f, 0.357f,		0.739f,	0.646f,		0.0f,	0.0f,	0.0f,	//1
		0.577f,-0.577f, 0.577f,		0.811f,	0.681f,		0.0f,	0.0f,	0.0f,	//2
		0.934f, -0.357f,  -0.0f,	0.859f,	0.580f,		0.0f,	0.0f,	0.0f,	//3
		0.577f,  -0.577f,  -0.577f,	0.813f,	0.476f,		0.0f,	0.0f,	0.0f,	//4

		// Cara10 Lila
		//x		y		z		       S		T			NX		NY		NZ
		-0.577f, 0.577f, -0.577f,	0.664f,  0.841f,		0.0f,	0.0f,	0.0f,	//0
		-0.934f, 0.357f, 0.0f,		0.716f,	0.940f,		0.0f,	0.0f,	0.0f,	//1
		-0.934f,-0.357f, 0.0f,		0.790f,	0.906f,		0.0f,	0.0f,	0.0f,	//2
		-0.577f, -0.577f, -0.577f,	0.788f,	0.773f,		0.0f,	0.0f,	0.0f,	//3
		-0.357f,  0.0f,  -0.934f,	0.716f,	0.744f,		0.0f,	0.0f,	0.0f,	//4

		// Cara11 Rosa
		//x		y		z		       S		T			NX		NY		NZ
		0.577f, -0.577f, -0.577f,		0.801f,  0.741f,		0.0f,	0.0f,	0.0f,	//0
		0.934f, -0.357f, -0.0f,			0.874f,	0.891f,		0.0f,	0.0f,	0.0f,	//1
		0.934f,0.357f, -0.0f,			0.992f,	0.833f,		0.0f,	0.0f,	0.0f,	//2
		0.577f, 0.577f, -0.577f,		0.992f,	0.645f,		0.0f,	0.0f,	0.0f,	//3
		0.357f,  0.0f,  -0.934f,		0.874f,	0.588f,		0.0f,	0.0f,	0.0f,	//4

		// Cara12 Amarillo oscuro
		//x		y		z		       S		T			NX		NY		NZ
		-0.577f, -0.577f, -0.577f,		0.800f,  0.422f,	0.0f,	0.0f,	0.0f,	//0
		-0.0f, -0.934f, -0.357f,		0.873f,	0.572f,		0.0f,	0.0f,	0.0f,	//1
		0.577f,-0.577f, -0.577f,		0.993f,	0.515f,		0.0f,	0.0f,	0.0f,	//2
		0.357f, 0.0f, -0.934f,			0.993f,	0.324f,		0.0f,	0.0f,	0.0f,	//3
		-0.357f,  0.0f,  -0.934f,		0.873f,	0.267f,		0.0f,	0.0f,	0.0f,	//4

	};
	
	Mesh *obj1 = new Mesh();
	obj1->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj1);

	Mesh *obj2 = new Mesh();
	obj2->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj2);

	Mesh *obj3 = new Mesh();
	obj3->CreateMesh(floorVertices, floorIndices, 32, 6);
	meshList.push_back(obj3);

	Mesh* obj4 = new Mesh();
	obj4->CreateMesh(vegetacionVertices, vegetacionIndices, 64, 12);
	meshList.push_back(obj4);

	calcAverageNormals(dode_indices, 108, dode_vertices, 480, 8, 5);

	Mesh* obj5 = new Mesh();
	obj5->CreateMesh(dode_vertices, dode_indices, 480, 108);
	meshList.push_back(obj5);

	//calcAverageNormals(indices, 12, vertices, 32, 8, 5);

	//calcAverageNormals(vegetacionIndices, 12, vegetacionVertices, 64, 8, 5);


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

	camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 3.0f, 0.5f);

	brickTexture = Texture("Textures/brick.png");
	brickTexture.LoadTextureA();
	dirtTexture = Texture("Textures/dirt.png");
	dirtTexture.LoadTextureA();
	plainTexture = Texture("Textures/plain.png");
	plainTexture.LoadTextureA();
	pisoTexture = Texture("Textures/piso.tga");
	pisoTexture.LoadTextureA();
	AgaveTexture = Texture("Textures/Agave.tga");
	AgaveTexture.LoadTextureA();

	//Dodecaedro Texture
	dodecaedroTexture = Texture("Textures/Dodecaedro_opt.tga");
	dodecaedroTexture.LoadTextureA();

	Kitt_M = Model();
	Kitt_M.LoadModel("Models/kitt_optimizado.obj");
	Llanta_M = Model();
	Llanta_M.LoadModel("Models/k_rueda.3ds");
	Blackhawk_M = Model();
	Blackhawk_M.LoadModel("Models/uh60.obj");
	Camino_M = Model();
	Camino_M.LoadModel("Models/railroad track.obj");

	// Modelos carro del previo

	CarroPrevio_SinCofre_M = Model();
	CarroPrevio_SinCofre_M.LoadModel("Models/CarroPrevio_SinCofre_M_Obj.obj");

	Cofre_CarroPrevio_M = Model();
	Cofre_CarroPrevio_M.LoadModel("Models/Cofre_CarroPrevio_M_Obj.obj");

	LlantaEstrella_M = Model();
	LlantaEstrella_M.LoadModel("Models/LlantaEstrella_M_Obj.obj");

	// Modelo pista de carreras

	Pista_carreras = Model();
	Pista_carreras.LoadModel("Models/Pista_carreras.obj");


	std::vector<std::string> skyboxFaces;
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_rt.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_lf.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_dn.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_up.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_bk.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_ft.tga");

	skybox = Skybox(skyboxFaces);

	Material_brillante = Material(4.0f, 256);
	Material_opaco = Material(0.3f, 4);


	//luz direccional, sólo 1 y siempre debe de existir
	mainLight = DirectionalLight(1.0f, 1.0f, 1.0f,
		0.5f, 0.3f,
		1.0f, 0.0f, -1.0f);
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

	//luz de helicóptero
	spotLights[1] = SpotLight(0.0f, 0.0f, 1.0f,
		1.0f, 2.0f,
		0.0f, 3.0f, -0.0f,
		0.0f, -1.0f, -1.0f,
		1.0f, 0.0f, 0.0f,
		15.0f);

	spotLightCount++;

	// Luz de coche

	spotLights[2] = SpotLight(1.0f, 0.5f, 0.0f,
		1.0f, 2.0f,
		-10.0f, 0.0f, -20.0f,
		-1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		15.0f);

	// Luz de coche

	spotLightCount++;

	spotLights[3] = SpotLight(0.0f, 1.0f, 0.0f,
		1.0f, 2.0f,
		-15.0f, 0.0f, -20.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		15.0f);

	spotLightCount++;

	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
		uniformSpecularIntensity = 0, uniformShininess = 0;
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
		shaderList[0].SetDirectionalLight(&mainLight);
		shaderList[0].SetPointLights(pointLights, pointLightCount);
		//shaderList[0].SetSpotLights(spotLights, spotLightCount);
		


		if (mainWindow.getluzDelantera_on() == 1.0f && mainWindow.getluzTrasera_on() == 0.0f) {
			shaderList[0].SetSpotLights(&spotLights[0], 3);
		}

		if (mainWindow.getluzDelantera_on() == 0.0f && mainWindow.getluzTrasera_on() == 1.0f) {
			shaderList[0].SetSpotLights(&spotLights[3], 1);

		}



		glm::mat4 model(1.0);
		glm::mat4 modelaux(1.0);
		glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -2.0f, 0.0f));
		model = glm::scale(model, glm::vec3(30.0f, 1.0f, 30.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));

		pisoTexture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);

		meshList[2]->RenderMesh();

		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 0.5f, -1.5f));
		modelaux = model;
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		//Kitt_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-60.0f, 6.0f, -1.0));
		model = glm::translate(model, glm::vec3(mainWindow.getposx_bh(), mainWindow.getelevacion_bh(), mainWindow.getposz_bh()));
		model = glm::scale(model, glm::vec3(4.0f, 4.0f, 4.0f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Blackhawk_M.RenderModel();
		glm::vec3 posLuzBH = glm::vec3(-60.0f + mainWindow.getposx_bh(), mainWindow.getelevacion_bh(), 0.0f + mainWindow.getposz_bh());
		spotLights[1].SetPos(posLuzBH);

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -1.53f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		//Camino_M.RenderModel();

//Agave ¿qué sucede si lo renderizan antes del coche y de la pista?
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 0.5f, -2.0f));
		model = glm::scale(model, glm::vec3(6.0f, 6.0f, 6.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		//blending: transparencia o traslucidez
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		AgaveTexture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		//meshList[3]->RenderMesh();
		glDisable(GL_BLEND);

		//Dodecaedro de Opengl
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(10.0f, 10.0f, 10.0f));
		model = glm::scale(model, glm::vec3(10.0f, 10.0f, 10.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		dodecaedroTexture.UseTexture();
		meshList[4]->RenderMesh();

		// R E N D E R  C A R R O  P R E V I O

		//Render carro sin cofre
		color = glm::vec3(0.1f, 0.1f, 0.1f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(170+mainWindow.getavance_carro2(), 1.0f, -150+mainWindow.getposz_carro2()));
		modelaux = model;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		CarroPrevio_SinCofre_M.RenderModel();
		glm::vec3 posLuzCarro = glm::vec3(170+mainWindow.getavance_carro2(), 3.0f, -150+mainWindow.getposz_carro2());
		spotLights[2].SetPos(posLuzCarro);
		spotLights[3].SetPos(posLuzCarro);


		// Articulacion de giro cofre

		model = modelaux;
		model = glm::translate(model, glm::vec3(-9.3f, 5.5f, 0.3f));
		model = glm::rotate(model, glm::radians(mainWindow.getmuevecofre2()), glm::vec3(0.0f, 0.0f, 1.0f));

		// Render cofre
		color = glm::vec3(0.3f, 0.3f, 0.3f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		model = glm::translate(model, glm::vec3(-3.3f, 0.0f, 0.1f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Cofre_CarroPrevio_M.RenderModel();

		// Render llanta 1
		color = glm::vec3(1.0f, 0.0f, 0.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		model = modelaux;
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		model = glm::translate(model, glm::vec3(-12.0f, 0.0f, -5.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		LlantaEstrella_M.RenderModel();

		// Render llanta 2
		color = glm::vec3(1.0f, 0.7f, 0.2f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		model = modelaux;
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		model = glm::translate(model, glm::vec3(9.6f, 0.0f, -5.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		LlantaEstrella_M.RenderModel();

		// Render llanta 3
		color = glm::vec3(0.9f, 0.0f, 0.7f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		model = modelaux;
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		model = glm::translate(model, glm::vec3(-12.0f, 0.0f, 6.5f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		LlantaEstrella_M.RenderModel();

		// Render llanta 4
		color = glm::vec3(0.9f, 0.5f, 0.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		model = modelaux;
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		model = glm::translate(model, glm::vec3(9.6f, 0.0f, 6.5f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		LlantaEstrella_M.RenderModel();

		//Pista de carreras
		model = glm::mat4(1.0);
		color = glm::vec3(1.0f, 1.0f, 0.0f);
		model = glm::translate(model, glm::vec3(50.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		Pista_carreras.RenderModel();

		glUseProgram(0);

		mainWindow.swapBuffers();

	}

	return 0;
}
