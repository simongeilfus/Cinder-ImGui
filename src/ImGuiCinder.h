#pragma once

#include "imgui.h"

//#define IMGUI_RENDERER_FIXED_PIPELINE

// cinder forward declarations
namespace cinder {
    namespace gl {
        typedef std::shared_ptr<class Texture> TextureRef;
        typedef std::shared_ptr<class VboMesh> VboMeshRef;
        typedef std::shared_ptr<class GlslProg> GlslProgRef;
    }
    namespace app { typedef std::shared_ptr<class Window> WindowRef; }
    template<typename T> class ColorAT;
    typedef class ColorAT<float> ColorA;
    typedef std::shared_ptr<class DataSource> DataSourceRef;
    class Buffer;
}

//! cinder imgui namespace
namespace ImGui {
    
    //! sets current window
    void setWindow( ci::app::WindowRef window );
    
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
        
    private:
        //! initalizes and returns the font texture
        static ci::gl::TextureRef getFontTextureRef();
        //! initalizes and returns the vbo mesh
        static ci::gl::VboMeshRef getVboMeshRef();
        //! initalizes and returns the shader
        static ci::gl::GlslProgRef getGlslProg();
        
        //! initalizes the font texture
        static void initFontTexture();
        //! initalizes the vbo mesh
        static void initVbo();
        //! initalizes the shader
        static void initGlslProg();
        
        static ci::gl::TextureRef   mFontTexture;
        static ci::gl::VboMeshRef   mVbo;
        static ci::gl::GlslProgRef  mShader;
        
        static ci::Buffer           mFontData;
    };

}

