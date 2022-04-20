#ifndef SCREEN_H
#define SCREEN_H

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
private:
	uint32_t VAO;
	uint32_t VBO;
	uint32_t EBO;

	vector<vec2> vertices;
	vector<uint32_t> indices;

};

#endif