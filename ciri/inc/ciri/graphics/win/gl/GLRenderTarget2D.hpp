#ifndef __ciri_graphics_GLRenderTarget2D__
#define __ciri_graphics_GLRenderTarget2D__

#include <memory>
#include <ciri/graphics/IRenderTarget2D.hpp>

namespace ciri {

class GLTexture2D;

class GLRenderTarget2D : public IRenderTarget2D {
public:
	GLRenderTarget2D();
	virtual ~GLRenderTarget2D();

	bool create( const std::shared_ptr<GLTexture2D>& texture, const std::shared_ptr<GLTexture2D>& depth );

	virtual void destroy() override;

	virtual std::shared_ptr<ITexture2D> getTexture() const override;
	virtual std::shared_ptr<ITexture2D> getDepth() const override;

private:
	std::shared_ptr<GLTexture2D> _texture;
	std::shared_ptr<GLTexture2D> _depth;
};

}

#endif