#version 330 core

in vec2 UV;
in vec3 fragPosition;
in vec3 fragNormal;

out vec4 finalColor;

uniform sampler2D textureSampler;
uniform vec3 lightDirection;
uniform vec3 lightColor;
uniform vec3 lightPosition;
uniform vec3 cameraPosition;
uniform float lightIntensity;

void main() {
    vec3 normal = normalize(fragNormal);
    vec3 lightDir = normalize(-lightDirection); // Ensure light direction is towards the surface

    // Diffuse lighting
    float diff = max(dot(normal, lightDir), 0.0);

    // Specular lighting
    vec3 viewDir = normalize(cameraPosition - fragPosition);
    vec3 reflectDir = reflect(-lightDir, normal);
    float shininess = 12.0f;
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess) * 0.4;

    vec3 ambient = 0.1 * lightColor; // Ambient light
    vec3 diffuse = diff * lightColor * lightIntensity;
    vec3 specular = spec * lightColor * lightIntensity;

    vec3 finalLighting = ambient + diffuse + specular;

    // Combine with texture color
    vec3 materialColor = texture(textureSampler, UV).rgb;
    finalColor = vec4(finalLighting * materialColor, 1.0);
}
