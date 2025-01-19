#include "Mesh.h"
//#include "Building.h"
#include "stb_image.h"
using namespace std;
Mesh::Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures)
{
    this->vertices = vertices;
    this->indices = indices;
    this->textures = textures;
    setupMesh();
}

Mesh::Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<unsigned int> textID)
{
    this->vertices = vertices;
    this->indices = indices;
    for (auto i : textID)
    {
        textures.push_back({ i,"skybox","" });
    }
    setupMesh();
}

//Mesh Mesh::Regenerate(int lod)
//{
//    if (orbit)
//    {
//        auto m = Mesh::generateOrbit(lod, lod, *orbitPlanet,false);
//        m.textures = this->textures;
//        m.color = this->color;
//        m.setupMesh();
//        return m;
//    }
//    else
//    {
//        Mesh m = Mesh();
//        m.generateCylinder(radius, height, lod);
//        m.color = this->color;
//        return m;
//    }
//}

    //Mesh Mesh::generateOrbit(int majorSegments, int minorSegments, Building p, bool setup)
    //{
    //    vector<Vertex> vertices;
    //    vector<unsigned int> indices;
    //    float majorRadius, minorRadius;
    //    if (p.transform.pos.x != 0.0f)
    //    {
    //        majorRadius = abs(p.transform.pos.x);
    //    }
    //    else if (p.transform.pos.z != 0.0f)
    //    {
    //        majorRadius = abs(p.transform.pos.z);
    //    }
    //    minorRadius = 0.05f;
    //    float majorStep = 2.0f * M_PI / majorSegments;
    //    float minorStep = 2.0f * M_PI / minorSegments;

    //    // Generate vertices
    //    for (int i = 0; i <= majorSegments; ++i) {
    //        float majorAngle = i * majorStep;
    //        glm::vec3 majorCircleCenter(
    //            cos(majorAngle) * majorRadius,
    //            0.0f,
    //            sin(majorAngle) * majorRadius
    //        );

    //        for (int j = 0; j <= minorSegments; ++j) {
    //            float minorAngle = j * minorStep;

    //            // Calculate position
    //            glm::vec3 minorOffset(
    //                cos(majorAngle) * (minorRadius * cos(minorAngle)),
    //                minorRadius * sin(minorAngle),
    //                sin(majorAngle) * (minorRadius * cos(minorAngle))
    //            );
    //            glm::vec3 position = majorCircleCenter + minorOffset;

    //            // Calculate normal
    //            glm::vec3 normal = glm::normalize(minorOffset);

    //            // Calculate texture coordinates
    //            float u = (float)i / majorSegments;
    //            float v = (float)j / minorSegments;

    //            // Add the vertex
    //            vertices.push_back({
    //                position, // Position
    //                normal,   // Normal
    //                glm::vec2(u, v) // TexCoords
    //                });
    //        }
    //    }
    //    // Generate indices
    //    for (int i = 0; i < majorSegments; ++i) {
    //        for (int j = 0; j < minorSegments; ++j) {
    //            int current = i * (minorSegments + 1) + j;
    //            int next = (i + 1) * (minorSegments + 1) + j;

    //            // Two triangles per quad
    //            indices.push_back(current);
    //            indices.push_back(next);
    //            indices.push_back(current + 1);

    //            indices.push_back(current + 1);
    //            indices.push_back(next);
    //            indices.push_back(next + 1);
    //        }
    //    }
    //    Mesh ret;
    //    if (p.Generated)
    //    {
    //        ret = Mesh(vertices, indices, vector<Texture>());
    //        ret.Generated = true;
    //        if (setup)
    //            ret.color = p.meshes[0].color;
    //    }
    //    else
    //    {
    //        vector<Texture> textures;
    //        if (setup)
    //            textures = p.meshes[0].textures;
    //        ret = Mesh(vertices, indices, textures);
    //    }
    //    ret.orbitPlanet = &p;
    //    if (setup)
    //        ret.setupMesh();
    //    return ret;
    //}


