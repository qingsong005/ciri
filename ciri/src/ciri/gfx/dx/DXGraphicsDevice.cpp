#include <ciri/gfx/dx/DXGraphicsDevice.hpp>
#include <ciri/wnd/Window.hpp>
#include <DirectXColors.h>

namespace ciri {
	DXGraphicsDevice::DXGraphicsDevice()
		: IGraphicsDevice(), _swapchain(nullptr), _device(nullptr), _context(nullptr), _backbuffer(nullptr),
			_activeShader(nullptr), _activeVertexBuffer(nullptr), _activeIndexBuffer(nullptr),
			_activeRasterizerState(nullptr) {
	}

	DXGraphicsDevice::~DXGraphicsDevice() {
		destroy();
	}

	bool DXGraphicsDevice::create( Window* window ) {
		cc::Vec2ui windowSize = window->getSize();
		if( !initDevice(windowSize.x, windowSize.y, window->getHandle()) ) {
			destroy();
			return false;
		}

		return true;
	}

	void DXGraphicsDevice::destroy() {
		// clean rasterizer states
		for( unsigned int i = 0; i < _rasterizerStates.size(); ++i ) {
			if( _rasterizerStates[i] != nullptr ) {
				_rasterizerStates[i]->destroy();
				delete _rasterizerStates[i];
				_rasterizerStates[i] = nullptr;
			}
		}
		_rasterizerStates.clear();

		// destroy 2d render targets
		for( unsigned int i = 0; i < _renderTarget2Ds.size(); ++i ) {
			if( _renderTarget2Ds[i] != nullptr ) {
				_renderTarget2Ds[i]->destroy();
				delete _renderTarget2Ds[i];
				_renderTarget2Ds[i] = nullptr;
			}
		}
		_renderTarget2Ds.clear();

		// destroy samplers
		for( unsigned int i = 0; i < _samplers.size(); ++i ) {
			if( _samplers[i] != nullptr ) {
				_samplers[i]->destroy();
				delete _samplers[i];
				_samplers[i] = nullptr;
			}
		}
		_samplers.clear();

		// destroy 2d textures
		for( unsigned int i = 0; i < _texture2Ds.size(); ++i ) {
			if( _texture2Ds[i] != nullptr ) {
				_texture2Ds[i]->destroy();
				delete _texture2Ds[i];
				_texture2Ds[i] = nullptr;
			}
		}
		_texture2Ds.clear();

		// destroy constant buffers
		for( unsigned int i = 0; i < _constantBuffers.size(); ++i ) {
			if( _constantBuffers[i] != nullptr ) {
				_constantBuffers[i]->destroy();
				delete _constantBuffers[i];
				_constantBuffers[i] = nullptr;
			}
		}
		_constantBuffers.clear();

		// destroy index buffers
		for( unsigned int i = 0; i < _indexBuffers.size(); ++i ) {
			if( _indexBuffers[i] != nullptr ) {
				_indexBuffers[i]->destroy();
				delete _indexBuffers[i];
				_indexBuffers[i] = nullptr;
			}
		}
		_indexBuffers.clear();

		// destroy all vertex buffers
		for( unsigned int i = 0; i < _vertexBuffers.size(); ++i ) {
			if( _vertexBuffers[i] != nullptr ) {
				_vertexBuffers[i]->destroy();
				delete _vertexBuffers[i];
				_vertexBuffers[i] = nullptr;
			}
		}
		_vertexBuffers.clear();

		// destroy all shaders
		for( unsigned int i = 0; i < _shaders.size(); ++i ) {
			if( _shaders[i] != nullptr ) {
				_shaders[i]->destroy();
				delete _shaders[i];
				_shaders[i] = nullptr;
			}
		}
		_shaders.clear();

		if( _backbuffer ) { _backbuffer->Release(); _backbuffer = nullptr; }
		if( _context )    { _context->ClearState(); _context = nullptr; }
		if( _swapchain )  { _swapchain->Release(); _swapchain = nullptr; }
		if( _device )     { _device->Release(); _device = nullptr; }
	}

	void DXGraphicsDevice::present() {
		_swapchain->Present(0, 0);
	}

	IShader* DXGraphicsDevice::createShader() {
		DXShader* shader = new DXShader(this);
		_shaders.push_back(shader);
		return shader;
	}

