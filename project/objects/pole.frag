#version 330 core

in vec3 fragNormal;       // Normal in world space
in vec3 fragPosition;     // Position in world space
in vec4 fragPosLightSpace; // Position in light's clip space

out vec4 FragColor;

uniform vec3 lightDirection;   // Direction of the light (normalized)
uniform vec3 lightColor;       // Light color
uniform vec3 cameraPosition;   // Camera position for specular calculations
uniform float lightIntensity;  // Light intensity
uniform sampler2D shadowMap;   // Shadow map

float ShadowCalculation(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir) {
    // Transform light space position to NDC
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5; // Map from [-1, 1] to [0, 1]

    // Check if fragment is outside shadow map bounds
    if (projCoords.x < 0.0 || projCoords.x > 1.0 || projCoords.y < 0.0 || projCoords.y > 1.0 || projCoords.z > 1.0) {
        return 1.0; // Fully lit if outside shadow map
    }

    float closestDepth = texture(shadowMap, projCoords.xy).r; // Depth from shadow map
    float currentDepth = projCoords.z;

    // Bias to prevent shadow acne
    float bias = max(0.005 * (1.0 - dot(normal, lightDir)), 0.001);

    // Compare depths
    float shadow = currentDepth - bias > closestDepth ? 0.2 : 1.0;

    return shadow;
}

void main() {
    vec3 normal = normalize(fragNormal);
    vec3 lightDir = normalize(-lightDirection);

    // Calculate shadow
    float shadow = ShadowCalculation(fragPosLightSpace, normal, lightDir);

    // Diffuse lighting
    float diff = max(dot(normal, lightDir), 0.0);

    // Specular lighting
    vec3 viewDir = normalize(cameraPosition - fragPosition);
    vec3 reflectDir = reflect(-lightDir, normal);
    float shininess = 32.0f; // High shininess for a shiny pole
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);

    // Ambient, diffuse, and specular contributions
    vec3 ambient = 0.1 * lightColor;
    vec3 diffuse = diff * lightColor * lightIntensity;
    vec3 specular = spec * lightColor * lightIntensity;

    // Combine shadow with lighting
    vec3 finalColor = ambient + shadow * (diffuse + specular);

    // Set the pole color and apply lighting
    vec3 baseColor = vec3(0.69, 0.77, 0.87); // Gray color for the pole
    FragColor = vec4(finalColor * baseColor, 1.0); // Combine lighting with base color
}
