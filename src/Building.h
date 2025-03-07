#pragma once
#include "Model.h"
#include <GLFW/glfw3.h> // Include glfw3.h after our OpenGL definitions

class Building : public Model
{
    
private:
    glm::vec3 GetPositionFromMatrix(const glm::mat4& matrix) {
        return glm::vec3(matrix[3][0], matrix[3][1], matrix[3][2]);
    }
public:
    
    std::list<std::shared_ptr<Building>> children;
    Building* parent = nullptr;
    int Speed;
    float ModY;
    bool Generated = false;
    bool indexed = false;
    unsigned int bufferVBO;
    int index;
    Light light;
    bool hasLight = false;
    bool dirty = true;
    float cutoff = 12.5;
    float ocutoff = 17.5;
    bool reflect = false;
    float refractionRatio = 1.0;
    // constructor, expects a filepath to a 3D model.
    Building() {};
    Building(int bufferVBO, int ID) : Model()
    {
        //meshes.at(0).VAO = VAO;
        this->bufferVBO = bufferVBO;
        index = ID;
        indexed = true;
    }
    Building(string const& path) : Model(path)
    {
        //ModX = static_cast <float> (rand()) / static_cast <float> (RAND_MAX) /2;
        /*if (static_cast <float> (rand()) / static_cast <float> (RAND_MAX) > 0.5)
        {
            transform.eulerRot.x = static_cast <float> (rand()) * 2 / static_cast <float> (RAND_MAX) - 1 * 3;
        }
        else
        {
            transform.eulerRot.z = static_cast <float> (rand()) * 2 / static_cast <float> (RAND_MAX) - 1 * 3;
        }
        ModY = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);*/
    }
    Building(Mesh mesh, int speed=0) : Model(mesh)
    {
        this->Speed = speed;
        ModY = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
        if (static_cast <float> (rand()) / static_cast <float> (RAND_MAX) > 0.5)
        {
            transform.eulerRot.x = static_cast <float> (rand()) * 2 / static_cast <float> (RAND_MAX) - 1 * 3;
        }
        else
        {
            transform.eulerRot.z = static_cast <float> (rand()) * 2 / static_cast <float> (RAND_MAX) - 1 * 3;
        }
        
        Generated = true;
    }

    //Building GenerateOrbit()
    //{
    //    Mesh m = Mesh::generateOrbit(30, 30, *this);
    //    Building orbit = Building(m);
    //    orbit.transform.eulerRot = this->transform.eulerRot;
    //    return orbit;
    //}


    shared_ptr<Building> addChild(Building planet)
    {
        shared_ptr<Building> c = std::make_shared<Building>(planet);
        children.emplace_back(c);
        c->parent = this;
        return c;
    }
    
    void updateSelfAndChild()
    {

        if ((parent && parent->dirty))
        {
            dirty = true;
        }
        if (!hasLight)
        {
            if (dirty)
            {
                if (parent)
                {
                    /*cout << parent->transform.scale[0] << endl;*/
                    transform.modelMatrix = transform.computeModelMatrix(parent->transform.modelMatrix);
                }
                else
                    transform.modelMatrix = transform.getLocalModelMatrix();
                if (indexed)
                {
                    //cout << bufferVBO << endl;
                    glBindBuffer(GL_ARRAY_BUFFER, bufferVBO);

                    glBufferSubData(GL_ARRAY_BUFFER, index * sizeof(glm::mat4), sizeof(glm::mat4), &transform.modelMatrix);

                    //assert(transform.modelMatrix[0][0] == retrievedMatrix[0][0]);
                }
            }
        }
        else
        {
            if (dirty)
            {
                if (parent)
                {
                    transform.modelMatrix = transform.getLocalModelMatrix();
                    light.position = GetPositionFromMatrix(transform.computeModelMatrix(parent->transform.modelMatrix));
                    transform.modelMatrix = transform.computeModelMatrix(parent->transform.modelMatrix);
                }
                else
                {
                    light.position = transform.pos;
                }
                light.cutoff = glm::cos(glm::radians(cutoff));
                light.outerCutoff = glm::cos(glm::radians(ocutoff));
            }
        }
        
        
        
        for (auto& child : children)
        {
            child->updateSelfAndChild();
        }
        dirty = false;
    }
    void Draw(Shader& shader)
    {
        shader.setMat4("model", transform.modelMatrix);
        shader.setBool("reflection", reflect);
        shader.setFloat("refractRatio", refractionRatio);
        for (unsigned int i = 0; i < meshes.size(); i++)
            meshes[i].Draw(shader);
        shader.setMat4("model", glm::mat4());
    }
    
};
