#include <ciri/graphics/win/dx/DXIndexBuffer.hpp>
#include <ciri/graphics/win/dx/DXGraphicsDevice.hpp>

using namespace ciri;

DXIndexBuffer::DXIndexBuffer( const std::shared_ptr<DXGraphicsDevice>& device )
	: IIndexBuffer(), _device(device), _indexBuffer(nullptr), _indexCount(0) {
}

DXIndexBuffer::~DXIndexBuffer() {
	destroy();
}

ErrorCode DXIndexBuffer::set( int* indices, int indexCount, bool dynamic ) {
	if( nullptr == indices || indexCount <= 0 ) {
		return ErrorCode::CIRI_INVALID_ARGUMENT;
	}

	// todo: add remapping (updating) support
	if( _indexBuffer != nullptr ) {
		return ErrorCode::CIRI_NOT_IMPLEMENTED;
	}

	_indexCount = indexCount;

	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.ByteWidth = sizeof(int) * indexCount;
	desc.Usage = dynamic ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	desc.CPUAccessFlags = dynamic ? D3D11_CPU_ACCESS_WRITE : 0;
	desc.MiscFlags = 0;
	desc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA data;
	ZeroMemory(&data, sizeof(data));
	data.pSysMem = indices;
	data.SysMemPitch = 0;
	data.SysMemSlicePitch = 0;

	if( FAILED(_device->getDevice()->CreateBuffer(&desc, &data, &_indexBuffer)) ) {
		destroy();
		return ErrorCode::CIRI_UNKNOWN_ERROR;
	}

	return ErrorCode::CIRI_OK;
}

void DXIndexBuffer::destroy() {
	if( _indexBuffer != nullptr ) {
		_indexBuffer->Release();
		_indexBuffer = nullptr;
	}
}

int DXIndexBuffer::getIndexCount() const {
	return _indexCount;
}

ID3D11Buffer* DXIndexBuffer::getIndexBuffer() const {
	return _indexBuffer;
}