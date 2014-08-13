Cinder-ImGui
===================
https://github.com/ocornut/imgui/   
Immediate mode GUI Library from Omar Cornut.   
Non-kosher wrapper for use with the last Cinder version.

####Basic Use
Call this in your setup:
```c++
void CinderApp::setup()
{
    // set ui window and io events callbacks
    ImGui::setWindow( getWindow() );
}
```

And then create your UI between ImGui::NewFrame and ImGui::Render:
```c++
void CinderApp::draw()
{
    // start gui
    ImGui::NewFrame();
    
    // make some nice ui here!
    
    // render gui
    ImGui::Render();
}
```
