#include "Sun.h"

void Sun::generateSphere(int stacks, int slices) {
    for (int i = 0; i <= stacks; ++i) {
        float stackAngle = M_PI / 2 - i * M_PI / stacks; // From pi/2 to -pi/2
        float xy = radius * cos(stackAngle);
        float z = radius * sin(stackAngle);

        for (int j = 0; j <= slices; ++j) {
            float sliceAngle = j * 2 * M_PI / slices; // From 0 to 2pi
            float x = xy * cos(sliceAngle);
            float y = xy * sin(sliceAngle);

            // Add vertex
            sunVertices.push_back(glm::vec3(x, y, z));

            // Add UV coordinates
            sunUVs.push_back(glm::vec2((float)j / slices, (float)i / stacks));
        }
    }

    // Generate indices for the sphere
    for (int i = 0; i < stacks; ++i) {
        for (int j = 0; j < slices; ++j) {
            int first = (i * (slices + 1)) + j;
            int second = first + slices + 1;

            sunIndices.push_back(first);
            sunIndices.push_back(second);
            sunIndices.push_back(first + 1);

            sunIndices.push_back(second);
            sunIndices.push_back(second + 1);
            sunIndices.push_back(first + 1);
        }
    }
}

void Sun::initialize(glm::vec3 position, float radius, glm::vec3 lightColor, const std::string& vertexShaderPath, const std::string& fragmentShaderPath) {
    this->position = position;
    this->radius = radius;
    this->lightColor = lightColor;

    // Generate sphere geometry
    generateSphere(36, 36); // Adjust stacks and slices for quality

    // Generate and bind VAO
    glGenVertexArrays(1, &sunVAO);
    glBindVertexArray(sunVAO);

    // Generate and bind VBO for vertices
    glGenBuffers(1, &sunVBO);
    glBindBuffer(GL_ARRAY_BUFFER, sunVBO);
    glBufferData(GL_ARRAY_BUFFER, sunVertices.size() * sizeof(glm::vec3), &sunVertices[0], GL_STATIC_DRAW);

    // Generate and bind UV buffer
    glGenBuffers(1, &sunUVBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, sunUVBuffer);
    glBufferData(GL_ARRAY_BUFFER, sunUVs.size() * sizeof(glm::vec2), &sunUVs[0], GL_STATIC_DRAW);

    // Generate and bind EBO for indices
    glGenBuffers(1, &sunEBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sunEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sunIndices.size() * sizeof(GLuint), &sunIndices[0], GL_STATIC_DRAW);

    // Load shaders
    sunProgramID = LoadShadersFromFile(vertexShaderPath.c_str(), fragmentShaderPath.c_str());
    if (sunProgramID == 0) {
        std::cerr << "Failed to load sun shaders." << std::endl;
    }

    // Get shader uniform locations
    sunMVPMatrixID = glGetUniformLocation(sunProgramID, "MVP");
    sunLightColorID = glGetUniformLocation(sunProgramID, "lightColor");
}

void Sun::render(glm::mat4 cameraMatrix) {
    glUseProgram(sunProgramID);

    // Create model matrix
    glm::mat4 sunModelMatrix = glm::mat4(1.0f);
    sunModelMatrix = glm::translate(sunModelMatrix, position);

    // Calculate MVP matrix
    glm::mat4 sunMVP = cameraMatrix * sunModelMatrix;
    glUniformMatrix4fv(sunMVPMatrixID, 1, GL_FALSE, &sunMVP[0][0]);

    // Set light color uniform
    glUniform3fv(sunLightColorID, 1, &lightColor[0]);

    // Bind VAO
    glBindVertexArray(sunVAO);

    // Enable vertex attribute 0 for positions
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, sunVBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    // Enable vertex attribute 1 for UVs
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, sunUVBuffer);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);

    // Bind EBO and draw the sphere
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sunEBO);
    glDrawElements(GL_TRIANGLES, sunIndices.size(), GL_UNSIGNED_INT, 0);

    // Disable vertex attributes
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
}

void Sun::cleanup() {
    glDeleteBuffers(1, &sunVBO);
    glDeleteBuffers(1, &sunUVBuffer);
    glDeleteBuffers(1, &sunEBO);
    glDeleteVertexArrays(1, &sunVAO);
    glDeleteProgram(sunProgramID);
}
