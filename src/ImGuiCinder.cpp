#include "imGuiCinder.h"

#include "cinder/app/App.h"
#include "cinder/gl/Context.h"
#include "cinder/Clipboard.h"

using namespace std;
using namespace ci;
using namespace ci::app;


namespace ImGui {
    
    
    // static variables
    gl::Texture2dRef	Renderer::mFontTexture;
    gl::VaoRef			Renderer::mVao;
    gl::VboRef			Renderer::mVbo;
    gl::GlslProgRef		Renderer::mShader;
    ci::Buffer			Renderer::mFontData;
	bool				Renderer::mInitialized = false;
    
    // same as ImDrawVert
    struct RenderData {
        vec2 pos;
        vec2 uv;
        vec4 color;
    };
	
	
	
	//! sets the right mouseDown IO values in imgui
	void mouseDown( ci::app::MouseEvent& event )
	{
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
		
		event.setHandled( io.WantCaptureMouse );
	}
	//! sets the right mouseMove IO values in imgui
	void mouseMove( ci::app::MouseEvent& event )
	{
		ImGuiIO& io = ImGui::GetIO();
		io.MousePos = ImVec2( event.getPos() );
		
		event.setHandled( io.WantCaptureMouse );
	}
	//! sets the right mouseDrag IO values in imgui
	void mouseDrag( ci::app::MouseEvent& event )
	{
		ImGuiIO& io = ImGui::GetIO();
		io.MousePos = ImVec2( event.getPos() );
		
		event.setHandled( io.WantCaptureMouse );
	}
	//! sets the right mouseDrag IO values in imgui
	void mouseUp( ci::app::MouseEvent& event )
	{
		ImGuiIO& io = ImGui::GetIO();
		io.MouseDown[0] = false;
		io.MouseDown[1] = false;
		
		event.setHandled( io.WantCaptureMouse );
	}
	//! sets the right mouseWheel IO values in imgui
	void mouseWheel( ci::app::MouseEvent& event )
	{
		ImGuiIO& io = ImGui::GetIO();
		io.MouseWheel = 10.0f * event.getWheelIncrement();
		
		event.setHandled( io.WantCaptureMouse );
	}
	//! sets the right keyDown IO values in imgui
	void keyDown( ci::app::KeyEvent& event )
	{
		ImGuiIO& io = ImGui::GetIO();
		//io.KeyCtrl = event.isAccelDown();
		//io.KeyShift = event.isShiftDown();
		io.KeysDown[ event.getCode() ] = true;
		
		uint32_t character = event.getCharUtf32();
		if( character > 0 && character <= 255 ){
			io.AddInputCharacter( (char) character );
		}
		
		event.setHandled( io.WantCaptureKeyboard );
	}
	//! sets the right keyUp IO values in imgui
	void keyUp( ci::app::KeyEvent& event )
	{
		ImGuiIO& io = ImGui::GetIO();
		//io.KeyCtrl = event.isAccelDown();
		//io.KeyShift = event.isShiftDown();
		io.KeysDown[ event.getCode() ] = false;
		
		event.setHandled( io.WantCaptureKeyboard );
	}
	
	//! sets current window size and events
	void setWindow( ci::app::WindowRef window )
	{
		connectSignals( window );
		setSize( window->getSize() );
		setDarkTheme();
	}
	//! sets current size
	void setSize( ci::ivec2 size )
	{
		ImGuiIO& io = ImGui::GetIO();
		io.DisplaySize = vec2( size );
	}
	
