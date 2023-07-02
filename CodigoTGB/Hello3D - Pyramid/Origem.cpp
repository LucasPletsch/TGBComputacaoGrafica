#include <iostream>
#include <string>
#include <assert.h>
#include <fstream>
#include <sstream>
#include <vector>

using namespace std;

// GLAD
#include <glad/glad.h>

// GLFW
#include <GLFW/glfw3.h>

//GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"

#include "Object.h"
#include "JsonReader.h"


struct Vertex
{
	glm::vec3 position;
	glm::vec3 v_color;
};

// Protótipo da função de callback de teclado
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);

// Protótipos das funções
void setTranslationsOnObjects();
void setRotationsOnObjects();
void setScalesOnObjects();
void translationKeyCallback(int key, int action);
void scaleKeyCallback(int key, int action);
void rotationKeyCallback(int key, int action);

// Dimensões da janela (pode ser alterado em tempo de execução)
const GLuint WIDTH = 1000, HEIGHT = 1000;


glm::vec3 cameraPos = glm::vec3(0.0, 0.0, 3.0);
glm::vec3 cameraFront = glm::vec3(0.0, 0.0, -1.0);
glm::vec3 cameraUp = glm::vec3(0.0, 1.0, 0.0);
float cameraSpeed = 0.10;

bool firstMouse = true;
float lastX = 0.0, lastY = 0.0;
float yaw = -90.0, pitch = 0.0;

// Código TGB: Rotação dos objetos:
bool rotateX = false, rotateY = false, rotateZ = false;
float rotationSpeed = 0.1f;
//const int numObjects = 3;
int selectedObject = 0;
vector<Object> objects;
//Object objects[numObjects];
bool moveForward = false;
bool moveBackward = false;
bool moveLeft = false;
bool moveRight = false;
bool moveUp = false;
bool moveDown = false;
float deltaTime = 0.5f;
float translationSpeed = 0.02f;
bool scaleUp = false;
bool scaleDown = false;
float scaleSpeed = 0.02f;
//Código TGB: JsonReader:
JsonReader jsonReader;


