#version 330 core

in vec3 worldPosition;       // Position of the fragment in world space
in vec3 worldNormal;         // Normal at the fragment in world space
in vec4 fragPosLightSpace;   // Position of the fragment in light space

out vec3 finalColor;         // Output color

uniform vec3 lightPosition;  // Light source position in world space
uniform vec3 lightIntensity; // Light intensity
uniform sampler2D shadowMap; // Shadow map texture

float calculateShadow(vec4 fragPosLightSpace) {
    // Transform to normalized device coordinates (NDC)
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5; // Transform to [0, 1] range

    // Sample the closest depth from the shadow map
    float closestDepth = texture(shadowMap, projCoords.xy).r;

    // Current fragment depth in light space
    float currentDepth = projCoords.z;

    // Apply a small bias to avoid shadow acne
    float bias = max(0.05 * (1.0 - dot(worldNormal, normalize(lightPosition - worldPosition))), 0.05);

    float shadow = currentDepth - bias > closestDepth ? 0.6 : 0.0;
    if (projCoords.z > 1.0 || projCoords.z < 0.0 || projCoords.x < 0.0 || projCoords.x > 1.0 || projCoords.y < 0.0 || projCoords.y > 1.0) {
        return 0.6; // Fully shadowed
    }

    return shadow;
}

void main()
{
    // Lighting calculations
    vec3 lightDir = lightPosition - worldPosition;
    float lightDist = dot(lightDir, lightDir);
    lightDir = normalize(lightDir);

    // Calculate shadow factor
    float shadow = calculateShadow(fragPosLightSpace);

    // Compute diffuse lighting with shadow
    vec3 lighting = lightIntensity * clamp(dot(lightDir, worldNormal), 0.0, 1.0) / lightDist;
    lighting *= (1.0 - shadow); // Attenuate light by shadow factor

    // Tone mapping
    vec3 toneMapped = lighting / (1.0 + lighting);

    // Gamma correction
    finalColor = pow(toneMapped, vec3(1.0 / 2.2));
}
