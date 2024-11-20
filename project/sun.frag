#version 330 core

out vec4 color;

uniform vec3 sunColor; // The color of the sun

void main() {
    color = vec4(sunColor, 1.0); // Set the sun's color with full opacity
}
