/*
 Cinder-ImGui
 This code is intended for use with the Cinder
 and Omar Cornut ImGui C++ libraries.
 
 http://libcinder.org
 https://github.com/ocornut
 
 Copyright (c) 2013-2015, Simon Geilfus - All rights reserved.
 
 Redistribution and use in source and binary forms, with or without modification, are permitted provided that
 the following conditions are met:
 
 * Redistributions of source code must retain the above copyright notice, this list of conditions and
 the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and
 the following disclaimer in the documentation and/or other materials provided with the distribution.
 
 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
 WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 POSSIBILITY OF SUCH DAMAGE.
 */

#pragma once

#include <memory>

#include "cinder/Vector.h"
#include "cinder/gl/Texture.h"

namespace cinder {
    typedef std::shared_ptr<class DataSource>	DataSourceRef;
    namespace app {
        typedef std::shared_ptr<class Window>   WindowRef;
    }
}

// Custom implicit cast operators
#define IM_VEC2_CLASS_EXTRA                                             \
ImVec2(const glm::vec2& f) { x = f.x; y = f.y; }                        \
operator glm::vec2() const { return glm::vec2(x,y); }                   \
ImVec2(const glm::ivec2& f) { x = f.x; y = f.y; }                       \
operator glm::ivec2() const { return glm::ivec2(x,y); }

#define IM_VEC4_CLASS_EXTRA                                             \
ImVec4(const glm::vec4& f) { x = f.x; y = f.y; z = f.z; w = f.w; }      \
operator glm::vec4() const { return glm::vec4(x,y,z,w); }               \
ImVec4(const ci::ColorA& f) { x = f.r; y = f.g; z = f.b; w = f.a; }     \
operator ci::ColorA() const { return ci::ColorA(x,y,z,w); }             \
ImVec4(const ci::Color& f) { x = f.r; y = f.g; z = f.b; w = 1.0f; }     \
operator ci::Color() const { return ci::Color(x,y,z); }

#include "imgui.h"

namespace ui = ImGui;

//! cinder imgui namespace
namespace ImGui {
    
    struct Options {
        //! defaults to using the current window, the basic ImGui font and the dark theme
        Options();
        
        //! sets the window that will be used to connect the signals and render ImGui
        Options& window( const ci::app::WindowRef &window );
        
        //! sets the font to use in ImGui
        Options& font( const ci::fs::path &fontPath, float size );
        //! sets the list of available fonts to use in ImGui
        Options& fonts( const std::vector<std::pair<ci::fs::path,float>> &fontPaths );
        //! sets the font to use in ImGui
        Options& fontGlyphRanges( const std::string &name, const std::vector<ImWchar> &glyphRanges );
        
        //! Global alpha applies to everything in ImGui
        Options& alpha( float a );
        //! Padding within a window
        Options& windowPadding( const glm::vec2 &padding );
        //! Minimum window size
        Options& windowMinSize( const glm::vec2 &minSize );
        //! Radius of window corners rounding. Set to 0.0f to have rectangular windows
        Options& windowRounding( float rounding );
        //! Padding within a framed rectangle (used by most widgets)
        Options& framePadding( const glm::vec2 &padding );
        //! Radius of frame corners rounding. Set to 0.0f to have rectangular frame (used by most widgets).
        Options& frameRounding( float rounding );
        //! Horizontal and vertical spacing between widgets/lines
        Options& itemSpacing( const glm::vec2 &spacing );
        //! Horizontal and vertical spacing between within elements of a composed widget (e.g. a slider and its label)
        Options& itemInnerSpacing( const glm::vec2 &spacing );
        //! Expand bounding box for touch-based system where touch position is not accurate enough (unnecessary for mouse inputs). Unfortunately we don't sort widgets so priority on overlap will always be given to the first widget running. So dont grow this too much!
        Options& touchExtraPadding( const glm::vec2 &padding );
        //! Extra space after auto-fit (double-clicking on resize grip)
        Options& autoFitPadding( const glm::vec2 &padding );
        //! Default alpha of window background, if not specified in ImGui::Begin()
        Options& windowFillAlphaDefault( float defaultAlpha );
        //! Horizontal spacing when entering a tree node
        Options& treeNodeSpacing( float spacing );
        //! Minimum horizontal spacing between two columns
        Options& columnsMinSpacing( float minSpacing );
        //! Width of the vertical scroll bar
        Options& scrollBarWidth( float width );
        
