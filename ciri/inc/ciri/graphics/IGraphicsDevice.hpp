#ifndef __ciri_graphics_IGraphicsDevice__
#define __ciri_graphics_IGraphicsDevice__

#include <memory>
#include <ciri/core/ErrorCodes.hpp>
#include "Viewport.hpp"
#include "IShader.hpp"
#include "IVertexBuffer.hpp"
#include "IIndexBuffer.hpp"
#include "IConstantBuffer.hpp"
#include "ITexture2D.hpp"
#include "ITexture3D.hpp"
#include "ITextureCube.hpp"
#include "ISamplerState.hpp"
#include "IRenderTarget2D.hpp"
#include "IRasterizerState.hpp"
#include "IDepthStencilState.hpp"
#include "IBlendState.hpp"
#include "ShaderStage.hpp"
#include "PrimitiveTopology.hpp"
#include "GraphicsApiType.hpp"
#include "DepthStencilFormat.hpp"

namespace ciri {

class IWindow;

class IGraphicsDevice {
protected:
	IGraphicsDevice() {
	}

public:
	virtual ~IGraphicsDevice() {
	}

	/**
		* Initializes the graphics device which will render to the provided IWindow.
		* @param window IWindow to render to.
		* @returns TODO
		*/
	virtual bool create( const std::shared_ptr<IWindow>& window )=0;

	/**
		* Uninitializes the graphics device and all of its managed resources that have not yet been freed.
		*/
	virtual void destroy()=0;

	/**
		* Presents the backbuffer to the screen.
		*/
	virtual void present()=0;

	/**
		* Sets the viewport.
		* @param vp Viewport to set.
		*/
	virtual void setViewport( const Viewport& vp )=0;

	/**
		* Gets the currently active Viewport.
		* @returns Currently active Viewport.
		*/
	virtual const Viewport& getViewport() const=0;

	/**
		* Creates a new shader.
		* @returns A pointer to a new IShader, or nullptr upon error.
		*/
	virtual std::shared_ptr<IShader> createShader()=0;

	/**
		* Creates a new vertex buffer.
		* @returns A pointer to a new IVertexBuffer, or nullptr upon error.
		*/
	virtual std::shared_ptr<IVertexBuffer> createVertexBuffer()=0;

	/**
		* Creates a new index buffer.
		* @returns A pointer to a new IIndexBuffer, or nullptr upon error.
		*/
	virtual std::shared_ptr<IIndexBuffer> createIndexBuffer()=0;

	/**
		* Creates a new constant buffer.
		* @returns A pointer to a new IConstantBuffer, or nullptr upon error.
		*/
	virtual std::shared_ptr<IConstantBuffer> createConstantBuffer()=0;

	/**
		* Creates a new 2d texture optionally initialized with data.
		* @param width  Width of the texture in pixels.
		* @param height Height of the texture in pixels.
		* @param format Format of the texture.
		* @param flags  Bitfield of optional TextureFlags.
		* @param pixels Data to initialize the texture with.  If null, no specific data is set.
		* @returns A pointer to a new ITexture2D of the given parameters, or nullptr upon error.
		*/
	virtual std::shared_ptr<ITexture2D> createTexture2D( int width, int height, TextureFormat::Format format, int flags, void* pixels=nullptr )=0;

	/**
		* Creates a new 3d texture optionally initialized with data.
		* @param width  Width of the textures in pixels.
		* @param height Height of the textures in pixels.
		* @param depth  Number of textures in the stack.
		* @param format Format of the texture.
		* @param flags  Bitfield of optional TextureFlags.
		* @param pixels Data to initialize the textures with.  If null, no specific data is set.
		* @returns A pointer to a new ITexture3D of the given parameters, or nullptr upon error.
		*/
	virtual std::shared_ptr<ITexture3D> createTexture3D( int width, int height, int depth, TextureFormat::Format format, int flags, void* pixels=nullptr )=0;

	/**
		* Creates a cubemap texture made up of six 2d textures.
		* Pixel format is expected to be RGBA with unsigned byte as the data type.
		* @param width  Width in pixels of each texture (must all be the same).
		* @param height Height in pixels of each texture (must all be the same).
		* @param right  Data for texture facing positive X.
		* @param left   Data for texture facing negative X.
		* @param top    Data for texture facing positive Y.
		* @param bottom Data for texture facing negative Y.
		* @param back   Data for texture facing positive Z.
		* @param front  Data for texture facing negative Z.
		* @returns A pointer to a new ITextureCube, or nullptr upon error.
		*/
	virtual std::shared_ptr<ITextureCube> createTextureCube( int width, int height, void* posx, void* negx, void* posy, void* negy, void* posz, void* negz )=0;

