#ifndef __ciri_game_ISpriteFont__
#define __ciri_game_ISpriteFont__

#include <string>
#include <unordered_map>
#include <ciri/core/ErrorCodes.hpp>
#include "SpriteFontGlyph.hpp"

namespace ciri {

class ISpriteFont {
protected:
	ISpriteFont() {
	}

public:
	virtual ~ISpriteFont() {
	}

	/**
		* Loads a font.  For instance, a TTF.
		* 
		* @param file Font file to load.
		* @returns ErrorCode indicating success or failure.
		*/
	virtual ErrorCode loadFromFile( const char* file )=0;

	/**
		* Preloads a string of characters to save lazy-loading.
		* 
		* @param str String of characters to load.
		* @returns ErrorCode indicating success or failure.
		*/
	virtual ErrorCode preloadString( const std::string& str )=0;

	/**
		* Sets the size of the font.
		* 
		* @param size Size in pixels.
		*/
	virtual void setSize( int size )=0;

	/**
		* Gets the size.
		* @return Size of the font in pixels.
		*/
	virtual int getSize() const=0;

	/**
		* Sets the spacing between new lines.
		* 
		* @param spacing Spacing in pixels.
		*/
	virtual void setLineSpacing( int spacing )=0;

	/**
		* Gets the line spacing.
		* @return Spacing between new lines in pixels.
		*/
	virtual int getLineSpacing() const=0;

	/**
		* Measures a given string's width and height in pixels.
		* 
		* @param str String to measure.
		* @param outWidth Width of string in pixels.
		* @param outHeight Height of string in pixels.
		*/
	virtual void measureString( const std::string& str, int* outWidth, int* outHeight ) const=0;

	/**
		* Gets a map of loaded characters and their associated SpriteFontGlyph information.
		* @returns Map of characters to SpriteFontGlyphs.
		*/
	virtual const std::unordered_map<char, SpriteFontGlyph> getLoadedCharacters() const=0;

	/**
		* Gets a specific character's glyph.  If not loaded, should load the glyph and return its result.
		* @param character Character to retrieve the glyph of.
		* @returns SpriteFontGlyph of the given character.
		*/
	virtual const SpriteFontGlyph& getGlyph( char character )=0;

	// change style
	// get texture of character
};

}

#endif