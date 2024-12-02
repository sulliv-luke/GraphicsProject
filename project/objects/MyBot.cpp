#include "MyBot.h"
#include <render/shader.h>
#include <iostream>
#include <cmath>
#include <sstream>
#define TINYGLTF_IMPLEMENTATION
#include <tinygltf-2.9.3/tiny_gltf.h>

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

// Global lighting and rendering parameters (you might want to make these configurable)
static glm::vec3 lightIntensity(5e6f, 5e6f, 5e6f);
static glm::vec3 lightPosition(-275.0f, 500.0f, 800.0f);

MyBot::MyBot()
	: loopStartTime(0.5f), loopEndTime(2.5f), useLooping(true),
	  programID(0), mvpMatrixID(0), jointMatricesID(0),
	  lightPositionID(0), lightIntensityID(0),
	  position(0.0f, 0.0f, -500.0f), speed(3.0f) {} // Initial position and speed


MyBot::~MyBot() {
    cleanup();
}

bool MyBot::initialize(const char* modelPath) {
    // Load model
    if (!loadModel(model, modelPath)) {
        return false;
    }

    // Prepare buffers for rendering
    primitiveObjects = bindModel(model);

    // Prepare joint matrices
    skinObjects = prepareSkinning(model);

    // Prepare animation data
    animationObjects = prepareAnimation(model);

    // Create and compile GLSL program from shaders
    // Modify shader paths as needed
    programID = LoadShadersFromFile("../project/model/bot.vert", "../project/model/bot.frag");
    if (programID == 0) {
        std::cerr << "Failed to load shaders." << std::endl;
        return false;
    }

    // Get handles for GLSL variables
    mvpMatrixID = glGetUniformLocation(programID, "MVP");
    lightPositionID = glGetUniformLocation(programID, "lightPosition");
    lightIntensityID = glGetUniformLocation(programID, "lightIntensity");
    jointMatricesID = glGetUniformLocation(programID, "jointMatrices");

    return true;
}

void MyBot::update(float time) {
    if (model.animations.size() > 0) {
        const tinygltf::Animation &animation = model.animations[0];
        const AnimationObject &animationObject = animationObjects[0];

        const tinygltf::Skin &skin = model.skins[0];
        std::vector<glm::mat4> localTransforms(model.nodes.size(), glm::mat4(1.0f));
        int rootNodeIndex = skin.joints[0];

        // Initialize localTransforms with initial node transforms
        computeLocalNodeTransform(model, rootNodeIndex, localTransforms);

        // Determine the animation time
        float animationTime;
        if (useLooping) {
            // Calculate total animation duration from input times
            float totalDuration = animationObject.samplers[0].input.back();

            // If current time is before loop start, reset to loop start
            if (time < loopStartTime) {
                animationTime = loopStartTime;
            }
            // If current time is past loop end, wrap back to loop start
            else if (time > loopEndTime) {
                // Adjust time to loop segment
                animationTime = loopStartTime +
                    fmod(time - loopEndTime, loopEndTime - loopStartTime);
            }
            // Otherwise, use the current time
            else {
                animationTime = time;
            }
        } else {
            // Default behavior - use full animation duration
            animationTime = time;
        }

        // Update local transforms with animation data
        updateAnimation(model, animation, animationObject, animationTime, localTransforms);

        // Recompute global transforms
        std::vector<glm::mat4> globalTransforms(model.nodes.size(), glm::mat4(1.0f));
        computeGlobalNodeTransform(model, localTransforms, rootNodeIndex, glm::mat4(1.0f), globalTransforms);

        // Update skinning
        updateSkinning(model.skins[0], globalTransforms);
    }
	// Move the bot forward along the Z-axis
	position.z += speed * 0.016f; // Adjust 0.016 for time delta if available
	if (position.z > 500.0f) { // Example condition
		position.z = -500.0f;  // Reset position
	}

}

