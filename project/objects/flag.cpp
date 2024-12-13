#include "flag.h"
#include <render/shader.h>
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>
#include <vector>
#include <stb/stb_image.h> // Ensure this header is included for texture loading
#include <iostream>
#include "utils/lightInfo.h"

GLuint LoadTexture2D(const char* texture_file_path) {
    int w, h, channels;
    unsigned char* img = stbi_load(texture_file_path, &w, &h, &channels, STBI_rgb_alpha);
    if (!img) {
        std::cerr << "Failed to load texture " << texture_file_path << std::endl;
        return 0;
    }
    GLuint textureID;
    glGenTextures(1, &textureID);

    glBindTexture(GL_TEXTURE_2D, textureID);
    // Set texture parameters here
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // Load the texture data into OpenGL
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, img);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(img);
    return textureID;
}

void Flag::initializePole(glm::vec3 polePosition, glm::vec3 poleScale) {
    this->polePosition = polePosition;
    this->poleScale = poleScale;

    const int segments = 36; // Number of segments for the pole
    const float radius = 0.05f; // Pole radius
    const float height = 1.0f; // Pole height

    std::vector<glm::vec3> poleVertices;
    std::vector<glm::vec2> poleUVs;
    std::vector<GLuint> poleIndices;
    std::vector<glm::vec3> poleNormals;  // Normals

    // Generate vertices, normals, and UVs for the pole
    for (int i = 0; i <= segments; ++i) {
        float theta = i * 2.0f * M_PI / segments;
        float x = radius * cos(theta);
        float z = radius * sin(theta);

        glm::vec3 normal = glm::normalize(glm::vec3(x, 0.0f, z)); // Normal is perpendicular to the surface

        // Bottom circle
        poleVertices.push_back(glm::vec3(x, 0.0f, z));
        poleNormals.push_back(normal);
        poleUVs.push_back(glm::vec2((float)i / segments, 0.0f));

        // Top circle
        poleVertices.push_back(glm::vec3(x, height, z));
        poleNormals.push_back(normal);
        poleUVs.push_back(glm::vec2((float)i / segments, 1.0f));
    }

    // Generate indices for the pole sides
    for (int i = 0; i < segments; ++i) {
        int bottom1 = i * 2;
        int bottom2 = (i * 2 + 2) % (segments * 2);
        int top1 = i * 2 + 1;
        int top2 = (i * 2 + 3) % (segments * 2);

        // Side triangles
        poleIndices.push_back(bottom1);
        poleIndices.push_back(bottom2);
        poleIndices.push_back(top1);

        poleIndices.push_back(top1);
        poleIndices.push_back(bottom2);
        poleIndices.push_back(top2);
    }

    glGenVertexArrays(1, &poleVAO);
    glBindVertexArray(poleVAO);

    glGenBuffers(1, &poleVBO);
    glBindBuffer(GL_ARRAY_BUFFER, poleVBO);
    glBufferData(GL_ARRAY_BUFFER, poleVertices.size() * sizeof(glm::vec3), &poleVertices[0], GL_STATIC_DRAW);

    glGenBuffers(1, &poleNormalBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, poleNormalBuffer);
    glBufferData(GL_ARRAY_BUFFER, poleNormals.size() * sizeof(glm::vec3), &poleNormals[0], GL_STATIC_DRAW);

    glGenBuffers(1, &poleEBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, poleEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, poleIndices.size() * sizeof(GLuint), &poleIndices[0], GL_STATIC_DRAW);


    poleProgramID = LoadShadersFromFile("../project/objects/pole.vert", "../project/objects/pole.frag");
    if (poleProgramID == 0) {
        std::cerr << "Failed to load pole shaders." << std::endl;
    }
    poleMVPMatrixID = glGetUniformLocation(poleProgramID, "MVP");
    lightPositionID = glGetUniformLocation(poleProgramID, "lightPosition");
    lightColorID = glGetUniformLocation(poleProgramID, "lightColor");
    lightIntensityID = glGetUniformLocation(poleProgramID, "lightIntensity");
    cameraPositionID = glGetUniformLocation(poleProgramID, "cameraPosition");
}


