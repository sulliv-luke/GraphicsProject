#version 330 core

in vec3 color;
in vec2 uv;

// TODO: To add UV input to this fragment shader

// TODO: To add the texture sampler
uniform sampler2D textureSampler;

out vec4 finalColor;


void main()
{
	// Perform texture lookup using the sampler and UV coordinates
	vec3 texColor = texture(textureSampler, uv).rgb;

	// Assign the sampled color to the output
	finalColor = vec4(texColor, 1.0);
}