void MyBot::render(glm::mat4 cameraMatrix) {
    glUseProgram(programID);

	glm::mat4 modelMatrix = glm::mat4(1.0f);
	modelMatrix = glm::translate(modelMatrix, position); // Adjust position here
	modelMatrix = glm::scale(modelMatrix, glm::vec3(0.25f, 0.25f, 0.25f)); // Scale down to a quarter
    // Set camera
    glm::mat4 mvp = cameraMatrix * modelMatrix;
    glUniformMatrix4fv(mvpMatrixID, 1, GL_FALSE, &mvp[0][0]);

    // Set animation data for linear blend skinning in shader
    glUniformMatrix4fv(jointMatricesID, skinObjects[0].jointMatrices.size(), GL_FALSE, glm::value_ptr(skinObjects[0].jointMatrices[0]));

    // Set light data
    glUniform3fv(lightPositionID, 1, &lightPosition[0]);
    glUniform3fv(lightIntensityID, 1, &lightIntensity[0]);

    // Draw the GLTF model
    drawModel(primitiveObjects, model);
}

void MyBot::cleanup() {
    glDeleteProgram(programID);
}

void MyBot::setPlaybackSpeed(float speed) {
    // Implement your speed adjustment logic here
}

float MyBot::getPlaybackSpeed() const {
    // Implement method to return current playback speed
    return 1.0f; // Default implementation
}

void MyBot::setLoopParameters(float startTime, float endTime) {
    loopStartTime = startTime;
    loopEndTime = endTime;
}

void MyBot::enableLooping(bool enable) {
    useLooping = enable;
}

// The rest of the implementation methods from the original code would go here
// (loadModel, bindMesh, computeLocalNodeTransform, etc.)
// I've omitted them for brevity, but you would copy the existing implementations

// ... (include all other method implementations from the original code)

