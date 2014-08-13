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
	gl::clear( Color( 0, 0, 0 ) );
    
    ImGui::NewFrame();
    
    static float f;
    ImGui::Text("Hello, world!");
    ImGui::SliderFloat("float", &f, 0.0f, 1.0f);
    
    ImGui::Begin("Another Window", NULL, ImVec2(200,100));
    {
        static ImGuiColorEditMode colorMode = ImGuiColorEditMode_RGB;
        ImGui::RadioButton("RGB", &colorMode, ImGuiColorEditMode_RGB);
        ImGui::SameLine();
        ImGui::RadioButton("HSV", &colorMode, ImGuiColorEditMode_HSV);
        ImGui::SameLine();
        ImGui::RadioButton("HEX", &colorMode, ImGuiColorEditMode_HEX);
        
        ImGui::ColorEditMode(colorMode);
        
        static ImVec4 color0 = ImVec4( 0.11f,0.11f, 0.11f, 1.0f );
        static ImVec4 color1 = ImVec4( 0.04f, 0.04f, 0.04f, 1.0f );
        static ImVec4 color2 = ImVec4( 0.08f, 0.08f, 0.08f, 1.0f );
        static ImVec4 color3 = ImVec4( 0.96f, 0.92f, 0.63f, 1.0f );
        static ImVec4 color4 = ImVec4( 0.16f, 0.16f, 0.16f, 1.0f );
        ImGui::ColorEdit4( "Color0", (float*)&color0, false );
        ImGui::ColorEdit4( "Color1", (float*)&color1, false );
        ImGui::ColorEdit4( "Color2", (float*)&color2, false );
        ImGui::ColorEdit4( "Color3", (float*)&color3, false );
        ImGui::ColorEdit4( "Color4", (float*)&color4, false );
        
        static bool apply = true;
        ImGui::Checkbox( "Apply", &apply );
        if( apply ){
            ImGui::setThemeColor( color0, color1, color2, color3, color4 );
        }
        if( ImGui::IsHovered() ){
            ImGui::SetTooltip( "Apply the theme colors" );
        }
    }
    ImGui::End();
    
    
    // More example code in ShowTestWindow()
    ImGui::SetNewWindowDefaultPos(ImVec2(350, 20));
    ImGui::ShowTestWindow(NULL);
    
    ImGui::Render();
}

CINDER_APP_NATIVE( ThemeEditorApp, RendererGl )
