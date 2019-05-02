// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <vector>

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>
GLFWwindow* window;

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

#include "shader.h"
#include "texture.h"
#include "controls.h"
#include "objloader.h"

GLuint vertexbuffer;
GLuint uvbuffer;
GLuint normalbuffer;

std::vector<glm::vec3> vertices;
std::vector<glm::vec2> uvs;
std::vector<glm::vec3> normals;

std::vector<GLuint> vaos;
std::vector<int> vsizes;

void newVAO(const char *obj_name, bool tex) 
{
	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);
	vaos.push_back(VertexArrayID);

	loadOBJ(obj_name, vertices, uvs, normals, tex);

	// 1rst attribute buffer : vertices
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0,(void*)0);

	vsizes.push_back(vertices.size());

	// 2nd attribute buffer : UVs
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
	glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(glm::vec2), &uvs[0], GL_STATIC_DRAW);
	glVertexAttribPointer(1,2,GL_FLOAT,GL_FALSE,0,(void*)0);

	// 3rd attribute buffer : normals
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals[0], GL_STATIC_DRAW);
	glVertexAttribPointer(2,3,GL_FLOAT,GL_FALSE,0,(void*)0);

	glBindVertexArray(0);
}

int main(void)
{
	if( !glfwInit()) {
		fprintf( stderr, "Failed to initialize GLFW\n" );
		getchar();
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow( 1024, 768, "Group 7 Museum", NULL, NULL);
	if( window == NULL ){
		fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n" );
		getchar();
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	glewExperimental = true; 
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		glfwTerminate();
		return -1;
	}

	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    
    	glfwPollEvents();
    	glfwSetCursorPos(window, 1024/2, 768/2);

	// Dark blue background
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	GLuint programID = LoadShaders("code/StandardShading.vertexshader", "code/StandardShading.fragmentshader");

	GLuint MatrixID = glGetUniformLocation(programID, "MVP");
	GLuint ViewMatrixID = glGetUniformLocation(programID, "V");
	GLuint ModelMatrixID = glGetUniformLocation(programID, "M");

	// Load the texture
	GLuint Texture = loadBMP("textures/nepplanetsculp.bmp");
	GLuint Texture2 = loadBMP("textures/Anime.bmp");
	GLuint Texture3 = loadBMP("textures/Planet.bmp");
	GLuint Texture4 = loadBMP("textures/God.bmp");
	
	GLuint TextureID  = glGetUniformLocation(programID, "myTexture");
	GLuint ColorID = glGetUniformLocation(programID, "MaterialAmbientColor");

	// Create VBOs
	glGenBuffers(1, &vertexbuffer);
	glGenBuffers(1, &uvbuffer);
	glGenBuffers(1, &normalbuffer);

	// Get a handle for our "LightPosition" uniform
	glUseProgram(programID);
	GLuint LightID = glGetUniformLocation(programID, "LightPosition_worldspace");

	newVAO("models/Museum.obj",false);
	newVAO("models/Default.obj",false);
	newVAO("models/Body.obj",false);
	newVAO("models/Eyelashes.obj",false);
	newVAO("models/Moustache.obj",false);
	newVAO("models/Beards.obj",false);
	newVAO("models/Hair.obj",false);
	newVAO("models/Planet.obj",true);
	newVAO("models/Painting2.obj",true);
	newVAO("models/Painting1.obj",true);
	newVAO("models/Painting3.obj",true);

	do{
		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Use our shader
		glUseProgram(programID);

		// Compute the MVP matrix from keyboard and mouse input
		computeMatricesFromInputs();
		glm::mat4 ProjectionMatrix = getProjectionMatrix();
		glm::mat4 ViewMatrix = getViewMatrix();
		glm::mat4 ModelMatrix = glm::mat4(1.0);
		glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;

		// Send our transformation to the currently bound shader, 
		// in the "MVP" uniform
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
		glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
		glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &ViewMatrix[0][0]);

		glm::vec3 lightPos = glm::vec3(4,4,4);
		glUniform3f(LightID, lightPos.x, lightPos.y, lightPos.z);
		glUniform3f(ColorID, 0.305256,0.029248,0.640000);

		// no textures first
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, Texture);
		glUniform1i(TextureID, -1);
		glBindVertexArray(vaos[0]);
		glDrawArrays(GL_TRIANGLES, 0, vsizes[0] );

		// change color and draw
		glUniform3f(ColorID, 1, 0.804, 0.58);
		glBindVertexArray(vaos[1]);
		glDrawArrays(GL_TRIANGLES, 0, vsizes[1]);

		glUniform3f(ColorID, 1, 0.804, 0.58);
		glBindVertexArray(vaos[2]);
		glDrawArrays(GL_TRIANGLES, 0, vsizes[2]);

		glUniform3f(ColorID, 0.957, 1.0, 0.98);
		glBindVertexArray(vaos[3]);
		glDrawArrays(GL_TRIANGLES, 0, vsizes[3]);

		glUniform3f(ColorID, 0.957, 1.0, 0.98);
		glBindVertexArray(vaos[4]);
		glDrawArrays(GL_TRIANGLES, 0, vsizes[4]);

		glUniform3f(ColorID, 0.957, 1.0, 0.98);
		glBindVertexArray(vaos[5]);
		glDrawArrays(GL_TRIANGLES, 0, vsizes[5]);

		glUniform3f(ColorID, 0.957, 1.0, 0.98);
		glBindVertexArray(vaos[6]);
		glDrawArrays(GL_TRIANGLES, 0, vsizes[6]);

		// change color
		glUniform3f(ColorID, 0,0,0);

		// change texture
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, Texture);
		glUniform1i(TextureID, 0);
		glBindVertexArray(vaos[7]);
		glDrawArrays(GL_TRIANGLES, 0, vsizes[7]);

		// change texture
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, Texture2);
		glUniform1i(TextureID, 0);
		glBindVertexArray(vaos[8]);
		glDrawArrays(GL_TRIANGLES, 0, vsizes[8]);

		// change texture
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, Texture3);
		glUniform1i(TextureID, 0);
		glBindVertexArray(vaos[9]);
		glDrawArrays(GL_TRIANGLES, 0, vsizes[9]);

		// change texture
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, Texture4);
		glUniform1i(TextureID, 0);
		glBindVertexArray(vaos[10]);
		glDrawArrays(GL_TRIANGLES, 0, vsizes[10]);

		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();

	} // Check if the ESC key was pressed or the window was closed
	while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
		   glfwWindowShouldClose(window) == 0 );

	// Cleanup VBO and shader
	glDeleteBuffers(1, &vertexbuffer);
	glDeleteBuffers(1, &uvbuffer);
	glDeleteBuffers(1, &normalbuffer);
	glDeleteProgram(programID);
	glDeleteTextures(1, &Texture);
	glDeleteTextures(1, &Texture2);
	glDeleteTextures(1, &Texture3);
	glDeleteTextures(1, &Texture4);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}

