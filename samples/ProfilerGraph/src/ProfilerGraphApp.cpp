#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"
#include "cinder/Perlin.h"

#include "Timer.h"

#include "imGuiCinder.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class ProfilerGraphApp : public AppNative {
  public:
	void setup();
	void shutdown();
	void update();
	void draw();
    
    void startProfiling();
    void stopProfiling();
    
    muon::Timer mGpuTimer;
    muon::Timer mCpuTimer;
    vector<float> mGpuTimes;
    vector<float> mCpuTimes;
};

void ProfilerGraphApp::setup()
{
    // set ui window and io events callbacks
    ImGui::setWindow( getWindow() );
    ImGui::setFont( loadAsset( "SourceCodePro.fnt" ), loadAsset( "SourceCodePro_0.png" ) );
    
    mCpuTimer.setMode( muon::Timer::CPU );
    
    // fill the two buffers with zeros
    for( int i = 0; i < 300; i++ ){
        mCpuTimes.push_back( 0.0f );
        mGpuTimes.push_back( 0.0f );
    }
}
void ProfilerGraphApp::shutdown()
{
    // close ui and save settings
    ImGui::Shutdown();
}

void ProfilerGraphApp::update()
{
}

void ProfilerGraphApp::draw()
{
    startProfiling();
    
	// clear out the window with black
	gl::clear( Color( 0.12, 0.12, 0.12 ) );
    
    // render ui
    ImGui::NewFrame();
    
    static int numSpheres = 1;
    static int sphereSegments = 12;
    ImGui::Begin("Render" );
    {
        ImGui::SliderInt( "Num Spheres", &numSpheres, 0, 5000 );
        ImGui::SliderInt( "Sphere Segments", &sphereSegments, 4, 128 );
    }
    ImGui::End();


    ImGui::Begin("Time Profiles", NULL, ImVec2(400,300) );
    {
        ImGui::Text( "FPS %.0f", getAverageFps() );
        if( getAverageFps() < 30 ){ ImGui::SameLine(); ImGui::Button( "LOW FPS" ); }
        ImGui::PlotLines( "", &mCpuTimes.front(), mCpuTimes.size(), 0, "CPU", FLT_MAX, FLT_MAX, ImVec2(0,70) );
        ImGui::SameLine(); ImGui::Text( "%.3f ms", mCpuTimes.back() );
        ImGui::PlotLines( "", &mGpuTimes.front(), mGpuTimes.size(), 0, "GPU", FLT_MAX, FLT_MAX, ImVec2(0,70) );
        ImGui::SameLine(); ImGui::Text( "%.3f ms", mGpuTimes.back() );
    }
    ImGui::End();
    
    gl::enableWireframe();
    for( int i = 0; i < numSpheres; i++ ){
        gl::drawSphere( Vec3f( getWindowWidth() * 0.5f, getWindowHeight() * 0.5f, 0.0f ), 300.0f, sphereSegments );
    }
    gl::disableWireframe();
    
    ImGui::Render();
    
    stopProfiling();
}

void ProfilerGraphApp::startProfiling()
{
    mGpuTimer.start();
    mCpuTimer.start();
}
void ProfilerGraphApp::stopProfiling()
{
    mGpuTimer.stop();
    mGpuTimes.push_back( static_cast<float>( mGpuTimer.getSeconds() ) );
    mGpuTimes.erase( mGpuTimes.begin() );
    
    mCpuTimer.stop();
    mCpuTimes.push_back( static_cast<float>( mCpuTimer.getSeconds() ) );
    mCpuTimes.erase( mCpuTimes.begin() );
}

CINDER_APP_NATIVE( ProfilerGraphApp, RendererGl )
