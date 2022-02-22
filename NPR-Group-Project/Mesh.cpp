#include "Mesh.hpp"
#include <glm/gtx/string_cast.hpp>

Mesh::Mesh(vector<Vertex> vertices, vector<unsigned int> indices, const mat4& meshTx, Texture diffuse, Texture specular) {
    this->diffuse = diffuse;
    this->specular = specular;

    this->vertices = vertices;
    this->indices = indices;
    this->transform = meshTx;
    this->setupMesh();
}

Mesh::Mesh() {

}


Mesh::~Mesh() {

}

void Mesh::setupMesh() {

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    // vertex positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    // vertex normals
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    // vertex texture coords
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tangent));

    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, bitangent));

    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));

    glBindVertexArray(0);
}
void Mesh::draw(const Shader& shader, const mat4& parentTx) const {

    //printf("drawing mesh\n");
    //shader.use();
    mat4 tx = parentTx * this->transform;
    shader.setMatFour("model", tx);
    shader.setMatFour("normalMatrix", glm::transpose(glm::inverse(tx)));




    //shader.setMatFour("model", mat4(1.0f));

    /*printf("indicies:%i%s\n", indices.size());
    for (int i = 0; i < this->indices.size(); i++) {
        printf("%s\n", glm::to_string(this->vertices[i].position));
    }
    exit(-1);
    return;*/







    glActiveTexture(GL_TEXTURE0 + 0);
    glBindTexture(GL_TEXTURE_2D, diffuse.id);
    shader.setInt("diffuseSampler", 0);

    glActiveTexture(GL_TEXTURE0 + 1);
    glBindTexture(GL_TEXTURE_2D, specular.id);
    shader.setInt("specularSampler", 1);

    glActiveTexture(GL_TEXTURE0);


    //unsigned int diffuseNr = 1;
    //unsigned int specularNr = 1;
    //for (unsigned int i = 0; i < textures.size(); i++)
    //{
    //    glActiveTexture(GL_TEXTURE0 + i); // activate proper texture unit before binding
    //    // retrieve texture number (the N in diffuse_textureN)
    //    string number;
    //    string name = textures[i].type;
    //    if (name == "texture_diffuse")
    //        number = std::to_string(diffuseNr++);
    //    else if (name == "texture_specular")
    //        number = std::to_string(specularNr++);

    //    shader.setInt(("material_" + name + number).c_str(), i);
    //    glBindTexture(GL_TEXTURE_2D, textures[i].id);
    //}
    //glActiveTexture(GL_TEXTURE0);

    // draw mesh
    // 
    glBindVertexArray(VAO);

    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}



//there's probably a more efficient way, but can't be arsed, not yet at least
pair<vec3, vec3> Mesh::getBoundingBox() const {
    vec3 minp = vec3(INFINITY);
    vec3 maxp = vec3(-INFINITY);
    for (const Vertex& point : this->vertices) {
        vec3 txp = this->transform * vec4(point.position, 1.0f);
        minp.x = std::min(minp.x, txp.x);
        minp.y = std::min(minp.y, txp.y);
        minp.z = std::min(minp.z, txp.z);
        maxp.x = std::max(maxp.x, txp.x);
        maxp.y = std::max(maxp.y, txp.y);
        maxp.z = std::max(maxp.z, txp.z);
    }
    return pair<vec3, vec3>{minp, maxp};
}