void Mesh::generateCylinder(float radius, float height, int numSegments) {
    float halfHeight = height / 2.0f;
    float angleStep = 2.0f * M_PI / numSegments;
    this->radius = radius;
    this->height = height;
    // Generate vertices for the top and bottom circles
    for (int i = 0; i <= numSegments; ++i) {
        float angle = i * angleStep;
        float x = radius * cos(angle);
        float z = radius * sin(angle);

        // Top circle vertex
        vertices.push_back({
            glm::vec3(x, halfHeight, z),  // Position
            glm::vec3(0.0f, 1.0f, 0.0f), // Normal (pointing up)
            glm::vec2((float)i / numSegments, 1.0f) // Texture coordinate
            });

        // Bottom circle vertex
        vertices.push_back({
            glm::vec3(x, -halfHeight, z), // Position
            glm::vec3(0.0f, -1.0f, 0.0f), // Normal (pointing down)
            glm::vec2((float)i / numSegments, 0.0f) // Texture coordinate
            });
    }

    // Generate indices for the side faces
    for (int i = 0; i < numSegments; ++i) {
        int top1 = i * 2;
        int top2 = (i + 1) * 2;
        int bottom1 = top1 + 1;
        int bottom2 = top2 + 1;

        // Side triangles
        indices.push_back(top1);
        indices.push_back(bottom1);
        indices.push_back(top2);

        indices.push_back(bottom1);
        indices.push_back(bottom2);
        indices.push_back(top2);
    }

    // Generate vertices for the center points of top and bottom circles
    int centerTopIndex = vertices.size();
    vertices.push_back({
        glm::vec3(0.0f, halfHeight, 0.0f),  // Position
        glm::vec3(0.0f, 1.0f, 0.0f),       // Normal (pointing up)
        glm::vec2(0.5f, 0.5f)              // Texture coordinate
        });

    int centerBottomIndex = vertices.size();
    vertices.push_back({
        glm::vec3(0.0f, -halfHeight, 0.0f), // Position
        glm::vec3(0.0f, -1.0f, 0.0f),       // Normal (pointing down)
        glm::vec2(0.5f, 0.5f)               // Texture coordinate
        });

    // Generate indices for the top and bottom circles
    for (int i = 0; i < numSegments; ++i) {
        int top = i * 2;
        int nextTop = ((i + 1) % numSegments) * 2;

        // Top circle triangle
        indices.push_back(centerTopIndex);
        indices.push_back(nextTop);
        indices.push_back(top);

        // Bottom circle triangle
        indices.push_back(centerBottomIndex);
        indices.push_back(top + 1);
        indices.push_back((i + 1) % numSegments * 2 + 1);
    }
    Generated = true;
    orbit = false;
    color = glm::vec3(static_cast <float> (rand()) / static_cast <float> (RAND_MAX), static_cast <float> (rand()) / static_cast <float> (RAND_MAX), static_cast <float> (rand()) / static_cast <float> (RAND_MAX));
    setupMesh();
}


