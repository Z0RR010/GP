#pragma once
#define _USE_MATH_DEFINES
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <string>
#include "Shader.h"
#include <cmath>
class Building;


using namespace std;
struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
};
struct Texture {
    unsigned int id;
    string type;
    string path;
};
using namespace std;
class Mesh
{
    public:
        // mesh data
        vector<Vertex> vertices;
        vector<unsigned int> indices;
        vector<Texture>      textures;
        glm::vec3 color;
        Building* orbitPlanet;
        Mesh(){}
        Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures);
        Mesh Regenerate(int lod);
        static Mesh generateOrbit(int slices, int stacks, Building p, bool setup = true);
        void generateCylinder(float radius, float height, int numSegments);
        void Draw(Shader& shader);
    public:
        //  render data
        unsigned int VAO, VBO, EBO;
        bool Generated = false;
        bool orbit = true;
        
        float height;
        float radius;
        void setupMesh();
    
};

