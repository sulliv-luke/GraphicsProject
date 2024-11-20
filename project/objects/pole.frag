#version 330 core

in vec3 fragNormal;       // Normal in world space (from vertex shader)
in vec3 fragPosition;     // Position in world space (from vertex shader)

out vec4 FragColor;

uniform vec3 lightDirection;   // Direction of the light (normalized)
uniform vec3 lightColor;       // Light color
uniform vec3 lightPosition;    // Light position (if needed for point lights)
uniform vec3 cameraPosition;   // Camera position for specular calculations
uniform float lightIntensity;  // Light intensity

void main() {
    vec3 normal = normalize(fragNormal);
    vec3 lightDir = normalize(-lightDirection); // Ensure light points towards the surface

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
    vec3 finalColor = ambient + diffuse + specular;

    // Set the pole color and apply lighting
    vec3 baseColor = vec3(0.69, 0.77, 0.87); // Gray color for the pole
    FragColor = vec4(finalColor * baseColor, 1.0); // Combine lighting with base color
}
