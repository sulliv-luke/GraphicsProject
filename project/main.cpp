#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <render/shader.h>
#include <stb/stb_image.h>
#include <cstdlib> // For random number generation
#include <ctime> // For seeding the random number generator
#include <vector>
#include <iostream>
#include <objects/skybox.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <bits/random.h>
#include <random>
#include "objects/floor.h"
#include "objects/flag.h"
#include "objects/sun.h"

static GLFWwindow *window;
static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode);
static void mouse_callback(GLFWwindow *window, double xpos, double ypos);

// OpenGL camera view parameters
static glm::vec3 eye_center;
static glm::vec3 lookat(0, 0, 0);
static glm::vec3 up(0, 1, 0);

// Add new global variables for camera movement
static glm::vec3 cameraPosition(0.0f, 15.0f, 300.0f); // Start just above the floor level
static glm::vec3 cameraFront(0.0f, 0.0f, -1.0f);      // Direction the camera is facing
static glm::vec3 cameraRight(1.0f, 0.0f, 0.0f);       // Perpendicular to cameraFront
static glm::vec3 cameraUp(0.0f, 1.0f, 0.0f);          // World up
static float cameraSpeed = 100.0f;                      // Movement speed
static float deltaTime = 0.0f;                        // Time between frames
static float lastFrame = 0.0f;                        // Time of the last frame

static glm::vec3 lightPosition(170.0f, 362.3f, 487.0f); // Sun's position
static glm::vec3 lightColor(1.0f, 1.0f, 1.0f);      // Warm light color
static float lightIntensity = 1.2f;             // Brightness multiplier
static glm::vec3 lightLookAt(0.0f, 0.0f, 0.0f); // Where the light is pointing
static glm::vec3 lightDirection;               // Computed in each frame


// Mouse settings
static bool firstMouse = true;
static float lastX = 1024 / 2.0f; // Center of the screen
static float lastY = 768 / 2.0f;
static float yaw = -90.0f; // Yaw (horizontal rotation)
static float pitch = 0.0f; // Pitch (vertical rotation)

// View control
static float viewAzimuth = 0.f;
static float viewPolar = 0.f;
static float viewDistance = 300.0f;

static GLuint LoadTextureTileBox(const char *texture_file_path) {
    int w, h, channels;
    uint8_t* img = stbi_load(texture_file_path, &w, &h, &channels, 3);
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    // To tile textures on a box, we set wrapping to repeat
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    if (img) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, img);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        std::cout << "Failed to load texture " << texture_file_path << std::endl;
    }
    stbi_image_free(img);

    return texture;
}

struct Building {
	glm::vec3 position;		// Position of the box
	glm::vec3 scale;		// Size of the box in each axis

	GLfloat vertex_buffer_data[72] = {	// Vertex definition for a canonical box
		// Front face
		-1.0f, -1.0f, 1.0f,
		1.0f, -1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f, 1.0f,

		// Back face
		1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f, 1.0f, -1.0f,
		1.0f, 1.0f, -1.0f,

		// Left face
		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f, 1.0f,
		-1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f, -1.0f,

		// Right face
		1.0f, -1.0f, 1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, 1.0f, -1.0f,
		1.0f, 1.0f, 1.0f,

		// Top face
		-1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, -1.0f,
		-1.0f, 1.0f, -1.0f,

		// Bottom face
		-1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, 1.0f,
		-1.0f, -1.0f, 1.0f,
	};

	GLfloat normal_buffer_data[72] = {
		// Front face
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		// Back face
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,
		// Other faces...
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,
		 // Other faces...
		 1.0f, 0.0f, 0.0f,
		 1.0f, 0.0f, 0.0f,
		 1.0f, 0.0f, 0.0f,
		 1.0f, 0.0f, 0.0f,

		 0.0f, 1.0f, 0.0f,
		 0.0f, 1.0f, 0.0f,
		 0.0f, 1.0f, 0.0f,
		 0.0f, 1.0f, 0.0f,

		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
	};

