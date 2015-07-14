/*
Cinder-ImGui
This code is intended for use with Cinder
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

#include "CinderImGui.h"

#include "cinder/app/App.h"
#include "cinder/gl/Scoped.h"
#include "cinder/gl/draw.h"
#include "cinder/gl/Context.h"
#include "cinder/gl/Vao.h"
#include "cinder/gl/Vbo.h"
#include "cinder/gl/Texture.h"
#include "cinder/gl/GlslProg.h"
#include "cinder/Clipboard.h"
#include "cinder/CinderAssert.h"
#include "cinder/Log.h"

using namespace std;
using namespace ci;
using namespace ci::app;


namespace ImGui {

// static variables
static bool sInitialized = false;

// same as ImDrawVert
struct RenderData {
    vec2 pos;
    vec2 uv;
    vec4 color;
};

ImGui::Options::Options()
: mWindow( ci::app::getWindow() )
{
    darkTheme();
}

ImGui::Options& ImGui::Options::window( const ci::app::WindowRef &window )
{
    mWindow = window;
    return *this;
}

ImGui::Options& ImGui::Options::font( const ci::fs::path &fontPath, float size )
{
    mFonts = { { fontPath, size } };
    return *this;
}
ImGui::Options& ImGui::Options::fonts( const std::vector<std::pair<ci::fs::path,float>> &fonts )
{
    mFonts = fonts;
    return *this;
}
ImGui::Options& ImGui::Options::fontGlyphRanges( const std::string &name, const vector<ImWchar> &glyphRanges )
{
    mFontsGlyphRanges[name] = glyphRanges;
    return *this;
}
ImGui::Options& ImGui::Options::alpha( float a )
{
    mStyle.Alpha = a;
    return *this;
}
ImGui::Options& ImGui::Options::windowPadding( const glm::vec2 &padding )
{
    mStyle.WindowPadding = padding;
    return *this;
}
ImGui::Options& ImGui::Options::windowMinSize( const glm::vec2 &minSize )
{
    mStyle.WindowMinSize = minSize;
    return *this;
}
ImGui::Options& ImGui::Options::windowRounding( float rounding )
{
    mStyle.WindowRounding = rounding;
    return *this;
}
ImGui::Options& ImGui::Options::windowTitleAlign( ImGuiAlign align )
{
    mStyle.WindowTitleAlign = align;
    return *this;
}
ImGui::Options& ImGui::Options::childWindowRounding( float rounding )
{
    mStyle.ChildWindowRounding = rounding;
    return *this;
}

ImGui::Options& ImGui::Options::framePadding( const glm::vec2 &padding )
{
    mStyle.FramePadding = padding;
    return *this;
}
ImGui::Options& ImGui::Options::frameRounding( float rounding )
{
    mStyle.FrameRounding = rounding;
    return *this;
}
ImGui::Options& ImGui::Options::itemSpacing( const glm::vec2 &spacing )
{
    mStyle.ItemSpacing = spacing;
    return *this;
}
ImGui::Options& ImGui::Options::itemInnerSpacing( const glm::vec2 &spacing )
{
    mStyle.ItemInnerSpacing = spacing;
    return *this;
}
ImGui::Options& ImGui::Options::touchExtraPadding( const glm::vec2 &padding )
{
    mStyle.TouchExtraPadding = padding;
    return *this;
}
ImGui::Options& ImGui::Options::windowFillAlphaDefault( float defaultAlpha )
{
    mStyle.WindowFillAlphaDefault = defaultAlpha;
    return *this;
}
ImGui::Options& ImGui::Options::indentSpacing( float spacing )
{
    mStyle.IndentSpacing = spacing;
    return *this;
}
ImGui::Options& ImGui::Options::columnsMinSpacing( float minSpacing )
{
    mStyle.ColumnsMinSpacing = minSpacing;
    return *this;
}
ImGui::Options& ImGui::Options::scrollBarWidth( float width )
{
    mStyle.ScrollbarWidth = width;
    return *this;
}
ImGui::Options& ImGui::Options::scrollbarRounding( float rounding )
{
	mStyle.ScrollbarRounding = rounding;
	return *this;
}
ImGui::Options& ImGui::Options::grabMinSize( float minSize )
{
	mStyle.GrabMinSize = minSize;
	return *this;
}
/*ImGui::Options& ImGui::Options::grabRounding( float rounding )
{
	mStyle.GrabRounding = rounding;
	return *this;
}*/
ImGui::Options& ImGui::Options::displayWindowPadding( const glm::vec2 &padding )
{
	mStyle.DisplayWindowPadding = padding;
	return *this;
}
ImGui::Options& ImGui::Options::displaySafeAreaPadding( const glm::vec2 &padding )
{
	mStyle.DisplaySafeAreaPadding = padding;
	return *this;
}
	
const ImWchar* ImGui::Options::getFontGlyphRanges( const std::string &name ) const
{
    if( mFontsGlyphRanges.count( name ) )
        return &mFontsGlyphRanges.find(name)->second[0];
    else return NULL;
}

//! cinder renderer
class Renderer {
public:
    Renderer();
    
    //! renders imgui drawlist
    void renderDrawList( ImDrawList** const cmd_lists, int cmd_lists_count );
    //! sets the font
    ImFont* addFont( ci::DataSourceRef font, float size, const ImWchar* glyph_ranges = NULL );
    
    //! initializes and returns the font texture
    ci::gl::Texture2dRef getFontTextureRef();
    //! initializes and returns the vao
    ci::gl::VaoRef getVao();
    //! initializes and returns the vbo
    ci::gl::VboRef getVbo();
    //! initializes and returns the shader
    ci::gl::GlslProgRef getGlslProg();
    
    //! initializes the font texture
    void initFontTexture();
    //! initializes the vbo mesh
    void initBuffers( size_t size = 1000 );
    //! initializes the shader
    void initGlslProg();
    
    ImFont* getFont( const std::string &name );
    
protected:
    ci::gl::Texture2dRef    mFontTexture;
    ci::gl::VaoRef          mVao;
    ci::gl::VboRef          mVbo;
    ci::gl::GlslProgRef     mShader;
    
    map<string,ImFont*>     mFonts;
};

typedef std::shared_ptr<Renderer> RendererRef;
RendererRef getRenderer()
{
    static RendererRef renderer = RendererRef( new Renderer() );
    return renderer;
}

