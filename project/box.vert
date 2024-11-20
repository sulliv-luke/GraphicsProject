#version 330 core

// Input attributes
layout(location = 0) in vec3 vertexPosition; // Vertex position
layout(location = 1) in vec3 vertexColor;    // Vertex color
layout(location = 2) in vec2 vertexUV;       // Vertex UV
layout(location = 3) in vec3 vertexNormal;   // Vertex normal

// Outputs to fragment shader
out vec3 fragColor;       // Interpolated color
out vec2 fragUV;          // UV coordinates
out vec3 fragPosition;    // Position in world space
out vec3 fragNormal;      // Normal in world space

// Uniforms
uniform mat4 MVP;         // Model-View-Projection matrix
uniform mat4 modelMatrix; // Model matrix for world space calculations
uniform mat3 normalMatrix; // Normal matrix for transforming normals

void main() {
    // Transform the vertex position into clip space
    gl_Position = MVP * vec4(vertexPosition, 1.0);

    // Pass interpolated color and UV to the fragment shader
    fragColor = vertexColor;
    fragUV = vertexUV;

    // Calculate position in world space
    fragPosition = vec3(modelMatrix * vec4(vertexPosition, 1.0));

    // Transform normal to world space
    fragNormal = normalize(normalMatrix * vertexNormal);
}
