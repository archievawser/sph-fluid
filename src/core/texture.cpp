#include <cstdint>
#include "GL/glew.h"
#include "core/texture.h"
#include "stb/stb_image.h"


namespace Core
{
	Texture::Texture(const char* source)
		: mSource(source)
	{
		stbi_set_flip_vertically_on_load(true);
		uint8_t* result = stbi_load(mSource, &mWidth, &mHeight, &mNumChannels, STBI_rgb_alpha);

		glGenTextures(1, &mRendererID);
		glBindTexture(GL_TEXTURE_2D, mRendererID);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, mWidth, mHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, result);
		glBindTexture(GL_TEXTURE_2D, 0);
		
		if (result)
			stbi_image_free(result);
	}

	Texture::Texture(const Texture& other)
		: Texture(other.mSource)
	{
		
	}

	Texture::~Texture()
	{
		glDeleteTextures(1, &mRendererID);
	}

	void Texture::Bind(GLuint slot)
	{
		glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(GL_TEXTURE_2D, mRendererID);
	}

	void Texture::Unbind()
	{
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}