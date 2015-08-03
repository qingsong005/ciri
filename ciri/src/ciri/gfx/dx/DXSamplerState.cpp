#include <ciri/gfx/dx/DXSamplerState.hpp>
#include <ciri/gfx/dx/DXGraphicsDevice.hpp>

namespace ciri {
	DXSamplerState::DXSamplerState( DXGraphicsDevice* device )
		: ISamplerState(), _device(device), _samplerState(nullptr) {
	}

	DXSamplerState::~DXSamplerState() {
		destroy();
	}

	bool DXSamplerState::create( const SamplerDesc& desc ) {
		if( _samplerState != nullptr ) {
			return false;
		}

		D3D11_SAMPLER_DESC samplerDesc;
		ZeroMemory(&samplerDesc, sizeof(samplerDesc));

		samplerDesc.AddressU = ciriToDxWrap(desc.wrapU);
		samplerDesc.AddressV = ciriToDxWrap(desc.wrapV);
		samplerDesc.AddressW = ciriToDxWrap(desc.wrapW);
		samplerDesc.BorderColor[0] = desc.borderColor[0];
		samplerDesc.BorderColor[1] = desc.borderColor[1];
		samplerDesc.BorderColor[2] = desc.borderColor[2];
		samplerDesc.BorderColor[3] = desc.borderColor[3];
		samplerDesc.ComparisonFunc = ciriToDxFunc(desc.comparisonFunc);
		samplerDesc.Filter = ciriToDxFilter(desc.filter);
		samplerDesc.MaxAnisotropy = desc.maxAnisotropy;
		samplerDesc.MaxLOD = desc.maxLod;
		samplerDesc.MinLOD = desc.minLod;
		samplerDesc.MipLODBias = desc.lodBias;

		if( FAILED(_device->getDevice()->CreateSamplerState(&samplerDesc, &_samplerState)) ) {
			destroy();
			return false;
		}

		return true;
	}

	void DXSamplerState::destroy() {
		if( _samplerState != nullptr ) {
			_samplerState->Release();
			_samplerState = nullptr;
		}
	}

	ID3D11SamplerState* DXSamplerState::getSamplerState() const {
		return _samplerState;
	}

	D3D11_TEXTURE_ADDRESS_MODE DXSamplerState::ciriToDxWrap( SamplerWrap::Mode mode ) const {
		switch( mode ) {
			case SamplerWrap::Wrap: {
				return D3D11_TEXTURE_ADDRESS_WRAP;
			}

			case SamplerWrap::Mirror: {
				return D3D11_TEXTURE_ADDRESS_MIRROR;
			}

			case SamplerWrap::Clamp: {
				return D3D11_TEXTURE_ADDRESS_CLAMP;
			}

			case SamplerWrap::Border: {
				return D3D11_TEXTURE_ADDRESS_BORDER;
			}

			case SamplerWrap::MirrorOnce: {
				return D3D11_TEXTURE_ADDRESS_MIRROR_ONCE;
			}

			default: {
				return D3D11_TEXTURE_ADDRESS_WRAP; // no fail available
			}
		}
	}

	D3D11_COMPARISON_FUNC DXSamplerState::ciriToDxFunc( SamplerComparison::Mode mode ) const {
		switch( mode ) {
			case SamplerComparison::Never: {
				return D3D11_COMPARISON_NEVER;
			}

			case SamplerComparison::Always: {
				return D3D11_COMPARISON_ALWAYS;
			}

			case SamplerComparison::Less: {
				return D3D11_COMPARISON_LESS;
			}

			case SamplerComparison::Equal: {
				return D3D11_COMPARISON_EQUAL;
			}

			case SamplerComparison::LessEqual: {
				return D3D11_COMPARISON_LESS_EQUAL;
			}

			case SamplerComparison::Greater: {
				return D3D11_COMPARISON_GREATER;
			}

			case SamplerComparison::GreaterEqual: {
				return D3D11_COMPARISON_GREATER_EQUAL;
			}

			case SamplerComparison::NotEqual: {
				return D3D11_COMPARISON_NOT_EQUAL;
			}

			default: {
				return D3D11_COMPARISON_NEVER; // no fail available
			}
		}
	}

	D3D11_FILTER DXSamplerState::ciriToDxFilter( SamplerFilter::Mode mode ) const {
		switch( mode ) {
			case SamplerFilter::Point: {
				return D3D11_FILTER_MIN_MAG_MIP_POINT;
			}

			case SamplerFilter::PointLinear: {
				return D3D11_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT;
			}

			case SamplerFilter::LinearPoint: {
				return D3D11_FILTER_MIN_LINEAR_MAG_MIP_POINT;
			}

			case SamplerFilter::Bilinear: {
				return D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
			}

			case SamplerFilter::Trilinear: {
				return D3D11_FILTER_MIN_MAG_MIP_LINEAR;
			}

			case SamplerFilter::Anisotropic: {
				return D3D11_FILTER_ANISOTROPIC;
			}

			default: {
				return D3D11_FILTER_MIN_MAG_MIP_POINT; // point filtering by default
			}
		}
	}
} // ciri