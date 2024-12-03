#ifndef PARTICLE_H
#define PARTICLE_H

#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <vector>

// Particle data structure
struct Particle {
    glm::vec3 position; // Position of the particle
    glm::vec3 velocity; // Velocity of the particle
    glm::vec4 color;    // Color (RGBA)
    float size;         // Size of the particle
    float life;         // Remaining life of the particle (0 = dead)
    glm::vec3 randomOffset;   // Unique random offset for more dynamic movement
};

// Particle system class
class ParticleSystem {
private:
    std::vector<Particle> particles;
    GLuint particleVAO, particleVBO; // OpenGL buffer IDs
    GLuint shaderProgramID;          // Shader program for particles

public:
    // Constructor
    ParticleSystem(int maxParticles, GLuint shaderProgramID);

    // Destructor
    ~ParticleSystem();

    // Initialize particles
    void initialize(glm::vec3 start, glm::vec3 end);

    // Update particles
    void update(float deltaTime, glm::vec3 start, glm::vec3 end);

    // Render particles
    void render(glm::mat4 vpMatrix);

    // Cleanup resources
    void cleanup();
};

#endif // PARTICLE_H
