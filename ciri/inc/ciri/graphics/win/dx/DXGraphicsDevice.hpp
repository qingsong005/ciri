#ifndef __ciri_graphics_DXGraphicsDevice__
#define __ciri_graphics_DXGraphicsDevice__

#include <memory>
#include <string>
#include <d3d11.h>
#include <ciri/graphics/IGraphicsDevice.hpp>
#include "DXShader.hpp"
#include "DXVertexBuffer.hpp"
#include "DXIndexBuffer.hpp"
#include "DXRasterizerState.hpp"
#include "DXDepthStencilState.hpp"

namespace ciri {

class DXGraphicsDevice : public IGraphicsDevice, public std::enable_shared_from_this<DXGraphicsDevice> {
public:
	DXGraphicsDevice();
	virtual ~DXGraphicsDevice();

	virtual bool create( const std::shared_ptr<IWindow>& window ) override;
	virtual void destroy() override;
	virtual void present() override;

	virtual void setViewport( const Viewport& vp ) override;
	virtual const Viewport& getViewport() const override;
	virtual std::shared_ptr<IShader> createShader() override;
	virtual std::shared_ptr<IVertexBuffer> createVertexBuffer() override;
	virtual std::shared_ptr<IIndexBuffer> createIndexBuffer() override;
	virtual std::shared_ptr<IConstantBuffer> createConstantBuffer() override;
	virtual std::shared_ptr<ITexture2D> createTexture2D( int width, int height, TextureFormat::Format format, int flags, void* pixels=nullptr ) override;
	virtual std::shared_ptr<ITexture3D> createTexture3D( int width, int height, int depth, TextureFormat::Format format, int flags, void* pixels=nullptr ) override;
	virtual std::shared_ptr<ITextureCube> createTextureCube( int width, int height, void* posx, void* negx, void* posy, void* negy, void* posz, void* negz ) override;
	virtual std::shared_ptr<ISamplerState> createSamplerState( const SamplerDesc& desc ) override;
	virtual std::shared_ptr<IRenderTarget2D> createRenderTarget2D( int width, int height, TextureFormat::Format format, DepthStencilFormat depthFormat ) override;
	virtual std::shared_ptr<IRasterizerState> createRasterizerState( const RasterizerDesc& desc ) override;
	virtual std::shared_ptr<IDepthStencilState> createDepthStencilState( const DepthStencilDesc& desc ) override;
	virtual std::shared_ptr<IBlendState> createBlendState( const BlendDesc& desc ) override;
	virtual void applyShader( const std::shared_ptr<IShader>& shader ) override;
	virtual void setVertexBuffer( const std::shared_ptr<IVertexBuffer>& buffer ) override;
	virtual void setIndexBuffer( const std::shared_ptr<IIndexBuffer>& buffer ) override;
	virtual void setTexture2D( int index, const std::shared_ptr<ITexture2D>& texture, ShaderStage::Stage shaderStage ) override;
	virtual void setTexture3D( int index, const std::shared_ptr<ITexture3D>& texture, ShaderStage::Stage shaderStage ) override;
	virtual void setTextureCube( int index, const std::shared_ptr<ITextureCube>& texture, ShaderStage::Stage shaderStage ) override;
	virtual void setSamplerState( int index, const std::shared_ptr<ISamplerState>& state, ShaderStage::Stage shaderStage ) override;
	virtual void setBlendState( const std::shared_ptr<IBlendState>& state ) override;
	virtual void drawArrays( PrimitiveTopology topology, int vertexCount, int startIndex ) override;
	virtual void drawIndexed( PrimitiveTopology topology, int indexCount ) override;
	virtual void setRenderTargets( IRenderTarget2D** renderTargets, int numRenderTargets ) override;
	virtual void restoreDefaultRenderTargets() override;
	virtual ErrorCode resize() override;
	virtual ErrorCode resizeTexture2D( const std::shared_ptr<ITexture2D>& texture, int width, int height ) override;
	virtual ErrorCode resizeRenderTarget2D( const std::shared_ptr<IRenderTarget2D>& target, int width, int height ) override;
	virtual void setClearColor( float r, float g, float b, float a ) override;
	virtual void setClearDepth( float depth ) override;
	virtual void setClearStencil( int stencil ) override;
	virtual void clear( int flags ) override;
	virtual void setRasterizerState( const std::shared_ptr<IRasterizerState>& state ) override;
	virtual void setDepthStencilState( const std::shared_ptr<IDepthStencilState>& state ) override;
	virtual void setShaderExt( const char* ext ) override;
	virtual const char* getShaderExt() const override;
	virtual std::shared_ptr<IWindow> getWindow() const override;
	virtual const char* getGpuName() const override;
	virtual const char* getApiInfo() const override;
	virtual GraphicsApiType getApiType() const override;
	virtual ErrorCode restoreDefaultStates() override;
	virtual ErrorCode restoreDefaultBlendState() override;
	virtual ErrorCode restoreDefaultRasterizerState() override;
	virtual ErrorCode restoreDefaultDepthStencilState() override;
	virtual std::shared_ptr<IBlendState> getDefaultBlendAdditive() override;
	virtual std::shared_ptr<IBlendState> getDefaultBlendAlpha() override;
	virtual std::shared_ptr<IBlendState> getDefaultBlendNonPremul() override;
	virtual std::shared_ptr<IBlendState> getDefaultBlendOpaque() override;
	virtual std::shared_ptr<IRasterizerState> getDefaultRasterNone() override;
	virtual std::shared_ptr<IRasterizerState> getDefaultRasterClockwise() override;
	virtual std::shared_ptr<IRasterizerState> getDefaultRasterCounterClockwise() override;
	virtual std::shared_ptr<IDepthStencilState> getDefaultDepthStencilDefault() override;
	virtual std::shared_ptr<IDepthStencilState> getDefaultDepthStencilDepthRead() override;
	virtual std::shared_ptr<IDepthStencilState> getDefaultDepthStencilNone() override;

