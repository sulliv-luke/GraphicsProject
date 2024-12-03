#include "Particle.h"
#include <cstdlib> // For random number generation
#include <iostream>

// Constructor
ParticleSystem::ParticleSystem(int maxParticles, GLuint shaderProgramID)
    : shaderProgramID(shaderProgramID) {
    particles.resize(maxParticles);

    // Generate VAO and VBO
    glGenVertexArrays(1, &particleVAO);
    glGenBuffers(1, &particleVBO);
    glBindVertexArray(particleVAO);
    glBindBuffer(GL_ARRAY_BUFFER, particleVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Particle) * particles.size(), nullptr, GL_DYNAMIC_DRAW);

    // Enable vertex attributes
    glEnableVertexAttribArray(0); // Position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (void*)offsetof(Particle, position));

    glEnableVertexAttribArray(1); // Color
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Particle), (void*)offsetof(Particle, color));

    glEnableVertexAttribArray(2); // Size
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(Particle), (void*)offsetof(Particle, size));

    glBindVertexArray(0);
}

// Destructor
ParticleSystem::~ParticleSystem() {
    cleanup();
}

// Initialize particles
void ParticleSystem::initialize(glm::vec3 start, glm::vec3 end) {
    for (size_t i = 0; i < particles.size(); ++i) {
        float t = static_cast<float>(i) / particles.size(); // Evenly distribute particles
        particles[i].position = glm::mix(start, end, t);    // Interpolate position
        particles[i].velocity = glm::normalize(end - start) * 10.0f; // Constant velocity
        particles[i].color = glm::vec4(0.2f, 0.8f, 1.0f, 1.0f);       // Cyan color
        particles[i].size = 5.0f + static_cast<float>(rand()) / RAND_MAX * 5.0f;
        particles[i].life = t; // Use 'life' to store progress along the path

        // Assign random offset
        particles[i].randomOffset = glm::vec3(
            (static_cast<float>(rand()) / RAND_MAX - 0.5f) * 2.0f, // Small X offset
            (static_cast<float>(rand()) / RAND_MAX - 0.5f) * 2.0f, // Small Y offset
            (static_cast<float>(rand()) / RAND_MAX - 0.5f) * 2.0f  // Small Z offset
        );
    }
}

void ParticleSystem::update(float deltaTime, glm::vec3 start, glm::vec3 end) {
    for (auto &particle : particles) {
        // Increment progress along the path
        particle.life += deltaTime * 0.1f; // Adjust speed factor as needed
        if (particle.life >= 1.0f) {
            particle.life -= 1.0f; // Wrap around to the start of the path
        }

        // Base position along the main wave path
        glm::vec3 basePosition = glm::mix(start, end, particle.life);

        // Apply per-particle random offset
        glm::vec3 offset = particle.randomOffset;

        // Optionally, make the offset dynamic (e.g., oscillating or noise-based)
        offset += glm::vec3(
            sin(glfwGetTime() + particle.life * 10.0f) * 0.5f,  // Dynamic X
            cos(glfwGetTime() + particle.life * 15.0f) * 0.5f,  // Dynamic Y
            0.0f                                                // No Z oscillation
        );

        // Update position with both base and offset
        particle.position = basePosition + offset;
    }

    // Update particle buffer
    glBindBuffer(GL_ARRAY_BUFFER, particleVBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Particle) * particles.size(), particles.data());
}



// Render particles
void ParticleSystem::render(glm::mat4 vpMatrix) {
    glUseProgram(shaderProgramID);
    glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "vpMatrix"), 1, GL_FALSE, &vpMatrix[0][0]);

    glBindVertexArray(particleVAO);
    glDrawArrays(GL_POINTS, 0, particles.size());
    glBindVertexArray(0);
}


// Cleanup resources
void ParticleSystem::cleanup() {
    glDeleteBuffers(1, &particleVBO);
    glDeleteVertexArrays(1, &particleVAO);
}
