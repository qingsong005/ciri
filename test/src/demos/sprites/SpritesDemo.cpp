#include "SpritesDemo.hpp"
#include <ciri/core/PNG.hpp>
#include <cc/Common.hpp>
#include <ctime>
#include <cc/Quaternion.hpp>
#include "MathHelper.hpp"

SpritesDemo::SpritesDemo()
	: IDemo() {
}

SpritesDemo::~SpritesDemo() {
}

void* SpritesDemo::operator new( size_t i ) {
	return _mm_malloc(i, 16); // allocate aligned to 16
}

void SpritesDemo::operator delete( void* p ) {
	_mm_free(p);
}

DemoConfig SpritesDemo::getConfig() {
	DemoConfig cfg;
	cfg.windowHeight = 720;
	cfg.windowWidth = 1280;
	cfg.windowTitle = "ciri : Sprites Demo";
	return cfg;
}

void SpritesDemo::onInitialize() {
	srand(static_cast<unsigned int>(time(0)));
	rand();

	window()->setCursorVisible(false);

	// print driver information
	printf("Device: %s\n", graphicsDevice()->getGpuName());
	printf("API: %s\n", graphicsDevice()->getApiInfo());

	// create spritebatch
	if( !_spritebatch.create(graphicsDevice()) ) {
		printf("Failed to initialize SpriteBatch.\n");
	} else {
		printf("Initialized SpriteBatch.\n");
	}

	// create states
	_blendState = graphicsDevice()->getDefaultBlendAlpha();
	_samplerState = graphicsDevice()->createSamplerState(ciri::SamplerDesc());
	_depthStencilState = graphicsDevice()->getDefaultDepthStencilNone();
	_rasterizerState = graphicsDevice()->getDefaultRasterCounterClockwise();

	// configure grid
	const ciri::Viewport& vp = graphicsDevice()->getViewport();
	_grid = new BMGrid(16, 16, vp.width(), vp.height(), graphicsDevice());
	_grid->resetAll();

	// configure player and set off initial spawn explosion
	_player = std::make_shared<PlayerShip>();
	_player->setPosition(cc::Vec2f(vp.width() * 0.5f, vp.height() * 0.5f));
	_grid->push(_player->getPosition().x, _player->getPosition().y, 10, 1);
}

void SpritesDemo::onLoadContent() {
	// load and set player texture
	ciri::PNG playerPng;
	if( playerPng.loadFromFile("sprites/textures/Player.png") && (4 == playerPng.getBytesPerPixel()) ) {
		_playerTexture = graphicsDevice()->createTexture2D(playerPng.getWidth(), playerPng.getHeight(), ciri::TextureFormat::Color, 0, playerPng.getPixels());
	}
	_player->setTexture(_playerTexture);

	// load bullet texture
	ciri::PNG bulletPng;
	if( bulletPng.loadFromFile("sprites/textures/Bullet.png") && (4 == bulletPng.getBytesPerPixel()) ) {
		_bulletTexture = graphicsDevice()->createTexture2D(bulletPng.getWidth(), bulletPng.getHeight(), ciri::TextureFormat::Color, 0, bulletPng.getPixels());
	}
	// assign bullet textures
	for( auto& bullet : _bullets ) {
		bullet.setTexture(_bulletTexture);
	}

	// load enemy textures
	ciri::PNG seekPng;
	if( seekPng.loadFromFile("sprites/textures/Seeker.png") && (4 == seekPng.getBytesPerPixel()) ) {
		_enemySeekerTexture = graphicsDevice()->createTexture2D(seekPng.getWidth(), seekPng.getHeight(), ciri::TextureFormat::Color, 0, seekPng.getPixels());
	}

	// load some enemies
	for( int i = 0; i < static_cast<int>(_enemies.size()); ++i ) {
		const float x = cc::math::randRange<float>(0.0f, static_cast<float>(window()->getWidth()));
		const float y = cc::math::randRange<float>(0.0f, static_cast<float>(window()->getHeight()));
		_enemies[i] = Enemy::createSeeker(cc::Vec2f(x, y));
		_enemies[i].setTexture(_enemySeekerTexture);
		_enemies[i].setIsAlive(true);
		_enemies[i].setTarget(_player);
	}

	// custom cursor texture
	ciri::PNG cursorPng;
	if( cursorPng.loadFromFile("sprites/textures/Pointer.png") && (4 == cursorPng.getBytesPerPixel()) ) {
		_cursorTexture = graphicsDevice()->createTexture2D(cursorPng.getWidth(), cursorPng.getHeight(), ciri::TextureFormat::Color, 0, cursorPng.getPixels());
		_cursorOrigin = cc::Vec2f(0.0f, static_cast<float>(_cursorTexture->getHeight()));
	}

	// load test particle system
	ciri::PNG glowPng;
	if( glowPng.loadFromFile("sprites/textures/dot.png") && (4 == glowPng.getBytesPerPixel()) ) {
		_testPsysTexture = graphicsDevice()->createTexture2D(glowPng.getWidth(), glowPng.getHeight(), ciri::TextureFormat::Color, 0, glowPng.getPixels());
		_psys.setTexture(_testPsysTexture);
	}
}

