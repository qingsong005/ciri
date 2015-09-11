#ifndef __ciri_wnd_iwindow__
#define __ciri_wnd_iwindow__

#include <memory>
#include "WindowEvent.hpp"

namespace ciri {
	class IWindow {
	public:
		IWindow() {
		}

		virtual ~IWindow() {
		}

		/**
		 * Creates and opens the underlying window.
		 * 
		 * @param width  Width of the window in pixels.
		 * @param height Height of the window in pixels.
		 * 
		 * @return True on success; false otherwise.
		 */
		virtual bool create( int width, int height )=0;

		/**
		 * Gets if the window is currently open and displayed.
		 * @return True if the window is currently open; false otherwise.
		 */
		virtual bool isOpen() const=0;

		/**
		 * Polls the latest available event.
		 * 
		 * @param evt WindowEvent to populate with the processed event.
		 * @return True if an event was processed; false if no events remain.
		 */
		virtual bool pollEvent( WindowEvent& evt )=0;

		/**
		 * Closes and destroys an open window.
		 * Note that this does not just hide the window; it destroy it.
		 */
		virtual void close()=0;


		/**
		 * Sets the text displayed at the top of the window.
		 * 
		 * @param str Text to set.
		 */
		virtual void setWindowText( const char* str )=0;

		/**
		 * Gets the width of the window in pixels.
		 * @return Width of window in pixels.
		 */
		virtual int getWidth() const=0;

		/**
		 * Gets the height of the window in pixels.
		 * @return Height of window in pixels.
		 */
		virtual int getHeight() const=0;

		/**
		 * Gets if the window currently has foreground focus.
		 * @return True if the window is the foreground window; false otherwise.
		 */
		virtual bool hasFocus() const=0;

		/**
		 * Gets a pointer to the native handle.  This can be cast as appropriate.
		 * For Windows, this is a HWND.
		 * @returns Pointer to native handle.
		 */
		virtual void* getNativeHandle() const=0;
	};

	/**
	 * Creates a new window.
	 * Note that this function is defined in the linked window library, not in ciri itself.
	 * @return Pointer to a new window.
	 */
	std::shared_ptr<IWindow> createWindow(); 
} // ciri

#endif /* __ciri_wnd_iwindow__ */
