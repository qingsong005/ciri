#include <ciri/core/TGA.hpp>
#include <fstream>

using namespace ciri;

TGA::TGA()
	: _format(RGB), _width(0), _height(0), _pixels(nullptr) {
}

TGA::~TGA() {
	destroy();
}

bool TGA::loadFromFile( const char* file, bool forceRGBA ) {
	std::ifstream in(file, std::ios::binary);
	if( !in.is_open() ) {
		return false;
	}

	// Read the header
	Header header;
	in.read((char*)&header, sizeof(Header));

	// Read useful data into the TGA from the header
	_width = static_cast<int>(header.imageWidth);
	_height = static_cast<int>(header.imageHeight);
	_format = (header.pixelDepth==24) ? RGB : RGBA;

	// Create the data buffer
	const int bpp = header.pixelDepth / 8;
	_pixels = new unsigned char[bpp * _width * _height];

	if( header.imageType == 2 ) {
		//
		// Uncompressed
		//
		// Read in the texture data
		in.read((char*)_pixels, bpp * _width * _height);
		// Swap byte order from BGR to RGB
		for( int i = 0; i < (bpp * _width * _height); i += bpp ) {
			_pixels[i] ^= _pixels[i+2] ^= _pixels[i] ^= _pixels[i+2];
		}
	}
	else if( header.imageType == 10 ) {
		//
		// Compressed
		//
		const int pixelCount = _width * _height;
		int currPixel = 0;
		int currByte = 0;
		unsigned char* buf = new unsigned char[bpp];
		do {
			unsigned char chunkHeader = 0;
			in.read((char*)&chunkHeader, sizeof(unsigned char));
			if( chunkHeader < 128 ) {
				chunkHeader++;
				for( short counter=0; counter < chunkHeader; ++counter ) {
					in.read((char*)buf, bpp);
					_pixels[currByte] = buf[2];
					_pixels[currByte+1] = buf[1];
					_pixels[currByte+2] = buf[0];
					if( _format == RGBA ) {
						_pixels[currByte+3] = buf[3];
					}

					currByte += bpp;
					currPixel++;
				}
			}
			else {
				chunkHeader -= 127;
				in.read((char*)buf, bpp);
				for( short counter=0; counter < chunkHeader; ++counter ) {
					_pixels[currByte] = buf[2];
					_pixels[currByte+1] = buf[1];
					_pixels[currByte+2] = buf[0];
					if( _format == RGBA ) {
						_pixels[currByte+3] = buf[3];
					}

					currByte += bpp;
					currPixel++;
				}
			}
		}
		while( currPixel < pixelCount );
		delete[] buf;
		buf = nullptr;
	}
	else {
		return false;
	}

	in.close();

	// if rgba was requested but the image is rgb, create a new buffer with an A channel and delete the old one.
	// can't do this normally as the data is read in one big chunk.
	if( forceRGBA && _format != RGBA ) {
		unsigned char* newPixels = new unsigned char[4 * _width * _height];
		int newPixelsOffset = 0;
		for( int i = 0; i < (bpp * _width * _height); i += bpp ) { // loop all old pixels
			memcpy(&newPixels[newPixelsOffset], &_pixels[i], bpp);
			newPixels[newPixelsOffset+3] = 255; // 100% alpha
			newPixelsOffset += 4;
		}
		delete[] _pixels;
		_pixels = newPixels;
		_format = RGBA;
	}

	return true;
}

void TGA::destroy() {
	if( _pixels != nullptr ) {
		delete[] _pixels;
		_pixels = nullptr;
	}
}

TGA::Format TGA::getFormat() const {
	return _format;
}

int TGA::getWidth() const {
	return _width;
}

int TGA::getHeight() const {
	return _height;
}

unsigned char* TGA::getPixels() const {
	return _pixels;
}

bool TGA::hasAlpha() const {
	return (Format::RGBA == _format);
}

bool TGA::writeToFile( const char* file, unsigned int width, unsigned int height, unsigned char* pixels, Format format, bool swap ) {
	if( nullptr == file || nullptr == pixels ) {
		return false;
	}
	std::ofstream out(file, std::ios::binary);
	if( !out.is_open() ) {
		return false;
	}

	Header header;
	header.idLength = 0;
	header.colorMapType = 0;
	header.imageType = 2; // uncompressed
	header.firstEntryIndex = 0;
	header.colorMapLength = 0;
	header.colorMapEntrySize = 0;
	header.xOrigin = 0;
	header.yOrigin = 0;
	header.imageWidth = width;
	header.imageHeight = height;
	header.pixelDepth = (RGB == format) ? 24 : 32;;
	header.imageDesc = (RGB == format) ? 0 : 8;
	out.write((char*)&header, sizeof(Header));

	const unsigned int bpp = (RGB == format) ? 3 : 4;
	if( swap ) {
		for( unsigned int i = 0; i < (bpp * width * height); i += bpp ) {
			pixels[i] ^= pixels[i+2] ^= pixels[i] ^= pixels[i+2];
		}
	}
	out.write((char*)pixels, width * height * bpp);

	out.close();
	return true;
}