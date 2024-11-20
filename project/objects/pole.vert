#version 330 core

layout(location = 0) in vec3 position;  // Vertex position
layout(location = 2) in vec3 normal;    // Vertex normal

out vec3 fragNormal;       // Normal in world space
out vec3 fragPosition;     // Position in world space

uniform mat4 MVP;
uniform mat4 modelMatrix;
uniform mat3 normalMatrix;

void main() {
    gl_Position = MVP * vec4(position, 1.0);

    // Transform position and normal to world space
    fragPosition = vec3(modelMatrix * vec4(position, 1.0));
    fragNormal = normalize(normalMatrix * normal);
}
