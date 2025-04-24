#pragma once


namespace Core
{
	class Texture
	{
	public:
		Texture(const char* source);
		Texture(const Texture& other);
		~Texture();

	public:
		void Bind(GLuint slot = 0);
		void Unbind();
		inline const char* GetSource() const { return mSource; }

	private:
		int mWidth;
		int mHeight;
		int mNumChannels;
		uint32_t mRendererID;
		const char* mSource;
	};
}