void initialize( const Options &options )
{
    auto renderer                       = getRenderer();
    auto window                         = options.getWindow();
    
    // set style
    const ImGuiStyle& style             = options.getStyle();
    ImGuiStyle& imGuiStyle              = ImGui::GetStyle();
    imGuiStyle.Alpha                    = style.Alpha;
    imGuiStyle.WindowPadding            = style.WindowPadding;
    imGuiStyle.WindowMinSize            = style.WindowMinSize;
    imGuiStyle.FramePadding             = style.FramePadding;
    imGuiStyle.FrameRounding            = style.FrameRounding;
    imGuiStyle.ItemSpacing              = style.ItemSpacing;
    imGuiStyle.ItemInnerSpacing         = style.ItemInnerSpacing;
    imGuiStyle.IndentSpacing            = style.IndentSpacing;
    imGuiStyle.TouchExtraPadding        = style.TouchExtraPadding;
    imGuiStyle.WindowFillAlphaDefault   = style.WindowFillAlphaDefault;
    imGuiStyle.WindowRounding           = style.WindowRounding;
    imGuiStyle.ColumnsMinSpacing        = style.ColumnsMinSpacing;
    imGuiStyle.ScrollbarWidth           = style.ScrollbarWidth;
    
    // set colors
    for( int i = 0; i < ImGuiCol_COUNT; i++ )
        imGuiStyle.Colors[i] = style.Colors[i];
    
    // set io and keymap
    ImGuiIO& io                         = ImGui::GetIO();
    io.DisplaySize                      = ImVec2( window->getSize().x, window->getSize().y );
    io.DeltaTime                        = 1.0f / 60.0f;
    io.KeyMap[ImGuiKey_Tab]             = KeyEvent::KEY_TAB;
    io.KeyMap[ImGuiKey_LeftArrow]       = KeyEvent::KEY_LEFT;
    io.KeyMap[ImGuiKey_RightArrow]      = KeyEvent::KEY_RIGHT;
    io.KeyMap[ImGuiKey_UpArrow]         = KeyEvent::KEY_UP;
    io.KeyMap[ImGuiKey_DownArrow]       = KeyEvent::KEY_DOWN;
    io.KeyMap[ImGuiKey_Home]            = KeyEvent::KEY_HOME;
    io.KeyMap[ImGuiKey_End]             = KeyEvent::KEY_END;
    io.KeyMap[ImGuiKey_Delete]          = KeyEvent::KEY_DELETE;
    io.KeyMap[ImGuiKey_Backspace]       = KeyEvent::KEY_BACKSPACE;
    io.KeyMap[ImGuiKey_Enter]           = KeyEvent::KEY_RETURN;
    io.KeyMap[ImGuiKey_Escape]          = KeyEvent::KEY_ESCAPE;
    io.KeyMap[ImGuiKey_A]               = KeyEvent::KEY_a;
    io.KeyMap[ImGuiKey_C]               = KeyEvent::KEY_c;
    io.KeyMap[ImGuiKey_V]               = KeyEvent::KEY_v;
    io.KeyMap[ImGuiKey_X]               = KeyEvent::KEY_x;
    io.KeyMap[ImGuiKey_Y]               = KeyEvent::KEY_y;
    io.KeyMap[ImGuiKey_Z]               = KeyEvent::KEY_z;
    
    // setup config file path
    string path = ( getAssetPath( "" ) / "imgui.ini" ).string();
    char* pathCStr = new char[ path.size() + 1];
    std::copy( path.begin(), path.end(), pathCStr );
    io.IniFilename = pathCStr;
    
    // setup fonts
    ImFontAtlas* fontAtlas  = ImGui::GetIO().Fonts;
    fontAtlas->Clear();
    for( auto font : options.getFonts() ){
        string name = font.first.stem().string();
        renderer->addFont( loadFile( font.first ), font.second, options.getFontGlyphRanges( name )  );
    }
    renderer->initFontTexture();
    
    // clipboard callbacks
    io.SetClipboardTextFn = []( const char* text ){
        const char* text_end = text + strlen(text);
        char* buf = (char*)malloc(text_end - text + 1);
        memcpy(buf, text, text_end-text);
        buf[text_end-text] = '\0';
        Clipboard::setString( buf );
        free(buf);
    };
	io.GetClipboardTextFn = [](){
		string str = Clipboard::getString();
		static vector<char> strCopy;
		strCopy = vector<char>(str.begin(), str.end());
		strCopy.push_back('\0');
        return (const char *) &strCopy[0];
    };
    
    // renderer callback
    io.RenderDrawListsFn = []( ImDrawList **const cmd_lists, int cmd_lists_count ){
        auto renderer = getRenderer();
        renderer->renderDrawList( cmd_lists, cmd_lists_count );
    };
    
    // connect window's signals
    connectWindow( window );
    ImGui::NewFrame();
    
    sInitialized = true;
}

Options& Options::defaultTheme()
{
	mStyle = ImGuiStyle();
	
    return *this;
}

