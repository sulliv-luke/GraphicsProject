#version 330 core

in vec2 UV;
in vec3 fragNormal;
in vec3 fragPosition;

out vec4 color;

uniform sampler2D textureSampler;
uniform vec3 lightDirection;
uniform vec3 lightColor;
uniform vec3 cameraPosition;
uniform float lightIntensity;

void main() {
    vec3 normal = normalize(fragNormal);
    vec3 lightDir = normalize(-lightDirection);

    // Diffuse lighting
    float diff = max(dot(normal, lightDir), 0.0);

    // Specular lighting (reduced for a matte effect)
    vec3 viewDir = normalize(cameraPosition - fragPosition);
    vec3 reflectDir = reflect(-lightDir, normal);
    float shininess = 8.0f; // Lower shininess for the flag
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    float specularScale = 0.3;

    vec3 ambient = 0.1 * lightColor;
    vec3 diffuse = diff * lightColor * lightIntensity;
    vec3 specular = spec * lightColor * lightIntensity * specularScale;

    vec3 finalLighting = ambient + diffuse + specular;

    // Combine lighting with flag texture
    vec3 materialColor = texture(textureSampler, UV).rgb;
    color = vec4(finalLighting * materialColor, 1.0);
}
