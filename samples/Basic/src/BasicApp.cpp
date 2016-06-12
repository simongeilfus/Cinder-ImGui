#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "CinderImGui.h"

using namespace ci;
using namespace ci::app;
using namespace std;

// Basic example class
struct Object {
	
	vec2 getPosition() const { return mPosition; }
	void setPosition( const ci::vec2& position ) { mPosition = position; }
	
	string	mName;
	Color	mColor;
	vec2	mPosition;
	float	mSize;
};

class BasicApp : public App {
  public:
	void setup() override;
	void mouseDown( MouseEvent event ) override;
	void update() override;
	void draw() override;
	
	vector<Object>	mObjects;
};

void BasicApp::setup()
{
	// this will create the renderer and initialize the ui
	// resources. Pass a ui::Options() argument if you want
	// to change the default settings
	ui::initialize();
	
	// add some starting dummy data
	mObjects = {
		{ "Object0", Color( 0.34f, 0.78f, 1.0f ), vec2( 200.0f ), 100.0f },
		{ "Object1", Color( 1.0f, 0.0f, 0.36f ), vec2( 20.0f, 100.0f ), 40.0f },
		{ "Object2", Color( 0.48f, 0.86f, 0.22f ), vec2( 100.0f, 350.0f ), 80.0f },
		{ "Object3", Color( 1.0f, 0.53f, 0.0f ), vec2( 450.0f, 100.0f ), 130.0f }
	};
}

void BasicApp::mouseDown( MouseEvent event )
{
}

void BasicApp::update()
{
	static bool showTestWindow = false;
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
	
	
	// Our List / Object selector
	static const Object* selection = &mObjects[1];
	{
		ui::ScopedWindow window( "Objects" );
		
		// add / remove buttons
		if( ui::Button( "Add" ) ) {
			static int objCount = mObjects.size();
			mObjects.push_back( { "Object" + to_string( objCount++ ), Color::white(), vec2( 0.0f ) } );
		}
		if( selection ) {
			ui::SameLine();
			if( ui::Button( "Remove" ) ) {
				auto it = std::find_if( mObjects.begin(), mObjects.end(), [] ( const Object& obj ) { return &obj == selection; } );
				if( it != mObjects.end() ) {
					mObjects.erase( it );
					selection = nullptr;
				}
			}
		}
		
		// selectable list
		ui::ListBoxHeader( "" );
		for( const auto& object : mObjects ) {
			if( ui::Selectable( object.mName.c_str(), selection == &object ) ) {
				selection = &object;
			}
		}
		ui::ListBoxFooter();
	}
	
	// The Object Inspector
	if( selection ) {
		ui::ScopedWindow window( "Inspector" );
		
		Object* object = (Object*) selection;
		ui::InputText( "Name", &object->mName );
		ui::ColorEdit3( "Color", &object->mColor[0] );
		// getter/setters are a bit longer but still possible
		vec2 pos = object->getPosition();
		if( ui::DragFloat2( "Position", &pos[0] ) ) object->setPosition( pos );
		ui::DragFloat( "Size", &object->mSize );
	}
}

void BasicApp::draw()
{
	static float gray = 0.65f;
	gl::clear( ColorA::gray( gray ) );
	
	// any widget added without a window will be added
	// in the default "debug" window
	ui::DragFloat( "Gray", &gray, 0.01f, 0.0f, 1.0f );
	
	// render our "objects"
	gl::ScopedBlendAlpha alphaBlending;
	for( auto object : mObjects ) {
		gl::color( object.mColor );
		gl::drawSolidCircle( object.getPosition(), object.mSize );
		gl::drawStringCentered( object.mName, vec2( ivec2( object.getPosition() ) ) - vec2( 0, 5 ), ColorA::black() );
	}
}

CINDER_APP(BasicApp, RendererGl( RendererGl::Options().msaa( 8 ) ) )
