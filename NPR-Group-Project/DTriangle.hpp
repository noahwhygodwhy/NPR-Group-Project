#ifndef DTRIANGLE_H
#define DTRIANGLE_H

#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>
#include <set>
#include <unordered_set>
using namespace std;
using namespace glm;


class DTriangle;

class DEdge
{
public:
	DEdge();
	DEdge(size_t a, size_t b);
	~DEdge();

	size_t pointA;//each of these is an index into polar points
	size_t pointB;//^
	DTriangle* triangles[2];

	void giveTriangle(DTriangle* t);
	void setTriangles(DTriangle* a, DTriangle* b);
private:
};


class DTriangle
{
public:
	DTriangle(DEdge* a, DEdge* b, DEdge* c);
	~DTriangle();
	DTriangle* neighbors[3];
	DEdge* edges[3];
	size_t points[3];
private:
};

class DFrontier
{
	set<DEdge*> edges;//why again does this need to be a set
public:

	DFrontier();
	~DFrontier();
	DEdge* findEdge(vec4* polarPoints, const vec2& polarOrigin, size_t thePoint, size_t& L, size_t& R);
	void removeEdge(DEdge* d);
	void addEdge(DEdge* edge);
private:

};


struct Triangulation {
	unordered_set<DTriangle*> triangles;
	unordered_set<DEdge*> edges;
	DFrontier frontier;
	vec4* polarPoints;
	Triangulation(vec4* polarPoints) {
		this->polarPoints = polarPoints;
	};
};

unordered_set<DTriangle*> triangulate(vec4* polarPoints, size_t numberOfPoints);

#endif;