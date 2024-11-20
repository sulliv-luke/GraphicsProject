#version 330 core
in vec2 UV;

uniform sampler2D textureSampler;

out vec4 finalColor;

void main() {
    vec3 color = texture(textureSampler, UV).rgb;
    // Assign the sampled color to the output
    finalColor = vec4(color, 1.0);
}