        //! sets light theme style
        Options& lightTheme();
        //! sets dark theme style
        Options& darkTheme();
        //! sets theme colors
        Options& themeColors( ImVec4 color0, ImVec4 color1, ImVec4 color2, ImVec4 color3, ImVec4 color4 );
        
        //! returns the window that will be use to connect the signals and render ImGui
        ci::app::WindowRef                                  getWindow() const { return mWindow; }
        //! returns the list of available fonts to use in ImGui
        const std::vector<std::pair<ci::fs::path,float>>&   getFonts() const { return mFonts; }
        //! returns the glyph ranges if available for this font
        const ImWchar*                                      getFontGlyphRanges( const std::string &name ) const;
        //! returns the window that will be use to connect the signals and render ImGui
        const ImGuiStyle&                                   getStyle() const { return mStyle; }
        
    protected:
        ImGuiStyle                                  mStyle;
        std::vector<std::pair<ci::fs::path,float>>  mFonts;
        std::map<std::string,std::vector<ImWchar>>  mFontsGlyphRanges;
        ci::app::WindowRef                          mWindow;
    };
    
    //! initializes ImGui and the Renderer
    void    initialize( const Options &options = Options() );
    //! connects window signals to imgui events
    void    connectWindow( ci::app::WindowRef window );
    //! disconnects window signals from imgui
    void    disconnectWindow( ci::app::WindowRef window );
	    
    // Cinder Helpers
    void Image( ci::gl::Texture2dRef texture, const ImVec2& size, const ImVec2& uv0 = ImVec2(0,0), const ImVec2& uv1 = ImVec2(1,1), ImU32 tint_col = 0xFFFFFFFF, ImU32 border_col = 0x00000000);
    void PushFont( const std::string& name = "" );
    
    // Scoped objects goodness (push the state when created and pop it when destroyed)
    struct ScopedWindow : public boost::noncopyable {
        ScopedWindow( const std::string &name = "Debug", bool* opened = NULL, glm::vec2 size = glm::vec2(0), float fillAlpha = -1.0f, ImGuiWindowFlags flags = 0 );
        ~ScopedWindow();
    };
    struct ScopedChild : public boost::noncopyable {
        ScopedChild( const std::string &name, glm::vec2 size = glm::vec2(0), bool border = false, ImGuiWindowFlags extraFlags = 0 );
        ~ScopedChild();
    };
    struct ScopedFont : public boost::noncopyable {
        ScopedFont( const std::string &name );
        ~ScopedFont();
    };
    struct ScopedStyleColor : public boost::noncopyable {
        ScopedStyleColor( ImGuiCol idx, const ImVec4& col );
        ~ScopedStyleColor();
    };
    struct ScopedStyleVar : public boost::noncopyable {
        ScopedStyleVar( ImGuiStyleVar idx, float val );
        ScopedStyleVar( ImGuiStyleVar idx, const ImVec2 &val );
        ~ScopedStyleVar();
    };
    struct ScopedItemWidth : public boost::noncopyable {
        ScopedItemWidth( float itemWidth );
        ~ScopedItemWidth();
    };
    struct ScopedTextWrapPos : public boost::noncopyable {
        ScopedTextWrapPos( float wrapPosX = 0.0f );
        ~ScopedTextWrapPos();
    };
    struct ScopedId : public boost::noncopyable {
        ScopedId( const std::string &name );
        ScopedId( const void *ptrId );
        ScopedId( const int intId );
        ~ScopedId();
    };
}

