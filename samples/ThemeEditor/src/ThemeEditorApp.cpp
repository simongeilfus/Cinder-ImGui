#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"

#include "CinderImGui.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class ThemeEditorApp : public AppNative {
  public:
	void setup();
	void shutdown();
	void draw();
	ImGuiIO io;
	// Shader variables
	static int shader_handle, vert_handle, frag_handle;
	static int texture_location, proj_mtx_location;
	static int position_location, uv_location, colour_location;
	const static size_t vbo_max_size = 20000;
	static unsigned int vbo_handle, vao_handle;
};
static void ImImpl_RenderDrawLists(ImDrawList** const cmd_lists, int cmd_lists_count)
{
	/*if (cmd_lists_count == 0)
		return;

	// Setup render state: alpha-blending enabled, no face culling, no depth testing, scissor enabled
	glEnable(GL_BLEND);
	glBlendEquation(GL_FUNC_ADD);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_SCISSOR_TEST);
	glActiveTexture(GL_TEXTURE0);

	// Setup orthographic projection matrix
	const float width = ImGui::GetIO().DisplaySize.x;
	const float height = ImGui::GetIO().DisplaySize.y;
	const float ortho_projection[4][4] =
	{
		{ 2.0f / width, 0.0f, 0.0f, 0.0f },
		{ 0.0f, 2.0f / -height, 0.0f, 0.0f },
		{ 0.0f, 0.0f, -1.0f, 0.0f },
		{ -1.0f, 1.0f, 0.0f, 1.0f },
	};
	glUseProgram(shader_handle);
	glUniform1i(texture_location, 0);
	glUniformMatrix4fv(proj_mtx_location, 1, GL_FALSE, &ortho_projection[0][0]);

	// Grow our buffer according to what we need
	size_t total_vtx_count = 0;
	for (int n = 0; n < cmd_lists_count; n++)
		total_vtx_count += cmd_lists[n]->vtx_buffer.size();
	glBindBuffer(GL_ARRAY_BUFFER, vbo_handle);
	size_t neededBufferSize = total_vtx_count * sizeof(ImDrawVert);
	if (neededBufferSize > vbo_max_size)
	{
		vbo_max_size = neededBufferSize + 5000;  // Grow buffer
		glBufferData(GL_ARRAY_BUFFER, vbo_max_size, NULL, GL_STREAM_DRAW);
	}

	// Copy and convert all vertices into a single contiguous buffer
	unsigned char* buffer_data = (unsigned char*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
	if (!buffer_data)
		return;
	for (int n = 0; n < cmd_lists_count; n++)
	{
		const ImDrawList* cmd_list = cmd_lists[n];
		memcpy(buffer_data, &cmd_list->vtx_buffer[0], cmd_list->vtx_buffer.size() * sizeof(ImDrawVert));
		buffer_data += cmd_list->vtx_buffer.size() * sizeof(ImDrawVert);
	}
	glUnmapBuffer(GL_ARRAY_BUFFER);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(vao_handle);

	int cmd_offset = 0;
	for (int n = 0; n < cmd_lists_count; n++)
	{
		const ImDrawList* cmd_list = cmd_lists[n];
		int vtx_offset = cmd_offset;
		const ImDrawCmd* pcmd_end = cmd_list->commands.end();
		for (const ImDrawCmd* pcmd = cmd_list->commands.begin(); pcmd != pcmd_end; pcmd++)
		{
			glBindTexture(GL_TEXTURE_2D, (GLuint)(intptr_t)pcmd->texture_id);
			glScissor((int)pcmd->clip_rect.x, (int)(height - pcmd->clip_rect.w), (int)(pcmd->clip_rect.z - pcmd->clip_rect.x), (int)(pcmd->clip_rect.w - pcmd->clip_rect.y));
			glDrawArrays(GL_TRIANGLES, vtx_offset, pcmd->vtx_count);
			vtx_offset += pcmd->vtx_count;
		}
		cmd_offset = vtx_offset;
	}

	// Restore modified state
	glBindVertexArray(0);
	glUseProgram(0);
	glDisable(GL_SCISSOR_TEST);
	glBindTexture(GL_TEXTURE_2D, 0);*/
}