Options& Options::darkTheme()
{
	mStyle.WindowMinSize            = ImVec2( 160, 20 );
	mStyle.FramePadding             = ImVec2( 4, 2 );
	mStyle.ItemSpacing              = ImVec2( 6, 2 );
	mStyle.ItemInnerSpacing         = ImVec2( 6, 4 );
	mStyle.Alpha					= 0.95f;
	mStyle.WindowFillAlphaDefault   = 1.0f;
	mStyle.WindowRounding           = 4.0f;
	mStyle.FrameRounding            = 2.0f;
	mStyle.IndentSpacing            = 6;
	mStyle.ItemInnerSpacing			= ImVec2( 2, 4 );
	mStyle.ColumnsMinSpacing        = 50;
	mStyle.GrabMinSize				= 2.0f;
	mStyle.ScrollbarWidth           = 12;
	
	ImGuiStyle& style = mStyle;
	style.Colors[ImGuiCol_Text]                  = ImVec4(0.86f, 0.93f, 0.89f, 0.61f);
	style.Colors[ImGuiCol_TextDisabled]          = ImVec4(0.86f, 0.93f, 0.89f, 0.28f);
	style.Colors[ImGuiCol_WindowBg]              = ImVec4(0.13f, 0.14f, 0.17f, 1.00f);
	style.Colors[ImGuiCol_ChildWindowBg]         = ImVec4(0.20f, 0.22f, 0.27f, 0.58f);
	style.Colors[ImGuiCol_Border]                = ImVec4(0.31f, 0.31f, 1.00f, 0.00f);
	style.Colors[ImGuiCol_BorderShadow]          = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	style.Colors[ImGuiCol_FrameBg]               = ImVec4(0.20f, 0.22f, 0.27f, 1.00f);
	style.Colors[ImGuiCol_FrameBgHovered]        = ImVec4(0.92f, 0.18f, 0.29f, 0.78f);
	style.Colors[ImGuiCol_FrameBgActive]         = ImVec4(0.92f, 0.18f, 0.29f, 1.00f);
	style.Colors[ImGuiCol_TitleBg]               = ImVec4(0.20f, 0.22f, 0.27f, 1.00f);
	style.Colors[ImGuiCol_TitleBgCollapsed]      = ImVec4(0.20f, 0.22f, 0.27f, 0.75f);
	style.Colors[ImGuiCol_TitleBgActive]         = ImVec4(0.92f, 0.18f, 0.29f, 1.00f);
	style.Colors[ImGuiCol_MenuBarBg]             = ImVec4(0.20f, 0.22f, 0.27f, 0.47f);
	style.Colors[ImGuiCol_ScrollbarBg]           = ImVec4(0.20f, 0.22f, 0.27f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarGrab]         = ImVec4(0.47f, 0.77f, 0.83f, 0.21f);
	style.Colors[ImGuiCol_ScrollbarGrabHovered]  = ImVec4(0.92f, 0.18f, 0.29f, 0.78f);
	style.Colors[ImGuiCol_ScrollbarGrabActive]   = ImVec4(0.92f, 0.18f, 0.29f, 1.00f);
	style.Colors[ImGuiCol_ComboBg]               = ImVec4(0.20f, 0.22f, 0.27f, 1.00f);
	style.Colors[ImGuiCol_CheckMark]             = ImVec4(0.71f, 0.22f, 0.27f, 1.00f);
	style.Colors[ImGuiCol_SliderGrab]            = ImVec4(0.47f, 0.77f, 0.83f, 0.14f);
	style.Colors[ImGuiCol_SliderGrabActive]      = ImVec4(0.92f, 0.18f, 0.29f, 1.00f);
	style.Colors[ImGuiCol_Button]                = ImVec4(0.92f, 0.18f, 0.29f, 0.60f);
	style.Colors[ImGuiCol_ButtonHovered]         = ImVec4(0.92f, 0.18f, 0.29f, 0.86f);
	style.Colors[ImGuiCol_ButtonActive]          = ImVec4(0.92f, 0.18f, 0.29f, 1.00f);
	style.Colors[ImGuiCol_Header]                = ImVec4(0.92f, 0.18f, 0.29f, 0.76f);
	style.Colors[ImGuiCol_HeaderHovered]         = ImVec4(0.92f, 0.18f, 0.29f, 0.86f);
	style.Colors[ImGuiCol_HeaderActive]          = ImVec4(0.92f, 0.18f, 0.29f, 1.00f);
	style.Colors[ImGuiCol_Column]                = ImVec4(0.47f, 0.77f, 0.83f, 0.32f);
	style.Colors[ImGuiCol_ColumnHovered]         = ImVec4(0.92f, 0.18f, 0.29f, 0.78f);
	style.Colors[ImGuiCol_ColumnActive]          = ImVec4(0.92f, 0.18f, 0.29f, 1.00f);
	style.Colors[ImGuiCol_ResizeGrip]            = ImVec4(0.47f, 0.77f, 0.83f, 0.32f);
	style.Colors[ImGuiCol_ResizeGripHovered]     = ImVec4(0.92f, 0.18f, 0.29f, 0.78f);
	style.Colors[ImGuiCol_ResizeGripActive]      = ImVec4(0.92f, 0.18f, 0.29f, 1.00f);
	style.Colors[ImGuiCol_CloseButton]           = ImVec4(0.86f, 0.93f, 0.89f, 0.16f);
	style.Colors[ImGuiCol_CloseButtonHovered]    = ImVec4(0.86f, 0.93f, 0.89f, 0.39f);
	style.Colors[ImGuiCol_CloseButtonActive]     = ImVec4(0.86f, 0.93f, 0.89f, 1.00f);
	style.Colors[ImGuiCol_PlotLines]             = ImVec4(0.86f, 0.93f, 0.89f, 0.63f);
	style.Colors[ImGuiCol_PlotLinesHovered]      = ImVec4(0.92f, 0.18f, 0.29f, 1.00f);
	style.Colors[ImGuiCol_PlotHistogram]         = ImVec4(0.86f, 0.93f, 0.89f, 0.63f);
	style.Colors[ImGuiCol_PlotHistogramHovered]  = ImVec4(0.92f, 0.18f, 0.29f, 1.00f);
	style.Colors[ImGuiCol_TextSelectedBg]        = ImVec4(0.92f, 0.18f, 0.29f, 0.43f);
	style.Colors[ImGuiCol_TooltipBg]             = ImVec4(0.47f, 0.77f, 0.83f, 0.72f);
	style.Colors[ImGuiCol_ModalWindowDarkening]  = ImVec4(0.20f, 0.22f, 0.27f, 0.73f);
	
/* Dark
	 
	 mStyle.WindowPadding            = ImVec2( 20, 10 );
    mStyle.WindowMinSize            = ImVec2( 160, 20 );
    mStyle.FramePadding             = ImVec2( 4, 4 );
    mStyle.ItemSpacing              = ImVec2( 8, 4 );
	mStyle.ItemInnerSpacing         = ImVec2( 6, 4 );
	mStyle.Alpha					= 0.95f;
	mStyle.WindowFillAlphaDefault   = 0.95f;
    mStyle.WindowRounding           = 2.0f;
    mStyle.FrameRounding            = 2.0f;
    mStyle.IndentSpacing            = 6;
    mStyle.ColumnsMinSpacing        = 50;
    mStyle.ScrollbarWidth           = 12;
	
	ImGuiStyle& style = mStyle;
	style.Colors[ImGuiCol_Text]                  = ImVec4(1.00f, 1.00f, 0.86f, 0.50f);
	style.Colors[ImGuiCol_TextDisabled]          = ImVec4(1.00f, 1.00f, 0.86f, 0.22f);
	style.Colors[ImGuiCol_WindowBg]              = ImVec4(0.18f, 0.18f, 0.18f, 1.00f);
	style.Colors[ImGuiCol_ChildWindowBg]         = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
	style.Colors[ImGuiCol_Border]                = ImVec4(0.31f, 0.31f, 1.00f, 0.00f);
	style.Colors[ImGuiCol_BorderShadow]          = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	style.Colors[ImGuiCol_FrameBg]               = ImVec4(0.22f, 0.22f, 0.22f, 1.00f);
	style.Colors[ImGuiCol_FrameBgHovered]        = ImVec4(0.27f, 0.27f, 0.27f, 1.00f);
	style.Colors[ImGuiCol_FrameBgActive]         = ImVec4(0.36f, 0.36f, 0.36f, 1.00f);
	style.Colors[ImGuiCol_TitleBg]               = ImVec4(0.23f, 0.23f, 0.23f, 1.00f);
	style.Colors[ImGuiCol_TitleBgCollapsed]      = ImVec4(0.24f, 0.24f, 0.24f, 1.00f);
	style.Colors[ImGuiCol_TitleBgActive]         = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
	style.Colors[ImGuiCol_MenuBarBg]             = ImVec4(0.97f, 0.48f, 0.32f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarBg]           = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarGrab]         = ImVec4(0.33f, 0.33f, 0.33f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarGrabHovered]  = ImVec4(0.26f, 0.26f, 0.26f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarGrabActive]   = ImVec4(0.36f, 0.36f, 0.36f, 1.00f);
	style.Colors[ImGuiCol_ComboBg]               = ImVec4(0.29f, 0.29f, 0.29f, 1.00f);
	style.Colors[ImGuiCol_CheckMark]             = ImVec4(0.22f, 0.22f, 0.22f, 0.50f);
	style.Colors[ImGuiCol_SliderGrab]            = ImVec4(0.23f, 0.23f, 0.23f, 1.00f);
	style.Colors[ImGuiCol_SliderGrabActive]      = ImVec4(0.38f, 0.38f, 0.38f, 1.00f);
	style.Colors[ImGuiCol_Button]                = ImVec4(0.32f, 0.32f, 0.32f, 1.00f);
	style.Colors[ImGuiCol_ButtonHovered]         = ImVec4(0.45f, 0.45f, 0.45f, 1.00f);
	style.Colors[ImGuiCol_ButtonActive]          = ImVec4(1.00f, 1.00f, 0.62f, 1.00f);
	style.Colors[ImGuiCol_Header]                = ImVec4(0.34f, 0.34f, 0.34f, 1.00f);
	style.Colors[ImGuiCol_HeaderHovered]         = ImVec4(0.22f, 0.22f, 0.22f, 1.00f);
	style.Colors[ImGuiCol_HeaderActive]          = ImVec4(0.27f, 0.27f, 0.27f, 1.00f);
	style.Colors[ImGuiCol_Column]                = ImVec4(0.38f, 0.38f, 0.38f, 1.00f);
	style.Colors[ImGuiCol_ColumnHovered]         = ImVec4(0.56f, 0.56f, 0.56f, 1.00f);
	style.Colors[ImGuiCol_ColumnActive]          = ImVec4(0.68f, 0.68f, 0.68f, 1.00f);
	style.Colors[ImGuiCol_ResizeGrip]            = ImVec4(0.38f, 0.38f, 0.38f, 1.00f);
	style.Colors[ImGuiCol_ResizeGripHovered]     = ImVec4(0.49f, 0.49f, 0.49f, 1.00f);
	style.Colors[ImGuiCol_ResizeGripActive]      = ImVec4(0.65f, 0.65f, 0.65f, 1.00f);
	style.Colors[ImGuiCol_CloseButton]           = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
	style.Colors[ImGuiCol_CloseButtonHovered]    = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
	style.Colors[ImGuiCol_CloseButtonActive]     = ImVec4(0.49f, 0.49f, 0.49f, 1.00f);
	style.Colors[ImGuiCol_PlotLines]             = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
	style.Colors[ImGuiCol_PlotLinesHovered]      = ImVec4(0.71f, 0.71f, 0.71f, 1.00f);
	style.Colors[ImGuiCol_PlotHistogram]         = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
	style.Colors[ImGuiCol_PlotHistogramHovered]  = ImVec4(0.49f, 0.49f, 0.49f, 1.00f);
	style.Colors[ImGuiCol_TextSelectedBg]        = ImVec4(0.35f, 0.35f, 0.35f, 1.00f);
	style.Colors[ImGuiCol_TooltipBg]             = ImVec4(0.16f, 0.16f, 0.16f, 0.72f);
	style.Colors[ImGuiCol_ModalWindowDarkening]  = ImVec4(0.20f, 0.20f, 0.20f, 0.65f);*/
	
	/* Flat UI Theme https://color.adobe.com/Flat-UI-color-theme-2469224
	ImGuiStyle& style = ImGui::GetStyle();
	style.Colors[ImGuiCol_Text]                  = ImVec4(0.93f, 0.94f, 0.95f, 0.61f);
	style.Colors[ImGuiCol_TextDisabled]          = ImVec4(1.00f, 1.00f, 0.86f, 0.22f);
	style.Colors[ImGuiCol_WindowBg]              = ImVec4(0.16f, 0.50f, 0.73f, 1.00f);
	style.Colors[ImGuiCol_ChildWindowBg]         = ImVec4(0.16f, 0.50f, 0.73f, 1.00f);
	style.Colors[ImGuiCol_Border]                = ImVec4(0.31f, 0.31f, 1.00f, 0.00f);
	style.Colors[ImGuiCol_BorderShadow]          = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	style.Colors[ImGuiCol_FrameBg]               = ImVec4(0.20f, 0.60f, 0.86f, 1.00f);
	style.Colors[ImGuiCol_FrameBgHovered]        = ImVec4(0.91f, 0.30f, 0.24f, 0.78f);
	style.Colors[ImGuiCol_FrameBgActive]         = ImVec4(0.91f, 0.30f, 0.24f, 1.00f);
	style.Colors[ImGuiCol_TitleBg]               = ImVec4(0.16f, 0.50f, 0.73f, 1.00f);
	style.Colors[ImGuiCol_TitleBgCollapsed]      = ImVec4(0.16f, 0.50f, 0.73f, 0.75f);
	style.Colors[ImGuiCol_TitleBgActive]         = ImVec4(0.91f, 0.30f, 0.24f, 1.00f);
	style.Colors[ImGuiCol_MenuBarBg]             = ImVec4(0.16f, 0.50f, 0.73f, 0.47f);
	style.Colors[ImGuiCol_ScrollbarBg]           = ImVec4(0.20f, 0.60f, 0.86f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarGrab]         = ImVec4(0.16f, 0.50f, 0.73f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarGrabHovered]  = ImVec4(0.91f, 0.30f, 0.24f, 0.78f);
	style.Colors[ImGuiCol_ScrollbarGrabActive]   = ImVec4(0.91f, 0.30f, 0.24f, 1.00f);
	style.Colors[ImGuiCol_ComboBg]               = ImVec4(0.20f, 0.60f, 0.86f, 1.00f);
	style.Colors[ImGuiCol_CheckMark]             = ImVec4(0.16f, 0.50f, 0.73f, 1.00f);
	style.Colors[ImGuiCol_SliderGrab]            = ImVec4(0.93f, 0.94f, 0.95f, 1.00f);
	style.Colors[ImGuiCol_SliderGrabActive]      = ImVec4(0.91f, 0.30f, 0.24f, 1.00f);
	style.Colors[ImGuiCol_Button]                = ImVec4(0.91f, 0.30f, 0.24f, 0.76f);
	style.Colors[ImGuiCol_ButtonHovered]         = ImVec4(0.91f, 0.30f, 0.24f, 0.86f);
	style.Colors[ImGuiCol_ButtonActive]          = ImVec4(0.91f, 0.30f, 0.24f, 1.00f);
	style.Colors[ImGuiCol_Header]                = ImVec4(0.91f, 0.30f, 0.24f, 0.76f);
	style.Colors[ImGuiCol_HeaderHovered]         = ImVec4(0.91f, 0.30f, 0.24f, 0.86f);
	style.Colors[ImGuiCol_HeaderActive]          = ImVec4(0.91f, 0.30f, 0.24f, 1.00f);
	style.Colors[ImGuiCol_Column]                = ImVec4(0.17f, 0.24f, 0.31f, 0.32f);
	style.Colors[ImGuiCol_ColumnHovered]         = ImVec4(0.91f, 0.30f, 0.24f, 0.78f);
	style.Colors[ImGuiCol_ColumnActive]          = ImVec4(0.91f, 0.30f, 0.24f, 1.00f);
	style.Colors[ImGuiCol_ResizeGrip]            = ImVec4(0.17f, 0.24f, 0.31f, 0.32f);
	style.Colors[ImGuiCol_ResizeGripHovered]     = ImVec4(0.91f, 0.30f, 0.24f, 0.78f);
	style.Colors[ImGuiCol_ResizeGripActive]      = ImVec4(0.91f, 0.30f, 0.24f, 1.00f);
	style.Colors[ImGuiCol_CloseButton]           = ImVec4(0.93f, 0.94f, 0.95f, 0.16f);
	style.Colors[ImGuiCol_CloseButtonHovered]    = ImVec4(0.93f, 0.94f, 0.95f, 0.39f);
	style.Colors[ImGuiCol_CloseButtonActive]     = ImVec4(0.93f, 0.94f, 0.95f, 1.00f);
	style.Colors[ImGuiCol_PlotLines]             = ImVec4(0.93f, 0.94f, 0.95f, 1.00f);
	style.Colors[ImGuiCol_PlotLinesHovered]      = ImVec4(0.91f, 0.30f, 0.24f, 1.00f);
	style.Colors[ImGuiCol_PlotHistogram]         = ImVec4(0.93f, 0.94f, 0.95f, 1.00f);
	style.Colors[ImGuiCol_PlotHistogramHovered]  = ImVec4(0.91f, 0.30f, 0.24f, 1.00f);
	style.Colors[ImGuiCol_TextSelectedBg]        = ImVec4(0.91f, 0.30f, 0.31f, 1.00f);
	style.Colors[ImGuiCol_TooltipBg]             = ImVec4(0.17f, 0.24f, 0.31f, 0.72f);
	style.Colors[ImGuiCol_ModalWindowDarkening]  = ImVec4(0.17f, 0.24f, 0.31f, 0.65f);
	*/
	
	/* Sea Wolf https://color.adobe.com/Sea-Wolf-color-theme-782171
	 ImGuiStyle& style = ImGui::GetStyle();
	 style.Colors[ImGuiCol_Text]                  = ImVec4(0.85f, 0.80f, 0.62f, 0.61f);
	 style.Colors[ImGuiCol_TextDisabled]          = ImVec4(0.85f, 0.80f, 0.62f, 0.22f);
	 style.Colors[ImGuiCol_WindowBg]              = ImVec4(0.16f, 0.17f, 0.17f, 1.00f);
	 style.Colors[ImGuiCol_ChildWindowBg]         = ImVec4(0.16f, 0.17f, 0.17f, 1.00f);
	 style.Colors[ImGuiCol_Border]                = ImVec4(0.31f, 0.31f, 1.00f, 0.00f);
	 style.Colors[ImGuiCol_BorderShadow]          = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	 style.Colors[ImGuiCol_FrameBg]               = ImVec4(0.22f, 0.25f, 0.25f, 1.00f);
	 style.Colors[ImGuiCol_FrameBgHovered]        = ImVec4(0.86f, 0.21f, 0.13f, 0.78f);
	 style.Colors[ImGuiCol_FrameBgActive]         = ImVec4(0.86f, 0.21f, 0.13f, 1.00f);
	 style.Colors[ImGuiCol_TitleBg]               = ImVec4(0.16f, 0.17f, 0.17f, 1.00f);
	 style.Colors[ImGuiCol_TitleBgCollapsed]      = ImVec4(0.16f, 0.17f, 0.17f, 0.75f);
	 style.Colors[ImGuiCol_TitleBgActive]         = ImVec4(0.86f, 0.21f, 0.13f, 1.00f);
	 style.Colors[ImGuiCol_MenuBarBg]             = ImVec4(0.16f, 0.17f, 0.17f, 0.47f);
	 style.Colors[ImGuiCol_ScrollbarBg]           = ImVec4(0.22f, 0.25f, 0.25f, 1.00f);
	 style.Colors[ImGuiCol_ScrollbarGrab]         = ImVec4(0.16f, 0.17f, 0.17f, 1.00f);
	 style.Colors[ImGuiCol_ScrollbarGrabHovered]  = ImVec4(0.86f, 0.21f, 0.13f, 0.78f);
	 style.Colors[ImGuiCol_ScrollbarGrabActive]   = ImVec4(0.86f, 0.21f, 0.13f, 1.00f);
	 style.Colors[ImGuiCol_ComboBg]               = ImVec4(0.22f, 0.25f, 0.25f, 1.00f);
	 style.Colors[ImGuiCol_CheckMark]             = ImVec4(0.16f, 0.17f, 0.17f, 1.00f);
	 style.Colors[ImGuiCol_SliderGrab]            = ImVec4(0.85f, 0.80f, 0.62f, 1.00f);
	 style.Colors[ImGuiCol_SliderGrabActive]      = ImVec4(0.86f, 0.21f, 0.13f, 1.00f);
	 style.Colors[ImGuiCol_Button]                = ImVec4(0.86f, 0.21f, 0.13f, 0.76f);
	 style.Colors[ImGuiCol_ButtonHovered]         = ImVec4(0.86f, 0.21f, 0.13f, 0.86f);
	 style.Colors[ImGuiCol_ButtonActive]          = ImVec4(0.86f, 0.21f, 0.13f, 1.00f);
	 style.Colors[ImGuiCol_Header]                = ImVec4(0.86f, 0.21f, 0.13f, 0.76f);
	 style.Colors[ImGuiCol_HeaderHovered]         = ImVec4(0.86f, 0.21f, 0.13f, 0.86f);
	 style.Colors[ImGuiCol_HeaderActive]          = ImVec4(0.86f, 0.21f, 0.13f, 1.00f);
	 style.Colors[ImGuiCol_Column]                = ImVec4(0.12f, 0.12f, 0.13f, 0.32f);
	 style.Colors[ImGuiCol_ColumnHovered]         = ImVec4(0.86f, 0.21f, 0.13f, 0.78f);
	 style.Colors[ImGuiCol_ColumnActive]          = ImVec4(0.86f, 0.21f, 0.13f, 1.00f);
	 style.Colors[ImGuiCol_ResizeGrip]            = ImVec4(0.12f, 0.12f, 0.13f, 0.32f);
	 style.Colors[ImGuiCol_ResizeGripHovered]     = ImVec4(0.86f, 0.21f, 0.13f, 0.78f);
	 style.Colors[ImGuiCol_ResizeGripActive]      = ImVec4(0.86f, 0.21f, 0.13f, 1.00f);
	 style.Colors[ImGuiCol_CloseButton]           = ImVec4(0.85f, 0.80f, 0.62f, 0.16f);
	 style.Colors[ImGuiCol_CloseButtonHovered]    = ImVec4(0.85f, 0.80f, 0.62f, 0.39f);
	 style.Colors[ImGuiCol_CloseButtonActive]     = ImVec4(0.85f, 0.80f, 0.62f, 1.00f);
	 style.Colors[ImGuiCol_PlotLines]             = ImVec4(0.85f, 0.80f, 0.62f, 1.00f);
	 style.Colors[ImGuiCol_PlotLinesHovered]      = ImVec4(0.86f, 0.21f, 0.13f, 1.00f);
	 style.Colors[ImGuiCol_PlotHistogram]         = ImVec4(0.85f, 0.80f, 0.62f, 1.00f);
	 style.Colors[ImGuiCol_PlotHistogramHovered]  = ImVec4(0.86f, 0.21f, 0.13f, 1.00f);
	 style.Colors[ImGuiCol_TextSelectedBg]        = ImVec4(0.86f, 0.21f, 0.13f, 1.00f);
	 style.Colors[ImGuiCol_TooltipBg]             = ImVec4(0.12f, 0.12f, 0.13f, 0.72f);
	 style.Colors[ImGuiCol_ModalWindowDarkening]  = ImVec4(0.12f, 0.12f, 0.13f, 0.65f);
	 */
	
	/* https://color.adobe.com/Cinderella-from-Nam-color-theme-2071015/edit/?copy=true
	 ImGuiStyle& style = ImGui::GetStyle();
	 style.Colors[ImGuiCol_Text]                  = ImVec4(0.95f, 0.90f, 0.64f, 0.61f);
	 style.Colors[ImGuiCol_TextDisabled]          = ImVec4(0.95f, 0.90f, 0.64f, 0.22f);
	 style.Colors[ImGuiCol_WindowBg]              = ImVec4(0.26f, 0.35f, 0.34f, 1.00f);
	 style.Colors[ImGuiCol_ChildWindowBg]         = ImVec4(0.26f, 0.35f, 0.34f, 1.00f);
	 style.Colors[ImGuiCol_Border]                = ImVec4(0.31f, 0.31f, 1.00f, 0.00f);
	 style.Colors[ImGuiCol_BorderShadow]          = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	 style.Colors[ImGuiCol_FrameBg]               = ImVec4(0.51f, 0.67f, 0.55f, 1.00f);
	 style.Colors[ImGuiCol_FrameBgHovered]        = ImVec4(0.85f, 0.40f, 0.40f, 0.78f);
	 style.Colors[ImGuiCol_FrameBgActive]         = ImVec4(0.85f, 0.40f, 0.40f, 1.00f);
	 style.Colors[ImGuiCol_TitleBg]               = ImVec4(0.26f, 0.35f, 0.34f, 1.00f);
	 style.Colors[ImGuiCol_TitleBgCollapsed]      = ImVec4(0.26f, 0.35f, 0.34f, 0.75f);
	 style.Colors[ImGuiCol_TitleBgActive]         = ImVec4(0.85f, 0.40f, 0.40f, 1.00f);
	 style.Colors[ImGuiCol_MenuBarBg]             = ImVec4(0.26f, 0.35f, 0.34f, 0.47f);
	 style.Colors[ImGuiCol_ScrollbarBg]           = ImVec4(0.51f, 0.67f, 0.55f, 1.00f);
	 style.Colors[ImGuiCol_ScrollbarGrab]         = ImVec4(0.26f, 0.35f, 0.34f, 1.00f);
	 style.Colors[ImGuiCol_ScrollbarGrabHovered]  = ImVec4(0.85f, 0.40f, 0.40f, 0.78f);
	 style.Colors[ImGuiCol_ScrollbarGrabActive]   = ImVec4(0.85f, 0.40f, 0.40f, 1.00f);
	 style.Colors[ImGuiCol_ComboBg]               = ImVec4(0.51f, 0.67f, 0.55f, 1.00f);
	 style.Colors[ImGuiCol_CheckMark]             = ImVec4(0.26f, 0.35f, 0.34f, 1.00f);
	 style.Colors[ImGuiCol_SliderGrab]            = ImVec4(0.95f, 0.90f, 0.64f, 1.00f);
	 style.Colors[ImGuiCol_SliderGrabActive]      = ImVec4(0.85f, 0.40f, 0.40f, 1.00f);
	 style.Colors[ImGuiCol_Button]                = ImVec4(0.85f, 0.40f, 0.40f, 0.76f);
	 style.Colors[ImGuiCol_ButtonHovered]         = ImVec4(0.85f, 0.40f, 0.40f, 0.86f);
	 style.Colors[ImGuiCol_ButtonActive]          = ImVec4(0.85f, 0.40f, 0.40f, 1.00f);
	 style.Colors[ImGuiCol_Header]                = ImVec4(0.85f, 0.40f, 0.40f, 0.76f);
	 style.Colors[ImGuiCol_HeaderHovered]         = ImVec4(0.85f, 0.40f, 0.40f, 0.86f);
	 style.Colors[ImGuiCol_HeaderActive]          = ImVec4(0.85f, 0.40f, 0.40f, 1.00f);
	 style.Colors[ImGuiCol_Column]                = ImVec4(0.97f, 0.60f, 0.51f, 0.32f);
	 style.Colors[ImGuiCol_ColumnHovered]         = ImVec4(0.85f, 0.40f, 0.40f, 0.78f);
	 style.Colors[ImGuiCol_ColumnActive]          = ImVec4(0.85f, 0.40f, 0.40f, 1.00f);
	 style.Colors[ImGuiCol_ResizeGrip]            = ImVec4(0.97f, 0.60f, 0.51f, 0.32f);
	 style.Colors[ImGuiCol_ResizeGripHovered]     = ImVec4(0.85f, 0.40f, 0.40f, 0.78f);
	 style.Colors[ImGuiCol_ResizeGripActive]      = ImVec4(0.85f, 0.40f, 0.40f, 1.00f);
	 style.Colors[ImGuiCol_CloseButton]           = ImVec4(0.95f, 0.90f, 0.64f, 0.16f);
	 style.Colors[ImGuiCol_CloseButtonHovered]    = ImVec4(0.95f, 0.90f, 0.64f, 0.39f);
	 style.Colors[ImGuiCol_CloseButtonActive]     = ImVec4(0.95f, 0.90f, 0.64f, 1.00f);
	 style.Colors[ImGuiCol_PlotLines]             = ImVec4(0.95f, 0.90f, 0.64f, 1.00f);
	 style.Colors[ImGuiCol_PlotLinesHovered]      = ImVec4(0.85f, 0.40f, 0.40f, 1.00f);
	 style.Colors[ImGuiCol_PlotHistogram]         = ImVec4(0.95f, 0.90f, 0.64f, 1.00f);
	 style.Colors[ImGuiCol_PlotHistogramHovered]  = ImVec4(0.85f, 0.40f, 0.40f, 1.00f);
	 style.Colors[ImGuiCol_TextSelectedBg]        = ImVec4(0.85f, 0.40f, 0.40f, 1.00f);
	 style.Colors[ImGuiCol_TooltipBg]             = ImVec4(0.97f, 0.60f, 0.51f, 0.72f);
	 style.Colors[ImGuiCol_ModalWindowDarkening]  = ImVec4(0.97f, 0.60f, 0.51f, 0.65f);
	*/

	/* CS04 Theme https://color.adobe.com/CS04-color-theme-1994456
	 ImGuiStyle& style = ImGui::GetStyle();
	 style.Colors[ImGuiCol_Text]                  = ImVec4(0.86f, 0.93f, 0.89f, 0.61f);
	 style.Colors[ImGuiCol_TextDisabled]          = ImVec4(0.86f, 0.93f, 0.89f, 0.22f);
	 style.Colors[ImGuiCol_WindowBg]              = ImVec4(0.20f, 0.22f, 0.27f, 1.00f);
	 style.Colors[ImGuiCol_ChildWindowBg]         = ImVec4(0.20f, 0.22f, 0.27f, 1.00f);
	 style.Colors[ImGuiCol_Border]                = ImVec4(0.31f, 0.31f, 1.00f, 0.00f);
	 style.Colors[ImGuiCol_BorderShadow]          = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	 style.Colors[ImGuiCol_FrameBg]               = ImVec4(0.20f, 0.22f, 0.27f, 1.00f);
	 style.Colors[ImGuiCol_FrameBgHovered]        = ImVec4(0.92f, 0.18f, 0.29f, 0.78f);
	 style.Colors[ImGuiCol_FrameBgActive]         = ImVec4(0.92f, 0.18f, 0.29f, 1.00f);
	 style.Colors[ImGuiCol_TitleBg]               = ImVec4(0.20f, 0.22f, 0.27f, 1.00f);
	 style.Colors[ImGuiCol_TitleBgCollapsed]      = ImVec4(0.20f, 0.22f, 0.27f, 0.75f);
	 style.Colors[ImGuiCol_TitleBgActive]         = ImVec4(0.92f, 0.18f, 0.29f, 1.00f);
	 style.Colors[ImGuiCol_MenuBarBg]             = ImVec4(0.20f, 0.22f, 0.27f, 0.47f);
	 style.Colors[ImGuiCol_ScrollbarBg]           = ImVec4(0.20f, 0.22f, 0.27f, 1.00f);
	 style.Colors[ImGuiCol_ScrollbarGrab]         = ImVec4(0.20f, 0.22f, 0.27f, 1.00f);
	 style.Colors[ImGuiCol_ScrollbarGrabHovered]  = ImVec4(0.92f, 0.18f, 0.29f, 0.78f);
	 style.Colors[ImGuiCol_ScrollbarGrabActive]   = ImVec4(0.92f, 0.18f, 0.29f, 1.00f);
	 style.Colors[ImGuiCol_ComboBg]               = ImVec4(0.20f, 0.22f, 0.27f, 1.00f);
	 style.Colors[ImGuiCol_CheckMark]             = ImVec4(0.71f, 0.22f, 0.27f, 1.00f);
	 style.Colors[ImGuiCol_SliderGrab]            = ImVec4(0.47f, 0.77f, 0.83f, 0.44f);
	 style.Colors[ImGuiCol_SliderGrabActive]      = ImVec4(0.92f, 0.18f, 0.29f, 1.00f);
	 style.Colors[ImGuiCol_Button]                = ImVec4(0.92f, 0.18f, 0.29f, 0.76f);
	 style.Colors[ImGuiCol_ButtonHovered]         = ImVec4(0.92f, 0.18f, 0.29f, 0.86f);
	 style.Colors[ImGuiCol_ButtonActive]          = ImVec4(0.92f, 0.18f, 0.29f, 1.00f);
	 style.Colors[ImGuiCol_Header]                = ImVec4(0.92f, 0.18f, 0.29f, 0.76f);
	 style.Colors[ImGuiCol_HeaderHovered]         = ImVec4(0.92f, 0.18f, 0.29f, 0.86f);
	 style.Colors[ImGuiCol_HeaderActive]          = ImVec4(0.92f, 0.18f, 0.29f, 1.00f);
	 style.Colors[ImGuiCol_Column]                = ImVec4(0.47f, 0.77f, 0.83f, 0.32f);
	 style.Colors[ImGuiCol_ColumnHovered]         = ImVec4(0.92f, 0.18f, 0.29f, 0.78f);
	 style.Colors[ImGuiCol_ColumnActive]          = ImVec4(0.92f, 0.18f, 0.29f, 1.00f);
	 style.Colors[ImGuiCol_ResizeGrip]            = ImVec4(0.47f, 0.77f, 0.83f, 0.32f);
	 style.Colors[ImGuiCol_ResizeGripHovered]     = ImVec4(0.92f, 0.18f, 0.29f, 0.78f);
	 style.Colors[ImGuiCol_ResizeGripActive]      = ImVec4(0.92f, 0.18f, 0.29f, 1.00f);
	 style.Colors[ImGuiCol_CloseButton]           = ImVec4(0.86f, 0.93f, 0.89f, 0.16f);
	 style.Colors[ImGuiCol_CloseButtonHovered]    = ImVec4(0.86f, 0.93f, 0.89f, 0.39f);
	 style.Colors[ImGuiCol_CloseButtonActive]     = ImVec4(0.86f, 0.93f, 0.89f, 1.00f);
	 style.Colors[ImGuiCol_PlotLines]             = ImVec4(0.86f, 0.93f, 0.89f, 1.00f);
	 style.Colors[ImGuiCol_PlotLinesHovered]      = ImVec4(0.92f, 0.18f, 0.29f, 1.00f);
	 style.Colors[ImGuiCol_PlotHistogram]         = ImVec4(0.86f, 0.93f, 0.89f, 1.00f);
	 style.Colors[ImGuiCol_PlotHistogramHovered]  = ImVec4(0.92f, 0.18f, 0.29f, 1.00f);
	 style.Colors[ImGuiCol_TextSelectedBg]        = ImVec4(0.92f, 0.18f, 0.29f, 1.00f);
	 style.Colors[ImGuiCol_TooltipBg]             = ImVec4(0.47f, 0.77f, 0.83f, 0.72f);
	 style.Colors[ImGuiCol_ModalWindowDarkening]  = ImVec4(0.47f, 0.77f, 0.83f, 0.65f);
	 
	 */
	
	
    return *this;
}


