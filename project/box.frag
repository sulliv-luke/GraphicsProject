#version 330 core

in vec3 fragNormal;
in vec3 fragPosition;
in vec2 fragUV;

out vec4 FragColor;

uniform sampler2D textureSampler;
uniform vec3 lightDirection; // Direction of the light
uniform vec3 lightColor;
uniform vec3 lightPosition;
uniform vec3 cameraPosition;
uniform float lightIntensity;

void main() {
	vec3 normal = normalize(fragNormal);
	vec3 lightDir = normalize(-lightDirection); // Invert direction if necessary

	// Diffuse lighting
	float diff = max(dot(normal, lightDir), 0.0);

	// Specular lighting
	vec3 viewDir = normalize(cameraPosition - fragPosition);
	vec3 reflectDir = reflect(-lightDir, normal);
	float shininess = 60.0f;
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);

	vec3 ambient = 0.1 * lightColor; // Ambient light
	vec3 diffuse = diff * lightColor;
	vec3 specular = spec * lightColor;

	vec3 finalColor = ambient + diffuse * lightIntensity + specular * lightIntensity;

	// Combine with texture
	vec4 textureColor = texture(textureSampler, fragUV);
	FragColor = vec4(finalColor * textureColor.rgb, textureColor.a);

}