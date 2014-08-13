#include "imGuiCinder.h"

#include "cinder/gl/Vbo.h"
#include "cinder/gl/Texture.h"
#include "cinder/gl/GlslProg.h"
#include "cinder/DataSource.h"
#include "cinder/Clipboard.h"

using namespace std;
using namespace ci;
using namespace ci::app;


namespace ImGui {
    
    
    // static variables
    gl::TextureRef Renderer::mFontTexture;
    gl::VboMeshRef Renderer::mVbo;
    gl::GlslProgRef Renderer::mShader;
    ci::Buffer Renderer::mFontData;
    
    //! sets current window
    void setWindow( app::WindowRef window )
    {
        window->getSignalMouseDown().connect( [](MouseEvent event){
            ImGuiIO& io = ImGui::GetIO();
            io.MousePos = ImVec2( event.getPos() );
            if( event.isLeftDown() ){
                io.MouseDown[0] = true;
                io.MouseDown[1] = false;
            }
            else if( event.isLeftDown() ){
                io.MouseDown[0] = false;
                io.MouseDown[1] = true;
            }
        } );
        window->getSignalMouseUp().connect( [](MouseEvent event){
            ImGuiIO& io = ImGui::GetIO();
            io.MouseDown[0] = false;
            io.MouseDown[1] = false;
        } );
        window->getSignalMouseDrag().connect( [](MouseEvent event){
            ImGuiIO& io = ImGui::GetIO();
            io.MousePos = ImVec2( event.getPos() );
        } );
        window->getSignalMouseMove().connect( [](MouseEvent event){
            ImGuiIO& io = ImGui::GetIO();
            io.MousePos = ImVec2( event.getPos() );
        } );
        window->getSignalMouseWheel().connect( [](MouseEvent event){
            ImGuiIO& io = ImGui::GetIO();
            io.MouseWheel = 10.0f * event.getWheelIncrement();
        } );
        window->getSignalResize().connect( [](){
            ImGuiIO& io = ImGui::GetIO();
            io.DisplaySize = ImVec2( getWindowSize() );
        } );
        window->getSignalKeyDown().connect( [](KeyEvent event){
            ImGuiIO& io = ImGui::GetIO();
            //io.KeyCtrl = event.isAccelDown();
            //io.KeyShift = event.isShiftDown();
            io.KeysDown[ event.getCode() ] = true;
            
            uint32_t character = event.getCharUtf32();
            if( character > 0 && character <= 255 ){
                io.AddInputCharacter( (char) character );
            }
        } );
        window->getSignalKeyUp().connect( [](KeyEvent event){
            ImGuiIO& io = ImGui::GetIO();
            //io.KeyCtrl = event.isAccelDown();
            //io.KeyShift = event.isShiftDown();
            io.KeysDown[ event.getCode() ] = false;
        } );
        
        ImGuiIO& io = ImGui::GetIO();
        io.DisplaySize = ImVec2( window->getSize() );
        io.DeltaTime = 1.0f / 60.0f;
        io.KeyMap[ImGuiKey_Tab] = KeyEvent::KEY_TAB;
        io.KeyMap[ImGuiKey_LeftArrow] = KeyEvent::KEY_LEFT;
        io.KeyMap[ImGuiKey_RightArrow] = KeyEvent::KEY_RIGHT;
        io.KeyMap[ImGuiKey_UpArrow] = KeyEvent::KEY_UP;
        io.KeyMap[ImGuiKey_DownArrow] = KeyEvent::KEY_DOWN;
        io.KeyMap[ImGuiKey_Home] = KeyEvent::KEY_HOME;
        io.KeyMap[ImGuiKey_End] = KeyEvent::KEY_END;
        io.KeyMap[ImGuiKey_Delete] = KeyEvent::KEY_DELETE;
        io.KeyMap[ImGuiKey_Backspace] = KeyEvent::KEY_BACKSPACE;
        io.KeyMap[ImGuiKey_Enter] = KeyEvent::KEY_RETURN;
        io.KeyMap[ImGuiKey_Escape] = KeyEvent::KEY_ESCAPE;
        /*io.KeyMap[ImGuiKey_A] = KeyEvent::KEY_A;
        io.KeyMap[ImGuiKey_C] = KeyEvent::KEY_C;
        io.KeyMap[ImGuiKey_V] = KeyEvent::KEY_V;
        io.KeyMap[ImGuiKey_X] = KeyEvent::KEY_X;
        io.KeyMap[ImGuiKey_Y] = KeyEvent::KEY_Y;
        io.KeyMap[ImGuiKey_Z] = KeyEvent::KEY_Z;*/
        
        io.SetClipboardTextFn = [](const char* text, const char* text_end){
            if (!text_end) text_end = text + strlen(text);
            char* buf = (char*)malloc(text_end - text + 1);
            memcpy(buf, text, text_end-text);
            buf[text_end-text] = '\0';
            Clipboard::setString( buf );
            free(buf);
        };
        io.GetClipboardTextFn = [](){
            return Clipboard::getString().c_str();
        };
        io.RenderDrawListsFn = Renderer::renderDrawList;
        
        setDarkTheme();
    }
    
