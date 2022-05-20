#include <stdio.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "Mesh.h"



//std::vector<glm::vec3> vertices = {
//	glm::vec3(0.5f, 0.5f, 0.0f),
//	glm::vec3(0.5f, -0.5f, 0.0f),
//	glm::vec3(-0.5f, -0.5f, 0.0f),
//	glm::vec3(-0.5f, 0.5f, 0.0f),
//	glm::vec3(-0.5f, 0.5f, -1.0f),
//	glm::vec3(0.5f, 0.5f, -1.0f)
//};
//
//std::vector<unsigned int> indicies = {
//	0, 1, 2,
//	0, 2, 3,
//	0, 3, 4,
//	4, 5, 0
//};

const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 720;

std::vector<Mesh> meshes;

std::string readShaderSource(const char* filePath)
{
	std::string content;
	std::ifstream fileStream(filePath, std::ios::in);
	std::string line = "";

	while (!fileStream.eof())
	{
		std::getline(fileStream, line);
		content.append(line + "\n");
	}

	fileStream.close();
	return content;
}

GLuint createShaderProgram(const char* vertFilePath, const char* fragmentFilePath)
{

	std::string vertShaderString = readShaderSource(vertFilePath);
	std::string fragmentShaderString = readShaderSource(fragmentFilePath);

	const char* vShaderSource = vertShaderString.c_str();
	const char* fShaderSource = fragmentShaderString.c_str();

	GLuint vShader = glCreateShader(GL_VERTEX_SHADER);

	glShaderSource(vShader, 1, &vShaderSource, nullptr);
	glCompileShader(vShader);

	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fShaderSource, nullptr);
	glCompileShader(fragmentShader);

	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);

	return shaderProgram;

}


Mesh processMesh(aiMesh* mesh, const aiScene* scene)
{
	std::vector<glm::vec3> vertices;
	std::vector<unsigned int> indices;

	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		glm::vec3 vertex;
		vertex.x = mesh->mVertices[i].x;
		vertex.y = mesh->mVertices[i].y;
		vertex.z = mesh->mVertices[i].z;

		vertices.push_back(vertex);
	}

	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++)
		{
			indices.push_back(face.mIndices[j]);
		}
	}

	return Mesh(vertices, indices);
}

void processNode(aiNode* node, const aiScene* scene)
{

	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(processMesh(mesh, scene));
	}

	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		processNode(node->mChildren[i], scene);
	}
}


int main()
{
	if (!glfwInit())
	{
		printf("Failed to initialize glfw.\n");
		return -1;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "A Test", nullptr, nullptr);
	glfwMakeContextCurrent(window);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330");

	gladLoadGL();
	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	GLuint renderingProgram = createShaderProgram("vertShader.glsl", "fragmentShader.glsl");

	glm::mat4 transform = glm::mat4(1.f);
	transform = glm::rotate(transform, glm::radians(90.f), glm::vec3(1.f, 0.f, 0.f));

	glm::vec3 cameraEyes = glm::vec3(0.f, 0.f, 5.f);
	glm::vec3 center = glm::vec3(0.f, 0.f, 0.f);
	glm::vec3 up = glm::vec3(0.f, 1.f, 0.f);
	glm::mat4 viewMatrix = glm::lookAt(cameraEyes, center, up);

	float fov = 45.f;
	float near = 0.01f;
	float far = 10000.f;
	float windowAspect = ((float)SCREEN_WIDTH / (float)SCREEN_HEIGHT);
	glm::mat4 projection = glm::perspective(fov, windowAspect, near, far);
	glm::mat4 mMVP = projection * viewMatrix * transform;

	glUseProgram(renderingProgram);
	unsigned int MVPUniformLoc = glGetUniformLocation(renderingProgram, "MVP");
	glUniformMatrix4fv(MVPUniformLoc, 1, GL_FALSE, glm::value_ptr(mMVP));

	Assimp::Importer importer;
	const aiScene* monke = importer.ReadFile("./monke.obj", aiProcess_Triangulate | aiProcess_FlipUVs);

	processNode(monke->mRootNode, monke);

	std::vector<glm::vec3> lightVerts = {
		glm::vec3(0.5f, 0.5f, 0.0f),
		glm::vec3(0.5f, -0.5f, 0.0f),
		glm::vec3(-0.5f, -0.5f, 0.0f),
		glm::vec3(-0.5f, 0.5f, 0.0f),
		glm::vec3(-0.5f, 0.5f, -1.0f),
		glm::vec3(0.5f, 0.5f, -1.0f),
		glm::vec3(-0.5f, -0.5f, -1.0f),
		glm::vec3(0.5f, -0.5f, -1.0f)
	};

	std::vector<unsigned int> lightIndices = {
		0, 1, 2,
		0, 2, 3,
		0, 3, 4,
		4, 5, 0,
		1, 2, 6,
		1, 6, 7,
		4, 5, 6,
		7, 6, 5
	};

	Mesh lightMesh = Mesh(lightVerts, lightIndices);

	GLuint lightShader = createShaderProgram("vertShader.glsl", "lightFragment.glsl");
	unsigned int lightCuberMVPLoc = glGetUniformLocation(lightShader, "MVP");
	glm::mat4 lightTransform = glm::mat4(1.f);
	lightTransform = glm::translate(lightTransform, glm::vec3(0.f, 1.0f, 1.0f));

	ImVec4 monkeeColor = ImVec4(1.0f, 0.5f, 0.31f, 1.f);

	while (!glfwWindowShouldClose(window))
	{
		glClearColor(0.0, 0.0, 0.0, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		ImGui::Begin("Monkee Color");
		ImGui::ColorEdit3("Monkee Color", (float*)&monkeeColor);
		ImGui::End();

		ImGui::EndFrame();

		glUseProgram(renderingProgram);
		unsigned int objectColorLoc = glGetUniformLocation(renderingProgram, "objectColor");
		unsigned int lightColorLoc = glGetUniformLocation(renderingProgram, "lightColor");
		glUniform3f(objectColorLoc, monkeeColor.x, monkeeColor.y, monkeeColor.z);
		glUniform3f(lightColorLoc, 1.0f, 1.0f, 1.0f);

		meshes[0].Draw();

		glUseProgram(lightShader);
		lightTransform = glm::rotate(lightTransform, glm::radians(.01f), glm::vec3(0.f, 1.f, 0.f));
		glm::mat4 lightMVP = projection * viewMatrix * lightTransform;
		glUniformMatrix4fv(lightCuberMVPLoc, 1, GL_FALSE, glm::value_ptr(lightMVP));

		lightMesh.Draw();

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glDeleteProgram(renderingProgram);
	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}