// Função MAIN
int main()
{
	// Inicialização da GLFW
	glfwInit();


	// Criação da janela GLFW
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Ola Visualizador 3D!", nullptr, nullptr);
	glfwMakeContextCurrent(window);

	// Fazendo o registro da função de callback para a janela GLFW
	glfwSetKeyCallback(window, key_callback);
	//glfwSetCursorPos(window, WIDTH / 2, HEIGHT / 2);
	glfwSetCursorPosCallback(window, mouse_callback);


	//Desabilita o desenho do cursor do mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// GLAD: carrega todos os ponteiros d funções da OpenGL
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;

	}

	// Obtendo as informações de versão
	const GLubyte* renderer = glGetString(GL_RENDERER); /* get renderer string */
	const GLubyte* version = glGetString(GL_VERSION); /* version as a string */
	cout << "Renderer: " << renderer << endl;
	cout << "OpenGL version supported " << version << endl;

	// Definindo as dimensões da viewport com as mesmas dimensões da janela da aplicação
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);


	// Compilando e buildando o programa de shader
	Shader shader("Hello3D.vs", "Hello3D.fs");
	
	glUseProgram(shader.ID);

	//Código TGB: Importando os dados da câmera.


	glm::mat4 model = glm::mat4(1); //matriz identidade;
	GLint modelLoc = glGetUniformLocation(shader.ID, "model");
	//
	model = glm::rotate(model, /*(GLfloat)glfwGetTime()*/glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	glUniformMatrix4fv(modelLoc, 1, FALSE, glm::value_ptr(model));

	//Código TGB: Definindo a matriz de view (posição e orientação da câmera) a partir do arquivo lido
	Scene scene = jsonReader.readJson("./SetupDeCena.json");
	glm::mat4 view = glm::lookAt(scene.camera.position, scene.camera.target, scene.camera.up);
	GLint viewLoc = glGetUniformLocation(shader.ID, "view");
	glUniformMatrix4fv(viewLoc, 1, FALSE, glm::value_ptr(view));

	//Código TGB Definindo a matriz de projeção perpectiva à partir do arquivo lido
	glm::mat4 projection = glm::perspective(scene.frustum.fov , scene.frustum.aspect_ratio, scene.frustum.znear, scene.frustum.zfar);
	GLint projLoc = glGetUniformLocation(shader.ID, "projection");
	glUniformMatrix4fv(projLoc, 1, FALSE, glm::value_ptr(projection));

	//Definindo as propriedades do material 
	shader.setFloat("q", 30);

	//Definindo as propriedades da fonte de luz
	shader.setVec3("lightPos", -2.0f, 10.0f, 3.0f);
	shader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);


	//Código TGB: buscando lista de caminhos dos obj do arquivo de setup:
	for (int x = 0; x < scene.OBJs.size(); x++) {
		objects[x].initialize(scene.OBJs[x].file, &shader, scene.OBJs[x].initialPosition);
	}
	
	glActiveTexture(GL_TEXTURE0);
	glUniform1i(glGetUniformLocation(shader.ID, "colorBuffer"), 0);


	glEnable(GL_DEPTH_TEST);

	// Loop da aplicação - "game loop"
	while (!glfwWindowShouldClose(window))
	{
		// Checa se houveram eventos de input (key pressed, mouse moved etc.) e chama as funções de callback correspondentes
		glfwPollEvents();

		// Limpa o buffer de cor
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f); //cor de fundo
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glLineWidth(10);
		glPointSize(20);

		//Alterando a matriz de view (posição e orientação da câmera)
		glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
		glUniformMatrix4fv(viewLoc, 1, FALSE, glm::value_ptr(view));

		//Enviando a posição da camera para o shader
		shader.setVec3("cameraPos", cameraPos.x, cameraPos.y, cameraPos.z);

	
		//Código do TGB: Desenha todos os objetos
		for (int i = 0; i < objects.size(); i++) {
			objects[i].update();
			objects[i].draw();
		}
	
		
		//Código do TGB: Aplica rotação nas objects:
		setRotationsOnObjects();

		//Código TGB: aplica translação nas Objects
		setTranslationsOnObjects();

		//Código TGB: aplica scale nas Objects:
		setScalesOnObjects();

		// Troca os buffers da tela
		glfwSwapBuffers(window);
	}

	// Finaliza a execução da GLFW, limpando os recursos alocados por ela
	glfwTerminate();
	return 0;
}
void rotationKeyCallback(int key, int action) {
	//Código TGB rotação
	if (key == GLFW_KEY_X && action == GLFW_PRESS)
	{
		rotateX = !rotateX;
		rotateY = false;
		rotateZ = false;
	}

	if (key == GLFW_KEY_Y && action == GLFW_PRESS)
	{
		rotateX = false;
		rotateY = !rotateY;
		rotateZ = false;
	}

	if (key == GLFW_KEY_Z && action == GLFW_PRESS)
	{
		rotateX = false;
		rotateY = false;
		rotateZ = !rotateZ;
	}
}

void translationKeyCallback(int key, int action) {
	//Código TGB: callback de teclas para translação
	if (action == GLFW_PRESS || action == GLFW_REPEAT) {
		if (key == GLFW_KEY_I) {
			moveForward = true;
		}
		else if (key == GLFW_KEY_K) {
			moveBackward = true;
		}
		else if (key == GLFW_KEY_J) {
			moveLeft = true;
		}
		else if (key == GLFW_KEY_L) {
			moveRight = true;
		}
		else if (key == GLFW_KEY_P) {
			moveUp = true;
		}
		else if (key == GLFW_KEY_SEMICOLON) { // tecla Ç
			moveDown = true;
		}
	}
	else if (action == GLFW_RELEASE) {
		if (key == GLFW_KEY_I) {
			moveForward = false;
		}
		else if (key == GLFW_KEY_K) {
			moveBackward = false;
		}
		else if (key == GLFW_KEY_J) {
			moveLeft = false;
		}
		else if (key == GLFW_KEY_L) {
			moveRight = false;
		}
		else if (key == GLFW_KEY_P) {
			moveUp = false;
		}
		else if (key == GLFW_KEY_SEMICOLON) {
			moveDown = false;
		}
	}
}