    void setThemeColor( ImVec4 color0, ImVec4 color1, ImVec4 color2, ImVec4 color3, ImVec4 color4 ){
        
        ImGuiStyle& style = ImGui::GetStyle();
        
        style.Colors[ImGuiCol_Text]					= color3;
        style.Colors[ImGuiCol_WindowBg]				= color0;
        style.Colors[ImGuiCol_Border]				= color0;
        style.Colors[ImGuiCol_BorderShadow]			= ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
        style.Colors[ImGuiCol_FrameBg]				= color2;	// Background of checkbox, radio button, plot, slider, text input
        style.Colors[ImGuiCol_TitleBg]				= color2;
        style.Colors[ImGuiCol_TitleBgCollapsed]		= color4;
        style.Colors[ImGuiCol_ScrollbarBg]			= color2;
        style.Colors[ImGuiCol_ScrollbarGrab]		= color1;
        style.Colors[ImGuiCol_ScrollbarGrabHovered]	= color0;
        style.Colors[ImGuiCol_ScrollbarGrabActive]	= color3;
        style.Colors[ImGuiCol_ComboBg]				= color2;
        style.Colors[ImGuiCol_CheckActive]			= color3;
        style.Colors[ImGuiCol_SliderGrab]			= color4;
        style.Colors[ImGuiCol_SliderGrabActive]		= color3;
        style.Colors[ImGuiCol_Button]				= color2;
        style.Colors[ImGuiCol_ButtonHovered]		= color4;
        style.Colors[ImGuiCol_ButtonActive]			= color3;
        style.Colors[ImGuiCol_Header]				= color2;
        style.Colors[ImGuiCol_HeaderHovered]		= color4;
        style.Colors[ImGuiCol_HeaderActive]			= color3;
        style.Colors[ImGuiCol_Column]				= color2;
        style.Colors[ImGuiCol_ColumnHovered]		= color3;
        style.Colors[ImGuiCol_ColumnActive]			= color4;
        style.Colors[ImGuiCol_ResizeGrip]			= color2;
        style.Colors[ImGuiCol_ResizeGripHovered]	= color3;
        style.Colors[ImGuiCol_ResizeGripActive]		= color4;
        style.Colors[ImGuiCol_CloseButton]			= color4;
        style.Colors[ImGuiCol_CloseButtonHovered]	= color2;
        style.Colors[ImGuiCol_CloseButtonActive]	= color3;
        style.Colors[ImGuiCol_PlotLines]			= color4;
        style.Colors[ImGuiCol_PlotLinesHovered]		= color3;
        style.Colors[ImGuiCol_PlotHistogram]		= color4;
        style.Colors[ImGuiCol_PlotHistogramHovered]	= color3;
        style.Colors[ImGuiCol_TextSelectedBg]		= color4;
        style.Colors[ImGuiCol_TooltipBg]			= color1;
    }
    
    
    //! sets light theme style
    void setLightTheme()
    {
        ImGui::GetStyle().WindowRounding = 4.0f;
        setThemeColor( ImVec4( 0.76f, 0.76f, 0.76f, 1.0f ),
                      ImVec4( 0.76f, 0.76f, 0.76f, 1.0f ),
                      ImVec4( 0.71f, 0.71f, 0.71f, 1.0f ),
                      ImVec4( 0.25f, 0.25f, 0.25f, 1.0f ),
                      ImVec4( 0.57f, 0.57f, 0.57f, 1.0f ) );
    }
    