void SpritesDemo::onEvent( ciri::WindowEvent evt ) {
	switch( evt.type) {
		case ciri::WindowEvent::Resized: {
			if( graphicsDevice()->resize() != ciri::ErrorCode::CIRI_OK ) {
				printf("Failed to resize default render targets.\n");
			}
			break;
		}
	}
}

void SpritesDemo::onUpdate( double deltaTime, double elapsedTime ) {
	if( !window()->hasFocus() ) {
		return;
	}
	// check for close w/ escape
	if( input()->isKeyDown(ciri::Key::Escape) ) {
		this->gtfo();
		return;
	}

	// move custom cursor
	if( _cursorTexture != nullptr ) {
		_cursorPos = cc::Vec2f(static_cast<float>(input()->mouseX()), static_cast<float>(window()->getHeight() - input()->mouseY()));
		_cursorPos.x = (_cursorPos.x < 0.0f) ? 0.0f : _cursorPos.x;
		_cursorPos.y = (_cursorPos.y < _cursorTexture->getHeight()) ? _cursorTexture->getHeight() : _cursorPos.y;
		_cursorPos.x = (_cursorPos.x > window()->getWidth() - _cursorTexture->getWidth()) ? window()->getWidth() - _cursorTexture->getWidth() : _cursorPos.x;
		_cursorPos.y = (_cursorPos.y > window()->getHeight()) ? window()->getHeight() : _cursorPos.y;
	}

	// update player movement
	_playerMovement = cc::Vec2f::zero();
	if( input()->isKeyDown(ciri::Key::A) ) {
		_playerMovement.x -= 1.0f;
	}
	if( input()->isKeyDown(ciri::Key::D) ) {
		_playerMovement.x += 1.0f;
	}
	if( input()->isKeyDown(ciri::Key::W) ) {
		_playerMovement.y += 1.0f;
	}
	if( input()->isKeyDown(ciri::Key::S) ) {
		_playerMovement.y -= 1.0f;
	}
	if( _playerMovement.sqrMagnitude() > 1.0f ) {
		_playerMovement.normalize();
	}

	const cc::Vec2f& currPlayerPos = _player->getPosition();
	cc::Vec2f newPlayerPos = currPlayerPos;
	if( currPlayerPos.x < 0.0f ) {
		newPlayerPos.x = window()->getWidth();
	} else if( currPlayerPos.x > window()->getWidth() ) {
		newPlayerPos.x = 0.0f;
	}
	if( currPlayerPos.y < 0.0f ) {
		newPlayerPos.y = window()->getHeight();
	} else if( currPlayerPos.y > window()->getHeight() ) {
		newPlayerPos.y = 0.0f;
	}
	_player->setPosition(newPlayerPos);

	_fireTimer += static_cast<float>(deltaTime);

	// firing
	if( (false||input()->isMouseButtonDown(ciri::MouseButton::Left)) && (_fireTimer > FIRE_DELAY) ) {
		_fireTimer = 0.0f;

		const cc::Vec2f mousePos(static_cast<float>(input()->mouseX()), static_cast<float>(window()->getHeight() - input()->mouseY()));
		const cc::Vec2f diff = (mousePos - _player->getPosition()).normalized();

		const float aimAngle = atan2f(diff.y, diff.x);
		const cc::Quatf aimQuat = MathHelper::quatYawPitchRoll(0.0f, 0.0f, aimAngle);

		const float randomSpread = cc::math::randRange<float>(-0.04f, 0.04f) + cc::math::randRange<float>(-0.04f, 0.04f);
		const cc::Vec2f vel = MathHelper::fromPolar(aimAngle + randomSpread, 11.0f);

		const cc::Vec2f offset1 = MathHelper::transformVec2Quat(cc::Vec2f(35.0f, -8.0f), aimQuat);
		addBullet(_player->getPosition() + offset1, vel);

		const cc::Vec2f offset2 = MathHelper::transformVec2Quat(cc::Vec2f(35.0f, 8.0f), aimQuat);
		addBullet(_player->getPosition() + offset2, vel);

		const cc::Vec2f offset3 = MathHelper::transformVec2Quat(cc::Vec2f(60.0f, 0.0f), aimQuat);
		addBullet(_player->getPosition() + offset3, vel);
	}

	// check collision of bullets and enemies
	for( int i = 0; i < static_cast<int>(_bullets.size()); ++i ) {
		if( !_bullets[i].isAlive() ) {
			continue;
		}

		for( int j = 0; j < static_cast<int>(_enemies.size()); ++j ) {
			if( !_enemies[j].isAlive() ) {
				continue;
			}

			if( isColliding(_bullets[i], _enemies[j]) ) {
				_bullets[i].setIsAlive(false);
				_enemies[j].setIsAlive(false);
				break;
			}
		}
	}
}

