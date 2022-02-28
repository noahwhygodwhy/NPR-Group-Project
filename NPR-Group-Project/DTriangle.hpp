#ifndef DTRIANGLE_H
#define DTRIANGLE_H

#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>
#include <set>
using namespace std;
using namespace glm;


class DTriangle;

class DEdge
{
public:
	DEdge();
	DEdge(size_t a, size_t b, DTriangle* triangle);
	~DEdge();

	size_t pointA;//each of these is an index into polar points
	size_t pointB;//^
	DTriae* triangle//each edge needs a pointer to possibly two triangles, not just one, cause edge duh

private:
};


class DTriangle
{
public:
	DTriangle();
	DTriangle(size_t a, size_t b, size_t c, vec4* polarPoints);
	~DTriangle();
	DTriangle* neighbors[3];
	DEdge edges[3];
	size_t points[3];
	vec4* polarPoints;
private:
};

class DFrontier
{
	set<DEdge> edges;
public:

	DFrontier(DTriangle* initialTriangle);
	~DFrontier();
	void findEdge(vec4* polarPoints, const vec2& polarOrigin, const vec2& thePoint, vec2& L, vec2& R);

private:

};

#endif;