	ID3D11Device* getDevice() const;
	ID3D11DeviceContext* getContext() const;

private:
	bool initDevice( unsigned int width, unsigned int height, HWND hwnd );
	bool createBackbufferRtv();
	bool createDepthStencilView();

private:
	bool _isValid;
	std::shared_ptr<IWindow> _window;
	//
	IDXGISwapChain* _swapchain;
	ID3D11Device* _device;
	ID3D11DeviceContext* _context;
	ID3D11RenderTargetView* _backbuffer;
	int _defaultWidth;
	int _defaultHeight;
	//
	Viewport _activeViewport;
	//
	float _clearColor[4];
	float _clearDepth;
	int _clearStencil;
	//
	std::weak_ptr<DXShader> _activeShader;
	std::weak_ptr<DXVertexBuffer> _activeVertexBuffer;
	std::weak_ptr<DXIndexBuffer> _activeIndexBuffer;
	//
	std::weak_ptr<DXRasterizerState> _activeRasterizerState;
	//
	std::weak_ptr<DXDepthStencilState> _activeDepthStencilState;
	ID3D11Texture2D* _depthStencil;
	ID3D11DepthStencilView* _depthStencilView;
	ID3D11RenderTargetView* _activeRenderTargets[D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT];
	//
	std::string _shaderExt;
	//
	std::string _gpuName;
	std::string _apiInfo;

	// default blend states
	std::shared_ptr<IBlendState> _defaultBlendAdditive;
	std::shared_ptr<IBlendState> _defaultBlendAlpha;
	std::shared_ptr<IBlendState> _defaultBlendNonPremul;
	std::shared_ptr<IBlendState> _defaultBlendOpaque;
	// default rasterizer states
	std::shared_ptr<IRasterizerState> _defaultRasterNone;
	std::shared_ptr<IRasterizerState> _defaultRasterClockwise;
	std::shared_ptr<IRasterizerState> _defaultRasterCounterClockwise;
	// default depth states
	std::shared_ptr<IDepthStencilState> _defaultDepthStencilDefault;
	std::shared_ptr<IDepthStencilState> _defaultDepthStencilDepthRead;
	std::shared_ptr<IDepthStencilState> _defaultDepthStencilNone;
};

}

#endif