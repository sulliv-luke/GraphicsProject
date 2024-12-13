#version 330 core

layout(location = 0) in vec3 inPosition;

// Uniforms
uniform mat4 lightSpaceMatrix;
uniform mat4 modelMatrix;
uniform float Time;

// Outputs
out vec4 fragPosLightSpace;

void main() {
    vec3 pos = inPosition;

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

    // Transform to light space
    vec4 worldPosition = modelMatrix * vec4(pos, 1.0);
    fragPosLightSpace = lightSpaceMatrix * worldPosition;

    gl_Position = fragPosLightSpace;
}
