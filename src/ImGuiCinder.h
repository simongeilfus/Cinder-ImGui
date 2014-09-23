/* 
 DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE
 Version 2, December 2004
 
 This code is intended for use with the Cinder 
 and Omar Cornut ImGui C++ libraries.
 
 http://libcinder.org
 https://github.com/ocornut
 
 Copyright (C) 2014 Simon Geilfus.
 
 Everyone is permitted to copy and distribute verbatim or modified
 copies of this license document, and changing it is allowed as long
 as the name is changed.
 
 DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE
 TERMS AND CONDITIONS FOR COPYING, DISTRIBUTION AND MODIFICATION
 
 0. You just DO WHAT THE FUCK YOU WANT TO. */

#pragma once

#include <memory>

#include "imgui.h"

#include "cinder/DataSource.h"
#include "cinder/Vector.h"
#include "cinder/app/Window.h"
#include "cinder/gl/Vao.h"
#include "cinder/gl/Vbo.h"
#include "cinder/gl/Texture.h"
#include "cinder/gl/GlslProg.h"


//! cinder imgui namespace
namespace ImGui {
	
	//! sets the right mouseDown IO values in imgui
	void mouseDown( ci::app::MouseEvent& event );
	//! sets the right mouseMove IO values in imgui
	void mouseMove( ci::app::MouseEvent& event );
	//! sets the right mouseDrag IO values in imgui
	void mouseDrag( ci::app::MouseEvent& event );
	//! sets the right mouseUp IO values in imgui
	void mouseUp( ci::app::MouseEvent& event );
	//! sets the right mouseWheel IO values in imgui
	void mouseWheel( ci::app::MouseEvent& event );
	//! sets the right keyDown IO values in imgui
	void keyDown( ci::app::KeyEvent& event );
	//! sets the right keyUp IO values in imgui
	void keyUp( ci::app::KeyEvent& event );
	
	//! sets current window size and events
	void setWindow( ci::app::WindowRef window );
	//! sets current size
	void setSize( ci::ivec2 );
	
	//! connects window signals to imgui events
	void connectSignals( ci::app::WindowRef window );
	
    //! sets light theme style
    void setLightTheme();
    //! sets dark theme style
    void setDarkTheme();
    //! sets custom theme colors
    void setThemeColor( ImVec4 color0, ImVec4 color1, ImVec4 color2, ImVec4 color3, ImVec4 color4 );
    //! sets custom theme colors
    void setThemeColor( ci::ColorA color0, ci::ColorA color1, ci::ColorA color2, ci::ColorA color3, ci::ColorA color4 );
        
    
    //! sets font must be in BMFont/Angelcode format (shortcut for Renderer::setFont)
    void setFont( ci::DataSourceRef font, ci::DataSourceRef texture );
    
    //! cinder renderer
    class Renderer {
    public:
        //! renders imgui drawlist
        static void renderDrawList( ImDrawList** const cmd_lists, int cmd_lists_count );
        //! sets font must be in BMFont/Angelcode format
        static void setFont( ci::DataSourceRef font, ci::DataSourceRef texture );
		
	protected:
        //! initializes and returns the font texture
        static ci::gl::Texture2dRef getFontTextureRef();
        //! initializes and returns the vao
        static ci::gl::VaoRef getVao();
        //! initializes and returns the vbo
        static ci::gl::VboRef getVbo();
        //! initializes and returns the shader
        static ci::gl::GlslProgRef getGlslProg();
		
        //! initalizes the font texture
        static void initFontTexture();
        //! initalizes the vbo mesh
        static void initBuffers( int size = 1000 );
        //! initalizes the shader
        static void initGlslProg();
        
        static ci::gl::Texture2dRef mFontTexture;
        static ci::gl::VaoRef       mVao;
        static ci::gl::VboRef       mVbo;
        static ci::gl::GlslProgRef  mShader;
        
        static ci::Buffer           mFontData;
		
		static bool					mInitialized;
    };

}

