#pragma once

// cinder forward declarations
namespace cinder {
    namespace gl {
        typedef std::shared_ptr<class Texture> TextureRef;
        typedef std::shared_ptr<class VboMesh> VboMeshRef;
        typedef std::shared_ptr<class GlslProg> GlslProgRef;
    }
    namespace app { typedef std::shared_ptr<class Window> WindowRef; }
    template<typename T> class ColorAT; typedef class ColorAT<float> ColorA;
    template<typename T> class Vec2; typedef Vec2<float> Vec2f;
    template<typename T> class Vec4; typedef Vec4<float> Vec4f;
    typedef std::shared_ptr<class DataSource> DataSourceRef;
    class Buffer;
}

// this would usually be in imguicondig.h but it is easier
// to leave it here so we don't need to modify the library
#define IM_VEC2_CLASS_EXTRA												\
ImVec2(const ci::Vec2f& f) { x = f.x; y = f.y; }						\
operator ci::Vec2f() const { return ci::Vec2f(x,y); }

#define IM_VEC4_CLASS_EXTRA												\
ImVec4(const ci::Vec4f& f) { x = f.x; y = f.y; z = f.z; w = f.w; }		\
operator ci::Vec4f() const { return ci::Vec4f(x,y,z,w); }


#include "imgui.h"

// uncomment this to change the way imgui batches got rendered
//#define IMGUI_RENDERER_FIXED_PIPELINE


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