	IVertexBuffer* DXGraphicsDevice::createVertexBuffer() {
		DXVertexBuffer* buffer = new DXVertexBuffer(this);
		_vertexBuffers.push_back(buffer);
		return buffer;
	}

	IIndexBuffer* DXGraphicsDevice::createIndexBuffer() {
		DXIndexBuffer* buffer = new DXIndexBuffer(this);
		_indexBuffers.push_back(buffer);
		return buffer;
	}

	IConstantBuffer* DXGraphicsDevice::createConstantBuffer() {
		DXConstantBuffer* buffer = new DXConstantBuffer(this);
		_constantBuffers.push_back(buffer);
		return buffer;
	}

	ITexture2D* DXGraphicsDevice::createTexture2D() {
		DXTexture2D* dxTexture = new DXTexture2D(this);
		_texture2Ds.push_back(dxTexture);
		return dxTexture;
	}

	ISamplerState* DXGraphicsDevice::createSamplerState( const SamplerDesc& desc ) {
		DXSamplerState* dxSampler = new DXSamplerState(this);
		if( !dxSampler->create(desc) ) {
			delete dxSampler;
			dxSampler = nullptr;
			return nullptr;
		}
		_samplers.push_back(dxSampler);
		return dxSampler;
	}

	IRenderTarget2D* DXGraphicsDevice::createRenderTarget2D( int width, int height, TextureFormat::Type format ) {
		DXRenderTarget2D* dxTarget = new DXRenderTarget2D(this);
		//...
		_renderTarget2Ds.push_back(dxTarget);
		return dxTarget;
	}

	IRasterizerState* DXGraphicsDevice::createRasterizerState( const RasterizerDesc& desc ) {
		DXRasterizerState* dxRaster = new DXRasterizerState(this);
		if( !dxRaster->create(desc) ) {
			delete dxRaster;
			dxRaster = nullptr;
			return nullptr;
		}
		_rasterizerStates.push_back(dxRaster);
		return dxRaster;
	}

	void DXGraphicsDevice::applyShader( IShader* shader ) {
		if( !shader->isValid() ) {
			_activeShader = nullptr;
			return;
		}

		DXShader* dxShader = reinterpret_cast<DXShader*>(shader);

		ID3D11VertexShader* vs = dxShader->getVertexShader();
		_context->VSSetShader(vs, nullptr, 0);
		const std::vector<DXConstantBuffer*>& vertexConstants = dxShader->getVertexConstants();
		for( unsigned int i = 0; i < vertexConstants.size(); ++i ) {
			ID3D11Buffer* buffer = vertexConstants[i]->getBuffer();
			const int index = vertexConstants[i]->getIndex();
			_context->VSSetConstantBuffers(index, 1, &buffer);
		}
		ID3D11InputLayout* il = dxShader->getInputLayout();
		_context->IASetInputLayout(il);

		ID3D11GeometryShader* gs = dxShader->getGeometryShader();
		if( gs != nullptr ) {
			_context->GSSetShader(gs, nullptr, 0);
			const std::vector<DXConstantBuffer*>& geometryConstants = dxShader->getGeometryConstants();
			for( unsigned int i = 0; i < geometryConstants.size(); ++i ) {
				ID3D11Buffer* buffer = geometryConstants[i]->getBuffer();
				const int index = geometryConstants[i]->getIndex();
				_context->GSSetConstantBuffers(index, 1, &buffer);
			}
		}

		ID3D11PixelShader* ps = dxShader->getPixelShader();
		_context->PSSetShader(ps, nullptr, 0);
		const std::vector<DXConstantBuffer*>& pixelConstants = dxShader->getPixelConstants();
		for( unsigned int i = 0; i < pixelConstants.size(); ++i ) {
			ID3D11Buffer* buffer = pixelConstants[i]->getBuffer();
			const int index = pixelConstants[i]->getIndex();
			_context->PSSetConstantBuffers(index, 1, &buffer);
		}

		_activeShader = dxShader;
	}

	void DXGraphicsDevice::setVertexBuffer( IVertexBuffer* buffer ) {
		// gl cannot set parameters with no active shader; dx can, but let's stay consistent
		if( nullptr == _activeShader ) {
			_activeVertexBuffer = nullptr;
			return;
		}

		DXVertexBuffer* dxBuffer = reinterpret_cast<DXVertexBuffer*>(buffer);
		UINT stride = buffer->getStride();
		UINT offset = 0;
		ID3D11Buffer* vb = dxBuffer->getVertexBuffer();
		_context->IASetVertexBuffers(0, 1, &vb, &stride, &offset);

		_activeVertexBuffer = dxBuffer;
	}