Options& Options::color( ImGuiCol option, const ci::ColorA &color )
{
    mStyle.Colors[ option ] = color;
    return *this;
}

Renderer::Renderer()
{
    initFontTexture();
    initGlslProg();
    initBuffers();
}

//! renders imgui drawlist
void Renderer::renderDrawList( ImDrawList** const cmd_lists, int cmd_lists_count )
{
    size_t total_vtx_count = 0;
    for (int n = 0; n < cmd_lists_count; n++)
        total_vtx_count += cmd_lists[n]->vtx_buffer.size();
    if (total_vtx_count == 0)
        return;
    
    if( getVbo()->getSize() < total_vtx_count * sizeof( RenderData ) ){
        initBuffers( total_vtx_count );
    }
    
    int vtx_consumed = 0;
    {
        gl::VboRef vbo = getVbo();
        gl::ScopedBuffer scopedBuffer( vbo );
        RenderData *data = static_cast<RenderData*>( vbo->mapWriteOnly(true) );
        
        for (int n = 0; n < cmd_lists_count; n++) {
            const ImDrawList* cmd_list = cmd_lists[n];
            if (!cmd_list->vtx_buffer.empty()) {
                vtx_consumed += cmd_list->vtx_buffer.size();
                size_t count = cmd_list->vtx_buffer.size();
                for( int i = 0; i < count; i++ ){
                    ImU32 color = cmd_list->vtx_buffer[i].col;
                    uint32_t a = color >> 24 & 255;
                    uint32_t b = color >> 16 & 255;
                    uint32_t g = color >> 8 & 255;
                    uint32_t r = color >> 0 & 255;
                    
                    RenderData d;
                    d.pos   = vec2( cmd_list->vtx_buffer[i].pos.x, cmd_list->vtx_buffer[i].pos.y );
                    d.uv    = vec2( cmd_list->vtx_buffer[i].uv.x, cmd_list->vtx_buffer[i].uv.y );
                    d.color = vec4( r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f  );
                    *data = d;
                    ++data;
                }
            }
        }
        
        vbo->unmap();
    }
    gl::ScopedVao           scopedVao( getVao().get() );
    gl::ScopedGlslProg      scopedShader( getGlslProg() );
    gl::ScopedTextureBind   scopedTexture( getFontTextureRef(), 0 );
	
	auto mat = gl::getModelViewProjection();
	// mat = glm::translate( mat, vec3( 0.375f, 0.375f, 0.0f ) );
	
    getGlslProg()->uniform( "uModelViewProjection", mat );
    getGlslProg()->uniform( "uTex", 0 );
    
    gl::enableAlphaBlending();
    gl::disableDepthRead();
    glDisable( GL_CULL_FACE );
    
    vtx_consumed = 0;						// offset in vertex buffer. each command consume ImDrawCmd::vtx_count of those
    
    
    for (int n = 0; n < cmd_lists_count; n++) {
        const ImDrawList* cmd_list = cmd_lists[n];
        if (cmd_list->commands.empty() || cmd_list->vtx_buffer.empty())
            continue;
        const ImDrawCmd* pcmd = &cmd_list->commands.front();
        const ImDrawCmd* pcmd_end = &cmd_list->commands.back();
		while (pcmd <= pcmd_end) {
			if (pcmd->user_callback) {
				pcmd->user_callback(cmd_list, pcmd);
			}
			else {
				const ImDrawCmd& cmd = *pcmd++;
				gl::ScopedTextureBind texture( GL_TEXTURE_2D, (GLuint)(intptr_t) cmd.texture_id );
				gl::ScopedScissor scissors( cmd.clip_rect.x, (ImGui::GetIO().DisplaySize.y - cmd.clip_rect.w), (cmd.clip_rect.z - cmd.clip_rect.x), (cmd.clip_rect.w - cmd.clip_rect.y) );
				
				gl::drawArrays( GL_TRIANGLES, vtx_consumed, cmd.vtx_count );
				vtx_consumed += cmd.vtx_count;
			}
        }
    }
}

