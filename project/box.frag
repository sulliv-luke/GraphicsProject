#version 330 core

in vec3 fragNormal;
in vec3 fragPosition;
in vec2 fragUV;
in vec4 fragPosLightSpace;

out vec4 FragColor;

uniform sampler2D textureSampler;
uniform sampler2D shadowMap;
uniform vec3 lightDirection;
uniform vec3 lightColor;
uniform vec3 lightPosition;
uniform vec3 cameraPosition;
uniform float lightIntensity;

float ShadowCalculation(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir) {
	vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
	projCoords = projCoords * 0.5 + 0.5;
	float closestDepth = texture(shadowMap, projCoords.xy).r;
	float currentDepth = projCoords.z;
	float bias = max(0.005 * (1.0 - dot(normal, lightDir)), 0.001);
	float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;
	if (projCoords.z > 1.0 || projCoords.z < 0.0 || projCoords.x < 0.0 || projCoords.x > 1.0 || projCoords.y < 0.0 || projCoords.y > 1.0) {
		return 1.0; // Fully shadowed
	}
	return shadow;
}

void main() {
	vec3 normal = normalize(fragNormal);
	vec3 lightDir = normalize(-lightDirection);
	float shadow = ShadowCalculation(fragPosLightSpace, normal, lightDir);

	// Lighting calculations...
	float diff = max(dot(normal, lightDir), 0.0);
	vec3 viewDir = normalize(cameraPosition - fragPosition);
	vec3 reflectDir = reflect(-lightDir, normal);
	float shininess = 60.0f;
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);

	vec3 ambient = 0.1 * lightColor;
	vec3 diffuse = diff * lightColor;
	vec3 specular = spec * lightColor;

	vec3 finalColor = ambient + (1.0 - shadow) * (diffuse * lightIntensity + specular * lightIntensity);

	vec4 textureColor = texture(textureSampler, fragUV);
	FragColor = vec4(finalColor * textureColor.rgb, textureColor.a);
}
