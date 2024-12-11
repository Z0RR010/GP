#pragma once
#include "Model.h"
#include <GLFW/glfw3.h> // Include glfw3.h after our OpenGL definitions

class Building : public Model
{
public:
    
    std::list<std::shared_ptr<Building>> children;
    Building* parent = nullptr;
    int Speed;
    float ModY;
    bool Generated = false;
    bool indexed = false;
    unsigned int bufferVBO;
    int index;
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

    Building GenerateOrbit()
    {
        Mesh m = Mesh::generateOrbit(30, 30, *this);
        Building orbit = Building(m);
        orbit.transform.eulerRot = this->transform.eulerRot;
        return orbit;
    }


    shared_ptr<Building> addChild(Building planet)
    {
        shared_ptr<Building> c = std::make_shared<Building>(planet);
        children.emplace_back(c);
        children.back()->parent = this;
        return c;
    }
    
    void updateSelfAndChild()
    {
        
        if (parent)
        {
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
        
        
        for (auto& child : children)
        {
            child->updateSelfAndChild();
        }
    }

};