void Flag::initialize(glm::vec3 position, glm::vec3 scale, const char* texturePath) {
    this->position = position;
    this->scale = scale;

    numSegments = 20;

    const int numVertices = (numSegments + 1) * (numSegments + 1);
    const int numIndices = numSegments * numSegments * 6;

    std::vector<glm::vec3> vertices(numVertices);
    std::vector<glm::vec2> uvs(numVertices); // UV coordinates
    std::vector<GLuint> indices(numIndices);

    // Generate vertices and UVs
    for (int i = 0; i <= numSegments; ++i) {
        for (int j = 0; j <= numSegments; ++j) {
            float x = (float)j / numSegments;
            float y = (float)i / numSegments;
            vertices[i * (numSegments + 1) + j] = glm::vec3(x, y, 0.0f);
            uvs[i * (numSegments + 1) + j] = glm::vec2(x, y); // UV coordinates
        }
    }

    // Generate indices
    int index = 0;
    for (int i = 0; i < numSegments; ++i) {
        for (int j = 0; j < numSegments; ++j) {
            int topLeft = i * (numSegments + 1) + j;
            int topRight = topLeft + 1;
            int bottomLeft = (i + 1) * (numSegments + 1) + j;
            int bottomRight = bottomLeft + 1;

            // First triangle
            indices[index++] = topLeft;
            indices[index++] = bottomLeft;
            indices[index++] = topRight;

            // Second triangle
            indices[index++] = topRight;
            indices[index++] = bottomLeft;
            indices[index++] = bottomRight;
        }
    }

    // Create VAO and VBOs
    glGenVertexArrays(1, &vertexArrayID);
    glBindVertexArray(vertexArrayID);

    // Vertex buffer
    glGenBuffers(1, &vertexBufferID);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);

    // UV buffer
    glGenBuffers(1, &uvBufferID);
    glBindBuffer(GL_ARRAY_BUFFER, uvBufferID);
    glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(glm::vec2), &uvs[0], GL_STATIC_DRAW);

    // Index buffer
    glGenBuffers(1, &indexBufferID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);

    // Unbind the VBOs for clarity
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // Load shaders
    programID = LoadShadersFromFile("../project/objects/flag.vert", "../project/objects/flag.frag");

    mvpMatrixID = glGetUniformLocation(programID, "MVP");
    timeID = glGetUniformLocation(programID, "Time");

    // Load texture
    textureID = LoadTexture2D(texturePath);
    if (textureID == 0) {
        std::cerr << "Failed to load flag texture." << std::endl;
    }
    textureSamplerID = glGetUniformLocation(programID, "textureSampler");
    lightPositionID = glGetUniformLocation(programID, "lightPosition");
    lightColorID = glGetUniformLocation(programID, "lightColor");
    lightIntensityID = glGetUniformLocation(programID, "lightIntensity");
    cameraPositionID = glGetUniformLocation(programID, "cameraPosition");

    // Store start time
    startTime = glfwGetTime();

    // Now, render the flagpole
    // Compute the height of the pole
    float poleHeight = position.y + scale.y;
    // Desired pole radius
    float desiredPoleRadius = 1.5f; // Adjust as needed

    // Compute the position of the pole
    glm::vec3 polePosition;
    polePosition.x = position.x; // Corrected
    polePosition.y = 0.0f; // Base at ground level
    polePosition.z = position.z;

    // Compute scaling factors
    float scaleX = desiredPoleRadius / 0.1f; // 0.1f is the initial radius in setupPole()
    float scaleY = poleHeight;
    float scaleZ = desiredPoleRadius / 0.1f;

    glm::vec3 poleScale = glm::vec3(scaleX, scaleY, scaleZ);               // Adjust height and thickness as necessary

    initializePole(polePosition, poleScale); // Initialize the pole
}

void Flag::renderFlagDepth(GLuint depthShaderProgramID, glm::mat4 lightSpaceMatrix) {
    glUseProgram(depthShaderProgramID);

    // Model matrix
    glm::mat4 modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, position);
    modelMatrix = glm::scale(modelMatrix, scale);

    // Set uniforms
    glUniformMatrix4fv(glGetUniformLocation(depthShaderProgramID, "lightSpaceMatrix"), 1, GL_FALSE, &lightSpaceMatrix[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(depthShaderProgramID, "modelMatrix"), 1, GL_FALSE, &modelMatrix[0][0]);

    float currentTime = glfwGetTime() - startTime;
    glUniform1f(glGetUniformLocation(depthShaderProgramID, "Time"), currentTime);

    // Bind VAO
    glBindVertexArray(vertexArrayID);

    // Enable vertex attribute array
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    // Draw elements
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);
    glDrawElements(GL_TRIANGLES, numSegments * numSegments * 6, GL_UNSIGNED_INT, 0);

    glDisableVertexAttribArray(0);
}





void Flag::render(glm::mat4 cameraMatrix, Light light, glm::vec3 cameraPosition) {
    glUseProgram(programID);

    // Model matrix
    glm::mat4 modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, position);
    modelMatrix = glm::scale(modelMatrix, scale);



    // Normal matrix (transpose of the inverse of the upper-left 3x3 part of the model matrix)
    glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(modelMatrix)));

    // MVP matrix
    glm::mat4 mvp = cameraMatrix * modelMatrix;
    glUniformMatrix4fv(mvpMatrixID, 1, GL_FALSE, &mvp[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(programID, "modelMatrix"), 1, GL_FALSE, &modelMatrix[0][0]);
    glUniformMatrix3fv(glGetUniformLocation(programID, "normalMatrix"), 1, GL_FALSE, &normalMatrix[0][0]);


    // Time uniform
    float currentTime = glfwGetTime() - startTime;
    glUniform1f(timeID, currentTime);

    // Bind VAO
    glBindVertexArray(vertexArrayID);

    // Enable vertex attribute array for positions
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    // Enable vertex attribute array for UVs
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, uvBufferID);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);

    // Bind the texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glUniform1i(textureSamplerID, 0);

    glUniform3fv(glGetUniformLocation(programID, "lightDirection"), 1, &light.direction[0]);
    glUniform3fv(glGetUniformLocation(programID, "lightColor"), 1, &light.color[0]);
    glUniform3fv(glGetUniformLocation(programID, "cameraPosition"), 1, &cameraPosition[0]);
    glUniform1f(glGetUniformLocation(programID, "lightIntensity"), light.intensity);


    // Draw elements
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);
    glDrawElements(GL_TRIANGLES, numSegments * numSegments * 6, GL_UNSIGNED_INT, 0);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
}

