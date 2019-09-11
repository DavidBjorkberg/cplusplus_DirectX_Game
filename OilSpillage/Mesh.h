#pragma once
#include"VertexBuffer.h"
#include"Structs.h"
#include<vector>
using namespace DirectX::SimpleMath;
class Mesh
{
public:
	void loadMesh(const char* filename) {
		Vertex3D vertex;
		vertex.position = Vector3(-1.0, 0.0 ,- 1.0);
		vertex.uv = Vector2(0.0, 1.0);
		vertex.normal = Vector3(0.0, 1.0, 0.0);
		vertices.push_back(vertex);

		vertex.position = Vector3(-1.0, 0.0, 1.0);
		vertex.uv = Vector2(0.0, 0.0);
		vertices.push_back(vertex);

		vertex.position = Vector3(1.0, 0.0, -1.0);
		vertex.uv = Vector2(1.0, 1.0);
		vertices.push_back(vertex);

		vertex.position = Vector3(-1.0, 0.0, 1.0);
		vertex.uv = Vector2(0.0, 0.0);
		vertices.push_back(vertex);

		vertex.position = Vector3(1.0, 0.0, 1.0);
		vertex.uv = Vector2(1.0, 0.0);
		vertices.push_back(vertex);

		vertex.position = Vector3(1.0, 0.0, -1.0);
		vertex.uv = Vector2(1.0, 1.0);
		vertices.push_back(vertex);

		this->vertexCount = vertices.size();
		
	};
	void insertDataToMesh(std::vector<Vertex3D> vertexData)
	{
		this->vertices = vertexData;

		this->vertexCount = vertices.size();
	};
	void operator=(const Mesh& rh) { this->vertices = rh.vertices; }
	std::vector<Vertex3D> vertices;
	ID3D11Buffer* vertexBuffer;
	int getVertexCount() const 
	{
		return this->vertexCount;
	};
private:
	int vertexCount; 
};