void scaleKeyCallback(int key, int action) {
	if (action == GLFW_PRESS || action == GLFW_REPEAT) {
		if (key == GLFW_KEY_EQUAL) {
			scaleUp = true;
		}
		else if (key == GLFW_KEY_MINUS) {
			scaleDown = true;
		}
	}
	else if (action == GLFW_RELEASE) {
		if (key == GLFW_KEY_EQUAL) {
			scaleUp = false;
		}
		else if (key == GLFW_KEY_MINUS) {
			scaleDown = false;
		}
	}

}


// Função de callback de teclado - só pode ter uma instância (deve ser estática se
// estiver dentro de uma classe) - É chamada sempre que uma tecla for pressionada
// ou solta via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) 
		glfwSetWindowShouldClose(window, GL_TRUE);

	//Código TGB Navegação entre malhas
	if (action == GLFW_PRESS || action == GLFW_REPEAT) {
		if (key == GLFW_KEY_LEFT) {
			selectedObject = (selectedObject - 1 + objects.size()) % objects.size();
		}
		else if (key == GLFW_KEY_RIGHT) {
			selectedObject = (selectedObject + 1) % objects.size();
		}
	}

	rotationKeyCallback(key, action);
	translationKeyCallback(key, action);
	scaleKeyCallback(key, action);

	if (key == GLFW_KEY_W)
	{
		cameraPos += cameraSpeed * cameraFront;
	}

	if (key == GLFW_KEY_S)
	{
		cameraPos -= cameraSpeed * cameraFront;
	}

	if (key == GLFW_KEY_A)
	{
		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	}

	if (key == GLFW_KEY_D)
	{
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	}



}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{

	// cout << xpos << " " << ypos << endl;

	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float offsetx = xpos - lastX;
	float offsety = lastY - ypos;
	lastX = xpos;
	lastY = ypos;

	offsetx *= cameraSpeed;
	offsety *= cameraSpeed;

	pitch += offsety;
	yaw += offsetx;

	glm::vec3 front;
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	cameraFront = glm::normalize(front);

}


void setRotationsOnObjects() {
	if (rotateX) {
		objects[selectedObject].rotation.x += rotationSpeed;
	}
	if (rotateY) {
		objects[selectedObject].rotation.y += rotationSpeed;
	}
	if (rotateZ) {
		objects[selectedObject].rotation.z += rotationSpeed;
	}
}


void setTranslationsOnObjects() {

	if (moveForward) {
		objects[selectedObject].translation.z -= translationSpeed;
	}
	if (moveBackward) {
		objects[selectedObject].translation.z += translationSpeed;
	}
	if (moveLeft) {
		objects[selectedObject].translation.x -= translationSpeed;
	}
	if (moveRight) {
		objects[selectedObject].translation.x += translationSpeed;
	}
	if (moveUp) {
		objects[selectedObject].translation.y += translationSpeed;
	}
	if (moveDown) {
		objects[selectedObject].translation.y -= translationSpeed;
	}
}

void setScalesOnObjects() {
	if (scaleUp) {
		objects[selectedObject].scale += scaleSpeed;
	}
	if (scaleDown) {
		objects[selectedObject].scale -= deltaTime * scaleSpeed;
		objects[selectedObject].scale = glm::max(objects[selectedObject].scale, 0.25f);
	}
}








