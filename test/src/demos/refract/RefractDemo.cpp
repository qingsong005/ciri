#include "RefractDemo.hpp"

RefractDemo::RefractDemo()
	: App(), _model(nullptr) {
	_config.width = 1280;
	_config.height = 720;
	_config.title = "ciri : Refraction Demo";
}

RefractDemo::~RefractDemo() {
}

void* RefractDemo::operator new( size_t i ) {
	return _mm_malloc(i, 16);
}

void RefractDemo::operator delete( void* p ) {
	_mm_free(p);
}

void RefractDemo::onInitialize() {
	App::onInitialize();

	// print driver information
	printf("Device: %s\n", graphicsDevice()->getGpuName());
	printf("API: %s\n", graphicsDevice()->getApiInfo());
	
	// configure camera
	_camera.setAspect((float)window()->getWidth() / (float)window()->getHeight());
	_camera.setPlanes(0.1f, 1000.0f);
	_camera.setYaw(59.0f);
	_camera.setPitch(18.f);
	_camera.setOffset(53.0f);
	_camera.setSensitivity(1.0f, 1.0f, 0.25f);
	_camera.setLerpStrength(100.0f);
	_camera.setTarget(cc::Vec3f(0.0f, 0.0f, 0.0f));
	_camera.resetPosition();
}