static const char* ImImpl_GetClipboardTextFn()
{
	//return glfwGetClipboardString(window);
}
static void ImImpl_SetClipboardTextFn(const char* text)
{
	//glfwSetClipboardString(window, text);
}
void ThemeEditorApp::setup()
{
    // set ui window and io events callbacks
    ImGui::connectWindow( getWindow() );
	ImGui::initialize();
	//io = ImGui::GetIO();

	/*
	
	ui::initialize( ui::Options()
                    .fonts( {
                        { getAssetPath( "Kontrapunkt Bob Light.ttf" ), 12 },
                        { getAssetPath( "Kontrapunkt Bob Bold.ttf" ), 20 },
                        { getAssetPath( "FontAwesome.ttf" ), 12 }
                    } )
                    .fontGlyphRanges( "FontAwesome", { 0xf000, 0xf06e, 0 } )
                );
	
	
	io.RenderDrawListsFn = ImImpl_RenderDrawLists;
	//io.SetClipboardTextFn = ImImpl_SetClipboardTextFn;
	//io.GetClipboardTextFn = ImImpl_GetClipboardTextFn;
	// LoadFontTexture
	unsigned char* pixels;
	int width, height;
	io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);   // Load as RGBA 32-bits for OpenGL3 demo because it is more likely to be compatible with user's existing shader.

	GLuint tex_id;
	glGenTextures(1, &tex_id);
	glBindTexture(GL_TEXTURE_2D, tex_id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

	// Store our identifier
	io.Fonts->TexID = (void *)(intptr_t)tex_id;*/
}
void ThemeEditorApp::shutdown()
{
    // close ui and save settings
    ImGui::Shutdown();
}