	/**
		* Creates a new sampler state.
		* @param desc Descriptor used to configure the sampler.
		* @returns A pointer to a new ISamplerState, or nullptr upon error.
		*/
	virtual std::shared_ptr<ISamplerState> createSamplerState( const SamplerDesc& desc )=0;

	/**
		* Creates a new 2d render target.
		* @param width       Width of the render target in pixels.
		* @param height      Height of the render target in pixels.
		* @param format      Format of the render target.
		* @param depthFormat Format of the optional depth-stencil buffer.
		* @returns A pointer to a new IRenderTarget2D, or nullptr upon error.
		*/
	virtual std::shared_ptr<IRenderTarget2D> createRenderTarget2D( int width, int height, TextureFormat::Format format, DepthStencilFormat depthFormat )=0;

	/**
		* Creates a new rasterizer state.
		* @param desc Descriptor used to configure the state.
		* @returns A pointer to a new IRasterizerState, or nullptr upon error.
		*/
	virtual std::shared_ptr<IRasterizerState> createRasterizerState( const RasterizerDesc& desc )=0;

	/**
		* Creates a new depth stencil state.
		* @param desc Descriptor used to configure the state.
		* @returns A pointer to a new IDepthStencilState, or nullptr upon error.
		*/
	virtual std::shared_ptr<IDepthStencilState> createDepthStencilState( const DepthStencilDesc& desc )=0;

	/**
		* Creates a new blend state.
		* @param desc Descriptor used to configure the state.
		* @returns A pointer to a new IBlendState, or nullptr upon error.
		*/
	virtual std::shared_ptr<IBlendState> createBlendState( const BlendDesc& desc )=0;

	/**
		* Makes the given shader active.
		* @param shader Shader to make active.
		*/
	virtual void applyShader( const std::shared_ptr<IShader>& shader )=0;

	/**
		* Makes the given vertex buffer active.
		* @param buffer Vertex buffer to make active.
		*/
	virtual void setVertexBuffer( const std::shared_ptr<IVertexBuffer>& buffer )=0;

	/**
		* Makes the given index buffer active.
		* @param buffer Index buffer to make active.
		*/
	virtual void setIndexBuffer( const std::shared_ptr<IIndexBuffer>& buffer )=0;

	/**
		* Binds a 2d texture to a slot at a given shader stage.
		* @param index       Texture slot to bind to.
		* @param texture     Texture to bind.
		* @param shaderStage Stage at which to bind.
		*/
	virtual void setTexture2D( int index, const std::shared_ptr<ITexture2D>& texture, ShaderStage::Stage shaderStage )=0;

	/**
		* Binds a 3d texture to a slot at a given shader stage.
		* @param index       Texture slot to bind to.
		* @param texture     Texture to bind.
		* @param shaderStage Stage at which to bind.
		*/
	virtual void setTexture3D( int index, const std::shared_ptr<ITexture3D>& texture, ShaderStage::Stage shaderStage )=0;

	/**
		* Binds a cube texture to a slot at a given shader stage.
		* @param index       Texture slot to bind to.
		* @param texture     Texture to bind.
		* @param shaderStage Stage at which to bind.
		*/
	virtual void setTextureCube( int index, const std::shared_ptr<ITextureCube>& texture, ShaderStage::Stage shaderStage )=0;

	/**
		* Binds a sampler state to a given slot at a given shader stage.
		* @param index       Sampler slot to bind to.
		* @param state       Sampler to bind.
		* @param shaderStage Stage at which to bind.
		*/
	virtual void setSamplerState( int index, const std::shared_ptr<ISamplerState>& state, ShaderStage::Stage shaderStage )=0;

	/**
		* Sets the current blend state.
		* If the provided state is null, the default state will be applied.
		* @param state Blend state to apply.
		*/
	virtual void setBlendState( const std::shared_ptr<IBlendState>& state )=0;

	/**
		* Draws primitives from the currently bound vertex buffer.
		* @param topology    Topology to draw with.
		* @param vertexCount Number of vertices to draw.
		* @param startIndex  Offset into the bound vertex buffer of which to start.
		*/
	virtual void drawArrays( PrimitiveTopology topology, int vertexCount, int startIndex )=0;

	/**
		* Draws indexed primitives from the currently bound vertex and index buffer.
		* @param topology   Topology to draw with.
		* @param indexCount Number of indices to draw.
		*/
	virtual void drawIndexed( PrimitiveTopology topology, int indexCount )=0;

	/**
		* Sets active render targets.
		* @param renderTargets    Array of pointers to IRenderTarget2D.
		* @param numRenderTargets Number of render targets in the provided array.
		*/
	virtual void setRenderTargets( IRenderTarget2D** renderTargets, int numRenderTargets )=0;

	/**
		* Restores the backbuffer and default depth stencil as the active render target.
		*/
	virtual void restoreDefaultRenderTargets()=0;