	void DXGraphicsDevice::setIndexBuffer( IIndexBuffer* buffer ) {
		DXIndexBuffer* dxBuffer = reinterpret_cast<DXIndexBuffer*>(buffer);

		ID3D11Buffer* ib = dxBuffer->getIndexBuffer();
		if( nullptr == ib ) {
			return; // todo: error
		}
		_context->IASetIndexBuffer(ib, DXGI_FORMAT_R32_UINT, 0);

		_activeIndexBuffer = dxBuffer;
	}

	void DXGraphicsDevice::setTexture2D( int index, ITexture2D* texture, ShaderStage::Stage shaderStage ) {
		DXTexture2D* dxTexture = reinterpret_cast<DXTexture2D*>(texture);

		// has to be an "array" to clear targets (if the input texture is nullptr) or dx shits its pants
		ID3D11ShaderResourceView* srv[1] = { (texture != nullptr) ? dxTexture->getShaderResourceView() : nullptr };

		const bool all = (shaderStage & ShaderStage::All);
		if( all || (shaderStage & ShaderStage::Vertex) ) {
			_context->VSSetShaderResources(index, 1, srv);
		}
		if( all || (shaderStage & ShaderStage::Geometry) ) {
			_context->GSSetShaderResources(index, 1, srv);
		}
		if( all || (shaderStage & ShaderStage::Pixel) ) {
			_context->PSSetShaderResources(index, 1, srv);
		}
	}

	void DXGraphicsDevice::setSamplerState( int index, ISamplerState* state, ShaderStage::Stage shaderStage ) {
		DXSamplerState* dxSampler = reinterpret_cast<DXSamplerState*>(state);

		// has to be an "array" to clear targets (if the input texture is nullptr) or dx shits its pants
		ID3D11SamplerState* sampler[1] = { (state != nullptr) ? dxSampler->getSamplerState() : nullptr };

		const bool all = (shaderStage & ShaderStage::All);
		if( all || (shaderStage & ShaderStage::Vertex) ) {
			_context->VSSetSamplers(index, 1, sampler);
		}
		if( all || (shaderStage & ShaderStage::Geometry) ) {
			_context->GSSetSamplers(index, 1, sampler);
		}
		if( all || (shaderStage & ShaderStage::Pixel) ) {
			_context->PSSetSamplers(index, 1, sampler);
		}
	}

	void DXGraphicsDevice::drawArrays( PrimitiveTopology::Type topology, int vertexCount, int startIndex ) {
		// cannot draw with no active shader
		if( nullptr == _activeShader ) {
			return;
		}

		// cannot draw with no active vertex buffer
		if( nullptr == _activeVertexBuffer ) {
			return;
		}

		// vertex count must be greater than 0!
		if( vertexCount <= 0 ) {
			return;
		}

		// start index must be greater than zero and less than vertex count
		if( startIndex < 0 || startIndex >= vertexCount ) {
			return;
		}

		_context->IASetPrimitiveTopology(convertTopology(topology));
		_context->Draw(vertexCount, startIndex);
	}

	void DXGraphicsDevice::drawIndexed( PrimitiveTopology::Type topology, int indexCount ) {
		// cannot draw with no active shader
		if( nullptr == _activeShader ) {
			return;
		}

		// cannot draw without a valid vertex and index buffer
		if( nullptr == _activeVertexBuffer || nullptr == _activeIndexBuffer ) {
			return;
		}

		// index count must be greater than 0
		if( indexCount <= 0 ) {
			return; // todo: error
		}

		_context->IASetPrimitiveTopology(convertTopology(topology));
		_context->DrawIndexed(indexCount, 0, 0);
	}
	
	void DXGraphicsDevice::setRenderTargets( IRenderTarget2D** renderTargets, int numRenderTargets ) {
		throw;
	}

	void DXGraphicsDevice::restoreDefaultRenderTargets() {
		throw;
	}

	void DXGraphicsDevice::clear( ClearFlags::Flags flags, float* color ) {
		if( color != nullptr ) {
			const FLOAT clearColor[4] = {color[0], color[1], color[2], color[3]};
			_context->ClearRenderTargetView(_backbuffer, clearColor);
		} else {
			_context->ClearRenderTargetView(_backbuffer, DirectX::Colors::CornflowerBlue);
		}
	}

