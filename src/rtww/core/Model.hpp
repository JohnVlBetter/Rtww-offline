#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"
#include "Mesh.hpp"

class Model {
public:
	Model(const char* filename, std::shared_ptr<Transform> t) : transform(t){
		Load(filename);
	}

	bool Load(const char* filename) {
		tinyobj::ObjReader reader;

		if (!reader.ParseFromFile(filename)) {
			if (!reader.Error().empty())
				std::cerr << "Load Model Error: " << reader.Error();
			return false;
		}

		if (!reader.Warning().empty())
			std::cout << "Load Model Warning: " << reader.Warning();

		auto& attrib = reader.GetAttrib();
		auto& shapes = reader.GetShapes();
		auto& materials = reader.GetMaterials();

		// Loop over shapes
		for (size_t s = 0; s < shapes.size(); s++) {
			// Loop over faces(polygon)
			size_t index_offset = 0;
			for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
				size_t fv = size_t(shapes[s].mesh.num_face_vertices[f]);

				// Loop over vertices in the face.
				for (size_t v = 0; v < fv; v++) {
					// access to vertex
					tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
					tinyobj::real_t vx = attrib.vertices[3 * size_t(idx.vertex_index) + 0];
					tinyobj::real_t vy = attrib.vertices[3 * size_t(idx.vertex_index) + 1];
					tinyobj::real_t vz = attrib.vertices[3 * size_t(idx.vertex_index) + 2];

					// Check if `normal_index` is zero or positive. negative = no normal data
					if (idx.normal_index >= 0) {
						tinyobj::real_t nx = attrib.normals[3 * size_t(idx.normal_index) + 0];
						tinyobj::real_t ny = attrib.normals[3 * size_t(idx.normal_index) + 1];
						tinyobj::real_t nz = attrib.normals[3 * size_t(idx.normal_index) + 2];
					}

					// Check if `texcoord_index` is zero or positive. negative = no texcoord data
					if (idx.texcoord_index >= 0) {
						tinyobj::real_t tx = attrib.texcoords[2 * size_t(idx.texcoord_index) + 0];
						tinyobj::real_t ty = attrib.texcoords[2 * size_t(idx.texcoord_index) + 1];
					}

					// Optional: vertex colors
					// tinyobj::real_t red   = attrib.colors[3*size_t(idx.vertex_index)+0];
					// tinyobj::real_t green = attrib.colors[3*size_t(idx.vertex_index)+1];
					// tinyobj::real_t blue  = attrib.colors[3*size_t(idx.vertex_index)+2];
				}
				index_offset += fv;

				// per-face material
				shapes[s].mesh.material_ids[f];
			}
		}

		for (const auto& shape : shapes) {
			int verticesNum = static_cast<int>(shape.mesh.indices.size());
			int trianglesNum = verticesNum / 3;
			Point3f* vertices = (Point3f*)malloc(sizeof(Point3f) * verticesNum);
			Point2f* uvs = (Point2f*)malloc(sizeof(Point2f) * verticesNum);
			Vector3f* normals = (Vector3f*)malloc(sizeof(Vector3f) * verticesNum);
			int* vertexIndices = (int*)malloc(sizeof(int) * verticesNum);
			int offset = 0;

			size_t index_offset = 0;
			for (size_t f = 0; f < shape.mesh.num_face_vertices.size(); f++) {
				size_t fv = size_t(shape.mesh.num_face_vertices[f]);
				for (size_t v = 0; v < fv; v++) {
					// access to vertex
					tinyobj::index_t idx = shape.mesh.indices[index_offset + v];
					tinyobj::real_t vx = attrib.vertices[3 * size_t(idx.vertex_index) + 0];
					tinyobj::real_t vy = attrib.vertices[3 * size_t(idx.vertex_index) + 1];
					tinyobj::real_t vz = attrib.vertices[3 * size_t(idx.vertex_index) + 2];
					vertices[offset] = Point3f(vx, vy, vz);
					vertexIndices[offset] = offset;
					++offset;
				}
				index_offset += fv;
				shape.mesh.material_ids[f];
			}

			//for (const auto& index : shape.mesh.indices) {
			//	vertices[offset] = Point3f(attrib.vertices[3 * index.vertex_index + 0],
			//		attrib.vertices[3 * index.vertex_index + 1],
			//		attrib.vertices[3 * index.vertex_index + 2]);
			//	/*uvs[offset] = Point2f(attrib.texcoords[2 * index.texcoord_index + 0],
			//		1.0f - attrib.texcoords[2 * index.texcoord_index + 1]);
			//	normals[offset] = Vector3f(attrib.normals[3 * index.normal_index + 0],
			//		attrib.normals[3 * index.normal_index + 1],
			//		attrib.normals[3 * index.normal_index + 2]);*/
			//	vertexIndices[offset] = offset;
			//	++offset;
			//}
			meshes.push_back(std::make_shared<Mesh>(transform, trianglesNum, vertexIndices, verticesNum, 
				vertices, normals, uvs, std::make_shared<Lambertian>(Color(0.89, 0.89, 0.89))));
		}
	}

public:
	std::vector<std::shared_ptr<Mesh>> meshes;
	std::shared_ptr<Transform> transform;
};