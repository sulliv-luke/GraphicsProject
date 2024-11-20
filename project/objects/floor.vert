#version 330 core

layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec2 vertexUV;

out vec2 UV;
out vec3 fragPosition;  // Position in world space
out vec3 fragNormal;    // Normal in world space

uniform mat4 MVP;
uniform mat4 modelMatrix;
uniform mat3 normalMatrix;

void main() {
    gl_Position = MVP * vec4(vertexPosition_modelspace, 1.0);
    UV = vertexUV;

    // Transform position to world space
    fragPosition = vec3(modelMatrix * vec4(vertexPosition_modelspace, 1.0));

    // Transform normal to world space
    fragNormal = normalize(normalMatrix * vec3(0.0, 1.0, 0.0)); // Assume up-facing normal for floor
}