//! initializes and returns the font texture
gl::TextureRef Renderer::getFontTextureRef()
{
    if( !mFontTexture ){
        initFontTexture();
    }
    return mFontTexture;
}
//! initializes and returns the vbo mesh
gl::VaoRef Renderer::getVao()
{
    if( !mVao ){
        initBuffers();
    }
    return mVao;
}

//! initializes and returns the vbo
gl::VboRef Renderer::getVbo()
{
    if( !mVbo ){
        initBuffers();
    }
    return mVbo;
}

//! initializes the vbo mesh
void Renderer::initBuffers( size_t size )
{
    
    vector<RenderData> renderData;
    renderData.assign( size, RenderData() );
    
    mVbo    = gl::Vbo::create( GL_ARRAY_BUFFER, renderData, GL_STREAM_DRAW );
    mVao    = gl::Vao::create();
    
    gl::ScopedVao mVaoScope( mVao );
    gl::ScopedBuffer mVboScope( mVbo );
    
    gl::enableVertexAttribArray( 0 );
    gl::enableVertexAttribArray( 1 );
    gl::enableVertexAttribArray( 2 );
    
    gl::vertexAttribPointer( 0, 2, GL_FLOAT, GL_FALSE, sizeof(RenderData), (const GLvoid*)offsetof(RenderData, pos) );
    gl::vertexAttribPointer( 1, 2, GL_FLOAT, GL_TRUE, sizeof(RenderData), (const GLvoid*)offsetof(RenderData, uv) );
    gl::vertexAttribPointer( 2, 4, GL_FLOAT, GL_TRUE, sizeof(RenderData), (const GLvoid*)offsetof(RenderData, color) );
    
}
//! initalizes the font texture
void Renderer::initFontTexture()
{
    unsigned char* pixels;
    int width, height;
    ImGui::GetIO().Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);
    mFontTexture = gl::Texture::create( pixels, GL_RGBA, width, height, gl::Texture::Format().magFilter(GL_LINEAR).minFilter(GL_LINEAR) );
    ImGui::GetIO().Fonts->ClearTexData();
    ImGui::GetIO().Fonts->TexID = (void *)(intptr_t) mFontTexture->getId();
}