	GLfloat color_buffer_data[72] = {
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

	GLuint index_buffer_data[36] = {		// 12 triangle faces of a box
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
	GLfloat uv_buffer_data[48] = {
		// Front
		0.0f, 1.0f,
		1.0f, 1.0f,
		1.0f, 0.0f,
		0.0f, 0.0f,
		// Back
		0.0f, 1.0f,
		1.0f, 1.0f,
		1.0f, 0.0f,
		0.0f, 0.0f,
		// Left
		0.0f, 1.0f,
		1.0f, 1.0f,
		1.0f, 0.0f,
		0.0f, 0.0f,
		// Right
		0.0f, 1.0f,
		1.0f, 1.0f,
		1.0f, 0.0f,
		0.0f, 0.0f,
		// Top - we do not want texture the top
		0.0f, 0.0f,
		0.0f, 0.0f,
		0.0f, 0.0f,
		0.0f, 0.0f,
		// Bottom - we do not want texture the bottom
		0.0f, 0.0f,
		0.0f, 0.0f,
		0.0f, 0.0f,
		0.0f, 0.0f,
		};

	// OpenGL buffers
	GLuint vertexArrayID;
	GLuint vertexBufferID;
	GLuint indexBufferID;
	GLuint colorBufferID;
	GLuint uvBufferID;
	GLuint textureID;

	// Shader variable IDs
	GLuint mvpMatrixID;
	GLuint textureSamplerID;
	GLuint programID;
	GLuint lightPositionID;
	GLuint lightColorID;
	GLuint lightIntensityID;
	GLuint cameraPositionID;
	GLuint normalBufferID;

	void initialize(glm::vec3 position, glm::vec3 scale) {
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

		glGenBuffers(1, &normalBufferID);
		glBindBuffer(GL_ARRAY_BUFFER, normalBufferID);
		glBufferData(GL_ARRAY_BUFFER, sizeof(normal_buffer_data), normal_buffer_data, GL_STATIC_DRAW);

		/*
		 *Typically, texture coordinates are stored in an array in pairs,
		 *where each pair corresponds to the (u, v) coordinates that map a texture to a polygon in computer graphics.
		 *In the format where each pair is stored as [u0, v0, u1, v1, ..., un, vn], the u values usually represent
		 *the horizontal component of the texture mapping, and the v values represent the vertical component.
		 *
		 *By multiplying every second element by 5, which corresponds to
		 *all the v components (given the starting index of 1 and the stride of 2),
		 *the loop is scaling the vertical component of these texture coordinates.
		 */
		for (int i = 0; i < 24; ++i) uv_buffer_data[2*i+1] *= 5;
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
		programID = LoadShadersFromFile("../project/box.vert", "../project/box.frag");
		if (programID == 0)
		{
			std::cerr << "Failed to load shaders." << std::endl;
		}

		// Get a handle for our "MVP" uniform
		mvpMatrixID = glGetUniformLocation(programID, "MVP");

        // --------------------
        // --------------------
		textureID = LoadTextureTileBox("../project/skyscraper1.png");

        // -------------------------------------
        // -------------------------------------
		textureSamplerID = glGetUniformLocation(programID,"textureSampler");
		lightPositionID = glGetUniformLocation(programID, "lightPosition");
		lightColorID = glGetUniformLocation(programID, "lightColor");
		lightIntensityID = glGetUniformLocation(programID, "lightIntensity");
		cameraPositionID = glGetUniformLocation(programID, "cameraPosition");

	}

	void render(glm::mat4 cameraMatrix) {
		glUseProgram(programID);

		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, colorBufferID);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);

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

		glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(modelMatrix)));
		glUniformMatrix4fv(glGetUniformLocation(programID, "modelMatrix"), 1, GL_FALSE, &modelMatrix[0][0]);
		glUniformMatrix3fv(glGetUniformLocation(programID, "normalMatrix"), 1, GL_FALSE, &normalMatrix[0][0]);

		// TODO: Enable UV buffer and texture sampler
		// ------------------------------------------
        // ------------------------------------------
		glEnableVertexAttribArray(2);
		glBindBuffer(GL_ARRAY_BUFFER, uvBufferID);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureID);
		glUniform1i(textureSamplerID, 0);

		glEnableVertexAttribArray(3);
		glBindBuffer(GL_ARRAY_BUFFER, normalBufferID);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, 0);


		glUniform3fv(lightPositionID, 1, &lightPosition[0]);
		glUniform3fv(lightColorID, 1, &lightColor[0]);
		glUniform1f(lightIntensityID, lightIntensity);
		glUniform3fv(cameraPositionID, 1, &cameraPosition[0]);
		// Calculate light direction
		lightDirection = glm::normalize(lightLookAt - lightPosition);

		// Pass light direction to the shader
		glUniform3fv(glGetUniformLocation(programID, "lightDirection"), 1, &lightDirection[0]);



		// Draw the box
		glDrawElements(
			GL_TRIANGLES,      // mode
			36,    			   // number of indices
			GL_UNSIGNED_INT,   // type
			(void*)0           // element array buffer offset
		);

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
        //glDisableVertexAttribArray(2);
	}

	void cleanup() {
		glDeleteBuffers(1, &vertexBufferID);
		glDeleteBuffers(1, &colorBufferID);
		glDeleteBuffers(1, &indexBufferID);
		glDeleteVertexArrays(1, &vertexArrayID);
		//glDeleteBuffers(1, &uvBufferID);
		//glDeleteTextures(1, &textureID);
		glDeleteProgram(programID);
	}
};

