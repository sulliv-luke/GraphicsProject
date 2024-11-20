#ifndef FLAG_H
#define FLAG_H

#include <glad/gl.h>
#include <glm/glm.hpp>

class Flag {
public:
    // Initialization and rendering methods for the flag
    void initialize(glm::vec3 position, glm::vec3 scale, const char* texturePath);
    void render(glm::mat4 cameraMatrix);
    void cleanup();

    // Initialization and rendering methods for the pole
    void initializePole(glm::vec3 polePosition, glm::vec3 poleScale);
    void renderPole(glm::mat4 cameraMatrix);
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
    GLuint textureSamplerID;

    glm::vec3 position;
    glm::vec3 scale;
    float startTime;

    int numSegments;

    // Variables for the pole
    GLuint poleVAO;
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
