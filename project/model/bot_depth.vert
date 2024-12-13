#version 330 core

// Inputs
layout(location = 0) in vec3 inPosition;  // Vertex position
layout(location = 3) in uvec4 inJoints;  // Joint indices
layout(location = 4) in vec4 inWeights;  // Joint weights

// Uniforms
uniform mat4 lightSpaceMatrix;
uniform mat4 modelMatrix;
uniform mat4 jointMatrices[50]; // Adjust size as per your skinning requirements

// Outputs
out vec4 fragPosLightSpace;

void main() {
    // Skinning transformation
    vec4 skinnedPosition = vec4(0.0);

    for (int i = 0; i < 4; i++) {
        if (inWeights[i] > 0.0) {
            uint jointIndex = inJoints[i];
            skinnedPosition += inWeights[i] * (jointMatrices[jointIndex] * vec4(inPosition, 1.0));
        }
    }

    // Apply model and light space transformations
    vec4 worldPosition = modelMatrix * skinnedPosition;
    fragPosLightSpace = lightSpaceMatrix * worldPosition;

    gl_Position = fragPosLightSpace;
}
