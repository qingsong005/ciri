#include "AxisWidget.hpp"
#include <vector>

AxisWidget::AxisWidget()
	: _vertexBuffer(nullptr), _shader(nullptr), _constantsBuffer(nullptr) {
}

AxisWidget::~AxisWidget() {
}

const std::shared_ptr<ciri::IVertexBuffer>& AxisWidget::getVertexBuffer() const {
	return _vertexBuffer;
}

const std::shared_ptr<ciri::IShader>& AxisWidget::getShader() const {
	return _shader;
}

bool AxisWidget::isValid() const {
	return (_vertexBuffer != nullptr) && (_shader != nullptr) && (_constantsBuffer != nullptr);
}

bool AxisWidget::create( float length, std::shared_ptr<ciri::IGraphicsDevice> device ) {
	std::vector<AxisVertex> vertices;
	const cc::Vec3f RED = cc::Vec3f(1.0f, 0.0f, 0.0f);
	const cc::Vec3f GREEN = cc::Vec3f(0.0f, 1.0f, 0.0f);
	const cc::Vec3f BLUE = cc::Vec3f(0.0f, 0.0f, 1.0f);

	// X
	vertices.push_back(AxisVertex(cc::Vec3f::zero(), RED));
	vertices.push_back(AxisVertex(cc::Vec3f(length, 0.0f, 0.0f), RED));

	// Y
	vertices.push_back(AxisVertex(cc::Vec3f::zero(), GREEN));
	vertices.push_back(AxisVertex(cc::Vec3f(0.0f, length, 0.0f), GREEN));

	//Z
	vertices.push_back(AxisVertex(cc::Vec3f::zero(), BLUE));
	vertices.push_back(AxisVertex(cc::Vec3f(0.0f, 0.0f, length), BLUE));

	_vertexBuffer = device->createVertexBuffer();
	if( ciri::failed(_vertexBuffer->set(vertices.data(), sizeof(AxisVertex), 6, false)) ) {
		return false;
	}

	_shader = device->createShader();
	_shader->addInputElement(ciri::VertexElement(ciri::VertexFormat::Float3, ciri::VertexUsage::Position, 0));
	_shader->addInputElement(ciri::VertexElement(ciri::VertexFormat::Float3, ciri::VertexUsage::Color, 0));
	const std::string shaderExt = device->getShaderExt();
	const std::string vsFile = ("data/shaders/axis_vs" + shaderExt);
	const std::string psFile = ("data/shaders/axis_ps" + shaderExt);
	if( ciri::failed(_shader->loadFromFile(vsFile.c_str(), nullptr, psFile.c_str())) ) {
		return false;
	}

	_constantsBuffer = device->createConstantBuffer();
	if( ciri::failed(_constantsBuffer->setData(sizeof(AxisConstants), &_constants)) ) {
		return false;
	}

	if( ciri::failed(_shader->addConstants(_constantsBuffer, "AxisConstants", ciri::ShaderStage::Vertex)) ) {
		return false;
	}

	return true;
}

bool AxisWidget::updateConstants( const cc::Mat4f& xform ) {
	if( nullptr == _shader || nullptr == _constantsBuffer ) {
		return false;
	}
	_constants.xform = xform;
	return ciri::success(_constantsBuffer->setData(sizeof(AxisConstants), &_constants));
}

void AxisWidget::clean() {
	if( _vertexBuffer != nullptr ) {
		_vertexBuffer->destroy();
		_vertexBuffer = nullptr;
	}
	if( _shader != nullptr ) {
		_shader->destroy();
		_shader = nullptr;
	}
	if( _constantsBuffer != nullptr ) {
		_constantsBuffer->destroy();
		_constantsBuffer = nullptr;
	}
}