void RefractDemo::onLoadContent() {
	App::onLoadContent();

	// create grid
	if( !_grid.create(graphicsDevice()) ) {
		printf("Failed to create grid.\n");
	}

	// create axis
	if( !_axis.create(5.0f, graphicsDevice()) ) {
		printf("Failed to create axis.\n");
	}

	// load shaders
	if( !loadShaders() ) {
		printf("Failed to load shaders.\n");
	}

	// load model
	_model = new Model();
	if( !_model->addFromObj("refract/stanford_dragon/dragon.obj") ) {
		printf("Failed to load obj model.\n");
	} else {
		if( !_model->build(graphicsDevice()) ) {
			printf("Failed to build model.\n");
		}
	}
	if( _model != nullptr && _model->isValid() ) {
		_model->setShader(_refractShader);
	} else {
		printf("Failed to create model.\n");
	}

	// create depth stencil state
	ciri::DepthStencilDesc depthDesc;
	_depthStencilState = graphicsDevice()->createDepthStencilState(depthDesc);
	if( nullptr == _depthStencilState ) {
		printf("Failed to create depth stencil state.\n");
	}

	// create rasterizer state
	ciri::RasterizerDesc rasterDesc;
	rasterDesc.cullMode = ciri::CullMode::None;
	//rasterDesc.fillMode = ciri::FillMode::Wireframe;
	_rasterizerState = graphicsDevice()->createRasterizerState(rasterDesc);
	if( nullptr == _rasterizerState ) {
		printf("Failed to create rasterizer state.\n");
	}

	// load the cubemap
	ciri::PNG cubeRight; cubeRight.loadFromFile("refract/skybox/posx.png", true);
	ciri::PNG cubeLeft; cubeLeft.loadFromFile("refract/skybox/negx.png", true);
	ciri::PNG cubeTop; cubeTop.loadFromFile("refract/skybox/negy.png", true);
	ciri::PNG cubeBottom; cubeBottom.loadFromFile("refract/skybox/posy.png", true);
	ciri::PNG cubeBack; cubeBack.loadFromFile("refract/skybox/posz.png", true);
	ciri::PNG cubeFront; cubeFront.loadFromFile("refract/skybox/negz.png", true);
	_cubemap = graphicsDevice()->createTextureCube(cubeRight.getWidth(), cubeRight.getHeight(), cubeRight.getPixels(), cubeLeft.getPixels(), cubeTop.getPixels(), cubeBottom.getPixels(), cubeBack.getPixels(), cubeFront.getPixels());

	// create cubemap sampler
	ciri::SamplerDesc cubeSamplerDesc;
	cubeSamplerDesc.filter = ciri::SamplerFilter::Linear;
	cubeSamplerDesc.wrapU = cubeSamplerDesc.wrapV = cubeSamplerDesc.wrapW = ciri::SamplerWrap::Clamp;
	_cubemapSampler = graphicsDevice()->createSamplerState(cubeSamplerDesc);

	// load bumpmap and diffuse map
	ciri::PNG bumpMap; bumpMap.loadFromFile("refract/dungeons-and-flagons_n.png", true);
	_bumpMap = graphicsDevice()->createTexture2D(bumpMap.getWidth(), bumpMap.getHeight(), ciri::TextureFormat::RGBA32_UINT, 0, bumpMap.getPixels());
	ciri::PNG diffuseMap; diffuseMap.loadFromFile("refract/dungeons-and-flagons_d.png", true);
	_diffuseMap = graphicsDevice()->createTexture2D(diffuseMap.getWidth(), diffuseMap.getHeight(), ciri::TextureFormat::RGBA32_UINT, 0, diffuseMap.getPixels());

	// create 3d texture
	const unsigned int T3D_WIDTH = 64;
	const unsigned int T3D_HEIGHT = 64;
	const unsigned int T3D_DEPTH = 4;
	#define LAYER(r) (T3D_WIDTH * T3D_HEIGHT * r * 4)
	#define TEX2(s, t) (4 * (s * T3D_WIDTH + t))
	#define TEX3(s, t, r) (TEX2(s, t) + LAYER(r))
	unsigned char* texels = new unsigned char[T3D_WIDTH * T3D_HEIGHT * T3D_DEPTH * 4];
	for( int s = 0; s < T3D_WIDTH; ++s ) {
		for( int t = 0; t < T3D_HEIGHT; ++t ) {
			texels[TEX3(s, t, 0)] = 0xFF;
			texels[TEX3(s, t, 0)+1] = 0x00;
			texels[TEX3(s, t, 0)+2] = 0x00;
			texels[TEX3(s, t, 0)+3] = 0xFF;
		}
	}
	for( int s = 0; s < T3D_WIDTH; ++s ) {
		for( int t = 0; t < T3D_HEIGHT; ++t ) {
			texels[TEX3(s, t, 1)] = 0x00;
			texels[TEX3(s, t, 1)+1] = 0xFF;
			texels[TEX3(s, t, 1)+2] = 0x00;
			texels[TEX3(s, t, 1)+3] = 0xFF;
		}
	}
	for( int s = 0; s < T3D_WIDTH; ++s ) {
		for( int t = 0; t < T3D_HEIGHT; ++t ) {
			texels[TEX3(s, t, 2)] = 0x00;
			texels[TEX3(s, t, 2)+1] = 0x00;
			texels[TEX3(s, t, 2)+2] = 0xFF;
			texels[TEX3(s, t, 2)+3] = 0xFF;
		}
	}
	for( int s = 0; s < T3D_WIDTH; ++s ) {
		for( int t = 0; t < T3D_HEIGHT; ++t ) {
			texels[TEX3(s, t, 3)] = 0x80;
			texels[TEX3(s, t, 3)+1] = 0x80;
			texels[TEX3(s, t, 3)+2] = 0x80;
			texels[TEX3(s, t, 3)+3] = 0xFF;
		}
	}
	_texture3D = graphicsDevice()->createTexture3D(T3D_WIDTH, T3D_HEIGHT, T3D_DEPTH, ciri::TextureFormat::RGBA32_UINT, 0, texels);
	delete[] texels;

	// create blend states
	_alphaBlendState = graphicsDevice()->getDefaultBlendAlpha();
	_defaultBlendState = graphicsDevice()->getDefaultBlendOpaque();

	// generate the skybox model
	_skyboxModel = modelgen::createFullscreenQuad(graphicsDevice());
	// load the skybox shader
	_skyboxShader = graphicsDevice()->createShader();
	_skyboxShader->addInputElement(ciri::VertexElement(ciri::VertexFormat::Float3, ciri::VertexUsage::Position, 0));
	_skyboxShader->addInputElement(ciri::VertexElement(ciri::VertexFormat::Float3, ciri::VertexUsage::Normal, 0));
	_skyboxShader->addInputElement(ciri::VertexElement(ciri::VertexFormat::Float4, ciri::VertexUsage::Tangent, 0));
	_skyboxShader->addInputElement(ciri::VertexElement(ciri::VertexFormat::Float2, ciri::VertexUsage::Texcoord, 0));
	const std::string shaderExt = graphicsDevice()->getShaderExt();
	const std::string vsFile = ("common/shaders/skybox_vs" + shaderExt);
	const std::string psFile = ("common/shaders/skybox_ps" + shaderExt);
	if( ciri::failed(_skyboxShader->loadFromFile(vsFile.c_str(), nullptr, psFile.c_str())) ) {
		printf("Failed to load skybox shader.\n");
	} else {
		_skyboxConstantBuffer = graphicsDevice()->createConstantBuffer();
		if( ciri::failed(_skyboxConstantBuffer->setData(sizeof(SkyboxConstants), &_skyboxConstants)) ) {
			printf("Failed to create skybox constants.\n");
		} else {
			if( ciri::failed(_skyboxShader->addConstants(_skyboxConstantBuffer, "SkyboxConstants", ciri::ShaderStage::Vertex)) ) {
				printf("Failed to assign constants to skybox shader.\n");
			}
		}
	}
	// create the skybox sampler
	ciri::SamplerDesc skySamplerDesc;
	skySamplerDesc.filter = ciri::SamplerFilter::Linear;
	skySamplerDesc.wrapU = skySamplerDesc.wrapV = skySamplerDesc.wrapW = ciri::SamplerWrap::Clamp;
	_skyboxSampler = graphicsDevice()->createSamplerState(skySamplerDesc);
	// create skybox depth state
	ciri::DepthStencilDesc skyboxDepthDesc;
	skyboxDepthDesc.depthWriteMask = false;
	_skyboxDepthState = graphicsDevice()->createDepthStencilState(skyboxDepthDesc);
}

