
#ifndef MESH_H
#define MESH_H

#include "Shader.hpp"
#include <glm/glm.hpp>
#include "glad/glad.h"
#include <vector>
#include <string>
//#include "Material.hpp"

using namespace std;
using namespace glm;

struct SimpleVertex {
    vec3 position;
    vec3 color;
};

struct Vertex {
    vec3 position;
    vec3 normal;
    vec3 tangent;
    vec3 bitangent;
    vec2 texCoords;
};


struct Texture {
    unsigned int id;
    string type;
    string path;
};



class Mesh
{
public:
    vector<Mesh> children;
    vector<Vertex> vertices;
    vector<unsigned int> indices;
    mat4 transform;

    

    Mesh(vector<Vertex> vertices, vector<unsigned int> indices, const mat4& meshTx, Texture diffuse, Texture specular);
    Mesh();
    ~Mesh();
    void draw(const Shader& shader, const mat4& parentTx) const;
    pair<vec3, vec3> getBoundingBox() const;
private:
    unsigned int VAO, VBO, EBO;
    Texture diffuse, specular;
    void setupMesh();

};


#endif
