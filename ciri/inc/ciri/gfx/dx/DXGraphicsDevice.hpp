#ifndef __ciri_dxgraphicsdevice__
#define __ciri_dxgraphicsdevice__

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "D3DCompiler.lib")
#pragma comment(lib, "dxguid.lib")

#include <vector>
#include <d3d11_1.h>
#include "../IGraphicsDevice.hpp"
#include "DXShader.hpp"
#include "DXVertexBuffer.hpp"
#include "DXConstantBuffer.hpp"

namespace ciri {
	class DXGraphicsDevice : public IGraphicsDevice {
	public:
		DXGraphicsDevice();
		virtual ~DXGraphicsDevice();

		virtual bool create( Window* window );
		virtual void destroy();
		virtual void present();
		virtual IShader* createShader();
		virtual void applyShader( IShader* shader );
		virtual IVertexBuffer* createVertexBuffer();
		virtual void setVertexBuffer( IVertexBuffer* buffer );
		virtual void drawArrays( PrimitiveTopology::Type topology, int vertexCount, int startIndex );
		virtual void clear();
		virtual IConstantBuffer* createConstantBuffer();

		ID3D11Device* getDevice() const;
		ID3D11DeviceContext* getContext() const;

	private:
		bool initDevice( unsigned int width, unsigned int height, HWND hwnd );

	private:
		D3D_DRIVER_TYPE _driverType;
		D3D_FEATURE_LEVEL _featureLevel;
		ID3D11Device* _device;
		ID3D11Device1* _device1;
		ID3D11DeviceContext* _immediateContext;
		ID3D11DeviceContext1* _immediateContext1;
		IDXGISwapChain* _swapchain;
		IDXGISwapChain1* _swapchain1;
		ID3D11RenderTargetView* _renderTargetView;
		//
		std::vector<DXShader*> _shaders;
		std::vector<DXVertexBuffer*> _vertexBuffers;
		//
		DXShader* _activeShader;
		DXVertexBuffer* _activeVertexBuffer;
		//
		std::vector<DXConstantBuffer*> _constantBuffers;
	};
} // ciri

#endif /* __ciri_dxgraphicsdevice__ */