void SpritesDemo::onFixedUpdate( double deltaTime, double elapsedTime ) {
	_player->update(_playerMovement);

	const ciri::Viewport& vp = graphicsDevice()->getViewport();
	const cc::Vec4f bounds(static_cast<float>(vp.x()), static_cast<float>(vp.y()), static_cast<float>(vp.width()), static_cast<float>(vp.height()));
	for( auto& curr : _bullets ) {
		if( !curr.isAlive() ) {
			continue;
		}
		curr.update(bounds);
		_grid->pull(curr.getPosition().x, curr.getPosition().y, 2, 4);
	}

	const cc::Vec2f screenSize = cc::Vec2f(static_cast<float>(vp.width()), static_cast<float>(vp.height()));
	for( auto& curr : _enemies ) {
		if( !curr.isAlive() ) {
			continue;
		}
		curr.update(screenSize);
	}

	if( _player->getVelocity().sqrMagnitude() > 10.0f ) {
		_psys.setEmitterPosition(_player->getPosition());
		_psys.setEmitDirection(-_player->getVelocity().normalized());
		_psys.emitParticles(1);
	}
	_psys.update(static_cast<float>(deltaTime));

	_grid->updateGrid();
}

void SpritesDemo::onDraw() {
	const std::shared_ptr<ciri::IGraphicsDevice> device = graphicsDevice();
	const ciri::Viewport& vp = device->getViewport();

	device->setClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	device->clear(ciri::ClearFlags::Color | ciri::ClearFlags::Depth);

	_spritebatch.begin(_blendState, _samplerState, _depthStencilState, _rasterizerState, SpriteSortMode::Deferred, nullptr);

	// grid
	_grid->draw(_spritebatch);

	// enemies
	for( auto& curr : _enemies ) {
		if( !curr.isAlive() ) {
			continue;
		}
		_spritebatch.draw(curr.getTexture(), curr.getPosition(), curr.getOrientation(), curr.getOrigin(), 1.0f, 1.0f);
	}

	// player
	_spritebatch.draw(_player->getTexture(), _player->getPosition(), _player->getOrientation(), _player->getOrigin(), 1.0f, 1.0f);

	// bullets
	for( auto& bullet : _bullets ) {
		if( !bullet.isAlive() ) {
			continue;
		}
		_spritebatch.draw(bullet.getTexture(), bullet.getPosition(), bullet.getOrientation(), bullet.getOrigin(), 1.0f, 1.0f);
	}

	// test particle system
	_psys.draw(_spritebatch);
	
	// cursor
	if( _cursorTexture != nullptr ) {
		_spritebatch.draw(_cursorTexture, _cursorPos, 0.0f, _cursorOrigin, 1.0f, 1.0f);
	}

	_spritebatch.end();

	device->present();
}

void SpritesDemo::onUnloadContent() {
	_spritebatch.clean();
	if( _grid != nullptr ) {
		delete _grid;
		_grid = nullptr;
	}
}

void SpritesDemo::addBullet( const cc::Vec2f& position, const cc::Vec2f& velocity ) {
	for( auto& curr : _bullets ) {
		if( curr.isAlive() ) {
			continue;
		}

		curr.setIsAlive(true);
		curr.setPosition(position);
		curr.setVelocity(velocity);
		break;
	}
}

bool SpritesDemo::isColliding( const Entity& a, const Entity& b ) const {
	const float radius = a.getCollisionRadius() + b.getCollisionRadius();
	return a.getPosition().sqrDistance(b.getPosition()) < (radius * radius);
}