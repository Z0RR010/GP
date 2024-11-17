#include "Mesh.h"
#include "Planet.h"
using namespace std;
Mesh::Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures)
{
    this->vertices = vertices;
    this->indices = indices;
    this->textures = textures;

    setupMesh();
}

Mesh Mesh::Regenerate(int lod)
{
    if (orbit)
    {
        auto m = Mesh::generateOrbit(lod, lod, *orbitPlanet,false);
        m.textures = this->textures;
        m.color = this->color;
        m.setupMesh();
        return m;
    }
    else
    {
        Mesh m = Mesh();
        m.generateCylinder(radius, height, lod);
        m.color = this->color;
        return m;
    }
}

    Mesh Mesh::generateOrbit(int majorSegments, int minorSegments, Planet p, bool setup)
    {
        vector<Vertex> vertices;
        vector<unsigned int> indices;
        float majorRadius, minorRadius;
        if (p.transform.pos.x != 0.0f)
        {
            majorRadius = abs(p.transform.pos.x);
        }
        else if (p.transform.pos.z != 0.0f)
        {
            majorRadius = abs(p.transform.pos.z);
        }
        minorRadius = 0.05f;
        float majorStep = 2.0f * M_PI / majorSegments;
        float minorStep = 2.0f * M_PI / minorSegments;

        // Generate vertices
        for (int i = 0; i <= majorSegments; ++i) {
            float majorAngle = i * majorStep;
            glm::vec3 majorCircleCenter(
                cos(majorAngle) * majorRadius,
                0.0f,
                sin(majorAngle) * majorRadius
            );

            for (int j = 0; j <= minorSegments; ++j) {
                float minorAngle = j * minorStep;

                // Calculate position
                glm::vec3 minorOffset(
                    cos(majorAngle) * (minorRadius * cos(minorAngle)),
                    minorRadius * sin(minorAngle),
                    sin(majorAngle) * (minorRadius * cos(minorAngle))
                );
                glm::vec3 position = majorCircleCenter + minorOffset;

                // Calculate normal
                glm::vec3 normal = glm::normalize(minorOffset);

                // Calculate texture coordinates
                float u = (float)i / majorSegments;
                float v = (float)j / minorSegments;

                // Add the vertex
                vertices.push_back({
                    position, // Position
                    normal,   // Normal
                    glm::vec2(u, v) // TexCoords
                    });
            }
        }
        // Generate indices
        for (int i = 0; i < majorSegments; ++i) {
            for (int j = 0; j < minorSegments; ++j) {
                int current = i * (minorSegments + 1) + j;
                int next = (i + 1) * (minorSegments + 1) + j;

                // Two triangles per quad
                indices.push_back(current);
                indices.push_back(next);
                indices.push_back(current + 1);

                indices.push_back(current + 1);
                indices.push_back(next);
                indices.push_back(next + 1);
            }
        }
        Mesh ret;
        if (p.Generated)
        {
            ret = Mesh(vertices, indices, vector<Texture>());
            ret.Generated = true;
            if (setup)
                ret.color = p.meshes[0].color;
        }
        else
        {
            vector<Texture> textures;
            if (setup)
                textures = p.meshes[0].textures;
            ret = Mesh(vertices, indices, textures);
        }
        ret.orbitPlanet = &p;
        if (setup)
            ret.setupMesh();
        return ret;
    }


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
    shader.setBool("generated", Generated);
    if (!Generated)
    {
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
        shader.setVec3("color", color);
    }
    
    // draw mesh
    
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}