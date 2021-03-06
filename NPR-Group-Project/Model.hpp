#pragma once
#ifndef MODEL_H
#define MODEL_H

#include <glm/glm.hpp>
#include <vector>
#include <string>
#include <map>
#include "Mesh.hpp"
#include "Shader.hpp"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

using namespace std;
using namespace glm;

//class Model;





class Model
{

    static map<string, Model*> loadedModels;

    vector<SimpleVertex> boundingVertices;
    unsigned int boundingVBO;


public:

    Model(string modelname);
    ~Model();
    void draw(Shader* shader);
    void drawBoundingBox(Shader* shader);
    float getNear(vec3 eye);
    float getFar(vec3 eye);
private:

    mat4 originalTransform;
    mat4 transform;
    vector<Mesh> children;
    string directory;
    pair<vec3, vec3> boundingBox;
    void loadModel(string path);
    vector<Mesh> processNode(aiNode* node, const aiScene* scene, const mat4& parentTx, float scaleFactor);
    Mesh processMesh(aiMesh* mesh, const aiScene* scene, const mat4& nodeTx);
    vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName);
    void genBoundingBuffers();

};

#endif