/*ImFont* addFont( ci::DataSourceRef font, float size, int fontId )
 {
 ImFont* newFont = getRenderer()->addFont( font, size, fontId );
 return newFont;
 }
 */

ImFont* Renderer::addFont( ci::DataSourceRef font, float size, const ImWchar* glyph_ranges )
{
    ImFontAtlas* fontAtlas  = ImGui::GetIO().Fonts;
    
    
    Font ciFont( font, size );
    
    BufferRef buffer		= font->getBuffer();
    void* bufferCopy        = (void*) malloc( buffer->getSize() );
    memcpy( bufferCopy, buffer->getData(), buffer->getSize() );
    
    ImFont* newFont         = fontAtlas->AddFontFromMemoryTTF( bufferCopy, buffer->getSize(), size, glyph_ranges );
    
    mFonts.insert( make_pair( font->getFilePath().stem().string(), newFont ) );
    return newFont;
}

//! initalizes and returns the shader
gl::GlslProgRef Renderer::getGlslProg()
{
    if( !mShader ){
        initGlslProg();
    }
    return mShader;
}

//! initalizes the shader
void Renderer::initGlslProg()
{
    try {
        mShader = gl::GlslProg::create( gl::GlslProg::Format()
									   .vertex(
#ifndef CINDER_GL_ES
												R"(
												#version 150
												uniform mat4 uModelViewProjection;
												in vec2      iPosition;
												in vec2      iUv;
												in vec4      iColor;
												out vec2     vUv;
												out vec4     vColor;
												void main() {
												   vColor       = iColor;
												   vUv          = iUv;
												   gl_Position  = uModelViewProjection * vec4( iPosition, 0.0, 1.0 );
												} )"
											   
#else
												R"(
												#version 300 es
												precision highp float;
												uniform mat4 uModelViewProjection;
												   
												in vec2      iPosition;
												in vec2      iUv;
												in vec4      iColor;
												   
												out vec2     vUv;
												out vec4     vColor;
												   
												void main() {
													vColor       = iColor;
													vUv          = iUv;
													gl_Position  = uModelViewProjection * vec4( iPosition, 0.0, 1.0 );
												} )"
#endif
										)
                                       .fragment(
#ifndef CINDER_GL_ES
												R"(
												#version 150
											 
												in vec2            vUv;
												in vec4				vColor;
												out vec4			oColor;
												uniform sampler2D	uTex;
											 
												void main() {
													vec4 color = texture( uTex, vUv ) * vColor;
													oColor = color;
												}  )"
#else
												R"(
												#version 300 es
												precision highp float;
											 
												in highp vec2		vUv;
												in highp vec4		vColor;
												out highp vec4      oColor;
												uniform sampler2D	uTex;
		
												void main() {
													vec4 color = texture( uTex, vUv ) * vColor;
													oColor = color;
												}  )"
		
