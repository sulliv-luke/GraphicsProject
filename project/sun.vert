#version 330 core

layout(location = 0) in vec3 vertexPosition_modelspace; // Input vertex position

uniform mat4 MVP; // Model-View-Projection matrix

void main() {
    gl_Position = MVP * vec4(vertexPosition_modelspace, 1.0); // Transform the sun's position
}
