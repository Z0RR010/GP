#pragma once
#define _USE_MATH_DEFINES
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <string>
#include "Shader.h"
#include <cmath>
//class Building;


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
        Mesh(){}
        Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures);
        Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<unsigned int> textID);
        static unsigned int loadCubemap(vector<std::string> faces);
        /*Mesh Regenerate(int lod);
        static Mesh generateOrbit(int slices, int stacks, Building p, bool setup = true);*/
        static Mesh generateCube(float width, float height, float depth);
        static Mesh generateCuboid(float frontWidth, float backWidth, float height, float depth);
        static Mesh generateSkybox(std::vector<std::string> faces);
        void generateCylinder(float radius, float height, int numSegments);
        bool skybox = false;
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

