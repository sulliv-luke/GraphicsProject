#ifndef SUN_H
#define SUN_H

#include <glad/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <render/shader.h>
#include <iostream>

class Sun {
public:
    void initialize(glm::vec3 position, float radius, glm::vec3 lightColor, const std::string& vertexShaderPath, const std::string& fragmentShaderPath);
    void render(glm::mat4 cameraMatrix);
    void cleanup();

private:
    glm::vec3 position;
    float radius;
    glm::vec3 lightColor;

    GLuint sunVAO, sunVBO, sunUVBuffer, sunEBO, sunProgramID;
    GLuint sunMVPMatrixID, sunLightColorID;

    std::vector<glm::vec3> sunVertices;
    std::vector<glm::vec2> sunUVs;
    std::vector<GLuint> sunIndices;

    void generateSphere(int stacks, int slices);
};

#endif // SUN_H
