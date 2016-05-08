#include "Gridlr.hpp"
#include <cctype>

Gridlr::Gridlr()
	: Game(), _grid(nullptr), _blendState(nullptr), _samplerState(nullptr), _depthStencilState(nullptr), _rasterizerState(nullptr), _cellTexture(nullptr),
		_isDragging(false) {
	_config.width = 1280;
	_config.height = 720;
	_config.title = "ciri : Gridlr";
}

Gridlr::~Gridlr() {
}

void Gridlr::onInitialize() {
	Game::onInitialize();

	// cell off grid to start
	_lastMouseCell.x = -1;
	_lastMouseCell.y = -1;

	// make new empty grid
	_grid = std::make_unique<gridlr::BlockGrid>();
	_grid->create(8, 8);

	// enable chains
	_grid->enableChain(gridlr::BlockState::One, *_grid->get(0, 0), *_grid->get(4, 4));
	_grid->enableChain(gridlr::BlockState::Two, *_grid->get(4, 6), *_grid->get(6, 2));
	_grid->enableChain(gridlr::BlockState::Three, *_grid->get(1, 4), *_grid->get(5, 6));

	// debug print grid
	_grid->print();

	// print driver info
	printf("Device: %s\n", graphicsDevice()->getGpuName());
	printf("API: %s\n", graphicsDevice()->getApiInfo());

	// create spritebatch
	printf(_spriteBatch.create(graphicsDevice()) ? "Initialized SpriteBatch.\n" : "Failed to initialize SpriteBatch.\n");

	// create states
	_blendState = graphicsDevice()->getDefaultBlendAlpha();
	_samplerState = graphicsDevice()->createSamplerState(ciri::SamplerDesc());
	_depthStencilState = graphicsDevice()->getDefaultDepthStencilNone();
	_rasterizerState = graphicsDevice()->getDefaultRasterCounterClockwise();
}

void Gridlr::onLoadContent() {
	Game::onLoadContent();

	// load grid texture
	ciri::PNG cellPng;
	if( cellPng.loadFromFile("gridlr/grid_empty.png", true) && cellPng.hasAlpha() ) {
		_cellTexture = graphicsDevice()->createTexture2D(cellPng.getWidth(), cellPng.getHeight(), ciri::TextureFormat::RGBA32_UINT, 0, cellPng.getPixels());
		const auto& vp = graphicsDevice()->getViewport();
		_gridOffset.x = static_cast<float>((vp.width()  / 2) - (_cellTexture->getWidth()  * _grid->width()  / 2));
		_gridOffset.y = static_cast<float>((vp.height() / 2) - (_cellTexture->getHeight() * _grid->height() / 2));
	}
}

void Gridlr::onEvent(const ciri::WindowEvent& evt) {
	Game::onEvent(evt);
}

void Gridlr::onUpdate(const double deltaTime, const double elapsedTime) {
	Game::onUpdate(deltaTime, elapsedTime);

	// check for close w/ escape
	if( window()->hasFocus() && input()->isKeyDown(ciri::Key::Escape) ) {
		gtfo();
		return;
	}

	// compute which cell the mouse is under
	const int mouseCellX = (input()->mouseX() - static_cast<int>(_gridOffset.x)) / _cellTexture->getWidth();
	const int mouseCellY = (input()->mouseY() - static_cast<int>(_gridOffset.y)) / _cellTexture->getHeight();
	//printf("X: %d; Y: %d\n", mouseCellX, mouseCellY);

	if( input()->isMouseButtonDown(ciri::MouseButton::Left) && !_isDragging ) {
		_isDragging = true;
		_lastMouseCell.x = mouseCellX;
		_lastMouseCell.y = mouseCellY;

		// start drag
	} else if( input()->isMouseButtonDown(ciri::MouseButton::Left) && _isDragging && (_lastMouseCell.x != mouseCellX || _lastMouseCell.y != mouseCellY) ) {
		_lastMouseCell.x = mouseCellX;
		_lastMouseCell.y = mouseCellY;

		// on drag
	} else if( input()->isMouseButtonUp(ciri::MouseButton::Left) && _isDragging ) {
		_isDragging = false;

		// stop drag
	}
}

void Gridlr::onFixedUpdate(const double deltaTime, const double elapsedTime) {
	Game::onFixedUpdate(deltaTime, elapsedTime);
}

void Gridlr::onDraw() {
	Game::onDraw();

	const auto device = graphicsDevice();

	// clear screen
	device->setClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	device->clear(ciri::ClearFlags::Color | ciri::ClearFlags::Depth);

	_spriteBatch.begin(_blendState, _samplerState, _depthStencilState, _rasterizerState, SpriteSortMode::Deferred, nullptr);
	// draw grid
	for( int y = 0; y < _grid->height(); ++y ) {
		for( int x = 0; x < _grid->width(); ++x ) {
			auto block = _grid->get(x, y);

			// vp to invert positions
			auto& vp = device->getViewport();

			// offset and inverted positions
			cc::Vec2f position = {
				_gridOffset.x + static_cast<float>(_cellTexture->getWidth() * x),
				(vp.height() - _gridOffset.y - _cellTexture->getHeight()) - (_cellTexture->getHeight() * y) // subtract from top of screen to invert the grid so it displays right; sub cell tex height is b/c of the pivot being at the top left (need to fix this)
			};

			// pick color based on state
			cc::Vec4f color;
			switch( block->state() ) {
				case gridlr::BlockState::Empty: { color = cc::Vec4f(1.0f, 1.0f, 1.0f, 1.0); break; }
				case gridlr::BlockState::One: { color = cc::Vec4f(1.0f, 0.0f, 0.0f, 1.0); break; }
				case gridlr::BlockState::Two: { color = cc::Vec4f(0.0f, 1.0f, 0.0f, 1.0); break; }
				case gridlr::BlockState::Three: { color = cc::Vec4f(0.0f, 0.0f, 1.0f, 1.0); break; }
				default: { color = cc::Vec4f(1.0f, 0.0f, 1.0f, 1.0f); break; }
			}

			// draw
			const cc::Vec2f origin(0.0f, 0.0f);
			const cc::Vec2f scale(1.0f, 1.0f);
			_spriteBatch.draw(_cellTexture, position, 0.0f, origin, scale, 0.0f, color);
		}
	}
	_spriteBatch.end();

	// present to screen
	device->present();
}

void Gridlr::onUnloadContent() {
	Game::onUnloadContent();

	// clean spritebatch
	_spriteBatch.clean();
}