	/**
		* Resizes the backbuffer and depth stencil to the bound IWindow's size.
		* @returns ErrorCode indicating success or failure.
		*/
	virtual ErrorCode resize()=0;

	/**
		* Resizes a given 2d texture to the given dimensions.
		* @param texture 2D texture to resize.
		* @param width   New width in pixels.
		* @param height  New height in pixels.
		* @returns ErrorCode indicating success or failure.
		*/
	virtual ErrorCode resizeTexture2D( const std::shared_ptr<ITexture2D>& texture, int width, int height )=0;

	/**
		* Resizes a given 2d render target to the given dimensions.
		* @param texture 2D render target to resize.
		* @param width   New width in pixels.
		* @param height  New height in pixels.
		* @returns ErrorCode indicating success or failure.
		*/
	virtual ErrorCode resizeRenderTarget2D( const std::shared_ptr<IRenderTarget2D>& target, int width, int height )=0;

	/**
		* Sets the color that will be applied to the active render targets when the clear function is called.
		* @param r Red.
		* @param g Green.
		* @param b Blue.
		* @param a Alpha.
		*/
	virtual void setClearColor( float r, float g, float b, float a )=0;

	/** 
	 * Sets the value that depth targets will be cleared to.
	 */
	virtual void setClearDepth( float depth )=0;

	/**
	 * Sets the value that stencil targets will be cleared to.
	 */
	virtual void setClearStencil( int stencil )=0;

	/**
		* Clears the currently bound render targets.
		* @param flags Bitfield of ClearFlags defining what to clear.
		*/
	virtual void clear( int flags )=0;

	/**
		* Sets the active rasterizer state.
		* @param state Rasterizer state to make active.
		*/
	virtual void setRasterizerState( const std::shared_ptr<IRasterizerState>& state )=0;

	/**
		* Sets the active depth stencil state.
		* @param state Depth stencil state to make active.
		*/
	virtual void setDepthStencilState( const std::shared_ptr<IDepthStencilState>& state )=0;

	/**
		* Sets the expected shader extension to be returned by getShaderExt.
		* Note that this does not affect shader loading; it exists purely for convenience when targeting multiple graphics APIs.
		* @param ext Extension to set.
		*/
	virtual void setShaderExt( const char* ext )=0;

	/**
		* Gets the shader extension set by the user.
		* Note that this does not affect shader loading; it exists purely for convenience when targeting multiple graphics APIs.
		* @returns User-set string defining the shader file extension.
		*/
	virtual const char* getShaderExt() const=0;

	/**
		* Gets the IWindow this device is rendering to.
		* @returns Pointer to the attached window.
		*/
	virtual std::shared_ptr<IWindow> getWindow() const=0;

	/**
		* Gets the name of the GPU being used.
		*/
	virtual const char* getGpuName() const=0;

	/**
		* Gets a string representing the underlying api version.
		*/
	virtual const char* getApiInfo() const=0;

	/**
		* Gets the underlying API type.
		*/
	virtual GraphicsApiType getApiType() const=0;

	/**
		* Restores all default states for the device.
		* @returns ErrorCode indicating success or failure.
		*/
	virtual ErrorCode restoreDefaultStates()=0;

	/**
		* Restores the device's default blending state.
		* @returns ErrorCode indicating success or failure.
		*/
	virtual ErrorCode restoreDefaultBlendState()=0;

	/**
		* Restores the device's default rasterizer state.
		* @returns ErrorCode indicating success or failure.
		*/
	virtual ErrorCode restoreDefaultRasterizerState()=0;

	/**
		* Restores the device's default depth stencil state.
		* @returns ErrorCode indicating success or failure.
		*/
	virtual ErrorCode restoreDefaultDepthStencilState()=0;

	virtual std::shared_ptr<IBlendState> getDefaultBlendAdditive()=0;
	virtual std::shared_ptr<IBlendState> getDefaultBlendAlpha()=0;
	virtual std::shared_ptr<IBlendState> getDefaultBlendNonPremul()=0;
	virtual std::shared_ptr<IBlendState> getDefaultBlendOpaque()=0;
	virtual std::shared_ptr<IRasterizerState> getDefaultRasterNone()=0;
	virtual std::shared_ptr<IRasterizerState> getDefaultRasterClockwise()=0;
	virtual std::shared_ptr<IRasterizerState> getDefaultRasterCounterClockwise()=0;
	virtual std::shared_ptr<IDepthStencilState> getDefaultDepthStencilDefault()=0;
	virtual std::shared_ptr<IDepthStencilState> getDefaultDepthStencilDepthRead()=0;
	virtual std::shared_ptr<IDepthStencilState> getDefaultDepthStencilNone()=0;
};

}

#endif