#ifndef __ciri_graphics_ITextureCube__
#define __ciri_graphics_ITextureCube__

#include <ciri/core/ErrorCodes.hpp>

namespace ciri {

class ITextureCube {
protected:
	ITextureCube() {
	}

public:
	virtual ~ITextureCube() {
	}

	virtual ErrorCode set( int width, int height, void* posx, void* negx, void* posy, void* negy, void* posz, void* negz )=0;

	virtual void destroy()=0;
};

}

#endif