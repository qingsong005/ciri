#include <crtdbg.h>
#include <memory>
#include <ciri/Core.hpp>
#include <ciri/Graphics.hpp>
#include "demos/dynvb/DynamicVertexBufferDemo.hpp"
#include "demos/terrain/TerrainDemo.hpp"
#include "demos/sprites/SpritesDemo.hpp"
#include "demos/refract/RefractDemo.hpp"
#include "demos/clipping/ClippingDemo.hpp"
#include "demos/parallax/ParallaxDemo.hpp"
//#include "demos/deferred/DeferredDemo.hpp"
#include "demos\gridlr\Gridlr.hpp"
#include <ciri/Game.hpp>

enum class Demo {
	Dynvb,
	Terrain,
	Sprites,
	Refract,
	Clipping,
	Parallax,
	//Deferred
	Gridlr
};

std::unique_ptr<ciri::App> createGame( Demo type ) {
	switch( type ) {
		case Demo::Dynvb: {
			return std::unique_ptr<ciri::App>(new DynamicVertexBufferDemo());
		}

		case Demo::Terrain:{ 
			return std::unique_ptr<ciri::App>(new TerrainDemo());
		}

		case Demo::Sprites: {
			return std::unique_ptr<ciri::App>(new SpritesDemo());
		}

		case Demo::Refract: {
			return std::unique_ptr<ciri::App>(new RefractDemo());
		}
												
		case Demo::Clipping: {
			return std::unique_ptr<ciri::App>(new ClippingDemo());
		}

		case Demo::Parallax: {
			return std::unique_ptr<ciri::App>(new ParallaxDemo());
		}

		//case Demo::Deferred: {
		//	return std::unique_ptr<IDemo>(new DeferredDemo());
		//}

		case Demo::Gridlr: {
			return std::unique_ptr<ciri::App>(new Gridlr());
		}

		default: {
			return nullptr;
		}
	}
}

int main() {
	// enable memory leak checking
#ifdef _DEBUG
	int debugFlag = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
	debugFlag|= _CRTDBG_LEAK_CHECK_DF;
	debugFlag |= _CRTDBG_CHECK_ALWAYS_DF;
	_CrtSetDbgFlag(debugFlag);
#endif

	// create the game
	std::unique_ptr<ciri::App> game = createGame(Demo::Gridlr);
	if( !game->run() ) {
		printf("ciri error: Game failed to run!\n");
	}

	// clean the game
	game = nullptr;

	return 0;
}