    //! sets dark theme style
    void setDarkTheme()
    {
        ImGui::GetStyle().WindowRounding = 4.0f;
        setThemeColor( ImVec4( 0.16f,0.16f, 0.16f, 1.0f ),
                      ImVec4( 0.04f, 0.04f, 0.04f, 1.0f ),
                      ImVec4( 0.20f, 0.20f, 0.20f, 1.0f ),
                      ImVec4( 0.96f, 0.92f, 0.63f, 1.0f ),
                      ImVec4( 0.27f, 0.27f, 0.27f, 1.0f ) );
    }
    
    
    //! sets font
    void setFont( ci::DataSourceRef font, ci::DataSourceRef texture )
    {
        ImGui::Renderer::setFont( font, texture );
    }
    
    
    //! renders imgui drawlist
    void Renderer::renderDrawList( ImDrawList** const cmd_lists, int cmd_lists_count )
    {
        
#ifdef IMGUI_RENDERER_FIXED_PIPELINE
        if (cmd_lists_count == 0)
            return;
        
        // Setup render state: alpha-blending enabled, no face culling, no depth testing
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDisable(GL_CULL_FACE);
        glDisable(GL_DEPTH_TEST);
        //glEnable(GL_SCISSOR_TEST);
        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glEnableClientState(GL_COLOR_ARRAY);
        
        // Bind texture
        getFontTextureRef()->enableAndBind();
        
        // Setup matrices
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(0.0f, ImGui::GetIO().DisplaySize.x, ImGui::GetIO().DisplaySize.y, 0.0f, -1.0f, +1.0f);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        
        // Render command lists
        for (int n = 0; n < cmd_lists_count; n++)
        {
            const ImDrawList* cmd_list = cmd_lists[n];
            const unsigned char* vtx_buffer = (const unsigned char*)cmd_list->vtx_buffer.begin();
            glVertexPointer(2, GL_FLOAT, sizeof(ImDrawVert), (void*)(vtx_buffer));
            glTexCoordPointer(2, GL_FLOAT, sizeof(ImDrawVert), (void*)(vtx_buffer+8));
            glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(ImDrawVert), (void*)(vtx_buffer+16));
            
            int vtx_offset = 0;
            const ImDrawCmd* pcmd_end = cmd_list->commands.end();
            for (const ImDrawCmd* pcmd = cmd_list->commands.begin(); pcmd != pcmd_end; pcmd++)
            {
                glScissor((int)pcmd->clip_rect.x, (int)pcmd->clip_rect.y, (int)(pcmd->clip_rect.z - pcmd->clip_rect.x), (int)(pcmd->clip_rect.w - pcmd->clip_rect.y));
                glDrawArrays(GL_TRIANGLES, vtx_offset, pcmd->vtx_count);
                vtx_offset += pcmd->vtx_count;
            }
        }
        glDisable(GL_SCISSOR_TEST);
        
        getFontTextureRef()->unbind();
        
#else
        size_t total_vtx_count = 0;
        for (int n = 0; n < cmd_lists_count; n++)
            total_vtx_count += cmd_lists[n]->vtx_buffer.size();
        if (total_vtx_count == 0)
            return;
        
        if( getVboMeshRef()->getNumVertices() < total_vtx_count ){
            gl::VboMesh::Layout layout;
            layout.setDynamicColorsRGBA();
            layout.setDynamicPositions();
            layout.setDynamicTexCoords2d();
            
            mVbo = gl::VboMesh::create( total_vtx_count, 0, layout, GL_TRIANGLES );
        }
        
        int vtx_consumed = 0;
        {
            gl::VboMesh::VertexIter iter = getVboMeshRef()->mapVertexBuffer();
            for (int n = 0; n < cmd_lists_count; n++) {
                const ImDrawList* cmd_list = cmd_lists[n];
                if (!cmd_list->vtx_buffer.empty()) {
                    vtx_consumed += cmd_list->vtx_buffer.size();
                    int count = cmd_list->vtx_buffer.size();
                    for( int i = 0; i < count; i++ ){
                        
                        iter.setPosition( Vec3f( cmd_list->vtx_buffer[ i ].pos, 0.0f ) );
                        iter.setTexCoord2d0( cmd_list->vtx_buffer[i].uv );
                        
                        ImU32 color = cmd_list->vtx_buffer[i].col;
                        
                        uint32_t a = color >> 24 & 255;
                        uint32_t b = color >> 16 & 255;
                        uint32_t g = color >> 8 & 255;
                        uint32_t r = color >> 0 & 255;
                        iter.setColorRGBA( ColorA( r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f  ) );
                        ++iter;
                    }
                }
            }
        }
        
        getGlslProg()->bind();
        
        float windowHeight = GetIO().DisplaySize.y;
        getGlslProg()->uniform( "height", windowHeight );
        
        gl::enableAlphaBlending();
        gl::disableDepthRead();
        glDisable( GL_CULL_FACE );
        
        getFontTextureRef()->enableAndBind();
        
        vtx_consumed = 0;						// offset in vertex buffer. each command consume ImDrawCmd::vtx_count of those
        
        getVboMeshRef()->enableClientStates();
        getVboMeshRef()->bindAllData();
        
