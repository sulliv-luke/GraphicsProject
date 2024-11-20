#version 330 core

out vec4 FragColor;

uniform vec3 lightColor;

void main() {
    // Simple emissive color for the sun
    FragColor = vec4(lightColor, 1.0);
}