void Flag::renderPoleDepth(GLuint depthShaderProgramID, glm::mat4 lightSpaceMatrix) {
    glUseProgram(depthShaderProgramID);

    glm::mat4 poleModelMatrix = glm::mat4(1.0f);
    poleModelMatrix = glm::translate(poleModelMatrix, polePosition);
    poleModelMatrix = glm::scale(poleModelMatrix, poleScale);

    // Set the light space matrix uniform
    glUniformMatrix4fv(glGetUniformLocation(depthShaderProgramID, "lightSpaceMatrix"), 1, GL_FALSE, &lightSpaceMatrix[0][0]);

    // Set the model matrix uniform
    glUniformMatrix4fv(glGetUniformLocation(depthShaderProgramID, "modelMatrix"), 1, GL_FALSE, &poleModelMatrix[0][0]);

    glBindVertexArray(poleVAO);

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, poleVBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, poleEBO);
    glDrawElements(GL_TRIANGLES, 36 * 6, GL_UNSIGNED_INT, 0);

    glDisableVertexAttribArray(0);
}


void Flag::renderPole(glm::mat4 cameraMatrix, Light light, glm::vec3 cameraPosition, glm::mat4 lightSpaceMatrix, GLuint shadowMap) {
    glUseProgram(poleProgramID);

    glm::mat4 poleModelMatrix = glm::mat4(1.0f);
    poleModelMatrix = glm::translate(poleModelMatrix, polePosition);
    poleModelMatrix = glm::scale(poleModelMatrix, poleScale);

    // Normal matrix (transpose of the inverse of the upper-left 3x3 part of the model matrix)
    glm::mat3 poleNormalMatrix  = glm::transpose(glm::inverse(glm::mat3(poleModelMatrix)));

    glm::mat4 poleMVP = cameraMatrix * poleModelMatrix;
    glUniformMatrix4fv(poleMVPMatrixID, 1, GL_FALSE, &poleMVP[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(poleProgramID, "modelMatrix"), 1, GL_FALSE, &poleModelMatrix[0][0]);
    glUniformMatrix3fv(glGetUniformLocation(poleProgramID, "normalMatrix"), 1, GL_FALSE, &poleNormalMatrix[0][0]);

    // Pass light space matrix
    glUniformMatrix4fv(glGetUniformLocation(poleProgramID, "lightSpaceMatrix"), 1, GL_FALSE, &lightSpaceMatrix[0][0]);

    glBindVertexArray(poleVAO);

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, poleVBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, poleUVBuffer);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);

    glEnableVertexAttribArray(2); // Assuming location 2 for normals
    glBindBuffer(GL_ARRAY_BUFFER, poleNormalBuffer);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, shadowMap);
    glUniform1i(glGetUniformLocation(poleProgramID, "shadowMap"), 0);

    // Pass lighting uniforms
    glUniform3fv(glGetUniformLocation(poleProgramID, "lightDirection"), 1, &light.direction[0]);
    glUniform3fv(glGetUniformLocation(poleProgramID, "lightColor"), 1, &light.color[0]);
    glUniform3fv(glGetUniformLocation(poleProgramID, "cameraPosition"), 1, &cameraPosition[0]);
    glUniform1f(glGetUniformLocation(poleProgramID, "lightIntensity"), light.intensity);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, poleEBO);
    glDrawElements(GL_TRIANGLES, 36 * 6, GL_UNSIGNED_INT, 0);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
}


void Flag::cleanupPole() {
    glDeleteBuffers(1, &poleVBO);
    glDeleteBuffers(1, &poleUVBuffer);
    glDeleteBuffers(1, &poleEBO);
    glDeleteTextures(1, &poleTextureID);
    glDeleteVertexArrays(1, &poleVAO);
    glDeleteProgram(poleProgramID);
}

void Flag::cleanup() {
    glDeleteBuffers(1, &vertexBufferID);
    glDeleteBuffers(1, &uvBufferID);
    glDeleteBuffers(1, &indexBufferID);
    glDeleteTextures(1, &textureID);
    glDeleteVertexArrays(1, &vertexArrayID);
    glDeleteProgram(programID);
    cleanupPole();
}