void RefractDemo::onEvent( const ciri::WindowEvent& evt ) {
	App::onEvent(evt);

	switch( evt.type ) {
		case ciri::WindowEvent::Resized: {
			if( graphicsDevice()->resize() != ciri::ErrorCode::CIRI_OK ) {
				printf("Failed to resize backbuffer.\n");
			}
			break;
		}
	}
}

void RefractDemo::onUpdate( const double deltaTime, const double elapsedTime ) {
	App::onUpdate(deltaTime, elapsedTime);

	// check for close w/ escape
	if( window()->hasFocus() && input()->isKeyDown(ciri::Key::Escape) ) {
		gtfo();
		return;
	}

	// reload shaders
	if( window()->hasFocus() && input()->isKeyDown(ciri::Key::F5) && input()->wasKeyUp(ciri::Key::F5) ) {
		unloadShaders();
		printf("Reloaded shaders: %s\n", loadShaders() ? "success" : "failed");
	}

	// debug print camera information
	if( window()->hasFocus() && input()->isKeyDown(ciri::Key::F9) && input()->wasKeyUp(ciri::Key::F9) ) {
		const cc::Vec3f& pos = _camera.getPosition();
		const float yaw = _camera.getYaw();
		const float pitch = _camera.getPitch();
		const float dolly = _camera.getOffset();
		const cc::Vec3f& target = _camera.getTarget();
		printf("pos(%f/%f/%f); yaw(%f); pitch(%f); dolly(%f); target(%f/%f/%f)\n", pos.x, pos.y, pos.z, yaw, pitch, dolly, target.x, target.y, target.z);
	}

	// camera movement
	if( window()->hasFocus() && input()->isKeyDown(ciri::Key::LAlt) ) {
		// rotation
		if( input()->isMouseButtonDown(ciri::MouseButton::Left) ) {
			const float dx = (float)input()->mouseX() - (float)input()->lastMouseX();
			const float dy = (float)input()->mouseY() - (float)input()->lastMouseY();
			_camera.rotateYaw(-dx);
			_camera.rotatePitch(-dy);
		}
		// dolly
		if( input()->isMouseButtonDown(ciri::MouseButton::Right) ) {
			const float dx = (float)input()->mouseX() - (float)input()->lastMouseX();
			const float dy = (float)input()->mouseY() - (float)input()->lastMouseY();
			const float val = (fabsf(dx) > fabsf(dy)) ? dx : dy;
			_camera.dolly(val);
		}
		// pan
		if( input()->isMouseButtonDown(ciri::MouseButton::Middle) ) {
			const float dx = (float)input()->mouseX() - (float)input()->lastMouseX();
			const float dy = (float)input()->mouseY() - (float)input()->lastMouseY();
			_camera.pan(dx, -dy);
		}
	}
}

