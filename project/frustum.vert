#version 330 core
layout(location = 0) in vec3 position;

uniform mat4 vpMatrix; // View-projection matrix of the camera

void main() {
    gl_Position = vpMatrix * vec4(position, 1.0);
}
