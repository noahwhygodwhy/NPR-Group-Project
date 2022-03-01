#include "DTriangle.hpp"


DEdge::DEdge():DEdge(0, 0) {
	
}

DEdge::DEdge(size_t a, size_t b)
{
	this->pointA = a;
	this->pointB = b;
	this->triangles[0] = NULL;
	this->triangles[1] = NULL;
}

DEdge::~DEdge()
{
}

DTriangle::DTriangle(DEdge* a, DEdge* b, DEdge* c) {
	a->giveTriangle(this);
	b->giveTriangle(this);
	c->giveTriangle(this);


	this->points[0] = a->pointA;
	if (this->points[0] == b->pointA) {
		this->points[1] = b->pointB;
	}
	else {
		this->points[1] = b->pointA;
	}
	if (this->points[0] == c->pointA
		|| this->points[1] == c->pointA) {

		this->points[2] = c->pointB;
	}
	else {
		this->points[2] = c->pointA;
	}
}

void DEdge::giveTriangle(DTriangle* t) {
	if (this->triangles[0] == NULL) {
		this->triangles[0] = t;
	}
	else {
		this->triangles[1] = t;
	}
}
void DEdge::setTriangles(DTriangle* a, DTriangle* b) {
	this->triangles[0] = a;
	this->triangles[1] = b;
}
/*DTriangle::DTriangle(size_t a, size_t b, size_t c)
{
	this->points[0] = a;
	this->points[1] = b;
	this->points[2] = c;
	this->edges[0] = new DEdge(a, b);
	this->edges[1] = new DEdge(b, c);
	this->edges[2] = new DEdge(c, a);
	this->neighbors[0] = NULL;
	this->neighbors[1] = NULL;
	this->neighbors[2] = NULL;
}*/

DTriangle::~DTriangle()
{
}


DFrontier::DFrontier()
{
	/*this->edges.insert(initialTriangle->edges[0]);
	this->edges.insert(initialTriangle->edges[1]);
	this->edges.insert(initialTriangle->edges[2]);*/
}

DFrontier::~DFrontier()
{
}
void DFrontier::addEdge(DEdge* edge) {
	this->edges.insert(edge);
}
void DFrontier::removeEdge(DEdge* d) {
	this->edges.erase(d);
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
DEdge* DFrontier::findEdge(vec4* polarPoints, const vec2& polarOrigin, size_t thePoint, size_t& L, size_t& R) {
	for (DEdge* e : this->edges) {
		vec4 pointA = polarPoints[e->pointA];
		vec4 pointB = polarPoints[e->pointB];
		if (lineRayHit(polarOrigin, glm::normalize(vec2(polarPoints[thePoint])-polarOrigin), vec2(pointA), vec2(pointB))) {
			if (pointA.w < pointB.w) {//Do i need to do this since I don't need to actually detect basins for concave shapes
				L = e->pointB;
				R = e->pointA;
			}
			else {
				L = e->pointA;
				R = e->pointB;
			}
			return e;
		}
	}

}






void legaliseTriangle(Triangulation& t, DTriangle* tri) {
	for (size_t i = 0; i < 3; i++) {
		DTriangle* otherTri = tri->neighbors[i];
		if (otherTri != NULL) {
			//check if the existing edge is longer than the
			//edge if the other two points were used
			//if so swap it
			//remove the edge from edges, add the new edge
			//remove the two triangles



		}
	}
}

void addTriangle(Triangulation& t, size_t point) {
	size_t L, R;
	DEdge* d = t.frontier.findEdge(t.polarPoints, vec2(0.5), point, L, R);
	t.frontier.removeEdge(d);
	//TODO: you were here
	DEdge* newL = new DEdge(point, L);
	DEdge* newR = new DEdge(point, R);
	DTriangle* newTriangle = new DTriangle(d, newL, newR);
	d->giveTriangle(newTriangle);
	newL->giveTriangle(newTriangle);
	newR->giveTriangle(newTriangle);
	t.edges.insert(newL);
	t.edges.insert(newR);
	legaliseTriangle(t, d->triangles[0]);

	






	//Do the basin walk lol



}

unordered_set<DTriangle*> triangulate(vec4* polarPoints, size_t numberOfPoints) {
	Triangulation t = Triangulation(polarPoints);

	DEdge* edgeA = new DEdge(0, 1);
	DEdge* edgeB = new DEdge(1, 2);
	DEdge* edgeC = new DEdge(2, 0);

	DTriangle* firstTriangle = new DTriangle(edgeA, edgeB, edgeC);
	
	t.frontier.addEdge(edgeA);
	t.frontier.addEdge(edgeB);
	t.frontier.addEdge(edgeC);

	t.triangles.insert(firstTriangle);
	t.edges.insert(edgeA);
	t.edges.insert(edgeB);
	t.edges.insert(edgeC);

	for (size_t i = 3; i < numberOfPoints; i++) {
		addTriangle(t, i);
	}


	
}