#endif
										 )
                                       .attribLocation( "iPosition", 0 )
                                       .attribLocation( "iUv", 1 )
                                       .attribLocation( "iColor", 2 )
                                       );
    }
    catch( gl::GlslProgCompileExc exc ){
        CI_LOG_E( "Problem Compiling ImGui::Renderer shader " << exc.what() );
    }
}
ImFont* Renderer::getFont( const std::string &name )
{
    if( !mFonts.count( name ) ){
        return nullptr;
    }
    else {
        return mFonts[name];
    }
}

// Cinder Helpers
void Image( const ci::gl::Texture2dRef &texture, const ImVec2& size, const ImVec2& uv0, const ImVec2& uv1, const ImVec4& tint_col, const ImVec4& border_col )
{
    Image( (void*)(intptr_t) texture->getId(), size, uv0, uv1, tint_col, border_col );
}
bool ImageButton( const ci::gl::Texture2dRef &texture, const ImVec2& size, const ImVec2& uv0, const ImVec2& uv1, int frame_padding, const ImVec4& bg_col, const ImVec4& tint_col)
{
    return ImageButton( (void*)(intptr_t) texture->getId(), size, uv0, uv1, frame_padding, bg_col, tint_col );
}

void PushFont( const std::string& name )
{
    auto renderer   = getRenderer();
    ImFont* font    = renderer->getFont( name );
    CI_ASSERT( font != nullptr );
    PushFont( font );
}

// Std Helpers
bool ListBox( const char* label, int* current_item, const std::vector<std::string>& items, int height_in_items )
{
	// copy names to a vector
	vector<const char*> names;
	for( auto item : items ) {
		char *cname = new char[ item.size() + 1 ];
		std::strcpy( cname, item.c_str() );
		names.push_back( cname );
	}
	
	bool result = ListBox( label, current_item, names.data(), names.size(), height_in_items );
	// cleanup
	for( auto &name : names ) delete [] name;
	return result;
}
bool InputText( const char* label, std::string* buf, ImGuiInputTextFlags flags, ImGuiTextEditCallback callback, void* user_data )
{
	// conversion
	char *buffer = new char[buf->size()+128];
	std::strcpy( buffer, buf->c_str() );
	bool result = InputText( label, buffer, buf->size()+128, flags, callback, user_data );
	if( result ){
		*buf = string( buffer );
	}
	// cleanup
	delete [] buffer;
	return result;
}
bool InputTextMultiline( const char* label, std::string* buf, const ImVec2& size, ImGuiInputTextFlags flags, ImGuiTextEditCallback callback, void* user_data )
{
	// conversion
	char *buffer = new char[buf->size()+128];
	std::strcpy( buffer, buf->c_str() );
	bool result = InputTextMultiline( label, buffer, buf->size()+128, size, flags, callback, user_data );
	if( result ){
		*buf = string( buffer );
	}
	// cleanup
	delete [] buffer;
	return result;
}
bool Combo( const char* label, int* current_item, const std::vector<std::string>& items, int height_in_items )
{
	// conversion
	string itemsNames;
	for( auto item : items )
		itemsNames += item + '\0';
	itemsNames += '\0';
	
	vector<char> charArray( itemsNames.begin(), itemsNames.end() );
	bool result = Combo( label, current_item, (const char*) &charArray[0], height_in_items );
	return result;
}
	
namespace {

