#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <render/shader.h>
#include "skybox.h"
#include <iostream>
#include <stb/stb_image.h>

GLuint LoadTextureSkyBox(const char *texture_file_path) {
	int w, h, channels;
	uint8_t* img = stbi_load(texture_file_path, &w, &h, &channels, 3);
	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	// Set texture wrapping to prevent seams
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // No repeating, clamp to edge
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // No repeating, clamp to edge

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // No mipmaps, linear filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // Linear filtering for magnification

	if (img) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, img);
	} else {
		std::cout << "Failed to load texture " << texture_file_path << std::endl;
	}
	stbi_image_free(img);

	return texture;
}

const GLfloat SkyBox::vertex_buffer_data[72] = {
		// Front face (+Z, viewed from inside)
		-1.0f, -1.0f,  1.0f,  // Bottom-left
		-1.0f,  1.0f,  1.0f,  // Top-left
		 1.0f,  1.0f,  1.0f,  // Top-right
		 1.0f, -1.0f,  1.0f,  // Bottom-right

		// Back face (-Z, viewed from inside)
		 1.0f, -1.0f, -1.0f,  // Bottom-left
		 1.0f,  1.0f, -1.0f,  // Top-left
		-1.0f,  1.0f, -1.0f,  // Top-right
		-1.0f, -1.0f, -1.0f,  // Bottom-right

		// Left face (-X, viewed from inside)
		-1.0f, -1.0f, -1.0f,  // Bottom-left
		-1.0f,  1.0f, -1.0f,  // Top-left
		-1.0f,  1.0f,  1.0f,  // Top-right
		-1.0f, -1.0f,  1.0f,  // Bottom-right

		// Right face (+X, viewed from inside)
		 1.0f, -1.0f,  1.0f,  // Bottom-left
		 1.0f,  1.0f,  1.0f,  // Top-left
		 1.0f,  1.0f, -1.0f,  // Top-right
		 1.0f, -1.0f, -1.0f,  // Bottom-right

		// Top face (+Y, viewed from inside)
		-1.0f,  1.0f,  1.0f,  // Bottom-left
		-1.0f,  1.0f, -1.0f,  // Bottom-right
		 1.0f,  1.0f, -1.0f,  // Top-right
		 1.0f,  1.0f,  1.0f,  // Top-left

		// Bottom face (-Y, viewed from inside)
		-1.0f, -1.0f, -1.0f,  // Bottom-left
		-1.0f, -1.0f,  1.0f,  // Bottom-right
		 1.0f, -1.0f,  1.0f,  // Top-right
		 1.0f, -1.0f, -1.0f   // Top-left
	};


const GLfloat SkyBox::color_buffer_data[72] = {
		// Front, red
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,

		// Back, yellow
		1.0f, 1.0f, 0.0f,
		1.0f, 1.0f, 0.0f,
		1.0f, 1.0f, 0.0f,
		1.0f, 1.0f, 0.0f,

		// Left, green
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,

		// Right, cyan
		0.0f, 1.0f, 1.0f,
		0.0f, 1.0f, 1.0f,
		0.0f, 1.0f, 1.0f,
		0.0f, 1.0f, 1.0f,

		// Top, blue
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,

		// Bottom, magenta
		1.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 1.0f,
	};

const GLuint SkyBox::index_buffer_data[36] = {		// 12 triangle faces of a box
		0, 1, 2,
		0, 2, 3,

		4, 5, 6,
		4, 6, 7,

		8, 9, 10,
		8, 10, 11,

		12, 13, 14,
		12, 14, 15,

		16, 17, 18,
		16, 18, 19,

		20, 21, 22,
		20, 22, 23,
	};

    // TODO: Define UV buffer data
    // ---------------------------
    // ---------------------------
	// Adjusted UV buffer data for SkyBox (slightly inset UVs)
