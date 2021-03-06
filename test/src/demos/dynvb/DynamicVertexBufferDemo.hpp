#ifndef __dynamicvertexbufferdemo__
#define __dynamicvertexbufferdemo__

#include <string>
#include <ciri/Game.hpp>
#include <ciri/graphics/MayaCamera.hpp>
#include "../../common/AxisGrid.hpp"
#include "../../common/AxisWidget.hpp"
#include "../../common/Model.hpp"
#include "../../common/ShaderPresets.hpp"
#include "OpenCloth.hpp"

class DynamicVertexBufferDemo : public ciri::App {
private:
	struct FlagVertex {
		cc::Vec3f position;
	};

public:
	DynamicVertexBufferDemo();
	virtual ~DynamicVertexBufferDemo();
	void* operator new( size_t i );
	void operator delete( void* p );

	virtual void onInitialize() override;
	virtual void onLoadContent() override;
	virtual void onEvent( const ciri::WindowEvent& evt ) override;
	virtual void onUpdate( const double deltaTime, const double elapsedTime ) override;
	virtual void onFixedUpdate( const double deltaTime, const double elapsedTime ) override;
	virtual void onDraw() override;
	virtual void onUnloadContent() override;

private:
	ciri::MayaCamera _camera;
	//
	std::shared_ptr<ciri::IDepthStencilState> _depthStencilState;
	std::shared_ptr<ciri::IRasterizerState> _rasterizerState;
	//
	AxisGrid _grid;
	AxisWidget _axis;
	//
	SimpleShader _simpleShader;
	Model _flagpole;
	//
	OpenCloth _cloth;
	bool _clothRunning;
};

#endif /* __dynamicvertexbufferdemo__ */