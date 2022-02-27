#include <stdint.h>
#include <vector>
#include "glm/glm.hpp"

#include "Shader.hpp"

class Screen
{
public:
	Screen();
	~Screen();
	void draw(Shader* shader) const;
	void drawInstances(Shader* shader, int count)const;
private:
	uint32_t VAO;
	uint32_t VBO;
	uint32_t EBO;

	vector<vec2> vertices;
	vector<uint32_t> indices;

};



void Screen::draw(Shader* shader) const {

	glBindVertexArray(VAO);

	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void Screen::drawInstances(Shader* shader, int count) const {

	glBindVertexArray(VAO);
	glDrawElementsInstanced(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0, count);
	glBindVertexArray(0);
}




Screen::Screen()
{
	this->vertices = {
		vec2(0.0f, 0.0f),
		vec2(1.0f, 0.0f),
		vec2(0.0f, 1.0f),
		vec2(1.0f, 1.0f),
	};
	this->indices = { 0, 1, 3, 0, 3, 2 };

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vec2), &vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(vec2), (void*)0);

	glBindVertexArray(0);
}

Screen::~Screen()
{
}