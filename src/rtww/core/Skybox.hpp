#pragma once
#include "shape/Triangle.hpp"
#include "Texture.hpp"

class Skybox {
public:
	Skybox() {
		cubeMesh->verticesNum = 24;
	}
private:
	std::shared_ptr<Triangle[]> cubeTriangles;
	std::shared_ptr<Texture[]> cubeMaps;
};
v - 1.000000 - 1.000000 1.000000
v 1.000000 - 1.000000 1.000000
v - 1.000000 1.000000 1.000000
v 1.000000 1.000000 1.000000
v - 1.000000 1.000000 - 1.000000
v 1.000000 1.000000 - 1.000000
v - 1.000000 - 1.000000 - 1.000000
v 1.000000 - 1.000000 - 1.000000
vt 0.0 0.0 #1
vt 0.4 0.0
vt 0.0 0.4
vt 0.4 0.4
vt 0.4 0.4 #5
vt 0.8 0.4
vt 0.4 0.8
vt 0.8 0.8
vt 0.4 0.6
vt 0.8 0.6 #10
vt 0.4 1.0
vt 0.8 1.0
vt 0.0 0.4
vt 0.4 0.4
vt 0.0 0.8 #15
vt 0.4 0.8
vt 0.4 0.0
vt 0.8 0.0
vt 0.4 0.4
vt 0.8 0.4
vt 0.0 0.6
vt 0.4 0.6
vt 0.0 1.0
vt 0.4 1.0 #24
f 1 / 1 / 1 2 / 2 / 2 3 / 3 / 3
f 3 / 3 / 3 2 / 2 / 2 4 / 4 / 4
f 3 / 13 / 5 4 / 14 / 6 5 / 15 / 7
f 5 / 15 / 7 4 / 14 / 6 6 / 16 / 8
f 5 / 21 / 9 6 / 22 / 10 7 / 23 / 11
f 7 / 23 / 11 6 / 22 / 10 8 / 24 / 12
#usemtl mtl2
f 7 / 17 / 13 8 / 18 / 14 1 / 19 / 15
f 1 / 19 / 15 8 / 18 / 14 2 / 20 / 16
f 2 / 5 / 17 8 / 6 / 18 4 / 7 / 19
f 4 / 7 / 19 8 / 6 / 18 6 / 8 / 20
f 7 / 9 / 21 1 / 10 / 22 5 / 11 / 23
f 5 / 11 / 23 1 / 10 / 22 3 / 12 / 24
std::unique_ptr<Point3f[]> vertices;
std::unique_ptr<Point2f[]> uvs;
std::unique_ptr<Vector3f[]> normals;
std::vector<int> vertexIndices;