void RefractDemo::onFixedUpdate( const double deltaTime, const double elapsedTime ) {
	App::onFixedUpdate(deltaTime, elapsedTime);

	//_model->getXform().setOrientation(_model->getXform().getOrientation() * cc::Quatf::createFromEulerAngles(0.1f, 0.25f, 0.0f));

	// update camera
	_camera.update((float)deltaTime);
}

void RefractDemo::onDraw() {
	App::onDraw();

	std::shared_ptr<ciri::IGraphicsDevice> device = graphicsDevice();

	// camera's viewproj
	const cc::Mat4f cameraViewProj = _camera.getProj() * _camera.getView();

	// clear backbuffer
	device->clear(ciri::ClearFlags::Color | ciri::ClearFlags::Depth);

	// set depth and raster states
	device->setDepthStencilState(_depthStencilState);
	// todo: figure out why some modelgen models are CCW and some are CW by default (sphere vs. plane e.g)
	device->setRasterizerState(_rasterizerState);

	// set default alpha blend
	//device->setBlendState(_defaultBlendState);

	drawSkybox(_camera.getView(), _camera.getProj());
	
	// render grid
	if( _grid.isValid() ) {
		const cc::Mat4f gridXform = cameraViewProj * cc::Mat4f(1.0f);
		if( _grid.updateConstants(gridXform) ) {
			device->applyShader(_grid.getShader());
			device->setVertexBuffer(_grid.getVertexBuffer());
			device->drawArrays(ciri::PrimitiveTopology::LineList, _grid.getVertexBuffer()->getVertexCount(), 0);
		}
	}

	// render axis
	if( _axis.isValid() ) {
		const cc::Mat4f axisXform = cameraViewProj * cc::Mat4f(1.0f);
		if( _axis.updateConstants(axisXform) ) {
			device->applyShader(_axis.getShader());
			device->setVertexBuffer(_axis.getVertexBuffer());
			device->drawArrays(ciri::PrimitiveTopology::LineList, _axis.getVertexBuffer()->getVertexCount(), 0);
		}
	}

	// render model
	if( _refractShader != nullptr && _refractShader->isValid() && _model != nullptr && _model->isValid() && _cubemap != nullptr && _cubemapSampler != nullptr ) {
		// set alpha blend state
		//device->setBlendState(_alphaBlendState);

		// update constant buffer
		_refractVertexConstants.world = _model->getXform().getWorld();
		_refractVertexConstants.xform = cameraViewProj * _refractVertexConstants.world;
		_refractVertexConstants.campos = _camera.getPosition();
		if( ciri::failed(_refractVertexConstantBuffer->setData(sizeof(RefractVertexConstants), &_refractVertexConstants)) ) {
			printf("Failed to update constants.\n");
		}
		// apply shader
		device->applyShader(_refractShader);
		// set cubemap texture
		//device->setTexture2D(0, _diffuseMap, ciri::ShaderStage::Pixel);
		//device->setTexture2D(1, _bumpMap, ciri::ShaderStage::Pixel);
		device->setTextureCube(0, _cubemap, ciri::ShaderStage::Pixel);

		// set cubemap sampler
		device->setSamplerState(0, _cubemapSampler, ciri::ShaderStage::Pixel);
		//device->setSamplerState(1, _cubemapSampler, ciri::ShaderStage::Pixel);
		//device->setSamplerState(2, _cubemapSampler, ciri::ShaderStage::Pixel);
		// set vertex and index buffer and draw
		device->setVertexBuffer(_model->getVertexBuffer());
		if( _model->getIndexBuffer() != nullptr ) {
			device->setIndexBuffer(_model->getIndexBuffer());
			device->drawIndexed(ciri::PrimitiveTopology::TriangleList, _model->getIndexBuffer()->getIndexCount());
		} else {
			device->drawArrays(ciri::PrimitiveTopology::TriangleList, _model->getVertexBuffer()->getVertexCount(), 0);
		}
	}

	// present backbuffer to screen
	device->present();
}