Mesh  Mesh::generateCube(float width, float height, float depth) {
    // Half dimensions for centered cuboid
    float halfWidth = width / 2.0f;
    float halfHeight = height / 2.0f;
    float halfDepth = depth / 2.0f;

    // Define vertices for a rectangular cuboid
    std::vector<Vertex> vertices = {
        // Front face
        {{-halfWidth, -halfHeight,  halfDepth}, {0.0f,  0.0f,  1.0f}, {0.0f, 0.0f}},
        {{ halfWidth, -halfHeight,  halfDepth}, {0.0f,  0.0f,  1.0f}, {1.0f, 0.0f}},
        {{ halfWidth,  halfHeight,  halfDepth}, {0.0f,  0.0f,  1.0f}, {1.0f, 1.0f}},
        {{-halfWidth,  halfHeight,  halfDepth}, {0.0f,  0.0f,  1.0f}, {0.0f, 1.0f}},

        // Back face
        {{-halfWidth, -halfHeight, -halfDepth}, {0.0f,  0.0f, -1.0f}, {1.0f, 0.0f}},
        {{ halfWidth, -halfHeight, -halfDepth}, {0.0f,  0.0f, -1.0f}, {0.0f, 0.0f}},
        {{ halfWidth,  halfHeight, -halfDepth}, {0.0f,  0.0f, -1.0f}, {0.0f, 1.0f}},
        {{-halfWidth,  halfHeight, -halfDepth}, {0.0f,  0.0f, -1.0f}, {1.0f, 1.0f}},

        // Left face
        {{-halfWidth, -halfHeight, -halfDepth}, {-1.0f,  0.0f,  0.0f}, {0.0f, 0.0f}},
        {{-halfWidth, -halfHeight,  halfDepth}, {-1.0f,  0.0f,  0.0f}, {1.0f, 0.0f}},
        {{-halfWidth,  halfHeight,  halfDepth}, {-1.0f,  0.0f,  0.0f}, {1.0f, 1.0f}},
        {{-halfWidth,  halfHeight, -halfDepth}, {-1.0f,  0.0f,  0.0f}, {0.0f, 1.0f}},

        // Right face
        {{ halfWidth, -halfHeight, -halfDepth}, {1.0f,  0.0f,  0.0f}, {1.0f, 0.0f}},
        {{ halfWidth, -halfHeight,  halfDepth}, {1.0f,  0.0f,  0.0f}, {0.0f, 0.0f}},
        {{ halfWidth,  halfHeight,  halfDepth}, {1.0f,  0.0f,  0.0f}, {0.0f, 1.0f}},
        {{ halfWidth,  halfHeight, -halfDepth}, {1.0f,  0.0f,  0.0f}, {1.0f, 1.0f}},

        // Bottom face
        {{-halfWidth, -halfHeight, -halfDepth}, {0.0f, -1.0f,  0.0f}, {0.0f, 1.0f}},
        {{ halfWidth, -halfHeight, -halfDepth}, {0.0f, -1.0f,  0.0f}, {1.0f, 1.0f}},
        {{ halfWidth, -halfHeight,  halfDepth}, {0.0f, -1.0f,  0.0f}, {1.0f, 0.0f}},
        {{-halfWidth, -halfHeight,  halfDepth}, {0.0f, -1.0f,  0.0f}, {0.0f, 0.0f}},

        // Top face
        {{-halfWidth,  halfHeight, -halfDepth}, {0.0f,  1.0f,  0.0f}, {0.0f, 0.0f}},
        {{ halfWidth,  halfHeight, -halfDepth}, {0.0f,  1.0f,  0.0f}, {1.0f, 0.0f}},
        {{ halfWidth,  halfHeight,  halfDepth}, {0.0f,  1.0f,  0.0f}, {1.0f, 1.0f}},
        {{-halfWidth,  halfHeight,  halfDepth}, {0.0f,  1.0f,  0.0f}, {0.0f, 1.0f}}
    };

    // Define indices for cube faces (two triangles per face)
    std::vector<unsigned int> indices = {
        // Front face
        0, 1, 2, 2, 3, 0,
        // Back face
        4, 5, 6, 6, 7, 4,
        // Left face
        8, 9, 10, 10, 11, 8,
        // Right face
        12, 13, 14, 14, 15, 12,
        // Bottom face
        16, 17, 18, 18, 19, 16,
        // Top face
        20, 21, 22, 22, 23, 20
    };
    Mesh m = Mesh(vertices, indices, std::vector<unsigned int>{0});
    m.Generated = true;
    m.color = glm::vec3(static_cast <float> (rand()) / static_cast <float> (RAND_MAX), static_cast <float> (rand()) / static_cast <float> (RAND_MAX), static_cast <float> (rand()) / static_cast <float> (RAND_MAX));
    return m;
}

unsigned int Mesh::loadCubemap(vector<std::string> faces)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
            );
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Cubemap tex failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}

