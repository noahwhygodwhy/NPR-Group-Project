#include "DTriangle.hpp"


DEdge::DEdge() {
	this->pointA = 0;
	this->pointB = 0;
	this->triangle = NULL;
}

DEdge::DEdge(size_t a, size_t b, DTriangle* d)
{
	this->pointA = a;
	this->pointB = b;
	this->triangle = d;
}

DEdge::~DEdge()
{
}

DTriangle::DTriangle():DTriangle(0, 0, 0, NULL) {
	
}
DTriangle::DTriangle(size_t a, size_t b, size_t c, vec4* polarPoints)
{
	this->points[0] = a;
	this->points[1] = b;
	this->points[2] = c;
	this->edges[0] = DEdge(a, b, this);
	this->edges[1] = DEdge(b, c, this);
	this->edges[2] = DEdge(c, a, this);
	this->neighbors[0] = NULL;
	this->neighbors[1] = NULL;
	this->neighbors[2] = NULL;
	this->polarPoints = polarPoints;
}

DTriangle::~DTriangle()
{
}


DFrontier::DFrontier(DTriangle* initialTriangle)
{
	this->edges.insert(initialTriangle->edges[0]);
	this->edges.insert(initialTriangle->edges[1]);
	this->edges.insert(initialTriangle->edges[2]);
}

DFrontier::~DFrontier()
{
}

//https://rootllama.wordpress.com/2014/06/20/ray-line-segment-intersection-test-in-2d/
bool lineRayHit(vec2 rayOrigin, vec2 rayDirection, vec2 pointA, vec2 pointB) {
	vec2 v1 = rayOrigin - pointA;
	vec2 v2 = pointB - pointA;
	vec2 v3 = vec2(-rayDirection.y, rayDirection.x);
	float v2Dotv3 = glm::dot(v2, v3);
	float v1Dotv3 = glm::dot(v1, v3);
	float v2Crossv1 = pointA.x * pointB.y - pointB.x * pointA.y;// glm::length(glm::cross(v2, v1));


	float t1 = v2Crossv1 / v2Dotv3;
	float t2 = v1Dotv3 / v2Dotv3;

	return t1 >= 0.0 && t2 <= 1.0 && t2 >= 0.0;

}
void DFrontier::findEdge(vec4* polarPoints, const vec2& polarOrigin, const vec2& thePoint, vec2& L, vec2& R) {
	for (const DEdge& e : this->edges) {
		vec4 pointA = polarPoints[e.pointA];
		vec4 pointB = polarPoints[e.pointB];
		if (lineRayHit(polarOrigin, glm::normalize(thePoint-polarOrigin), vec2(pointA), vec2(pointB))) {
			if (pointA.w < pointB.w) {//Do i need to do this since I don't need to actually detect basins for concave shapes
				L = pointB;
				R = pointA;
			}
			else {
				L = pointA;
				R = pointB;
			}
		}
	}

}