#pragma once
#include "Model.h"
#include <glm/gtc/matrix_transform.hpp>
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
class Planet : public Model
{
public:
    Transform transform;
    std::list<std::unique_ptr<Planet>> children;
    Planet* parent = nullptr;
    // constructor, expects a filepath to a 3D model.
    Planet(string const& path, bool gamma = false) : Model(path)
    {}
    template<typename... TArgs>
    void addChild(const TArgs&... args)
    {
        children.emplace_back(std::make_unique<Planet>(args...));
        children.back()->parent = this;
    }

    void updateSelfAndChild()
    {
        if (parent)
            transform.modelMatrix = parent->transform.computeModelMatrix(transform.getLocalModelMatrix());
        else
            transform.modelMatrix = transform.getLocalModelMatrix();

        for (auto&& child : children)
        {
            child->updateSelfAndChild();
        }
    }
};