glm::mat4 MyBot::getNodeTransform(const tinygltf::Node& node) {
		glm::mat4 transform(1.0f);

		if (node.matrix.size() == 16) {
			transform = glm::make_mat4(node.matrix.data());
		} else {
			if (node.translation.size() == 3) {
				transform = glm::translate(transform, glm::vec3(node.translation[0], node.translation[1], node.translation[2]));
			}
			if (node.rotation.size() == 4) {
				glm::quat q(node.rotation[3], node.rotation[0], node.rotation[1], node.rotation[2]);
				transform *= glm::mat4_cast(q);
			}
			if (node.scale.size() == 3) {
				transform = glm::scale(transform, glm::vec3(node.scale[0], node.scale[1], node.scale[2]));
			}
		}
		return transform;
	}

	void MyBot::computeLocalNodeTransform(const tinygltf::Model& model,
						   int nodeIndex,
						   std::vector<glm::mat4> &localTransforms)
	{
		const tinygltf::Node& node = model.nodes[nodeIndex];

		// Compute the local transformation using the provided helper function.
		localTransforms[nodeIndex] = getNodeTransform(node);

		// Recursively compute the local transform for each child node.
		for (int childIndex : node.children) {
			computeLocalNodeTransform(model, childIndex, localTransforms);
		}
	}

	void MyBot::computeGlobalNodeTransform(const tinygltf::Model& model,
								 const std::vector<glm::mat4> &localTransforms,
								 int nodeIndex,
								 const glm::mat4& parentTransform,
								 std::vector<glm::mat4> &globalTransforms)
	{
		// Combine the parent's transform with the node's local transform.
		globalTransforms[nodeIndex] = parentTransform * localTransforms[nodeIndex];

		const tinygltf::Node& node = model.nodes[nodeIndex];

		// Recursively compute the global transforms for child nodes.
		for (int childIndex : node.children) {
			computeGlobalNodeTransform(model, localTransforms, childIndex, globalTransforms[nodeIndex], globalTransforms);
		}
	}

	std::vector<MyBot::SkinObject> MyBot::prepareSkinning(const tinygltf::Model &model) {
    std::vector<SkinObject> skinObjects;

    for (size_t i = 0; i < model.skins.size(); i++) {
        SkinObject skinObject;

        const tinygltf::Skin &skin = model.skins[i];

        // Read inverseBindMatrices
        const tinygltf::Accessor &accessor = model.accessors[skin.inverseBindMatrices];
        assert(accessor.type == TINYGLTF_TYPE_MAT4);
        const tinygltf::BufferView &bufferView = model.bufferViews[accessor.bufferView];
        const tinygltf::Buffer &buffer = model.buffers[bufferView.buffer];
        const float *ptr = reinterpret_cast<const float *>(
            buffer.data.data() + accessor.byteOffset + bufferView.byteOffset);

        // Populate inverseBindMatrices
        skinObject.inverseBindMatrices.resize(accessor.count);
        for (size_t j = 0; j < accessor.count; j++) {
            float m[16];
            memcpy(m, ptr + j * 16, 16 * sizeof(float));
            skinObject.inverseBindMatrices[j] = glm::make_mat4(m);
        }

        assert(skin.joints.size() == accessor.count);

        skinObject.globalJointTransforms.resize(skin.joints.size());
        skinObject.jointMatrices.resize(skin.joints.size());

        // Prepare to compute node transforms
        std::vector<glm::mat4> localTransforms(model.nodes.size(), glm::mat4(1.0f));
        std::vector<glm::mat4> globalTransforms(model.nodes.size(), glm::mat4(1.0f));

        // Compute transforms starting from the root node of the skeleton
    	int rootNodeIndex = skin.joints[0];
        computeLocalNodeTransform(model, rootNodeIndex, localTransforms);
    	// Compute global transforms starting from root nodes
    	computeGlobalNodeTransform(model, localTransforms,rootNodeIndex, glm::mat4(1.0f), globalTransforms);
    	// Step 3: Compute joint matrices
    	for (size_t j = 0; j < skin.joints.size(); j++) {
    		int jointIndex = skin.joints[j];
    		skinObject.jointMatrices[j] = globalTransforms[jointIndex] * skinObject.inverseBindMatrices[j];
    	}

        skinObjects.push_back(skinObject);
    }
    return skinObjects;
}




	int MyBot::findKeyframeIndex(const std::vector<float>& times, float animationTime)
	{
		int left = 0;
		int right = times.size() - 1;

		while (left <= right) {
			int mid = (left + right) / 2;

			if (mid + 1 < times.size() && times[mid] <= animationTime && animationTime < times[mid + 1]) {
				return mid;
			}
			else if (times[mid] > animationTime) {
				right = mid - 1;
			}
			else { // animationTime >= times[mid + 1]
				left = mid + 1;
			}
		}

		// Target not found
		return times.size() - 2;
	}

	std::vector<MyBot::AnimationObject> MyBot::prepareAnimation(const tinygltf::Model &model)
	{
		std::vector<AnimationObject> animationObjects;
		for (const auto &anim : model.animations) {
			AnimationObject animationObject;

			for (const auto &sampler : anim.samplers) {
				SamplerObject samplerObject;

				const tinygltf::Accessor &inputAccessor = model.accessors[sampler.input];
				const tinygltf::BufferView &inputBufferView = model.bufferViews[inputAccessor.bufferView];
				const tinygltf::Buffer &inputBuffer = model.buffers[inputBufferView.buffer];

				assert(inputAccessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT);
				assert(inputAccessor.type == TINYGLTF_TYPE_SCALAR);

				// Input (time) values
				samplerObject.input.resize(inputAccessor.count);

				const unsigned char *inputPtr = &inputBuffer.data[inputBufferView.byteOffset + inputAccessor.byteOffset];
				const float *inputBuf = reinterpret_cast<const float*>(inputPtr);

				// Read input (time) values
				int stride = inputAccessor.ByteStride(inputBufferView);
				for (size_t i = 0; i < inputAccessor.count; ++i) {
					samplerObject.input[i] = *reinterpret_cast<const float*>(inputPtr + i * stride);
				}

				const tinygltf::Accessor &outputAccessor = model.accessors[sampler.output];
				const tinygltf::BufferView &outputBufferView = model.bufferViews[outputAccessor.bufferView];
				const tinygltf::Buffer &outputBuffer = model.buffers[outputBufferView.buffer];

				assert(outputAccessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT);

				const unsigned char *outputPtr = &outputBuffer.data[outputBufferView.byteOffset + outputAccessor.byteOffset];
				const float *outputBuf = reinterpret_cast<const float*>(outputPtr);

				int outputStride = outputAccessor.ByteStride(outputBufferView);

				// Output values
				samplerObject.output.resize(outputAccessor.count);

				for (size_t i = 0; i < outputAccessor.count; ++i) {

					if (outputAccessor.type == TINYGLTF_TYPE_VEC3) {
						memcpy(&samplerObject.output[i], outputPtr + i * 3 * sizeof(float), 3 * sizeof(float));
					} else if (outputAccessor.type == TINYGLTF_TYPE_VEC4) {
						memcpy(&samplerObject.output[i], outputPtr + i * 4 * sizeof(float), 4 * sizeof(float));
					} else {
						std::cout << "Unsupport accessor type ..." << std::endl;
					}

				}

				animationObject.samplers.push_back(samplerObject);
			}

			animationObjects.push_back(animationObject);
		}
		return animationObjects;
	}

	void MyBot::updateAnimation(
    const tinygltf::Model &model,
    const tinygltf::Animation &anim,
    const AnimationObject &animationObject,
    float time,
    std::vector<glm::mat4> &nodeTransforms)
{
    // For each node, store separate components
    struct TransformComponents {
        glm::vec3 translation = glm::vec3(0.0f);
        glm::quat rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
        glm::vec3 scale = glm::vec3(1.0f);
    };

    std::vector<TransformComponents> nodeComponents(model.nodes.size());

    // Initialize with initial node transforms
    for (size_t i = 0; i < model.nodes.size(); ++i) {
        const tinygltf::Node &node = model.nodes[i];

        if (node.translation.size() == 3) {
            nodeComponents[i].translation = glm::vec3(
                node.translation[0], node.translation[1], node.translation[2]);
        }
        if (node.rotation.size() == 4) {
            nodeComponents[i].rotation = glm::quat(
                node.rotation[3], node.rotation[0], node.rotation[1], node.rotation[2]);
        }
        if (node.scale.size() == 3) {
            nodeComponents[i].scale = glm::vec3(
                node.scale[0], node.scale[1], node.scale[2]);
        }
    }

    // Apply animation data
    for (const auto &channel : anim.channels) {
        int targetNodeIndex = channel.target_node;
        const auto &sampler = anim.samplers[channel.sampler];

        // Calculate current animation time (wrap if necessary)
        const std::vector<float> &times = animationObject.samplers[channel.sampler].input;
        float animationTime = fmod(time, times.back());

        // Find keyframes
        int keyframeIndex = findKeyframeIndex(times, animationTime);
        int nextKeyframeIndex = (keyframeIndex + 1) % times.size();

        // Calculate interpolation factor
        float t0 = times[keyframeIndex];
        float t1 = times[nextKeyframeIndex];
        float factor = (animationTime - t0) / (t1 - t0);

        // Get output data
        const std::vector<glm::vec4> &outputs = animationObject.samplers[channel.sampler].output;

        if (channel.target_path == "translation") {
            glm::vec3 translation0 = glm::vec3(outputs[keyframeIndex]);
            glm::vec3 translation1 = glm::vec3(outputs[nextKeyframeIndex]);

            // Linearly interpolate
            glm::vec3 translation = glm::mix(translation0, translation1, factor);
            nodeComponents[targetNodeIndex].translation = translation;

        } else if (channel.target_path == "rotation") {
            glm::quat rotation0(outputs[keyframeIndex].w, outputs[keyframeIndex].x,
                                outputs[keyframeIndex].y, outputs[keyframeIndex].z);
            glm::quat rotation1(outputs[nextKeyframeIndex].w, outputs[nextKeyframeIndex].x,
                                outputs[nextKeyframeIndex].y, outputs[nextKeyframeIndex].z);

            // Spherical linear interpolation
        	glm::quat rotation = glm::slerp(rotation0, rotation1, factor);
        	if (glm::dot(rotation0, rotation1) < 0.0f) {
        		rotation0 = -rotation0;
        	}
            nodeComponents[targetNodeIndex].rotation = rotation;

        } else if (channel.target_path == "scale") {
            glm::vec3 scale0 = glm::vec3(outputs[keyframeIndex]);
            glm::vec3 scale1 = glm::vec3(outputs[nextKeyframeIndex]);

            // Linearly interpolate
            glm::vec3 scale = glm::mix(scale0, scale1, factor);
            nodeComponents[targetNodeIndex].scale = scale;
        }
    }

    // Reconstruct node transforms
    for (size_t i = 0; i < model.nodes.size(); ++i) {
        nodeTransforms[i] = glm::translate(glm::mat4(1.0f), nodeComponents[i].translation) *
                            glm::mat4_cast(nodeComponents[i].rotation) *
                            glm::scale(glm::mat4(1.0f), nodeComponents[i].scale);
    }
}


	void MyBot::updateSkinning(const tinygltf::Skin &skin, const std::vector<glm::mat4> &nodeTransforms) {
		for (SkinObject &skinObject : skinObjects) {
			// Loop through each joint in the skin
			for (size_t i = 0; i < skinObject.jointMatrices.size(); ++i) {
				int jointIndex = skin.joints[i];
				// Compute the joint matrix: Global transform * Inverse bind matrix
				skinObject.jointMatrices[i] = nodeTransforms[jointIndex] * skinObject.inverseBindMatrices[i];
			}
		}
	}