	void DXGraphicsDevice::setRasterizerState( IRasterizerState* state ) {
		// todo: maybe make nullptr reset to default states? can just make a state myself and make it "default".
		if( state == _activeRasterizerState ) {
			return;
		}
		_activeRasterizerState = state;

		DXRasterizerState* dxRaster = reinterpret_cast<DXRasterizerState*>(state);
		ID3D11RasterizerState* dxState = dxRaster->getRasterizerState();
		_context->RSSetState(dxState);
	}

	ID3D11Device* DXGraphicsDevice::getDevice() const {
		return _device;
	}

	ID3D11DeviceContext* DXGraphicsDevice::getContext() const {
		return _context;
	}

	bool DXGraphicsDevice::initDevice( unsigned int width, unsigned int height, HWND hwnd ) {
		HRESULT hr = S_OK;

		// todo: make these as parameters
		const bool IS_VSYNC_ENABLED = true;
		const UINT MSAA_SAMPLES = 1;

		// create a dxgi factory
		IDXGIFactory* factory = nullptr;
		if( FAILED(CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory)) ) {
			return false;
		}

		// disable alt+enter
		factory->MakeWindowAssociation(hwnd, DXGI_MWA_NO_ALT_ENTER);

		// use factory to create an adapter for primary gpu
		IDXGIAdapter* adapter = nullptr;
		if( FAILED(factory->EnumAdapters(0, &adapter)) ) {
			factory->Release();
			return false;
		}

		// enumerate adapter output (monitor)
		IDXGIOutput* adapterOutput = nullptr;
		if( FAILED(adapter->EnumOutputs(0, &adapterOutput)) ) {
			adapter->Release();
			factory->Release();
			return false;
		}

		// get the number of modes that fit the backbuffer display format
		const DXGI_FORMAT backbufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
		UINT numDisplayModes = 0;
		if( FAILED(adapterOutput->GetDisplayModeList(backbufferFormat, DXGI_ENUM_MODES_INTERLACED, &numDisplayModes, nullptr)) ) {
			adapterOutput->Release();
			adapter->Release();
			factory->Release();
			return false;
		}

		// get all of the display modes
		DXGI_MODE_DESC* displayModes = new DXGI_MODE_DESC[numDisplayModes];
		if( FAILED(adapterOutput->GetDisplayModeList(backbufferFormat, DXGI_ENUM_MODES_INTERLACED, &numDisplayModes, displayModes)) ) {
			delete[] displayModes;
			return false;
		}

		// find a display mode that matches the screen width and height, and store its refresh rate information
		UINT refreshRateNumerator = 0;
		UINT refreshRateDenominator = 1;
		for( unsigned int i = 0; i < numDisplayModes; ++i ) {
			if( (width == displayModes[i].Width) && (height == displayModes[i].Height) ) {
				refreshRateNumerator = displayModes[i].RefreshRate.Numerator;
				refreshRateDenominator = displayModes[i].RefreshRate.Denominator;
				//break;
			}
		}

		// delete display modes array
		delete[] displayModes;
		displayModes = nullptr;

		// get the adapter description
		DXGI_ADAPTER_DESC adapterDesc;
		if( FAILED(adapter->GetDesc(&adapterDesc)) ) {
			return false;
		}

		// store gpu memory and gpu name
		const int gpuMemory = static_cast<int>(adapterDesc.DedicatedVideoMemory / 1024 / 1024);
		const std::wstring gpuName = adapterDesc.Description;

		// release adapter output, adapter, and factory
		adapterOutput->Release();
		adapterOutput = nullptr;
		adapter->Release();
		adapter = nullptr;
		factory->Release();
		factory = nullptr;

