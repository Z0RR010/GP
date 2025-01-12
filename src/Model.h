#pragma once
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <Shader.h>
#include "Mesh.h"
#include <string>
#include <glm/gtc/matrix_transform.hpp>
enum MovementDirection {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    UP,
    DOWN
};
struct Transform
{
    /*SPACE INFORMATION*/
    //Local space information
    glm::vec3 pos = { 0.0f, 0.0f, 0.0f };
    glm::vec3 eulerRot = { 0.0f, 0.0f, 0.0f };
    glm::vec3 scale = { 1.0f, 1.0f, 1.0f };

    //Global space information concatenate in matrix
    glm::mat4 modelMatrix = glm::mat4(1.0f);
    glm::mat4 getLocalModelMatrix()
    {
        const glm::mat4 transformX = glm::rotate(glm::mat4(1.0f),
            glm::radians(eulerRot.x),
            glm::vec3(1.0f, 0.0f, 0.0f));
        const glm::mat4 transformY = glm::rotate(glm::mat4(1.0f),
            glm::radians(eulerRot.y),
            glm::vec3(0.0f, 1.0f, 0.0f));
        const glm::mat4 transformZ = glm::rotate(glm::mat4(1.0f),
            glm::radians(eulerRot.z),
            glm::vec3(0.0f, 0.0f, 1.0f));

        // Y * X * Z
        const glm::mat4 rotationMatrix = transformY * transformX * transformZ;
        // translation * rotation * scale (also know as TRS matrix)
        return glm::translate(glm::mat4(1.0f), pos) *
            rotationMatrix *
            glm::scale(glm::mat4(1.0f), scale);
    }

    glm::mat4 computeModelMatrix(const glm::mat4& parentGlobalModelMatrix)
    {
        return parentGlobalModelMatrix * getLocalModelMatrix();
    }
};
using namespace std;
class Model
{
public:
    Model() {};
    Model(string path)
    {
        transformMatrix = transform.getLocalModelMatrix();
        loadModel(path);
    }
    Model(Mesh mesh)
    {
        meshes.push_back(mesh);
    }
    void Draw(Shader& shader);
    Transform transform;
    glm::mat4 transformMatrix;
    bool Instanced;
    vector<Mesh> meshes;
public:
    // model data
    
    string directory;
    vector<Texture> textures_loaded;
    void loadModel(string path);
    void processNode(aiNode* node, const aiScene* scene);
    Mesh processMesh(aiMesh* mesh, const aiScene* scene);
    vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type,
        string typeName);

	
};