void ThemeEditorApp::draw()
{
	// clear out the window with black
    static float f = 0.0f;
	gl::clear( Color( f, f, f ) );
    
    ImGui::NewFrame();
    
    // start a new window
    ImGui::Begin("Theme Editor", NULL, ImVec2(200,100));
    {
        // our theme variables
        static ImVec4 color0                = ImVec4( 0.16f,0.16f, 0.16f, 1.0f );
        static ImVec4 color1                = ImVec4( 0.04f, 0.04f, 0.04f, 1.0f );
        static ImVec4 color2                = ImVec4( 0.20f, 0.20f, 0.20f, 1.0f );
        static ImVec4 color3                = ImVec4( 0.69f, 0.69f, 0.69f, 1.0f );
        static ImVec4 color4                = ImVec4( 0.27f, 0.27f, 0.27f, 1.0f );
        
        static float WindowPadding[2]       = { 25, 10 };
        static float WindowMinSize[2]       = { 160, 80 };
        static float FramePadding[2]        = { 4, 4 };
        static float ItemSpacing[2]         = { 10, 5 };
        static float ItemInnerSpacing[2]    = { 5, 5 };
        
        static float WindowFillAlphaDefault = 0.7;
        static float WindowRounding         = 4;
        static float TreeNodeSpacing        = 22;
        static float ColumnsMinSpacing		= 50;
        static float ScrollBarWidth         = 12;
        
        if( ImGui::CollapsingHeader( "Colors" ) ){
            
            // add radios on one line to switch between color modes
            static ImGuiColorEditMode colorMode = ImGuiColorEditMode_RGB;
            ImGui::RadioButton("RGB", &colorMode, ImGuiColorEditMode_RGB);
            ImGui::SameLine();
            ImGui::RadioButton("HSV", &colorMode, ImGuiColorEditMode_HSV);
            ImGui::SameLine();
            ImGui::RadioButton("HEX", &colorMode, ImGuiColorEditMode_HEX);
            
            // change color mode
            ImGui::ColorEditMode(colorMode);
            
            // add color controls
            ImGui::ColorEdit4( "Color0", (float*) &color0, false );
            ImGui::ColorEdit4( "Color1", (float*) &color1, false );
            ImGui::ColorEdit4( "Color2", (float*) &color2, false );
            ImGui::ColorEdit4( "Color3", (float*) &color3, false );
            ImGui::ColorEdit4( "Color4", (float*) &color4, false );
        }
        
        if( ImGui::CollapsingHeader( "Layout" ) ){
            
            // layout controls
            ImGui::SliderFloat2( "WindowPadding", WindowPadding, 0, 50, "%.0f" );
            ImGui::SliderFloat2( "WindowMinSize", WindowMinSize, 0, 250, "%.0f" );
            ImGui::SliderFloat2( "FramePadding", FramePadding, 0, 40, "%.0f" );
            ImGui::SliderFloat2( "ItemSpacing", ItemSpacing, 0, 40, "%.0f" );
            ImGui::SliderFloat2( "ItemInnerSpacing", ItemInnerSpacing, 0, 40, "%.0f" );
            ImGui::SliderFloat( "WindowFillAlphaDefault", &WindowFillAlphaDefault, 0, 1 );
            ImGui::SliderFloat( "WindowRounding", &WindowRounding, 0, 15, "%.0f" );
            ImGui::SliderFloat( "TreeNodeSpacing", &TreeNodeSpacing, 0, 50, "%.0f" );
            ImGui::SliderFloat( "ColumnsMinSpacing", &ColumnsMinSpacing, 0, 150, "%.0f" );
            ImGui::SliderFloat( "ScrollBarWidth", &ScrollBarWidth, 0, 35, "%.0f" );
        }
        
        
        // add a checkbox
        static bool apply = false;
        ImGui::Checkbox( "Apply", &apply );
        ImGui::SameLine();
        if( ImGui::Button( "Reset" ) ){
            color0 = ImVec4( 0.16f,0.16f, 0.16f, 1.0f );
            color1 = ImVec4( 0.04f, 0.04f, 0.04f, 1.0f );
            color2 = ImVec4( 0.20f, 0.20f, 0.20f, 1.0f );
            color3 = ImVec4( 0.69f, 0.69f, 0.69f, 1.0f );
            color4 = ImVec4( 0.27f, 0.27f, 0.27f, 1.0f );
            
            WindowPadding[0] = WindowPadding[1]       = 8;
            WindowMinSize[0] = WindowMinSize[1]       = 48;
            FramePadding[0] = 5; FramePadding[1]        = 4;
            ItemSpacing[0] = 10; ItemSpacing[1]         = 5;
            ItemInnerSpacing[0] = ItemInnerSpacing[1]    = 5;
            
            WindowFillAlphaDefault = 0.70f;
            WindowRounding			= 4.0f;
            TreeNodeSpacing			= 22.0f;
            ColumnsMinSpacing		= 6.0f;
            ScrollBarWidth			= 16.0f;
        }
        
        // show tooltip over checkbox
        if( ImGui::IsItemHovered() ){
            ImGui::SetTooltip( "Apply the theme colors" );
        }
        
        // if apply is checked make modifications
        if( apply ){
            //ImGui::setThemeColor( color0, color1, color2, color3, color4 );
            
            ImGui::GetStyle().WindowPadding          = ImVec2( WindowPadding[0], WindowPadding[1] );
            ImGui::GetStyle().WindowMinSize          = ImVec2( WindowMinSize[0], WindowMinSize[1] );
            ImGui::GetStyle().FramePadding           = ImVec2( FramePadding[0], FramePadding[1] );
            ImGui::GetStyle().ItemSpacing            = ImVec2( ItemSpacing[0], ItemSpacing[1] );
            ImGui::GetStyle().ItemInnerSpacing       = ImVec2( ItemInnerSpacing[0], ItemInnerSpacing[1] );
            ImGui::GetStyle().WindowFillAlphaDefault = WindowFillAlphaDefault;
            ImGui::GetStyle().WindowRounding         = WindowRounding;
            ImGui::GetStyle().IndentSpacing				= TreeNodeSpacing;
            ImGui::GetStyle().ColumnsMinSpacing		 = ColumnsMinSpacing;
            ImGui::GetStyle().GrabMinSize		 = ScrollBarWidth;
        }
        
        // if log pressed generate code and output to the console
        if( ImGui::Button( "Log to Console" ) ){
            console() << "ImVec4 color0 = ImVec4( " << color0.x << ", " << color0.y << ", " << color0.z << ", " << color0.w << " );" << endl
            << "ImVec4 color1 = ImVec4( " << color1.x << ", " << color1.y << ", " << color1.z << ", " << color1.w << " );" << endl
            << "ImVec4 color2 = ImVec4( " << color2.x << ", " << color2.y << ", " << color2.z << ", " << color2.w << " );" << endl
            << "ImVec4 color3 = ImVec4( " << color3.x << ", " << color3.y << ", " << color3.z << ", " << color3.w << " );" << endl
            << "ImVec4 color4 = ImVec4( " << color4.x << ", " << color4.y << ", " << color4.z << ", " << color4.w << " );" << endl
            << "ImGui::setThemeColor( color0, color1, color2, color3, color4 );" << endl << endl
            << "ImGuiStyle& style = ImGui::GetStyle();" << endl
            << "style.WindowPadding          = ImVec2( " << WindowPadding[0] << "," << WindowPadding[1] << " );" << endl
            << "style.WindowMinSize          = ImVec2( " << WindowMinSize[0] << "," << WindowMinSize[1] << " );" << endl
            << "style.FramePadding           = ImVec2( " << FramePadding[0] << "," << FramePadding[1] << " );" << endl
            << "style.ItemSpacing            = ImVec2( " << ItemSpacing[0] << "," << ItemSpacing[1] << " );" << endl
            << "style.ItemInnerSpacing       = ImVec2( " << ItemInnerSpacing[0] << "," << ItemInnerSpacing[1] << " );" << endl
            << "style.WindowFillAlphaDefault = " << WindowFillAlphaDefault << ";" << endl
            << "style.WindowRounding         = " << WindowRounding << ";" << endl
            << "style.TreeNodeSpacing		 = " << TreeNodeSpacing << ";" << endl
            << "style.ColumnsMinSpacing		 = " << ColumnsMinSpacing << ";" << endl
            << "style.ScrollBarWidth		 = " << ScrollBarWidth << ";" << endl;
        }
        
        
        // add a slider to control the background brightness
        ImGui::SliderFloat( "Background", &f, 0, 1 );
    }
    ImGui::End();
    
    
    // test Window
    ImGui::SetNextWindowPos(ImVec2(350, 20)); //SetNewWindowDefaultPos(ImVec2(350, 20));
    ImGui::ShowTestWindow(NULL);
    
    ImGui::Render();
}

CINDER_APP_NATIVE( ThemeEditorApp, RendererGl )
