#ifndef __ciri_graphics_DXRenderTarget2D__
#define __ciri_graphics_DXRenderTarget2D__

#include <memory>
#include <d3d11.h>
#include <ciri/graphics/IRenderTarget2D.hpp>

namespace ciri {

class DXGraphicsDevice;
class DXTexture2D;

class DXRenderTarget2D : public IRenderTarget2D {
public:
	DXRenderTarget2D( const std::shared_ptr<DXGraphicsDevice>& device );
	virtual ~DXRenderTarget2D();

	bool create( const std::shared_ptr<DXTexture2D>& texture, const std::shared_ptr<DXTexture2D>& depthTexture );
	virtual void destroy() override;

	virtual std::shared_ptr<ITexture2D> getTexture() const override;
	virtual std::shared_ptr<ITexture2D> getDepth() const override;

	ID3D11RenderTargetView* getRenderTargetView() const;

private:
	std::shared_ptr<DXGraphicsDevice> _device;
	std::shared_ptr<DXTexture2D> _texture;
	std::shared_ptr<DXTexture2D> _depthTexture;
	ID3D11RenderTargetView* _renderTargetView;
};

}

#endif