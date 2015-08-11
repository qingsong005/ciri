#include <ciri/gfx/gl/GLTexture2D.hpp>

namespace ciri {
	GLTexture2D::GLTexture2D( int flags )
		: ITexture2D(flags), _flags(flags), _textureId(0), _internalFormat(0), _pixelFormat(0), _pixelType(0), _width(0), _height(0) {
	}

	GLTexture2D::~GLTexture2D() {
		destroy();
	}

	void GLTexture2D::destroy() {
		if( _textureId != 0 ) {
			glDeleteTextures(1, &_textureId);
			_textureId = 0;
		}
	}

	err::ErrorCode GLTexture2D::setData( int xOffset, int yOffset, int width, int height, void* data, TextureFormat::Type format ) {
		// todo: some check about size differences when updating

		_width = (width > _width) ? width : _width;
		_height = (height > _height) ? height : _height;

		ciriFormatToGlFormat(format);

		const int level = 0;

		if( _textureId != 0 ) {
			glBindTexture(GL_TEXTURE_2D, _textureId);
			glPixelStorei(GL_UNPACK_ALIGNMENT, TextureFormat::bytesPerPixel(format));
			glTexSubImage2D(GL_TEXTURE_2D, level, xOffset, yOffset, width, height, _pixelFormat, _pixelType, data);
			glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
			glBindTexture(GL_TEXTURE_2D, 0);
		} else {
			if( xOffset != 0 || yOffset != 0 ) {
				return err::CIRI_UNKNOWN_ERROR; // todo: error (cannot set a nonzeroed rect on a blank image)
			}

			glGenTextures(1, &_textureId);
			glBindTexture(GL_TEXTURE_2D, _textureId);
			glPixelStorei(GL_UNPACK_ALIGNMENT, TextureFormat::bytesPerPixel(format));
			glTexImage2D(GL_TEXTURE_2D, level, _internalFormat, width, height, 0, _pixelFormat, _pixelType, data);
			glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

			if( _flags & TextureFlags::Mipmaps ) {
				glGenerateMipmap(GL_TEXTURE_2D);
			}

			glBindTexture(GL_TEXTURE_2D, 0);
		}

		return err::CIRI_OK;
	}

	int GLTexture2D::getWidth() const {
		return _width;
	}

	int GLTexture2D::getHeight() const {
		return _height;
	}

	GLuint GLTexture2D::getTextureId() const {
		return _textureId;
	}

	void GLTexture2D::ciriFormatToGlFormat( TextureFormat::Type ciriFormat ) {
		// http://www.opentk.com/files/doc/namespace_open_t_k_1_1_graphics_1_1_open_g_l.html#ae0f3f1e7b978e4937984b34fdebabf62a8f0fb883eb5a52838534191513e365a2
		// MonoGame.Framework.Graphics.GraphicsExtensions.cs: GetGLFormat
		_internalFormat = GL_RGBA;
		_pixelFormat = GL_RGBA;
		_pixelType = GL_UNSIGNED_BYTE;

		switch( ciriFormat ) {
			case TextureFormat::Color: {
				_internalFormat = GL_RGBA;
				_pixelFormat = GL_RGBA;
				_pixelType = GL_UNSIGNED_BYTE;
				break;
			}

			case TextureFormat::RGB32_Float: {
				_internalFormat = GL_RGB32F;
				_pixelFormat = GL_RGB;
				_pixelType = GL_FLOAT;
				break;
			}

			case TextureFormat::RGBA32_Float: {
				_internalFormat = GL_RGBA32F;
				_pixelFormat = GL_RGBA;
				_pixelType = GL_FLOAT;
			}
		}
	}
} // ciri