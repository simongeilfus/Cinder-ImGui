#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "CinderImGui.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class BasicApp : public App {
  public:
	void setup() override;
	void mouseDown( MouseEvent event ) override;
	void update() override;
	void draw() override;
};

void BasicApp::setup()
{
	// this will create the renderer and initialize the ui
	// resources. Pass a ui::Options() argument if you want
	// to change the default settings
	ui::initialize();
}

void BasicApp::mouseDown( MouseEvent event )
{
}

void BasicApp::update()
{
	static bool showTestWindow = true;
	static bool showWindowWithMenu = false;
	
	// a few scoped object like ScopedWindow allow to quickly
	// push and pop states
	if( showWindowWithMenu ) {
		ui::ScopedWindow window( "Window with Menu", ImGuiWindowFlags_MenuBar );
		
		// setup the window menu bar
		if( ui::BeginMenuBar() ){
			if( ui::BeginMenu( "Edit" ) ){
				ui::MenuItem( "Copy" );
				ui::MenuItem( "Cut" );
				ui::MenuItem( "Paste" );
				ui::EndMenu();
			}
			ui::EndMenuBar();
		}
		
		// add some text
		ui::Text( "Window Content" );
	}
	
	// create the main menu bar
	{
		ui::ScopedMainMenuBar menuBar;
		
		// add a file menu
		if( ui::BeginMenu( "File" ) ){
			ui::MenuItem( "Open" );
			ui::MenuItem( "Save" );
			ui::MenuItem( "Save As" );
			ui::EndMenu();
		}
		
		// and a view menu
		if( ui::BeginMenu( "View" ) ){
			ui::MenuItem( "TestWindow", nullptr, &showTestWindow );
			ui::MenuItem( "Window with Menu", nullptr, &showWindowWithMenu );
			ui::EndMenu();
		}
	}
	
	if( showTestWindow ){
		// have a look at this function for more examples
		ui::ShowTestWindow();
	}
}

void BasicApp::draw()
{
	static float red = 0.0f;
	gl::clear( Color( red, 0, 0 ) );
	
	// any widget added without a window will be added
	// in the default "debug" window
	ui::DragFloat( "Red", &red, 0.01f, 0.0f, 1.0f );
}

CINDER_APP( BasicApp, RendererGl )
