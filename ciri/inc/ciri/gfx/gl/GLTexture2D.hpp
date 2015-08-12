#ifndef __ciri_gltexture2d__
#define __ciri_gltexture2d__

#include <gl/glew.h>
#include "../ITexture2D.hpp"

namespace ciri {
	class GLTexture2D : public ITexture2D {
	public:
		GLTexture2D( int flags );
		virtual ~GLTexture2D();

		virtual void destroy();
		virtual err::ErrorCode setData( int xOffset, int yOffset, int width, int height, void* data, TextureFormat::Type format );

		virtual int getWidth() const;
		virtual int getHeight() const;

		virtual bool writeToTGA( const char* file );

		GLuint getTextureId() const;

	private:
		void ciriFormatToGlFormat( TextureFormat::Type ciriFormat );

	private:
		int _flags;
		TextureFormat::Type _format;
		GLuint _textureId;
		GLint _internalFormat;
		GLenum _pixelFormat;
		GLenum _pixelType;
		int _width;
		int _height;
	};
} // ciri

#endif /* __ciri_gltexture2d__ */