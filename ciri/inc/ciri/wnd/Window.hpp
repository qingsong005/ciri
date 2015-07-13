#ifndef __ciri_window__
#define __ciri_window__

#include <queue>
#include <Windows.h>
#include <cc/Vec2.hpp>
#include "WindowEvent.hpp"

namespace ciri {
	class Window{
	public:
		Window();
		~Window();

		bool create( int width, int height );
		bool isOpen() const;
		bool pollEvent( WindowEvent& evt );
		void destroy();

		cc::Vec2ui getSize() const;
		HWND getHandle() const;

	private:
		bool createWindow( int width, int height );
		void processEvents();
		void processEvent( UINT msg, WPARAM wparam, LPARAM lparam );
		void pushEvent( WindowEvent& evt );
		static LRESULT CALLBACK WndProc( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam );

	private:
		HWND _hwnd;
		std::queue<WindowEvent> _events;
		bool _resizing;
		cc::Vec2ui _lastSize;
	};
} // ciri

#endif /* __ciri_window__ */