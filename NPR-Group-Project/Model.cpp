#define STB_IMAGE_IMPLEMENTATION

#include "Model.hpp"
#include "stb_image.h"
#include "glm/gtx/string_cast.hpp"
#include "glm/gtx/matrix_decompose.hpp"
#include "glm/gtx/quaternion.hpp"

using namespace std;
using namespace glm;


vector<Texture> textures_loaded;

Model::Model(string modelname) {
    string path = "./Models/" + modelname + "/" + modelname + ".fbx";
    this->loadModel(path);
    //printf("number of children: %i\n", this->children.size());

    //for (int i = 0; i < this->children.size(); i++) {
    //    printf("child %i has %i vertices\n", i, this->children.at(i).vertices.size());
    //}
}

Model::~Model() {

}

void Model::draw(Shader* shader) {
    for (const Mesh& m : children) {
        m.draw(shader, this->transform);
    }
}
void Model::drawBoundingBox(Shader* shader) {

    shader->setMatFour("model", this->transform);

    glBindBuffer(GL_ARRAY_BUFFER, boundingVBO);
    glDrawArrays(GL_POINT, 0, this->boundingVertices.size());
    glBindVertexArray(0);
}

vec3 tv3(const aiVector3D& a) {
    vec3 v;
    v.x = a.x;
    v.y = a.y;
    v.z = a.z;
    return v;
}
vec2 tv2(const aiVector3D& a) {
    vec2 v;
    v.x = a.x;
    v.y = a.y;
    return v;
}

//TODO: you eventually need to fix this lol
mat4 tm4(aiMatrix4x4 a) {
    return transpose(mat4(a.a1, a.a2, a.a3, a.a4,
        a.b1, a.b2, a.b3, a.b4,
        a.c1, a.c2, a.c3, a.c4,
        a.d1, a.d2, a.d3, a.d4));
}






void Model::genBoundingBuffers() {

    glGenBuffers(1, &boundingVBO);
    glBindBuffer(GL_ARRAY_BUFFER, boundingVBO);

    vec3 possiblePoints[] = { this->boundingBox.first, this->boundingBox.second };

    vec3 color(1.0f, 0.2f, 0.0f);

    for (const vec3& xpoint : possiblePoints) {
        for (const vec3& ypoint : possiblePoints) {
            for (const vec3& zpoint : possiblePoints) {
                boundingVertices.push_back({ vec3(xpoint.x, ypoint.y, zpoint.z), color });
            }
        }
    }

    /*for (SimpleVertex x : boundingVertices) {
        printf("%s\n", glm::to_string(x.position).c_str());
    }*/


    glBufferData(GL_ARRAY_BUFFER, boundingVertices.size() * sizeof(SimpleVertex), &boundingVertices[0], GL_STATIC_DRAW);
    // vertex positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(SimpleVertex), (void*)0);
    // vertex color
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(SimpleVertex), (void*)offsetof(SimpleVertex, color));

    glBindVertexArray(0);
}


void Model::loadModel(string path) {
    cout << "loading model " << path.c_str() << endl;
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);
    this->originalTransform = mat4(1.0f);
    this->transform = mat4(1.0f);


    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        cout << "ERROR::ASSIMP::" << importer.GetErrorString() << endl;
        return;
    }


    double factor;
    scene->mMetaData->Get("UnitScaleFactor", factor);

    //printf("factor: %lf\n", factor);
    directory = path.substr(0, path.find_last_of('/'));

    this->children = processNode(scene->mRootNode, scene, mat4(1.0f), 0.01f);


    vec3 minp = vec3(INFINITY);
    vec3 maxp = vec3(-INFINITY);
    for (const Mesh& m : this->children) {
        pair<vec3, vec3> mBox = m.getBoundingBox();

        minp.x = std::min(minp.x, mBox.first.x);
        minp.y = std::min(minp.y, mBox.first.y);
        minp.z = std::min(minp.z, mBox.first.z);
        maxp.x = std::max(maxp.x, mBox.second.x);
        maxp.y = std::max(maxp.y, mBox.second.y);
        maxp.z = std::max(maxp.z, mBox.second.z);
    }
    this->boundingBox = pair<vec3, vec3>{ minp, maxp };

    this->genBoundingBuffers();

}



string getTranslate(mat4 a) {
    glm::vec3 scale;
    glm::quat rotation;
    glm::vec3 translation;
    glm::vec3 skew;
    glm::vec4 perspective;
    glm::decompose(a, scale, rotation, translation, skew, perspective);

    return glm::to_string(translation);
    //return transformation.to_string();
}

string getRotation(mat4 a) {
    glm::vec3 scale;
    glm::quat rotation;
    glm::vec3 euler = eulerAngles(rotation) * 3.14159f / 180.f;
    glm::vec3 translation;
    glm::vec3 skew;
    glm::vec4 perspective;
    glm::decompose(a, scale, rotation, translation, skew, perspective);

    return glm::to_string(euler);
    //return transformation.to_string();
}
string getScale(mat4 a) {
    glm::vec3 scale;
    glm::quat rotation;
    glm::vec3 euler = eulerAngles(rotation) * 3.14159f / 180.f;
    glm::vec3 translation;
    glm::vec3 skew;
    glm::vec4 perspective;
    glm::decompose(a, scale, rotation, translation, skew, perspective);

    return glm::to_string(scale);
    //return transformation.to_string();
}










