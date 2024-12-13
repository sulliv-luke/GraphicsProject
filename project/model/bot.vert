#version 330 core

// Attributes
layout(location = 0) in vec3 inPosition;   // Vertex position
layout(location = 1) in vec3 inNormal;     // Vertex normal
layout(location = 3) in uvec4 inJoints;    // Joint indices (as unsigned integers)
layout(location = 4) in vec4 inWeights;    // Joint weights

// Uniforms
uniform mat4 MVP;                  // Model-View-Projection matrix
uniform mat4 jointMatrices[50];    // Array of joint matrices
uniform mat4 lightSpaceMatrix;     // Light space matrix for shadow mapping

// Outputs to the fragment shader
out vec3 worldPosition;            // Vertex position in world space
out vec3 worldNormal;              // Vertex normal in world space
out vec4 fragPosLightSpace;        // Vertex position in light space

void main() {
    // Skinning transformation
    vec4 skinnedPosition = vec4(0.0); // Initialize skinned position
    vec3 skinnedNormal = vec3(0.0);   // Initialize skinned normal

    // Loop over the four possible joint influences
    for (int i = 0; i < 4; i++) {
        float weight = inWeights[i];
        if (weight > 0.0) {
            // Get the joint index from the vertex attribute
            uint jointIndex = inJoints[i];

            // Retrieve the joint matrix
            mat4 jointMatrix = jointMatrices[jointIndex];

            // Apply skinning to the position
            skinnedPosition += weight * (jointMatrix * vec4(inPosition, 1.0));

            // Apply skinning to the normal
            mat3 jointMatrix3 = mat3(jointMatrix);
            skinnedNormal += weight * (jointMatrix3 * inNormal);
        }
    }

    // Pass world-space data to the fragment shader
    worldPosition = vec3(skinnedPosition);
    worldNormal = normalize(skinnedNormal);

    // Transform position to light space
    fragPosLightSpace = lightSpaceMatrix * skinnedPosition;

    // Transform to clip space
    gl_Position = MVP * skinnedPosition;
}