	//! connects window signals to imgui events
	void connectSignals( ci::app::WindowRef window )
	{
		window->getSignalMouseDown().connect( mouseDown );
		window->getSignalMouseUp().connect( mouseUp );
		window->getSignalMouseDrag().connect( mouseDrag );
		window->getSignalMouseMove().connect( mouseMove );
		window->getSignalMouseWheel().connect( mouseWheel );
		window->getSignalKeyDown().connect( keyDown );
		window->getSignalKeyUp().connect( keyUp );
		window->getSignalResize().connect( [](){
			setSize( getWindowSize() );
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
        style.Colors[ImGuiCol_ScrollbarGrab]		= color4;
        style.Colors[ImGuiCol_ScrollbarGrabHovered]	= color0;
        style.Colors[ImGuiCol_ScrollbarGrabActive]	= color3;
        style.Colors[ImGuiCol_ComboBg]				= color2;
        style.Colors[ImGuiCol_CheckActive]			= color3;
        style.Colors[ImGuiCol_CheckHovered]			= color4;
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
        
        ImGuiStyle& style            = ImGui::GetStyle();
        style.WindowPadding          = ImVec2( 10,10 );
        style.WindowMinSize          = ImVec2( 160,80 );
        style.FramePadding           = ImVec2( 4,4 );
        style.ItemSpacing            = ImVec2( 10,5 );
        style.ItemInnerSpacing       = ImVec2( 5,5 );
        style.WindowFillAlphaDefault = 0.7;
        style.WindowRounding         = 4;
        style.TreeNodeSpacing		 = 22;
        style.ColumnsMinSpacing		 = 50;
        style.ScrollBarWidth         = 12;
    }
    
    //! sets dark theme style
    void setDarkTheme()
    {
        ImGui::GetStyle().WindowRounding = 4.0f;
        setThemeColor( ImVec4( 0.16f,0.16f, 0.16f, 1.0f ),
                      ImVec4( 0.04f, 0.04f, 0.04f, 1.0f ),
                      ImVec4( 0.20f, 0.20f, 0.20f, 1.0f ),
                      ImVec4( 0.69f, 0.69f, 0.69f, 1.0f ),
                      ImVec4( 0.27f, 0.27f, 0.27f, 1.0f ) );
        
        ImGuiStyle& style            = ImGui::GetStyle();
        style.WindowPadding          = ImVec2( 10, 10 );
        style.WindowMinSize          = ImVec2( 160, 80 );
        style.FramePadding           = ImVec2( 4, 4 );
        style.ItemSpacing            = ImVec2( 10, 5 );
        style.ItemInnerSpacing       = ImVec2( 5, 5 );
        style.WindowFillAlphaDefault = 0.7;
        style.WindowRounding         = 4;
        style.TreeNodeSpacing		 = 22;
        style.ColumnsMinSpacing		 = 50;
        style.ScrollBarWidth         = 12;
    }
    
    
    //! sets font
    void setFont( ci::DataSourceRef font, ci::DataSourceRef texture )
    {
        ImGui::Renderer::setFont( font, texture );
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
            cout << "re-init " << total_vtx_count << endl;
        }
        
        int vtx_consumed = 0;
        {
            gl::VboRef vbo = getVbo();
            gl::ScopedBuffer scopedBuffer( vbo );
            RenderData *data = static_cast<RenderData*>( vbo->map( GL_WRITE_ONLY ) );
            
            for (int n = 0; n < cmd_lists_count; n++) {
                const ImDrawList* cmd_list = cmd_lists[n];
                if (!cmd_list->vtx_buffer.empty()) {
                    vtx_consumed += cmd_list->vtx_buffer.size();
                    int count = cmd_list->vtx_buffer.size();
                    for( int i = 0; i < count; i++ ){
                        ImU32 color = cmd_list->vtx_buffer[i].col;
                        uint32_t a = color >> 24 & 255;
                        uint32_t b = color >> 16 & 255;
                        uint32_t g = color >> 8 & 255;
                        uint32_t r = color >> 0 & 255;
                        
                        RenderData d;
                        d.pos   = vec2( cmd_list->vtx_buffer[i].pos );
                        d.uv    = vec2( cmd_list->vtx_buffer[i].uv );
                        d.color = vec4( r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f  );
                        *data = d;
                        ++data;
                    }
                }
            }
            
            vbo->unmap();
        }
        
        gl::ScopedVao           scopedVao( getVao() );
        gl::ScopedGlslProg      scopedShader( getGlslProg() );
        gl::ScopedTextureBind   scopedTexture( getFontTextureRef(), 0 );
        
        getGlslProg()->uniform( "uModelViewProjection", gl::getModelViewProjection() );
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
                const ImDrawCmd& cmd = *pcmd++;
                
                gl::ScopedScissor scissors( vec2( cmd.clip_rect.x, GetIO().DisplaySize.y-cmd.clip_rect.y-cmd.clip_rect.w), vec2( cmd.clip_rect.z, cmd.clip_rect.w ) );
                
                glDrawArrays( GL_TRIANGLES, vtx_consumed, cmd.vtx_count );
                vtx_consumed += cmd.vtx_count;
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
    void Renderer::initBuffers( int size )
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
            mShader = gl::GlslProg::create( gl::GlslProg::Format()
                                           .vertex(
                                                   CI_GLSL( 150,
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
                                                           } ) )
                                           .fragment(
                                                     CI_GLSL( 150,
                                                             
                                                             in vec2            vUv;
                                                             in vec4            vColor;
                                                             
                                                             uniform sampler2D  uTex;
                                                             
                                                             out vec4           oColor;
                                                             
                                                             void main() {
                                                                 vec4 color = texture( uTex, vec2( vUv.x, 1.0 - vUv.y ) ) * vColor;
                                                                 //vec2 pixel_pos = gl_FragCoord.st;
                                                                 //pixel_pos.y = uHeight - pixel_pos.y;
                                                                 // Clipping: using discard
                                                                 //   if (pixel_pos.x < uClipRect.x || pixel_pos.y < uClipRect.y || pixel_pos.x > uClipRect.z || pixel_pos.y > uClipRect.w) discard;
                                                                 // Clipping: using discard and step
                                                                 //   if (step(uClipRect.x,pixel_pos.x) * step(uClipRect.y,pixel_pos.y) * step(pixel_pos.x,uClipRect.z) * step(pixel_pos.y,uClipRect.w) < 1.0f) discard;
                                                                 // Clipping: branch-less
                                                                 //color.w *= (step(uClipRect.x,pixel_pos.x) * step(uClipRect.y,pixel_pos.y) * step(pixel_pos.x,uClipRect.z) * step(pixel_pos.y,uClipRect.w));
                                                                 oColor = color;
                                                             } ) )
                                           .attribLocation( "iPosition", 0 )
                                           .attribLocation( "iUv", 1 )
                                           .attribLocation( "iColor", 2 )
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
        format.setMagFilter( GL_LINEAR );
        format.setMinFilter( GL_LINEAR );
        mFontTexture = gl::Texture::create( loadImage( texture ), format );
    }
}

