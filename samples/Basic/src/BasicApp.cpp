#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"

#include "CinderImGui.h"

using namespace ci;
using namespace ci::app;
using namespace std;


class BasicApp : public AppNative {
  public:
	void setup();
	void draw();
	void shutdown();
};

void BasicApp::setup()
{
	// set ui window and io events callbacks
	//ImGui::connectWindow(getWindow());
	//ImGui::initialize();
	ui::initialize();
}

void BasicApp::shutdown()
{
    // close ui and save settings
   //ImGui::Shutdown();
	 ui::Shutdown();
}
void BasicApp::draw()
{
	//ui::Combo("Blending", &blendMode, blendModes, 3);
	static int n = 4;
	ui::SliderInt("Circles", &n, 0, 500);
	//ui::SliderFloat("Min Radius", &minRadius, 1, 499);
	//ui::Image(mFbo->getColorTexture(), mFbo->getSize());
    // start gui. all code lives between ImGui::NewFrame and ImGui::Render
    /*ImGui::NewFrame();
    
    static float f = 0.1f;
    static bool displayTestWindow = true;
    
    // add some text. the %.xf allows to specify float precision
    ImGui::Text( "Average FPS %.0f", getAverageFps() );
    // add a slider from 0 to 1
    ImGui::SliderFloat("Background", &f, 0.0f, 1.0f);
    // add a checkbox to display the test window
    ImGui::Checkbox( "Display TestWindow", &displayTestWindow );
    
    // more example code in ShowTestWindow()
    if( displayTestWindow ){
        ImGui::SetNextWindowPos( ImVec2(350, 20) );
        ImGui::ShowTestWindow( &displayTestWindow );
    }
    
	// clear out the window
	gl::clear( Color( f, f, f ) );
    
    // render gui
    ImGui::Render();*/
}

CINDER_APP_NATIVE( BasicApp, RendererGl )
