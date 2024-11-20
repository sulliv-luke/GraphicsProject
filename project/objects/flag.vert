#version 330 core

layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec2 vertexUV;

out vec2 UV;
out vec3 fragNormal;
out vec3 fragPosition;

uniform mat4 MVP;
uniform mat4 modelMatrix;
uniform mat3 normalMatrix;
uniform float Time;

void main() {
    vec3 pos = vertexPosition_modelspace;

    // Waving effect
    float waveAmplitude1 = 0.35;
    float waveFrequency1 = 20.0;
    float waveSpeed1 = 5.0;
    float waveAmplitude2 = 0.15;
    float waveFrequency2 = 30.0;
    float waveSpeed2 = 3.0;

    float amplitudeModifier = pos.x;
    pos.z += sin(pos.x * waveFrequency1 + Time * waveSpeed1) * waveAmplitude1 * amplitudeModifier;
    pos.z += sin(pos.x * waveFrequency2 + Time * waveSpeed2) * waveAmplitude2 * amplitudeModifier;

    // Calculate the normal based on the waving effect
    vec3 normal = vec3(0.0, 0.0, 1.0);

    // Output data
    fragNormal = normalize(normalMatrix * normal);
    fragPosition = vec3(modelMatrix * vec4(pos, 1.0));
    UV = vertexUV;

    gl_Position = MVP * vec4(pos, 1.0);
}