        for (int n = 0; n < cmd_lists_count; n++) {
            const ImDrawList* cmd_list = cmd_lists[n];
            if (cmd_list->commands.empty() || cmd_list->vtx_buffer.empty())
                continue;
            const ImDrawCmd* pcmd = &cmd_list->commands.front();
            const ImDrawCmd* pcmd_end = &cmd_list->commands.back();
            while (pcmd <= pcmd_end) {
                const ImDrawCmd& cmd = *pcmd++;
                getGlslProg()->uniform( "ClipRect", Vec4f( cmd.clip_rect ) );
                glDrawArrays(GL_TRIANGLES, vtx_consumed, cmd.vtx_count);
                vtx_consumed += cmd.vtx_count;
            }
        }
        getVboMeshRef()->unbindBuffers();
        getVboMeshRef()->disableClientStates();
        
        getFontTextureRef()->unbind();
        getGlslProg()->unbind();
        
        
#endif
    }
    
    //! initalizes and returns the font texture
    gl::TextureRef Renderer::getFontTextureRef()
    {
        if( !mFontTexture ){
            initFontTexture();
        }
        return mFontTexture;
    }
    //! initalizes and returns the vbo mesh
    gl::VboMeshRef Renderer::getVboMeshRef()
    {
        if( !mVbo ){
            initVbo();
        }
        return mVbo;
    }
    
    //! initalizes the vbo mesh
    void Renderer::initVbo()
    {
        gl::VboMesh::Layout layout;
        layout.setDynamicColorsRGBA();
        layout.setDynamicPositions();
        layout.setDynamicTexCoords2d();
        
        mVbo = gl::VboMesh::create( 1000, 0, layout, GL_TRIANGLES );
    }
    //! initalizes the font texture
    void Renderer::initFontTexture()
    {
        gl::Texture::Format format;
        format.setMagFilter( GL_NEAREST );
        format.setMinFilter( GL_NEAREST );
        
        const void* png_data;
        unsigned int png_size;
        ImGui::GetDefaultFontData( NULL, NULL, &png_data, &png_size);
        Buffer buffer = Buffer( const_cast<void*>( png_data ), png_size );
        DataSourceBufferRef dataSource = DataSourceBuffer::create( buffer );
        mFontTexture = gl::Texture::create( loadImage( dataSource, ImageSource::Options(), "png" ), format );
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
            mShader = gl::GlslProg::create(
                                           "void main() {"
                                           "   gl_FrontColor = gl_Color;"
                                           "   gl_TexCoord[0] = gl_MultiTexCoord0;"
                                           "   gl_Position = ftransform();"
                                           "}",
                                           
                                           "uniform sampler2D Tex;"
                                           "uniform vec4 ClipRect;"
                                           "uniform float height;"
                                           "void main() {"
                                           "   vec4 color = texture2D( Tex, gl_TexCoord[0].st ) * gl_Color;"
                                           "   vec2 pixel_pos = gl_FragCoord.st;"
                                           "   pixel_pos.y = height - pixel_pos.y;"
                                           // Clipping: using discard
                                           //"   if (pixel_pos.x < ClipRect.x || pixel_pos.y < ClipRect.y || pixel_pos.x > ClipRect.z || pixel_pos.y > ClipRect.w) discard;"
                                           // Clipping: using discard and step
                                           //"   if (step(ClipRect.x,pixel_pos.x) * step(ClipRect.y,pixel_pos.y) * step(pixel_pos.x,ClipRect.z) * step(pixel_pos.y,ClipRect.w) < 1.0f) discard;"
                                           // Clipping: branch-less
                                           "   color.w *= (step(ClipRect.x,pixel_pos.x) * step(ClipRect.y,pixel_pos.y) * step(pixel_pos.x,ClipRect.z) * step(pixel_pos.y,ClipRect.w));"
                                           "   gl_FragColor = color;"
                                           "}"
                                       );
        }
        catch( gl::GlslProgCompileExc exc ){
            cout << "Problem Compiling ImGui::Renderer shader" << endl << exc.what() << endl;
        }
    }
    
    //! sets font
    void Renderer::setFont( ci::DataSourceRef font, ci::DataSourceRef texture )
    {
        mFontData = font->getBuffer();
        ImGui::GetIO().Font = new ImBitmapFont();
        ImGui::GetIO().Font->LoadFromMemory( mFontData.getData(), mFontData.getDataSize() );
        ImGui::GetIO().FontHeight = ImGui::GetIO().Font->GetFontSize();
        
        gl::Texture::Format format;
        format.setMagFilter( GL_NEAREST );
        format.setMinFilter( GL_NEAREST );
        mFontTexture = gl::Texture::create( loadImage( texture ), format );
    }
}

