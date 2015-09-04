#ifndef __ciri_dxtexturecube__
#define __ciri_dxtexturecube__

#include <d3d11.h>
#include "../ITextureCube.hpp"

namespace ciri {
	class DXGraphicsDevice;

	class DXTextureCube : public ITextureCube {
	public:
		DXTextureCube( DXGraphicsDevice* device );
		virtual ~DXTextureCube();

		virtual err::ErrorCode set( int width, int height, void* posx, void* negx, void* posy, void* negy, void* posz, void* negz ) override;
		virtual void destroy() override;

		ID3D11Texture2D* getTexture() const;
		ID3D11ShaderResourceView* getShaderResourceView() const;

	private:
		DXGraphicsDevice* _device;
		ID3D11Texture2D* _cubeTexture;
		ID3D11ShaderResourceView* _shaderResourceView;
	};
} // ciri

#endif /* __ciri_dxtexturecube__ */