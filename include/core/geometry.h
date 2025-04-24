#pragma once
#include <fstream>
#include <iostream>
#include <vector>
#include "GL/glew.h"
#include "core/vertex.h"


namespace Core
{
	class Geometry
	{
	public:
		Geometry(unsigned int* indices, int indexCount, Vertex* vertices, int vertexCount);
		Geometry(std::shared_ptr<std::vector<uint32_t>> indices, std::shared_ptr<std::vector<Vertex>> vertices);
		~Geometry();

	public:
		static std::shared_ptr<Geometry> FromObj(std::string path);
		static std::shared_ptr<Geometry> FromFbx(std::string path);

		void Bind();
		void Unbind();

		inline int GetIndexCount() const { return m_IndexCount; }

	private:
		unsigned int m_VertexBuffer;
		unsigned int m_IndexBuffer;
		unsigned int m_VertexArray;
		int m_IndexCount;
	};
}