	//! sets the right mouseDown IO values in imgui
	void mouseDown( ci::app::MouseEvent& event )
	{
		ImGuiIO& io = ImGui::GetIO();
		io.MousePos = toPixels( event.getPos() );
		if( event.isLeftDown() ){
			io.MouseDown[0] = true;
			io.MouseDown[1] = false;
		}
		else if( event.isRightDown() ){
			io.MouseDown[0] = false;
			io.MouseDown[1] = true;
		}
		
		event.setHandled( io.WantCaptureMouse );
	}
	//! sets the right mouseMove IO values in imgui
	void mouseMove( ci::app::MouseEvent& event )
	{
		ImGuiIO& io = ImGui::GetIO();
		io.MousePos = toPixels( event.getPos() );
		
		event.setHandled( io.WantCaptureMouse );
	}
	//! sets the right mouseDrag IO values in imgui
	void mouseDrag( ci::app::MouseEvent& event )
	{
		ImGuiIO& io = ImGui::GetIO();
		io.MousePos = toPixels( event.getPos() );
		
		event.setHandled( io.WantCaptureMouse );
	}
	//! sets the right mouseDrag IO values in imgui
	void mouseUp( ci::app::MouseEvent& event )
	{
		ImGuiIO& io     = ImGui::GetIO();
		io.MouseDown[0] = false;
		io.MouseDown[1] = false;
		
		event.setHandled( io.WantCaptureMouse );
	}
	//! sets the right mouseWheel IO values in imgui
	void mouseWheel( ci::app::MouseEvent& event )
	{
		ImGuiIO& io     = ImGui::GetIO();
		io.MouseWheel   += event.getWheelIncrement();
		
		event.setHandled( io.WantCaptureMouse );
	}
		

	vector<int> sAccelKeys;
		
	//! sets the right keyDown IO values in imgui
	void keyDown( ci::app::KeyEvent& event )
	{
		ImGuiIO& io = ImGui::GetIO();
		
		uint32_t character = event.getCharUtf32();
		
		io.KeysDown[event.getCode()] = true;
		
		if ( !event.isAccelDown() && character > 0 && character <= 255 ) {
			io.AddInputCharacter( ( char ) character );
		}
		else if( event.getCode() != KeyEvent::KEY_LMETA
				&& event.getCode() != KeyEvent::KEY_RMETA
				&& event.isAccelDown()
				&& find( sAccelKeys.begin(), sAccelKeys.end(), event.getCode() ) == sAccelKeys.end() ){
			sAccelKeys.push_back( event.getCode() );
		}
		
		io.KeyCtrl = io.KeysDown[KeyEvent::KEY_LCTRL] || io.KeysDown[KeyEvent::KEY_RCTRL] || io.KeysDown[KeyEvent::KEY_LMETA] || io.KeysDown[KeyEvent::KEY_RMETA];
		io.KeyShift = io.KeysDown[KeyEvent::KEY_LSHIFT] || io.KeysDown[KeyEvent::KEY_RSHIFT];
		io.KeyAlt = io.KeysDown[KeyEvent::KEY_LALT] || io.KeysDown[KeyEvent::KEY_RALT];
		
		event.setHandled( io.WantCaptureKeyboard );
	}
	//! sets the right keyUp IO values in imgui
	void keyUp( ci::app::KeyEvent& event )
	{
		ImGuiIO& io = ImGui::GetIO();
		
		io.KeysDown[event.getCode()] = false;
		
		for( auto key : sAccelKeys ){
			io.KeysDown[key] = false;
		}
		sAccelKeys.clear();
		
		io.KeyCtrl = io.KeysDown[KeyEvent::KEY_LCTRL] || io.KeysDown[KeyEvent::KEY_RCTRL] || io.KeysDown[KeyEvent::KEY_LMETA] || io.KeysDown[KeyEvent::KEY_RMETA];
		io.KeyShift = io.KeysDown[KeyEvent::KEY_LSHIFT] || io.KeysDown[KeyEvent::KEY_RSHIFT];
		io.KeyAlt = io.KeysDown[KeyEvent::KEY_LALT] || io.KeysDown[KeyEvent::KEY_RALT];
		
		event.setHandled( io.WantCaptureKeyboard );
	}
	void resize()
	{
		ImGuiIO& io		= ImGui::GetIO();
		io.DisplaySize	= toPixels( getWindowSize() );
	}

	static bool sNewFrame = false;
	//static uint32_t sLastFrame = -1;
	void render()
	{
		//if( getElapsedFrames() == sLastFrame ) return;
		
		gl::ScopedMatrices matrices;
		gl::ScopedViewport viewport( ivec2(0), getWindowSize() );
		gl::setMatricesWindow( getWindowSize() );
		ImGui::Render();
		
		static float elapsedTime    = 0.0f;
		float currentElapsedTime    = getElapsedSeconds();
		ImGuiIO& io                 = ImGui::GetIO();
		io.DeltaTime                = ( currentElapsedTime - elapsedTime );
		elapsedTime                 = currentElapsedTime;
		//sLastFrame                  = getElapsedFrames();
		sNewFrame                   = false;
		App::get()->dispatchAsync( [](){ ImGui::NewFrame(); sNewFrame = true; } );
	}

	void newFrameGuard()
	{
		if( !sNewFrame ){
			ImGui::NewFrame();
		}
	}
	
} // Anonymous namespace
	
// wrong... and would not work in a multi-windows scenario
static vector<signals::Connection> sWindowConnections;

void connectWindow( ci::app::WindowRef window )
{
    sWindowConnections = {
        window->getSignalMouseDown().connect( mouseDown ),
        window->getSignalMouseUp().connect( mouseUp ),
        window->getSignalMouseDrag().connect( mouseDrag ),
        window->getSignalMouseMove().connect( mouseMove ),
        window->getSignalMouseWheel().connect( mouseWheel ),
        window->getSignalKeyDown().connect( keyDown ),
        window->getSignalKeyUp().connect( keyUp ),
        window->getSignalResize().connect( resize ),
        window->getSignalDraw().connect( newFrameGuard ),
        window->getSignalPostDraw().connect( render )
    };
}
void disconnectWindow( ci::app::WindowRef window )
{
    for( auto connection : sWindowConnections ){
        connection.disconnect();
    }
}
	
ScopedWindow::ScopedWindow( const std::string &name, ImGuiWindowFlags flags)
{
	ImGui::Begin( name.c_str(), nullptr, flags );
}
ScopedWindow::ScopedWindow( const std::string &name, glm::vec2 size, float fillAlpha, ImGuiWindowFlags flags )
{
    ImGui::Begin( name.c_str(), nullptr, size, fillAlpha, flags );
}
ScopedWindow::~ScopedWindow()
{
    ImGui::End();
}
ScopedChild::ScopedChild( const std::string &name, glm::vec2 size, bool border, ImGuiWindowFlags extraFlags )
{
    ImGui::BeginChild( name.c_str(), size, border, extraFlags );
}
ScopedChild::~ScopedChild()
{
    ImGui::EndChild();
}
ScopedGroup::ScopedGroup()
{
	ImGui::BeginGroup();
}
ScopedGroup::~ScopedGroup()
{
	ImGui::EndGroup();
}
ScopedFont::ScopedFont( const std::string &name )
{
    ImGui::PushFont( name );
}
ScopedFont::~ScopedFont()
{
    ImGui::PopFont();
}
ScopedStyleColor::ScopedStyleColor( ImGuiCol idx, const ImVec4& col )
{
    ImGui::PushStyleColor( idx, col );
}
ScopedStyleColor::~ScopedStyleColor()
{
    ImGui::PopStyleColor();
}
ScopedStyleVar::ScopedStyleVar( ImGuiStyleVar idx, float val )
{
    ImGui::PushStyleVar( idx, val );
}
ScopedStyleVar::ScopedStyleVar( ImGuiStyleVar idx, const ImVec2 &val )
{
    ImGui::PushStyleVar( idx, val );
}
ScopedStyleVar::~ScopedStyleVar()
{
    ImGui::PopStyleVar();
}

ScopedItemWidth::ScopedItemWidth( float itemWidth )
{
    ImGui::PushItemWidth( itemWidth );
}
ScopedItemWidth::~ScopedItemWidth()
{
    ImGui::PopItemWidth();
}

ScopedTextWrapPos::ScopedTextWrapPos( float wrapPosX )
{
    ImGui::PushTextWrapPos( wrapPosX );
}
ScopedTextWrapPos::~ScopedTextWrapPos()
{
    ImGui::PopTextWrapPos();
}

ScopedId::ScopedId( const std::string &name )
{
    ImGui::PushID( name.c_str() );
}
ScopedId::ScopedId( const void *ptrId )
{
    ImGui::PushID( ptrId );
}
ScopedId::ScopedId( const int intId )
{
    ImGui::PushID( intId );
}
ScopedId::~ScopedId()
{
    ImGui::PopID();
}

ScopedMainMenuBar::ScopedMainMenuBar()
{
	mOpened = ImGui::BeginMainMenuBar();
}
ScopedMainMenuBar::~ScopedMainMenuBar()
{
	if( mOpened ) ImGui::EndMainMenuBar();
}

ScopedMenuBar::ScopedMenuBar()
{
	mOpened = ImGui::BeginMenuBar();
}
ScopedMenuBar::~ScopedMenuBar()
{
	if( mOpened ) ImGui::EndMenuBar();
}
}