#ifndef MYBOT_H
#define MYBOT_H

#include "utils/lightInfo.h"
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#include <tinygltf-2.9.3/tiny_gltf.h>


#include <vector>
#include <map>
#include <string>

class MyBot {
public:
    // Constructor and Destructor
    MyBot();
    ~MyBot();

    // Initialize the bot (load model, prepare buffers, etc.)
    bool initialize(const char* modelPath, Light lightInfo);

    // Update animation state
    void update(float time);

    // Render the bot
    void render(glm::mat4 cameraMatrix, Light lightInfo, glm::mat4 lightSpaceMatrix, GLuint shadowMapID);
    void renderDepth(GLuint shadowShaderProgramID, glm::mat4 lightSpaceMatrix);

    // Cleanup resources
    void cleanup();

    // Getters and setters for animation and rendering parameters
    void setPlaybackSpeed(float speed);
    float getPlaybackSpeed() const;

    void setLoopParameters(float startTime, float endTime);
    void enableLooping(bool enable);

private:
    // Nested structs for managing bot components
    struct PrimitiveObject {
        GLuint vao;
        std::map<int, GLuint> vbos;
    };

    struct SkinObject {
        std::vector<glm::mat4> inverseBindMatrices;
        std::vector<glm::mat4> globalJointTransforms;
        std::vector<glm::mat4> jointMatrices;
    };

    struct SamplerObject {
        std::vector<float> input;
        std::vector<glm::vec4> output;
        int interpolation;
    };

    struct AnimationObject {
        std::vector<SamplerObject> samplers;
    };

    // Private helper methods
    bool loadModel(tinygltf::Model &model, const char *filename);

    void bindMesh(std::vector<PrimitiveObject> &primitiveObjects,
                  tinygltf::Model &model, tinygltf::Mesh &mesh);

    void bindModelNodes(std::vector<PrimitiveObject> &primitiveObjects,
                        tinygltf::Model &model,
                        tinygltf::Node &node);

    std::vector<PrimitiveObject> bindModel(tinygltf::Model &model);

    void drawMesh(const std::vector<PrimitiveObject> &primitiveObjects,
                  tinygltf::Model &model, tinygltf::Mesh &mesh);

    void drawModelNodes(const std::vector<PrimitiveObject>& primitiveObjects,
                        tinygltf::Model &model, tinygltf::Node &node);

    void drawModel(const std::vector<PrimitiveObject>& primitiveObjects,
                   tinygltf::Model &model);

    glm::mat4 getNodeTransform(const tinygltf::Node& node);

    void computeLocalNodeTransform(const tinygltf::Model& model,
                                   int nodeIndex,
                                   std::vector<glm::mat4> &localTransforms);

    void computeGlobalNodeTransform(const tinygltf::Model& model,
                                    const std::vector<glm::mat4> &localTransforms,
                                    int nodeIndex,
                                    const glm::mat4& parentTransform,
                                    std::vector<glm::mat4> &globalTransforms);

    std::vector<SkinObject> prepareSkinning(const tinygltf::Model &model);

    int findKeyframeIndex(const std::vector<float>& times, float animationTime);

    std::vector<AnimationObject> prepareAnimation(const tinygltf::Model &model);

    void updateAnimation(const tinygltf::Model &model,
                         const tinygltf::Animation &anim,
                         const AnimationObject &animationObject,
                         float time,
                         std::vector<glm::mat4> &nodeTransforms);

    void updateSkinning(const tinygltf::Skin &skin, const std::vector<glm::mat4> &nodeTransforms);

    // Private member variables
    tinygltf::Model model;
    std::vector<PrimitiveObject> primitiveObjects;
    std::vector<SkinObject> skinObjects;
    std::vector<AnimationObject> animationObjects;
    glm::vec3 position; // Current position of the bot
    float speed;        // Speed of movement

    // Shader and rendering variables
    GLuint programID;
    GLuint mvpMatrixID;
    GLuint jointMatricesID;
    GLuint lightPositionID;
    GLuint lightIntensityID;

    // Animation parameters
    float loopStartTime;
    float loopEndTime;
    bool useLooping;
};

#endif // MYBOT_H