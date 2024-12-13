#ifndef FLAG_H
#define FLAG_H

#include <glad/gl.h>
#include <glm/glm.hpp>
#include "utils/lightInfo.h"

class Flag {
public:
    // Initialization and rendering methods for the flag
    void initialize(glm::vec3 position, glm::vec3 scale, const char* texturePath);
    void render(glm::mat4 cameraMatrix, Light light, glm::vec3 cameraPosition);
    void renderFlagDepth(GLuint depthShaderProgramID, glm::mat4 lightSpaceMatrix);
    void cleanup();

    // Initialization and rendering methods for the pole
    void initializePole(glm::vec3 polePosition, glm::vec3 poleScale);
    void renderPole(glm::mat4 cameraMatrix, Light light, glm::vec3 cameraPosition, glm::mat4 lightSpaceMatrix, GLuint shadowMap);
    void renderPoleDepth(GLuint depthShaderProgramID, glm::mat4 lightSpaceMatrix);
    void cleanupPole();

private:
    // Variables for the flag
    GLuint vertexArrayID;
    GLuint vertexBufferID;
    GLuint uvBufferID;
    GLuint indexBufferID;
    GLuint programID;
    GLuint mvpMatrixID;
    GLuint timeID;
    GLuint textureID;
    GLuint textureSamplerID, lightPositionID, lightColorID, lightIntensityID, lightDirectionID, cameraPositionID;

    glm::vec3 position;
    glm::vec3 scale;
    float startTime;

    int numSegments;

    // Variables for the pole
    GLuint poleVAO;
    GLuint poleNormalBuffer;
    GLuint poleVBO;
    GLuint poleUVBuffer;
    GLuint poleEBO;
    GLuint poleProgramID;
    GLuint poleTextureID;
    GLuint poleMVPMatrixID;
    GLuint poleTextureSamplerID;

    glm::vec3 polePosition;
    glm::vec3 poleScale;
};

#endif // FLAG_H
