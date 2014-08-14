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
	void draw();
    
    void startProfiling();
    void stopProfiling();
    
    muon::Timer     mGpuTimer;
    muon::Timer     mCpuTimer;
    deque<float>    mGpuTimes;
    deque<float>    mCpuTimes;
    
    bool            mPauseProfiling = false;
    float           mMin            = 0.0f;
    float           mMax            = 0.0f;
};

void ProfilerGraphApp::setup()
{
    // set ui window and io events callbacks
    ImGui::setWindow( getWindow() );
    
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

void ProfilerGraphApp::draw()
{
    startProfiling();
    
	// clear out the window with black
	gl::clear( Color( 0.12, 0.12, 0.12 ) );
    
    // render ui
    ImGui::NewFrame();
    
    static int numSpheres = 1;
    static int sphereSegments = 12;
    
    // start the "rendering" window
    ImGui::Begin("Render" );
    {
        ImGui::SliderInt( "Num Spheres", &numSpheres, 0, 5000 );
        ImGui::SliderInt( "Sphere Segments", &sphereSegments, 4, 128 );
    }
    ImGui::End();

    // start the Profiling window
    ImGui::Begin("Time Profiles", NULL, ImVec2(400,300) );
    {
        // fps
        if( getAverageFps() < 30 ){ ImGui::Button( "LOW FPS!" ); ImGui::SameLine(); }
        ImGui::Text( "FPS %.0f", getAverageFps() );
        
        // find min / max
        float min = *min_element( mCpuTimes.begin(), mCpuTimes.end() );
        float max = *max_element( mCpuTimes.begin(), mCpuTimes.end() );
        
        // interpolate so we don't get huge jumps
        mMin += ( min - mMin ) * 0.15f;
        mMax += ( max - mMax ) * 0.15f;
        
        // cpu plot
        ImGui::PlotLines( "", &mCpuTimes.front(), mCpuTimes.size(), 0, "CPU", mMin, mMax, ImVec2(0,70) );
        if( ImGui::IsHovered() && ImGui::IsMouseClicked( 0 ) ){
            mPauseProfiling = !mPauseProfiling;
        }
        ImGui::SameLine(); ImGui::Text( "%.3f ms", mCpuTimes.back() );
        
        // gpu plot
        ImGui::PlotLines( "", &mGpuTimes.front(), mGpuTimes.size(), 0, "GPU", mMin, mMax, ImVec2(0,70) );
        if( ImGui::IsHovered() && ImGui::IsMouseClicked( 0 ) ){
            mPauseProfiling = !mPauseProfiling;
        }
        ImGui::SameLine(); ImGui::Text( "%.3f ms", mGpuTimes.back() );
    }
    ImGui::End();
    
    // draw spheres
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
    if( mPauseProfiling ) return;
    
    mGpuTimer.stop();
    mGpuTimes.push_back( static_cast<float>( mGpuTimer.getSeconds() ) );
    mGpuTimes.erase( mGpuTimes.begin(), mGpuTimes.begin() + 1 );
    
    mCpuTimer.stop();
    mCpuTimes.push_back( static_cast<float>( mCpuTimer.getSeconds() ) );
    mCpuTimes.erase( mCpuTimes.begin(), mCpuTimes.begin() + 1 );
}

CINDER_APP_NATIVE( ProfilerGraphApp, RendererGl )