const GLfloat SkyBox::uv_buffer_data[48] = {
		// Front face (+Z), vertices 0-3
		0.499f,  0.666f,  // Vertex 0: Bottom-left
		0.499f,  0.335f,  // Vertex 1: Top-left
		0.251f,  0.335f,  // Vertex 2: Top-right
		0.251f,  0.666f,  // Vertex 3: Bottom-right

		// Back face (-Z), vertices 4-7
		0.999f,  0.666f,  // Vertex 4: Bottom-left
		0.999f,  0.335f,  // Vertex 5: Top-left
		0.751f,  0.335f,  // Vertex 6: Top-right
		0.751f,  0.666f,  // Vertex 7: Bottom-right

		// Left face (-X), vertices 8-11
		0.751f,  0.666f,  // Vertex 8: Bottom-left
		0.751f,  0.335f,  // Vertex 9: Top-left
		0.499f,  0.335f,  // Vertex 10: Top-right
		0.499f,  0.666f,  // Vertex 11: Bottom-right

		// Right face (+X), vertices 12-15
		0.251f,  0.666f,  // Vertex 12: Bottom-left
		0.251f,  0.335f,  // Vertex 13: Top-left
		0.001f,  0.335f,  // Vertex 14: Top-right
		0.001f,  0.666f,  // Vertex 15: Bottom-right

		// Top face (+Y), vertices 16-19
		0.499f,  0.335f,
		0.499f,  0.001f,
		0.251f,  0.001f,
		0.251f,  0.335f,

		// Bottom face (-Y), vertices 20-23
		0.499f,  0.999f,  // Vertex 20: Bottom-left
		0.499f,  0.666f,  // Vertex 21: Top-left
		0.251f,  0.666f,  // Vertex 22: Top-right
		0.251f,  0.999f   // Vertex 23: Bottom-right
	};




	void SkyBox::initialize(glm::vec3 position, glm::vec3 scale) {
		// Define scale of the building geometry
		this->position = position;
		this->scale = scale;

		// Create a vertex array object
		glGenVertexArrays(1, &vertexArrayID);
		glBindVertexArray(vertexArrayID);

		// Create a vertex buffer object to store the vertex data
		glGenBuffers(1, &vertexBufferID);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_buffer_data), vertex_buffer_data, GL_STATIC_DRAW);

		// Create a vertex buffer object to store the color data
        // TODO:
		glGenBuffers(1, &colorBufferID);
		glBindBuffer(GL_ARRAY_BUFFER, colorBufferID);
		glBufferData(GL_ARRAY_BUFFER, sizeof(color_buffer_data), color_buffer_data, GL_STATIC_DRAW);
		// TODO: Create a vertex buffer object to store the UV data
		// --------------------------------------------------------
        // --------------------------------------------------------
		glGenBuffers(1, &uvBufferID);
		glBindBuffer(GL_ARRAY_BUFFER, uvBufferID);
		glBufferData(GL_ARRAY_BUFFER, sizeof(uv_buffer_data), uv_buffer_data,
	GL_STATIC_DRAW);

		// Create an index buffer object to store the index data that defines triangle faces
		glGenBuffers(1, &indexBufferID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(index_buffer_data), index_buffer_data, GL_STATIC_DRAW);

		// Create and compile our GLSL program from the shaders
		programID = LoadShadersFromFile("../project/box2.vert", "../project/box2.frag");
		if (programID == 0)
		{
			std::cerr << "Failed to load shaders." << std::endl;
		}

		// Get a handle for our "MVP" uniform
		mvpMatrixID = glGetUniformLocation(programID, "MVP");

        // TODO: Load a texture
        // --------------------
        // --------------------
		textureID = LoadTextureSkyBox("../project/sky.png");

        // TODO: Get a handle to texture sampler
        // -------------------------------------
        // -------------------------------------
		textureSamplerID = glGetUniformLocation(programID,"textureSampler");
	}

	void SkyBox::render(glm::mat4 cameraMatrix) {
		glUseProgram(programID);

		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, colorBufferID);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);

		glDisable(GL_CULL_FACE);

		// TODO: Model transform
		// ----------------------
		// Create the model transformation matrix
		glm::mat4 modelMatrix = glm::mat4(1.0f);
		modelMatrix = glm::translate(modelMatrix, position); // Translate to the building's position
		modelMatrix = glm::scale(modelMatrix, scale);        // Scale to the building's dimensions

		// -----------------------

		// Set model-view-projection matrix
		glm::mat4 mvp = cameraMatrix * modelMatrix;
		glUniformMatrix4fv(mvpMatrixID, 1, GL_FALSE, &mvp[0][0]);

		// TODO: Enable UV buffer and texture sampler
		// ------------------------------------------
		// ------------------------------------------
		// Bind skybox texture
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureID);  // Bind skybox texture
		glUniform1i(textureSamplerID, 0);

		glEnableVertexAttribArray(2);
		glBindBuffer(GL_ARRAY_BUFFER, uvBufferID);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);


		// Draw the box
		glDrawElements(
			GL_TRIANGLES,      // mode
			36,    			   // number of indices
			GL_UNSIGNED_INT,   // type
			(void*)0           // element array buffer offset
		);

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(2);
	}

	void SkyBox::cleanup() {
		glDeleteBuffers(1, &vertexBufferID);
		glDeleteBuffers(1, &colorBufferID);
		glDeleteBuffers(1, &indexBufferID);
		glDeleteVertexArrays(1, &vertexArrayID);
		//glDeleteBuffers(1, &uvBufferID);
		//glDeleteTextures(1, &textureID);
		glDeleteProgram(programID);
	}
