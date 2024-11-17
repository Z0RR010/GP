#pragma once
#include "Model.h"
#include <GLFW/glfw3.h> // Include glfw3.h after our OpenGL definitions

class Planet : public Model
{
public:
    
    std::list<std::shared_ptr<Planet>> children;
    Planet* parent = nullptr;
    int Speed;
    float ModY;
    bool Generated = false;
    // constructor, expects a filepath to a 3D model.
    Planet(string const& path, int speed) : Model(path)
    {
        this->Speed = speed;
        //ModX = static_cast <float> (rand()) / static_cast <float> (RAND_MAX) /2;
        if (static_cast <float> (rand()) / static_cast <float> (RAND_MAX) > 0.5)
        {
            transform.eulerRot.x = static_cast <float> (rand()) * 2 / static_cast <float> (RAND_MAX) - 1 * 3;
        }
        else
        {
            transform.eulerRot.z = static_cast <float> (rand()) * 2 / static_cast <float> (RAND_MAX) - 1 * 3;
        }
        ModY = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
    }
    Planet(Mesh mesh, int speed=0) : Model(mesh)
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

    Planet GenerateOrbit()
    {
        Mesh m = Mesh::generateOrbit(30, 30, *this);
        Planet orbit = Planet(m);
        orbit.transform.eulerRot = this->transform.eulerRot;
        return orbit;
    }


    shared_ptr<Planet> addChild(Planet planet)
    {
        shared_ptr<Planet> c = std::make_shared<Planet>(planet);
        children.emplace_back(c);
        children.back()->parent = this;
        return c;
    }

    void Rotate()
    {
        transform.eulerRot.y += 0.1 * Speed * ModY;
    }
    
    void updateSelfAndChild()
    {
        
        if (parent)
        {
            transform.modelMatrix = transform.computeModelMatrix(parent->transform.modelMatrix);
        }
        else
            transform.modelMatrix = transform.getLocalModelMatrix();
        
        for (auto& child : children)
        {
            child->updateSelfAndChild();
        }
    }

};

