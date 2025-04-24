#include <string>
#include <fstream>
#include <memory>
#include "GL/glew.h"
#include "fbxsdk.h"
#include "core/geometry.h"
#include "core/vertex.h"
#include "core/numericParser.h"
#include "utils/math.h"


namespace Core
{
	static void nextLine(const char* buffer, int& index)
	{
		for (index; buffer[index] != '\n'; index++);
	}

	Geometry::Geometry(unsigned int* indices, int indexCount, Vertex* vertices, int vertexCount)
		: m_IndexCount(indexCount)
	{
		glGenVertexArrays(1, &m_VertexArray);
		glGenBuffers(1, &m_VertexBuffer);
		glGenBuffers(1, &m_IndexBuffer);

		glBindVertexArray(m_VertexArray);

		glBindBuffer(GL_ARRAY_BUFFER, m_VertexBuffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IndexBuffer);

		glBufferData(GL_ARRAY_BUFFER, vertexCount * sizeof(Vertex), vertices, GL_STATIC_DRAW);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * sizeof(unsigned int), indices, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)0);
		glEnableVertexAttribArray(0);
		
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)sizeof(Vector3));
		glEnableVertexAttribArray(1);

		glBindVertexArray(0);
	}

	Geometry::Geometry(std::shared_ptr<std::vector<uint32_t>> indices, std::shared_ptr<std::vector<Vertex>> vertices)
		: m_IndexCount(indices->size())
	{
		glGenVertexArrays(1, &m_VertexArray);
		glGenBuffers(1, &m_VertexBuffer);
		glGenBuffers(1, &m_IndexBuffer);

		glBindVertexArray(m_VertexArray);

		glBindBuffer(GL_ARRAY_BUFFER, m_VertexBuffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IndexBuffer);

		glBufferData(GL_ARRAY_BUFFER, vertices->size() * sizeof(Vertex), vertices->data(), GL_STATIC_DRAW);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices->size() * sizeof(unsigned int), indices->data(), GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)0);
		glEnableVertexAttribArray(0);

		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)sizeof(Vector3));
		glEnableVertexAttribArray(1);

		glBindVertexArray(0);
	}

	Geometry::~Geometry()
	{
		glDeleteBuffers(1, &m_VertexBuffer);
		glDeleteBuffers(1, &m_IndexBuffer);
		glDeleteVertexArrays(1, &m_VertexArray);
	}
	
	std::shared_ptr<Geometry> Geometry::FromObj(std::string path)
	{
		std::ifstream file(path);

		int lineIndex = 0;
		std::string line;

		std::vector<Vector3> positionIndex;
		std::vector<Vector3> normalIndex;
		std::vector<Vector2> uvIndex;
		std::shared_ptr<std::vector<Vertex>> vertices = std::make_shared<std::vector<Vertex>>();
		std::shared_ptr<std::vector<uint32_t>> indices = std::make_shared<std::vector<uint32_t>>();

		while (std::getline(file, line))
		{
			char first = line[0];
			char second = line[1];

			switch (first)
			{
			case 'v':
			{
				if (second == ' ')
				{
					char* positionData = (char*)line.c_str();
					float vertexPosition[3];

					for (int axis = 0; axis < 3; axis++)
					{
						positionData = strchr(positionData, ' ') + 1;
						vertexPosition[axis] = std::atof(positionData);
					}

					positionIndex.emplace_back(Vector3(vertexPosition[0], vertexPosition[1], vertexPosition[2]));
				}
				else if (second == 't')
				{
					char* uvData = (char*)line.c_str();
					float vertexUV[2];

					for (int axis = 0; axis < 2; axis++)
					{
						uvData = strchr(uvData, ' ') + 1;
						vertexUV[axis] = std::atof(uvData);
					}

					uvIndex.emplace_back(Vector2(vertexUV[0], vertexUV[1]));
				}
				else if (second == 'n')
				{
					char* normalData = (char*)line.c_str();
					float vertexNormal[3];

					for (int axis = 0; axis < 3; axis++)
					{
						normalData = strchr(normalData, ' ') + 1;
						vertexNormal[axis] = std::atof(normalData);
					}

					normalIndex.emplace_back(Vector3(vertexNormal[0], vertexNormal[1], vertexNormal[2]));
				}
			}
				break;
			case 'f':
			{
				char* vertexData = (char*)line.c_str() + 2;

				for (int vertex = 0; vertex < 3; vertex++)
				{
					int vertexDataIndices[3];
					for (int attribute = 0; attribute < 3; attribute++)
					{
						vertexDataIndices[attribute] = Math::ToUnsignedInt(vertexData);
						
						if (attribute != 2)
							vertexData = strchr(vertexData, '/') + 1;
						else
							vertexData = strchr(vertexData, ' ') + 1;
					}

					vertices->emplace_back(Vertex(positionIndex[vertexDataIndices[0] - 1], uvIndex[vertexDataIndices[1] - 1]));
					indices->emplace_back(indices->size());
				}
			}
				break;
			default:
			{ }
				break;
			}

			lineIndex++;
		}

		file.close();

		return std::make_shared<Geometry>( indices, vertices );
	}

	void Geometry::Bind()
	{
		glBindVertexArray(m_VertexArray);
	}

	void Geometry::Unbind()
	{
		glBindVertexArray(0);
	}
}