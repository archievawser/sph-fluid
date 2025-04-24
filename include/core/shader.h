#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <sstream>
#include <string>
#include <memory>
#include "GL/glew.h"
#include "glm/glm.hpp"
#include "utils/utils.h"


namespace Core
{
	class Shader
	{
	public:
		Shader(std::string& vertSource, std::string& fragSource);
		~Shader();

	public:
		static std::shared_ptr<Shader> FromPath(const char* vertPath, const char* fragPath);
		void Bind();
		void Unbind();
		void SetUniform(const char* dest, glm::mat4& value);
		void SetUniform(const char* dest, glm::vec3& value);
		void SetUniform(const char* dest, int value);
	
	private:
		unsigned int mShaderID;
		unsigned int mFragmentID;
		unsigned int mVertexID;
	};


	class ComputeShader
	{
	public:
		ComputeShader(const char* computePath);
		~ComputeShader();
		ComputeShader(const ComputeShader& other);

	public:
		void Bind();
		void Unbind();
		void Dispatch(int numGroupsX, int numGroupsY, int numGroupsZ);
		void SetUniform(const char* dest, float value);

	private:
		std::unordered_map<const char*, uint32_t> mUniformLocationCache;
		const char* mComputePath;
		unsigned int mProgramID;
	};


	template<typename T>
	class LargeShaderBuffer
	{
	public:
		LargeShaderBuffer()
		{
			glGenBuffers(1, &mBufferId);
		}

		LargeShaderBuffer(std::vector<T>& bufferData)
			: LargeShaderBuffer(bufferData.data(), bufferData.size())
		{ }

		LargeShaderBuffer(T* bufferData, int dataSize)
			: mSize(dataSize)
		{
			glGenBuffers(1, &mBufferId);

			glBindBuffer(GL_SHADER_STORAGE_BUFFER, mBufferId);
			glBufferData(GL_SHADER_STORAGE_BUFFER, mSize * sizeof(T), bufferData, GL_DYNAMIC_DRAW);
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

			Write(bufferData, dataSize);
		}

	public:
		void Write(std::vector<T>& bufferData)
		{
			Write(bufferData.data(), bufferData.size());
		}

		void Write(T* bufferData, int dataSize)
		{
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, mBufferId);
			glBufferData(GL_SHADER_STORAGE_BUFFER, mSize * sizeof(T), bufferData, GL_DYNAMIC_DRAW);
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
		}

		void Bind(unsigned int index)
		{
			glBindBufferBase(GL_SHADER_STORAGE_BUFFER, index, mBufferId);
		}

		void ReadInto(std::vector<T>* out)
		{
			out->reserve(mSize);
			glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, mSize * sizeof(T), out->data());
		}

	private:
		uint32_t mSize;
		uint32_t mBufferId;
	};
}