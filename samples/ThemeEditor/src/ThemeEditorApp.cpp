#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"

#include "imGuiCinder.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class ThemeEditorApp : public AppNative {
  public:
	void setup();
	void shutdown();
	void draw();
};

void ThemeEditorApp::setup()
{
    // set ui window and io events callbacks
    ImGui::setWindow( getWindow() );
    
    // change the font. needs to be in BMFont file format (http://www.angelcode.com/products/bmfont/ )
    ImGui::setFont( loadAsset( "SourceCodePro.fnt" ), loadAsset( "SourceCodePro_0.png" ) );
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
    
    ImGui::Begin("Theme Editor", NULL, ImVec2(200,100));
    {
        // add radios on one line to switch between color modes
        static ImGuiColorEditMode colorMode = ImGuiColorEditMode_RGB;
        ImGui::RadioButton("RGB", &colorMode, ImGuiColorEditMode_RGB);
        ImGui::SameLine();
        ImGui::RadioButton("HSV", &colorMode, ImGuiColorEditMode_HSV);
        ImGui::SameLine();
        ImGui::RadioButton("HEX", &colorMode, ImGuiColorEditMode_HEX);
        
        // add a slider to control the background brightness
        ImGui::SliderFloat( "Background", &f, 0, 1 );
        
        // change color mode
        ImGui::ColorEditMode(colorMode);
        
        // add color controls
        static ImVec4 color0 = ImVec4( 0.16f,0.16f, 0.16f, 1.0f );
        static ImVec4 color1 = ImVec4( 0.04f, 0.04f, 0.04f, 1.0f );
        static ImVec4 color2 = ImVec4( 0.20f, 0.20f, 0.20f, 1.0f );
        static ImVec4 color3 = ImVec4( 0.96f, 0.92f, 0.63f, 1.0f );
        static ImVec4 color4 = ImVec4( 0.27f, 0.27f, 0.27f, 1.0f );
        ImGui::ColorEdit4( "Color0", (float*) &color0, false );
        ImGui::ColorEdit4( "Color1", (float*) &color1, false );
        ImGui::ColorEdit4( "Color2", (float*) &color2, false );
        ImGui::ColorEdit4( "Color3", (float*) &color3, false );
        ImGui::ColorEdit4( "Color4", (float*) &color4, false );
        
        // add a checkbox
        static bool apply = false;
        ImGui::Checkbox( "Apply", &apply );
        
        // show tooltip over checkbox
        if( ImGui::IsHovered() ){
            ImGui::SetTooltip( "Apply the theme colors" );
        }
        
        // if apply is checked make modifications
        if( apply ){
            ImGui::setThemeColor( color0, color1, color2, color3, color4 );
        }
    }
    ImGui::End();
    
    
    // test Window
    ImGui::SetNewWindowDefaultPos(ImVec2(350, 20));
    ImGui::ShowTestWindow(NULL);
    
    ImGui::Render();
}

CINDER_APP_NATIVE( ThemeEditorApp, RendererGl )
