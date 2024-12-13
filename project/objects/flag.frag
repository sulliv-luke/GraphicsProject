#version 330 core

in vec2 UV;
in vec3 fragNormal;
in vec3 fragPosition;
in vec4 fragPosLightSpace; // Position in light space for shadow mapping

out vec4 color;

uniform sampler2D textureSampler;
uniform sampler2D shadowMap; // Shadow map sampler
uniform vec3 lightDirection;
uniform vec3 lightColor;
uniform vec3 cameraPosition;
uniform float lightIntensity;

// Controls for translucency
uniform float translucencyFactor = 0.5; // How much light passes through
uniform vec3 backLightColor = vec3(1.0, 1.0, 1.0); // Color of light passing through

// Function to calculate shadow factor
float calculateShadow(vec4 fragPosLightSpace) {
    // Transform to normalized device coordinates
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5; // Transform to [0, 1] range

    // Sample the closest depth from the shadow map
    float closestDepth = texture(shadowMap, projCoords.xy).r;

    // Current fragment depth in light space
    float currentDepth = projCoords.z;

    // Bias to prevent shadow acne
    float bias = max(0.005 * (1.0 - dot(normalize(fragNormal), lightDirection)), 0.0005);

    // Check if the fragment is in shadow
    float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;

    // Ensure the fragment is within the light's frustum
    if (projCoords.z > 1.0 || projCoords.x < 0.0 || projCoords.x > 1.0 || projCoords.y < 0.0 || projCoords.y > 1.0) {
        shadow = 0.0; // Outside the light's frustum
    }

    return shadow;
}

void main() {
    // Normalize inputs
    vec3 normal = normalize(fragNormal);
    vec3 lightDir = normalize(-lightDirection);
    vec3 viewDir = normalize(cameraPosition - fragPosition);

    // Determine the actual surface normal based on gl_FrontFacing
    vec3 surfaceNormal = gl_FrontFacing ? -normal : normal;

    // Calculate shadow factor
    float shadow = calculateShadow(fragPosLightSpace);

    // Diffuse lighting for the front face
    float diffFront = max(dot(surfaceNormal, lightDir), 0.0);

    // Specular lighting for the front face
    vec3 reflectDir = reflect(-lightDir, surfaceNormal);
    float shininess = 8.0f; // Lower shininess for the flag
    float specFront = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    float specularScale = 0.3;

    // Ambient, diffuse, and specular contributions for the front
    vec3 ambientFront = 0.1 * lightColor;
    vec3 diffuseFront = diffFront * lightColor * lightIntensity * (1.0 - shadow); // Apply shadow
    vec3 specularFront = specFront * lightColor * lightIntensity * specularScale * (1.0 - shadow); // Apply shadow

    // Combine front lighting
    vec3 lightingFront = ambientFront + diffuseFront + specularFront;

    // Simulate translucency (light passing through)
    float diffBack = max(dot(-surfaceNormal, lightDir), 0.0);
    vec3 diffuseBack = diffBack * backLightColor * translucencyFactor;

    // Combine lighting with the flag's texture
    vec3 materialColor = texture(textureSampler, UV).rgb;

    // Blend front and back lighting
    vec3 finalColor = lightingFront * materialColor + diffuseBack * materialColor;

    // Set the final color with translucency effect
    color = vec4(finalColor, 1.0);
}