Mesh  Mesh::generateCuboid(float frontWidth, float backWidth, float height, float depth) {
    // Half dimensions for the cuboid
    float halfHeight = height / 2.0f;
    float halfDepth = depth / 2.0f;
    float halfFrontWidth = frontWidth / 2.0f;
    float halfBackWidth = backWidth / 2.0f;

    // Define vertices for a cuboid where front and back sides have different widths
    std::vector<Vertex> vertices = {
        // Front face (front width)
        {{-halfFrontWidth, -halfHeight,  halfDepth}, {0.0f,  0.0f,  1.0f}, {0.0f, 0.0f}},
        {{ halfFrontWidth, -halfHeight,  halfDepth}, {0.0f,  0.0f,  1.0f}, {1.0f, 0.0f}},
        {{ halfFrontWidth,  halfHeight,  halfDepth}, {0.0f,  0.0f,  1.0f}, {1.0f, 1.0f}},
        {{-halfFrontWidth,  halfHeight,  halfDepth}, {0.0f,  0.0f,  1.0f}, {0.0f, 1.0f}},

        // Back face (back width)
        {{-halfBackWidth, -halfHeight, -halfDepth}, {0.0f,  0.0f, -1.0f}, {1.0f, 0.0f}},
        {{ halfBackWidth, -halfHeight, -halfDepth}, {0.0f,  0.0f, -1.0f}, {0.0f, 0.0f}},
        {{ halfBackWidth,  halfHeight, -halfDepth}, {0.0f,  0.0f, -1.0f}, {0.0f, 1.0f}},
        {{-halfBackWidth,  halfHeight, -halfDepth}, {0.0f,  0.0f, -1.0f}, {1.0f, 1.0f}},

        // Left face
        {{-halfBackWidth, -halfHeight, -halfDepth}, {-1.0f,  0.0f,  0.0f}, {0.0f, 0.0f}},
        {{-halfFrontWidth, -halfHeight,  halfDepth}, {-1.0f,  0.0f,  0.0f}, {1.0f, 0.0f}},
        {{-halfFrontWidth,  halfHeight,  halfDepth}, {-1.0f,  0.0f,  0.0f}, {1.0f, 1.0f}},
        {{-halfBackWidth,  halfHeight, -halfDepth}, {-1.0f,  0.0f,  0.0f}, {0.0f, 1.0f}},

        // Right face
        {{ halfBackWidth, -halfHeight, -halfDepth}, {1.0f,  0.0f,  0.0f}, {1.0f, 0.0f}},
        {{ halfFrontWidth, -halfHeight,  halfDepth}, {1.0f,  0.0f,  0.0f}, {0.0f, 0.0f}},
        {{ halfFrontWidth,  halfHeight,  halfDepth}, {1.0f,  0.0f,  0.0f}, {0.0f, 1.0f}},
        {{ halfBackWidth,  halfHeight, -halfDepth}, {1.0f,  0.0f,  0.0f}, {1.0f, 1.0f}},

        // Bottom face
        {{-halfFrontWidth, -halfHeight, -halfDepth}, {0.0f, -1.0f,  0.0f}, {0.0f, 1.0f}},
        {{ halfFrontWidth, -halfHeight, -halfDepth}, {0.0f, -1.0f,  0.0f}, {1.0f, 1.0f}},
        {{ halfFrontWidth, -halfHeight,  halfDepth}, {0.0f, -1.0f,  0.0f}, {1.0f, 0.0f}},
        {{-halfFrontWidth, -halfHeight,  halfDepth}, {0.0f, -1.0f,  0.0f}, {0.0f, 0.0f}},

        // Top face
        {{-halfFrontWidth,  halfHeight, -halfDepth}, {0.0f,  1.0f,  0.0f}, {0.0f, 0.0f}},
        {{ halfFrontWidth,  halfHeight, -halfDepth}, {0.0f,  1.0f,  0.0f}, {1.0f, 0.0f}},
        {{ halfFrontWidth,  halfHeight,  halfDepth}, {0.0f,  1.0f,  0.0f}, {1.0f, 1.0f}},
        {{-halfFrontWidth,  halfHeight,  halfDepth}, {0.0f,  1.0f,  0.0f}, {0.0f, 1.0f}}
    };

    // Define indices for the cuboid
    std::vector<unsigned int> indices = {
        // Front face
        0, 1, 2, 2, 3, 0,
        // Back face
        4, 5, 6, 6, 7, 4,
        // Left face
        8, 9, 10, 10, 11, 8,
        // Right face
        12, 13, 14, 14, 15, 12,
        // Bottom face
        16, 17, 18, 18, 19, 16,
        // Top face
        20, 21, 22, 22, 23, 20
    };
    Mesh m = Mesh(vertices, indices, std::vector<unsigned int>{0});
    m.Generated = true;
    m.color = glm::vec3(static_cast <float> (rand()) / static_cast <float> (RAND_MAX), static_cast <float> (rand()) / static_cast <float> (RAND_MAX), static_cast <float> (rand()) / static_cast <float> (RAND_MAX));
    return m;
}

Mesh Mesh::generateSkybox(std::vector<std::string> faces)
{
    Mesh cube = generateCube(20,20,20);
    cube.skybox = true;
    cube.textures[0].id = loadCubemap(faces);
    return cube;
}

void Mesh::setupMesh()
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
        &indices[0], GL_STATIC_DRAW);

    // vertex positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    // vertex normals
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
    // vertex texture coords
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));

    glBindVertexArray(0);
}



void Mesh::Draw(Shader& shader)
{
    unsigned int diffuseNr = 1;
    unsigned int specularNr = 1;
    shader.use();
    
    if (skybox)
    {
        glBindTexture(GL_TEXTURE_CUBE_MAP, textures[0].id);
    }
    else if (!Generated)
    {
        shader.setBool("generated", Generated);
        for (unsigned int i = 0; i < textures.size(); i++)
        {
            glActiveTexture(GL_TEXTURE0 + i); // activate proper texture unit before binding
            // retrieve texture number (the N in diffuse_textureN)
            string number;
            string name = textures[i].type;
            if (name == "texture_diffuse")
                number = std::to_string(diffuseNr++);
            else if (name == "texture_specular")
                number = std::to_string(specularNr++);
            shader.setInt(name + number, i);
            glBindTexture(GL_TEXTURE_2D, textures[i].id);
        }
        glActiveTexture(GL_TEXTURE0);
    }
    else
    {
        shader.setBool("generated", Generated);
        shader.setVec3("color", color);
    }
    
    // draw mesh
    
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}