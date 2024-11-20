#version 330 core

layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec2 vertexUV;

out vec2 UV;

uniform mat4 MVP;
uniform float Time;

void main() {
    vec3 pos = vertexPosition_modelspace;

    // Base waving effect along x-axis
    float waveAmplitude1 = 0.35;
    float waveFrequency1 = 20.0;
    float waveSpeed1 = 5.0;

    // Additional waving component
    float waveAmplitude2 = 0.15;
    float waveFrequency2 = 30.0;
    float waveSpeed2 = 3.0;

    // Vary amplitude away from the pole (assumed at x = 0)
    float amplitudeModifier = pos.x; // Linearly increase with x
    //float amplitudeModifier = pow(pos.x, 2.0); // Quadratic increase


    // Apply waving effects
    pos.z += sin(pos.x * waveFrequency1 + Time * waveSpeed1) * waveAmplitude1 * amplitudeModifier;
    pos.z += sin(pos.x * waveFrequency2 + Time * waveSpeed2) * waveAmplitude2 * amplitudeModifier;

    // Simulate sagging due to gravity
    float sagAmplitude = 0.02;
    pos.z -= sagAmplitude * pow(pos.x, 2.0);

    gl_Position = MVP * vec4(pos, 1.0);
    UV = vertexUV;
}