void RefractDemo::onUnloadContent() {
	App::onUnloadContent();

	if( _skyboxModel != nullptr ) {
		delete _skyboxModel;
		_skyboxModel = nullptr;
	}

	if( _cubemapSampler != nullptr ) {
		_cubemapSampler->destroy();
	}

	if( _cubemap != nullptr ) {
		_cubemap->destroy();
	}

	if( _model != nullptr ) {
		delete _model;
		_model = nullptr;
	}

	// clean grid
	_grid.clean();

	// clean axis
	_axis.clean();
}

bool RefractDemo::loadShaders() {
	// create shader
	if( nullptr == _refractShader ) {
		_refractShader = graphicsDevice()->createShader();
		// add input elements to shader
		_refractShader->addInputElement(ciri::VertexElement(ciri::VertexFormat::Float3, ciri::VertexUsage::Position, 0));
		_refractShader->addInputElement(ciri::VertexElement(ciri::VertexFormat::Float3, ciri::VertexUsage::Normal, 0));
		_refractShader->addInputElement(ciri::VertexElement(ciri::VertexFormat::Float4, ciri::VertexUsage::Tangent, 0));
		_refractShader->addInputElement(ciri::VertexElement(ciri::VertexFormat::Float2, ciri::VertexUsage::Texcoord, 0));
	}

	// load shader from file
	const std::string shaderExt = graphicsDevice()->getShaderExt();
	const std::string vsFile = ("refract/refract_vs" + shaderExt);
	const std::string psFile = ("refract/refract_ps" + shaderExt);
	if( ciri::failed(_refractShader->loadFromFile(vsFile.c_str(), nullptr, psFile.c_str())) ) {
		printf("Shader failed to compile with %d errors:\n", _refractShader->getErrors().size());
		int idx = 0;
		for( const auto& err : _refractShader->getErrors() ) {
			printf("[%d]: %s\n", idx, err.msg.c_str());
			idx += 1;
		}
		return false;
	}

	// create constant buffer
	if( nullptr == _refractVertexConstantBuffer ) {
		_refractVertexConstantBuffer = graphicsDevice()->createConstantBuffer();
	}
	if( ciri::failed(_refractVertexConstantBuffer->setData(sizeof(RefractVertexConstants), &_refractVertexConstants)) ) {
		printf("Failed to create constant buffer.\n");
		return false;
	}

	// assign constant buffer
	if( ciri::failed(_refractShader->addConstants(_refractVertexConstantBuffer, "RefractVertexConstants", ciri::ShaderStage::Vertex)) ) {
		printf("Failed to assign constant buffer to shader.\n");
		return false;
	}

	return true;
}

void RefractDemo::unloadShaders() {
	if( _refractVertexConstantBuffer != nullptr ) {
		_refractVertexConstantBuffer->destroy();
	}

	if( _refractShader != nullptr ) {
		_refractShader->destroy();
	}
}

void RefractDemo::drawSkybox( const cc::Mat4f& view, const cc::Mat4f& proj ) {
	if( !_skyboxModel->isValid() ) {
		return;
	}

	// disable depth write
	graphicsDevice()->setDepthStencilState(_skyboxDepthState);
	// apply skybox shader
	graphicsDevice()->applyShader(_skyboxShader);
	// set skybox constants
	_skyboxConstants.view = view;
	_skyboxConstants.proj = proj;
	_skyboxConstants.proj.invert();
	_skyboxConstantBuffer->setData(sizeof(SkyboxConstants), &_skyboxConstants);
	// set skybox texture and sampler
	graphicsDevice()->setTextureCube(0, _cubemap, ciri::ShaderStage::Pixel);
	graphicsDevice()->setSamplerState(0, _skyboxSampler, ciri::ShaderStage::Pixel);
	// set buffers and draw
	graphicsDevice()->setVertexBuffer(_skyboxModel->getVertexBuffer());
	graphicsDevice()->setIndexBuffer(_skyboxModel->getIndexBuffer());
	graphicsDevice()->drawIndexed(ciri::PrimitiveTopology::TriangleList, _skyboxModel->getIndexBuffer()->getIndexCount());
	// unbind texture and sampler
	graphicsDevice()->setSamplerState(0, nullptr, ciri::ShaderStage::Pixel);
	graphicsDevice()->setTextureCube(0, nullptr, ciri::ShaderStage::Pixel);
	// restore default depth state
	graphicsDevice()->setDepthStencilState(_depthStencilState);
}