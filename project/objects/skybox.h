#ifndef SKYBOX_H
#define SKYBOX_H

#include <glad/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <string>

class SkyBox {
public:
    glm::vec3 position;
    glm::vec3 scale;

    void initialize(glm::vec3 position, glm::vec3 scale);
    void render(glm::mat4 cameraMatrix);
    void cleanup();
    void printPosition();

private:
    GLuint vertexArrayID, vertexBufferID, indexBufferID, colorBufferID, uvBufferID, textureID;
    GLuint mvpMatrixID, textureSamplerID, programID;

    static const GLfloat vertex_buffer_data[72];
    static const GLfloat color_buffer_data[72];
    static const GLuint index_buffer_data[36];
    static const GLfloat uv_buffer_data[48];
};

#endif // SKYBOX_H
