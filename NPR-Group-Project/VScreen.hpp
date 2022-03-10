#ifndef VSCREEN_H
#define VSCREEN_H


#include <vector>

#include <glm/glm.hpp>

using namespace std;
using namespace glm;




struct VVert {
	vec2 pos;
	vec2 origPoint;
};

class VScreen
{
public:
	VScreen(vector<vec2> vertsIn);
	~VScreen();

private:

	uint32_t VAO;
	uint32_t VBO;
	uint32_t EBO;

	vector<vec2> vertices;
	vector<uint32_t> indices;
};

VScreen::VScreen(vector<vec2> vertsIn)
{

}

VScreen::~VScreen()
{
}

#endif;