int main(void)
{
	// Initialise GLFW
	if (!glfwInit())
	{
		std::cerr << "Failed to initialize GLFW." << std::endl;
		return -1;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // For MacOS
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Open a window and create its OpenGL context
	window = glfwCreateWindow(1024, 768, "Final Project", NULL, NULL);
	if (window == NULL)
	{
		std::cerr << "Failed to open a GLFW window." << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	// Set GLFW input modes and callbacks
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); // Disable cursor for FPS-style control
	glfwSetCursorPosCallback(window, mouse_callback);            // Capture mouse movements
	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
	glfwSetKeyCallback(window, key_callback);

	// Load OpenGL functions, gladLoadGL returns the loaded version, 0 on error.
	int version = gladLoadGL(glfwGetProcAddress);
	if (version == 0)
	{
		std::cerr << "Failed to initialize OpenGL context." << std::endl;
		return -1;
	}

	// Background
	glClearColor(0.2f, 0.2f, 0.25f, 0.0f);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	SkyBox skybox;
	skybox.initialize(glm::vec3(0,0,0), glm::vec3(500, 500, 500));

	Floor floor;
	floor.initialize(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(500.0f, 1.0f, 500.0f), "../project/floor.jpg");

	Flag flag;
	glm::vec3 flagPosition = cameraPosition + cameraFront * 50.0f; // Adjust distance as needed
	// Add a height offset to the y-component
	float heightOffset = 100.0f; // Adjust this value as needed
	flagPosition.y += heightOffset;
	glm::vec3 flagScale = glm::vec3(30.0f, 15.0f, 1.0f); // Adjust size as needed
	flag.initialize(flagPosition, flagScale, "../project/ireland_flag.jpg");

	// Seed random number generator for varied building sizes and positions
	std::srand(static_cast<unsigned int>(std::time(0)));


	// TODO: Create more buildings
	// ---------------------------
	int rows =5;
	int cols = 5;
	float spacing =50.0f;

	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<> height_dist(30.0f, 150.0f);
	std::uniform_real_distribution<> offset_dist(-5.0f, 5.0f);
	std::vector<Building> buildings;

	for (int i = 0; i < rows; ++i) {
		for (int j = 0; j < cols; ++j) {
			Building b;

			// Randomize position with slight offset
			float x = i * spacing + offset_dist(gen);
			float z = j * spacing + offset_dist(gen);
			glm::vec3 position = glm::vec3(x, 0, z);

			// Randomize height while keeping width and depth constant
			float height = height_dist(gen);
			glm::vec3 scale = glm::vec3(16.0f, height, 16.0f);
			position.y=height;
			b.initialize(position, scale);
			buildings.push_back(b);
		}
	}

	// In your main program
	Sun sun;
	sun.initialize(lightPosition, 30.0f, lightColor, "../project/sun.vert", "../project/sun.frag");

    // ---------------------------

	// Camera setup
    eye_center.y = viewDistance * cos(viewPolar);
    eye_center.x = viewDistance * cos(viewAzimuth);
    eye_center.z = viewDistance * sin(viewAzimuth);

	glm::mat4 viewMatrix, projectionMatrix;
    glm::float32 FoV = 45;
	glm::float32 zNear = 0.1f;
	glm::float32 zFar = 3000.0f;
	projectionMatrix = glm::perspective(glm::radians(FoV), 4.0f / 3.0f, zNear, zFar);

	do
	{
		// Time management for consistent speed
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
			glm::vec3 newPos = cameraPosition + cameraSpeed * deltaTime * cameraFront;
			if (newPos.y >= 1.8f) { // Ensure camera stays above the floor
				cameraPosition = newPos;
			}
		}
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
			glm::vec3 newPos = cameraPosition - cameraSpeed * deltaTime * cameraFront;
			if (newPos.y >= 1.8f) { // Ensure camera stays above the floor
				cameraPosition = newPos;
			}
		}
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
			glm::vec3 newPos = cameraPosition - glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed * deltaTime;
			if (newPos.y >= 1.8f) { // Ensure camera stays above the floor
				cameraPosition = newPos;
			}
		}
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
			glm::vec3 newPos = cameraPosition + glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed * deltaTime;
			if (newPos.y >= 1.8f) { // Ensure camera stays above the floor
				cameraPosition = newPos;
			}
		}


		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Update view matrix
		viewMatrix = glm::lookAt(cameraPosition, cameraPosition + cameraFront, cameraUp);
		glm::mat4 vp = projectionMatrix * viewMatrix;

		// Debug: Print camera position and lookat direction
		std::cout << "Camera Position: ("
				  << cameraPosition.x << ", "
				  << cameraPosition.y << ", "
				  << cameraPosition.z << ") LookAt: ("
				  << (cameraPosition + cameraFront).x << ", "
				  << (cameraPosition + cameraFront).y << ", "
				  << (cameraPosition + cameraFront).z << ")"
				  << std::endl;

		// Render the skybox
		skybox.render(vp);
		// Render the floor
		floor.render(vp);

		// Render the buildings
		for (Building building : buildings) {
			building.render(vp);
		}

		// Render the flag
		flag.render(vp);

		sun.render(vp);


		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();

	} // Check if the ESC key was pressed or the window was closed
	while (!glfwWindowShouldClose(window));

	// Clean up
	for (Building building : buildings) {
		building.cleanup();
	}

	skybox.cleanup();
	flag.cleanup();
	floor.cleanup();
	sun.cleanup();

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}

// Is called whenever a key is pressed/released via GLFW
// Handle key input for camera movement
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, GL_TRUE); // Close window
	}
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	static const float sensitivity = 0.1f;
	if (firstMouse) {
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // Reversed since y-coordinates range from bottom to top
	lastX = xpos;
	lastY = ypos;

	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yaw += xoffset;
	pitch += yoffset;

	// Constrain the pitch
	if (pitch > 89.0f) pitch = 89.0f;
	if (pitch < -89.0f) pitch = -89.0f;

	// Update camera direction
	glm::vec3 front;
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	cameraFront = glm::normalize(front);
}
