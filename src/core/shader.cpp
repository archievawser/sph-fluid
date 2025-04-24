#include "GL/glew.h"
#include "glm/gtc/type_ptr.hpp"
#include "core/shader.h"
#include "utils/utils.h"


namespace Core
{
	Shader::Shader(std::string& vertSource, std::string& fragSource)
	{
		const char* fragSource_cstr = fragSource.c_str();
		const char* vertSource_cstr = vertSource.c_str();

		mShaderID = glCreateProgram();
	
		mFragmentID = glCreateShader(GL_FRAGMENT_SHADER);
		mVertexID = glCreateShader(GL_VERTEX_SHADER);

		glShaderSource(mFragmentID, 1, &fragSource_cstr, NULL);
		glShaderSource(mVertexID, 1, &vertSource_cstr, NULL);

		glCompileShader(mFragmentID);
		glCompileShader(mVertexID);

		glAttachShader(mShaderID, mFragmentID);
		glAttachShader(mShaderID, mVertexID);

		char infoLog[512];
		int success;
		glGetShaderiv(mVertexID, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(mVertexID, 512, NULL, infoLog);
			std::cout << infoLog << std::endl;
		}

		glGetShaderiv(mFragmentID, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(mFragmentID, 512, NULL, infoLog);
			std::cout << infoLog << std::endl; 
		}

		glLinkProgram(mShaderID);
	}

	Shader::~Shader()
	{
		Unbind();

		glDeleteShader(mFragmentID);
		glDeleteShader(mVertexID);
		glDeleteProgram(mShaderID);
	}

	std::shared_ptr<Shader> Shader::FromPath(const char* vertPath, const char* fragPath)
	{
		std::ifstream file(vertPath);
		std::string vertexSource ((std::istreambuf_iterator<char>(file)), (std::istreambuf_iterator<char>()));

		file = std::ifstream(fragPath);
		std::string fragmentSource ((std::istreambuf_iterator<char>(file)), (std::istreambuf_iterator<char>()));

		return std::make_shared<Shader>( vertexSource, fragmentSource );
	}

	void Shader::Bind()
	{
		glUseProgram(mShaderID);
	}

	void Shader::Unbind()
	{
		glUseProgram(0);
	}

	void Shader::SetUniform(const char* dest, glm::mat4& value)
	{
		unsigned int location = glGetUniformLocation(mShaderID, dest);
		glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
	}

	void Shader::SetUniform(const char* dest, int value)
	{
		unsigned int location = glGetUniformLocation(mShaderID, dest);
		glUniform1i(location, value);
	}
	
	void Shader::SetUniform(const char* dest, glm::vec3& value)
	{
		unsigned int location = glGetUniformLocation(mShaderID, dest);
		glUniform3f(location, value.x, value.y, value.z);
	}

	ComputeShader::ComputeShader(const char* computePath)
		: mComputePath(computePath)
	{
		unsigned int compute;
		std::ifstream file(computePath);
		std::string computeSource((std::istreambuf_iterator<char>(file)), (std::istreambuf_iterator<char>()));
		file.close();
		const char* rawComputeSource = computeSource.c_str();

		compute = glCreateShader(GL_COMPUTE_SHADER);
		glShaderSource(compute, 1, &rawComputeSource, NULL);
		glCompileShader(compute);
		
		mProgramID = glCreateProgram();
		glAttachShader(mProgramID, compute);

		char infoLog[512];
		int success;
		glGetShaderiv(compute, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(compute, 512, NULL, infoLog);
			RABID_ERROR("failed to compile compute shader\n" << infoLog);
		}

		glLinkProgram(mProgramID);
	}

	ComputeShader::~ComputeShader()
	{
		glDeleteProgram(mProgramID);
	}

	ComputeShader::ComputeShader(const ComputeShader& other)
		: ComputeShader(other.mComputePath)
	{ }

	void ComputeShader::Bind()
	{
		glUseProgram(mProgramID);
	}

	void ComputeShader::Unbind()
	{
		glUseProgram(0);
	}
	void ComputeShader::Dispatch(int numGroupsX, int numGroupsY, int numGroupsZ)
	{
		Bind();
		glDispatchCompute(numGroupsX, numGroupsY, numGroupsZ);
	}

	void ComputeShader::SetUniform(const char* dest, float value)
	{
		if (mUniformLocationCache.find(dest) == mUniformLocationCache.end())
			mUniformLocationCache[dest] = glGetUniformLocation(mProgramID, dest);

		glUniform1f(mUniformLocationCache[dest], value);
	}
}