		// create and populate the swapchain description
		DXGI_SWAP_CHAIN_DESC swapDesc;
		ZeroMemory(&swapDesc, sizeof(swapDesc));
		swapDesc.BufferCount = 1;
		swapDesc.BufferDesc.Width = width;
		swapDesc.BufferDesc.Height = height;
		swapDesc.BufferDesc.Format = backbufferFormat;
		swapDesc.BufferDesc.RefreshRate.Numerator = (IS_VSYNC_ENABLED) ? refreshRateNumerator : 0;
		swapDesc.BufferDesc.RefreshRate.Denominator = (IS_VSYNC_ENABLED) ? refreshRateDenominator : 1;
		swapDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapDesc.OutputWindow = hwnd;
		swapDesc.SampleDesc.Count = MSAA_SAMPLES;
		swapDesc.SampleDesc.Quality = 0;
		swapDesc.Windowed = TRUE; // todo: fullscreen support
		swapDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		swapDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		swapDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		swapDesc.Flags = 0;

		// setup device creation flags
		UINT createDeviceFlags = 0;
		#ifdef _DEBUG
			createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
		#endif

		// create device, context, and swapchain
		const D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;
		if( FAILED(D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, createDeviceFlags, &featureLevel, 1, D3D11_SDK_VERSION, &swapDesc, &_swapchain, &_device, nullptr, &_context)) ) {
			return false;
		}

		// get a pointer to the backbuffer
		ID3D11Texture2D* backbufferPtr = nullptr;
		if( FAILED(_swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backbufferPtr)) ) {
			return false;
		}
		// create a render target view from the backbuffer
		if( FAILED(_device->CreateRenderTargetView(backbufferPtr, nullptr, &_backbuffer)) ) {
			return false;
		}
		// clean up backbuffer pointer
		backbufferPtr->Release();
		backbufferPtr = nullptr;

		// set the backbuffer as the current render target
		_context->OMSetRenderTargets(1, &_backbuffer, nullptr);
		// setup the viewport
		D3D11_VIEWPORT vp;
		vp.Width = static_cast<FLOAT>(width);
		vp.Height = static_cast<FLOAT>(height);
		vp.MinDepth = 0.0f;
		vp.MaxDepth = 1.0f;
		vp.TopLeftX = 0;
		vp.TopLeftY = 0;
		_context->RSSetViewports(1, &vp);

		// holy crap that was long
		return true;



		//HRESULT hr = S_OK;

		//UINT createDeviceFlags = 0;
		//#ifdef _DEBUG
		//	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
		//#endif

		//D3D_DRIVER_TYPE driverTypes[] = {
		//	D3D_DRIVER_TYPE_HARDWARE,
		//	D3D_DRIVER_TYPE_WARP,
		//	D3D_DRIVER_TYPE_REFERENCE
		//};
		//UINT numDriverTypes = ARRAYSIZE(driverTypes);

		//D3D_FEATURE_LEVEL featureLevels[] = {
		//	D3D_FEATURE_LEVEL_11_1,
		//	D3D_FEATURE_LEVEL_11_0,
		//	D3D_FEATURE_LEVEL_10_1,
		//	D3D_FEATURE_LEVEL_10_0
		//};
		//UINT numFeatureLevels = ARRAYSIZE(featureLevels);

		//for( UINT driverTypeIdx = 0; driverTypeIdx < numDriverTypes; ++driverTypeIdx ) {
		//	_driverType = driverTypes[driverTypeIdx];
		//	hr = D3D11CreateDevice(nullptr, _driverType, nullptr, createDeviceFlags, featureLevels, numFeatureLevels, D3D11_SDK_VERSION, &_device, &_featureLevel, &_context);
		//	if( E_INVALIDARG == hr ) {
		//		// dx 11.0 platforms don't recognize d3d_feature_leelv_11_1, so retry without it
		//		hr = D3D11CreateDevice(nullptr, _driverType, nullptr, createDeviceFlags, &featureLevels[1], numFeatureLevels-1, D3D11_SDK_VERSION, &_device, &_featureLevel, &_context);
		//	}
		//	if( SUCCEEDED(hr) ) {
		//		break;
		//	}
		//}
		//if( FAILED(hr) ) {
		//	return false;
		//}

		//// obtain dxgi factory from device (because we used nullptr for adapter above)
		//IDXGIFactory1* dxgiFactory = nullptr;
		//{
		//	IDXGIDevice* dxgiDevice = nullptr;
		//	hr = _device->QueryInterface(__uuidof(IDXGIDevice), reinterpret_cast<void**>(&dxgiDevice));
		//	if( SUCCEEDED(hr) ) {
		//		IDXGIAdapter* adapter = nullptr;
		//		hr = dxgiDevice->GetAdapter(&adapter);
		//		if( SUCCEEDED(hr) ) {
		//			hr = adapter->GetParent(__uuidof(IDXGIFactory1), reinterpret_cast<void**>(&dxgiFactory));
		//			adapter->Release();
		//		}
		//		dxgiDevice->Release();
		//	}
		//}
		//if( FAILED(hr) ) {
		//	return false;
		//}

		//// create swapchain
		//IDXGIFactory2* dxgiFactory2 = nullptr;
		//hr = dxgiFactory->QueryInterface(__uuidof(IDXGIFactory2), reinterpret_cast<void**>(&dxgiFactory2));
		//if( dxgiFactory2 ) {
		//	// dx 11.1 or later
		//	hr = _device->QueryInterface(__uuidof(ID3D11Device1), reinterpret_cast<void**>(&_device1));
		//	if( SUCCEEDED(hr) ) {
		//		_context->QueryInterface(__uuidof(ID3D11DeviceContext1), reinterpret_cast<void**>(&_context1));
		//	}

		//	DXGI_SWAP_CHAIN_DESC1 desc;
		//	ZeroMemory(&desc, sizeof(desc));
		//	desc.Width = width;
		//	desc.Height = height;
		//	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		//	desc.SampleDesc.Count = 1;
		//	desc.SampleDesc.Quality = 0;
		//	desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		//	desc.BufferCount = 1;

		//	hr = dxgiFactory2->CreateSwapChainForHwnd(_device, hwnd, &desc, nullptr, nullptr, &_swapchain1);
		//	if( SUCCEEDED(hr) ) {
		//		hr = _swapchain1->QueryInterface(__uuidof(IDXGISwapChain), reinterpret_cast<void**>(&_swapchain));
		//	}

		//	dxgiFactory2->Release();
		//} else {
		//	// dx 11.0
		//	DXGI_SWAP_CHAIN_DESC desc;
		//	ZeroMemory(&desc, sizeof(desc));
		//	desc.BufferCount = 1;
		//	desc.BufferDesc.Width = width;
		//	desc.BufferDesc.Height = height;
		//	desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		//	desc.BufferDesc.RefreshRate.Numerator = 60;
		//	desc.BufferDesc.RefreshRate.Denominator = 1;
		//	desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		//	desc.OutputWindow = hwnd;
		//	desc.SampleDesc.Count = 1;
		//	desc.SampleDesc.Quality = 0;
		//	desc.Windowed = TRUE;

		//	hr = dxgiFactory->CreateSwapChain(_device, &desc, &_swapchain);
		//}

		//// disable alt+enter to toggle fullscreen
		//dxgiFactoryDXGI_MWA_NO_ALT_ENTERMakeWindowAssociation(hwnd, ->);

		//dxgiFactory->Release();

		//if( FAILED(hr) ) {
		//	return false;
		//}

		//// create render target view
		//ID3D11Texture2D* backbuffer = nullptr;
		//hr = _swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backbuffer));
		//if( FAILED(hr) ) {
		//	return false;
		//}
		//hr = _device->CreateRenderTargetView(backbuffer, nullptr, &_backbuffer);
		//backbuffer->Release();
		//if( FAILED(hr) ) {
		//	return false;
		//}
		//_context->OMSetRenderTargets(1, &_backbuffer, nullptr);

		//// setup the viewport
		//D3D11_VIEWPORT vp;
		//vp.Width = (FLOAT)width;
		//vp.Height = (FLOAT)height;
		//vp.MinDepth = 0.0f;
		//vp.MaxDepth = 1.0f;
		//vp.TopLeftX = 0;
		//vp.TopLeftY = 0;
		//_context->RSSetViewports(1, &vp);

		//return true;
	}

	D3D_PRIMITIVE_TOPOLOGY DXGraphicsDevice::convertTopology( PrimitiveTopology::Type topology ) const {
		switch( topology ) {
			case PrimitiveTopology::PointList: {
				return D3D11_PRIMITIVE_TOPOLOGY_POINTLIST;
			}

			case PrimitiveTopology::LineList: {
				return D3D11_PRIMITIVE_TOPOLOGY_LINELIST;
			}

			case PrimitiveTopology::LineStrip: {
				return D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP;
			}

			case PrimitiveTopology::TriangleList: {
				return D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
			}

			case PrimitiveTopology::TriangleStrip: {
				return D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
			}

			default: {
				return D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED;
			}
		}
	}
} // ciri