bool MyBot::loadModel(tinygltf::Model &model, const char *filename) {
	tinygltf::TinyGLTF loader;
	std::string err;
	std::string warn;

	bool res = loader.LoadASCIIFromFile(&model, &err, &warn, filename);
	if (!warn.empty()) {
		std::cout << "WARN: " << warn << std::endl;
	}

	if (!err.empty()) {
		std::cout << "ERR: " << err << std::endl;
	}

	if (!res)
		std::cout << "Failed to load glTF: " << filename << std::endl;
	else
		std::cout << "Loaded glTF: " << filename << std::endl;

	return res;
}

void MyBot::bindMesh(std::vector<PrimitiveObject> &primitiveObjects,
              tinygltf::Model &model, tinygltf::Mesh &mesh) {

    std::map<int, GLuint> vbos;
    for (size_t i = 0; i < model.bufferViews.size(); ++i) {
        const tinygltf::BufferView &bufferView = model.bufferViews[i];

        int target = bufferView.target;

        if (bufferView.target == 0) {
            continue;
        }

        const tinygltf::Buffer &buffer = model.buffers[bufferView.buffer];
        GLuint vbo;
        glGenBuffers(1, &vbo);
        glBindBuffer(target, vbo);
        glBufferData(target, bufferView.byteLength,
                     &buffer.data.at(0) + bufferView.byteOffset, GL_STATIC_DRAW);

        vbos[i] = vbo;
    }

    for (size_t i = 0; i < mesh.primitives.size(); ++i) {

        tinygltf::Primitive primitive = mesh.primitives[i];
        tinygltf::Accessor indexAccessor = model.accessors[primitive.indices];

        GLuint vao;
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        for (auto &attrib : primitive.attributes) {
            tinygltf::Accessor accessor = model.accessors[attrib.second];
            int byteStride =
                accessor.ByteStride(model.bufferViews[accessor.bufferView]);
            glBindBuffer(GL_ARRAY_BUFFER, vbos[accessor.bufferView]);

            int size = 1;
            if (accessor.type != TINYGLTF_TYPE_SCALAR) {
                size = accessor.type;
            }

            if (attrib.first.compare("POSITION") == 0) {
                int vaa = 0;
                glEnableVertexAttribArray(vaa);
                glVertexAttribPointer(vaa, size, accessor.componentType,
                                      accessor.normalized ? GL_TRUE : GL_FALSE,
                                      byteStride, BUFFER_OFFSET(accessor.byteOffset));
            } else if (attrib.first.compare("NORMAL") == 0) {
                int vaa = 1;
                glEnableVertexAttribArray(vaa);
                glVertexAttribPointer(vaa, size, accessor.componentType,
                                      accessor.normalized ? GL_TRUE : GL_FALSE,
                                      byteStride, BUFFER_OFFSET(accessor.byteOffset));
            } else if (attrib.first.compare("TEXCOORD_0") == 0) {
                int vaa = 2;
                glEnableVertexAttribArray(vaa);
                glVertexAttribPointer(vaa, size, accessor.componentType,
                                      accessor.normalized ? GL_TRUE : GL_FALSE,
                                      byteStride, BUFFER_OFFSET(accessor.byteOffset));
            } else if (attrib.first.compare("JOINTS_0") == 0) {
                int vaa = 3; // Attribute location for JOINTS_0
                glEnableVertexAttribArray(vaa);
                glVertexAttribIPointer(vaa, size, accessor.componentType,
                                       byteStride, BUFFER_OFFSET(accessor.byteOffset));
            } else if (attrib.first.compare("WEIGHTS_0") == 0) {
                int vaa = 4; // Attribute location for WEIGHTS_0
                glEnableVertexAttribArray(vaa);
                glVertexAttribPointer(vaa, size, accessor.componentType,
                                      accessor.normalized ? GL_TRUE : GL_FALSE,
                                      byteStride, BUFFER_OFFSET(accessor.byteOffset));
            } else {
                std::cout << "Unrecognized attribute: " << attrib.first << std::endl;
            }
        }

        // Record VAO for later use
        PrimitiveObject primitiveObject;
        primitiveObject.vao = vao;
        primitiveObject.vbos = vbos;
        primitiveObjects.push_back(primitiveObject);

        glBindVertexArray(0);
    }
}


	void MyBot::bindModelNodes(std::vector<PrimitiveObject> &primitiveObjects,
						tinygltf::Model &model,
						tinygltf::Node &node) {
		// Bind buffers for the current mesh at the node
		if ((node.mesh >= 0) && (node.mesh < model.meshes.size())) {
			bindMesh(primitiveObjects, model, model.meshes[node.mesh]);
		}

		// Recursive into children nodes
		for (size_t i = 0; i < node.children.size(); i++) {
			assert((node.children[i] >= 0) && (node.children[i] < model.nodes.size()));
			bindModelNodes(primitiveObjects, model, model.nodes[node.children[i]]);
		}
	}

	std::vector<MyBot::PrimitiveObject> MyBot::bindModel(tinygltf::Model &model) {
		std::vector<PrimitiveObject> primitiveObjects;

		const tinygltf::Scene &scene = model.scenes[model.defaultScene];
		for (size_t i = 0; i < scene.nodes.size(); ++i) {
			assert((scene.nodes[i] >= 0) && (scene.nodes[i] < model.nodes.size()));
			bindModelNodes(primitiveObjects, model, model.nodes[scene.nodes[i]]);
		}

		return primitiveObjects;
	}

	void MyBot::drawMesh(const std::vector<PrimitiveObject> &primitiveObjects,
				tinygltf::Model &model, tinygltf::Mesh &mesh) {

		for (size_t i = 0; i < mesh.primitives.size(); ++i)
		{
			GLuint vao = primitiveObjects[i].vao;
			std::map<int, GLuint> vbos = primitiveObjects[i].vbos;

			glBindVertexArray(vao);

			tinygltf::Primitive primitive = mesh.primitives[i];
			tinygltf::Accessor indexAccessor = model.accessors[primitive.indices];

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbos.at(indexAccessor.bufferView));

			glDrawElements(primitive.mode, indexAccessor.count,
						indexAccessor.componentType,
						BUFFER_OFFSET(indexAccessor.byteOffset));

			glBindVertexArray(0);
		}
	}

	void MyBot::drawModelNodes(const std::vector<PrimitiveObject>& primitiveObjects,
						tinygltf::Model &model, tinygltf::Node &node) {
		// Draw the mesh at the node, and recursively do so for children nodes
		if ((node.mesh >= 0) && (node.mesh < model.meshes.size())) {
			drawMesh(primitiveObjects, model, model.meshes[node.mesh]);
		}
		for (size_t i = 0; i < node.children.size(); i++) {
			drawModelNodes(primitiveObjects, model, model.nodes[node.children[i]]);
		}
	}
	void MyBot::drawModel(const std::vector<PrimitiveObject>& primitiveObjects,
				tinygltf::Model &model) {
		// Draw all nodes
		const tinygltf::Scene &scene = model.scenes[model.defaultScene];
		for (size_t i = 0; i < scene.nodes.size(); ++i) {
			drawModelNodes(primitiveObjects, model, model.nodes[scene.nodes[i]]);
		}
	}