vector<Mesh> Model::processNode(aiNode* node, const aiScene* scene, const mat4& parentTx, float scaleFactor) {
    vector<Mesh> toReturn = vector<Mesh>(node->mNumMeshes);
    //printf("---------------------------\n");

    //printf("before tx:, %s\n", glm::to_string(tm4(node->mTransformation)).c_str());

    mat4 tx = (tm4(node->mTransformation) * 0.01f);
    tx[3][3] = 1.0f;//fixes the scaling

    //printf("tx:, %s\n", glm::to_string(tx).c_str());;

    //printf("translation: %s\n", getTranslate(tx).c_str());
    //printf("rotation: %s\n", getRotation(tx).c_str());
    //printf("scale: %s\n", getScale(tx).c_str());




    for (uint i = 0; i < node->mNumMeshes; i++) {

        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        toReturn.push_back(processMesh(mesh, scene, tx));;
    }

    for (uint i = 0; i < node->mNumChildren; i++) {
        vector<Mesh> toAdd = processNode(node->mChildren[i], scene, tx, scaleFactor);
        toReturn.insert(toReturn.end(), toAdd.begin(), toAdd.end());
    }
    return toReturn;
}





Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene, const mat4& nodeTx) {

    vector<Vertex> vertices;
    vector<unsigned int> indices;
    //vector<Texture> textures;
    //Material material;


    //process vertices
    //printf("process mesh should have %i vertices\n", mesh->mNumVertices);
    for (uint i = 0; i < mesh->mNumVertices; i++) {
        Vertex vertex;
        if (mesh->HasPositions()) {

            vertex.position = tv3(mesh->mVertices[i]);
            //printf("vertex position: %s\n", glm::to_string(vertex.position).c_str());
        }
        if (mesh->HasNormals()) {
            vertex.normal = tv3(mesh->mNormals[i]);
        }
        if (mesh->HasTangentsAndBitangents()) {
            vertex.tangent = tv3(mesh->mTangents[i]);
            vertex.bitangent = tv3(mesh->mBitangents[i]);
        }
        if (mesh->HasTextureCoords(0)) { //TODO: what is indexes of texture coords
            vertex.texCoords = tv2(mesh->mTextureCoords[0][i]);
        }
        else {
            vertex.texCoords = vec2(0.0f, 0.0f);
        }
        vertices.push_back(vertex);
    }
    //process indices
    for (uint i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        for (uint j = 0; j < face.mNumIndices; j++) {
            indices.push_back(face.mIndices[j]);
        }
    }
    Texture diffuse, specular;
    if (mesh->mMaterialIndex >= 0)
    {
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
        diffuse = loadMaterialTextures(material,
            aiTextureType_DIFFUSE, "texture_diffuse")[0];
        //textures.push_back(diffuseMaps);

        specular = loadMaterialTextures(material,
            aiTextureType_SPECULAR, "texture_specular")[0];
        //textures.push_back(specularMaps);
    }
    return Mesh(vertices, indices, nodeTx, diffuse, specular);
}

unsigned int textureFromFile(const char* path, const string& directory)
{
    //stbi_set_flip_vertically_on_load(true);
    string filename = string(path);
    filename = directory + '/' + filename;

    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;


    unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
    printf("read in image width %i height %i comp %i\n", width, height, nrComponents);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}

vector<Texture> Model::loadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName)
{

    vector<Texture> textures;
    for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
    {
        aiString str;
        mat->GetTexture(type, i, &str);
        bool skip = false;
        for (unsigned int j = 0; j < textures_loaded.size(); j++)
        {
            if (std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0)
            {
                textures.push_back(textures_loaded[j]);
                skip = true;
                break;
            }
        }
        if (!skip)
        {   // if texture hasn't been loaded already, load it
            Texture texture;
            texture.id = textureFromFile(str.C_Str(), directory);
            texture.type = typeName;
            texture.path = str.C_Str();
            textures.push_back(texture);
            textures_loaded.push_back(texture); // add to loaded textures
        }
    }
    if (textures.empty()) {
        Texture texture;
        texture.id = textureFromFile("BUG.png", "");
        texture.type = typeName;
        texture.path = "BUG.png";
        textures.push_back(texture);
    }


    return textures;
}


float Model::getNear(vec3 eye) {
    //printf("getnear");
    float min = INFINITY;

    uint32_t minIndex = -1;
    for (uint32_t i = 0; i < boundingVertices.size(); i++) {
        float dtwo = distance2(eye, boundingVertices[i].position);
        //printf("dtwo: %f\n", glm::sqrt(dtwo));
        if (dtwo < min) {
            minIndex = i;
            min = dtwo;
        }
    }
    return glm::sqrt(min) - 1.0f;
}
float Model::getFar(vec3 eye) {
    //printf("getfar");
    float max = -INFINITY;

    uint32_t maxIndex = -1;
    for (uint32_t i = 0; i < boundingVertices.size(); i++) {
        float dtwo = distance2(eye, boundingVertices[i].position);
        //printf("dtwo: %f\n", glm::sqrt(dtwo));
        if (dtwo > max) {
            maxIndex = i;
            max = dtwo;
        }
    }
    return glm::sqrt(max) + 1.0f;
}