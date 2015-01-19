Cinder-ImGui
===================
https://github.com/ocornut/imgui/   
Immediate mode GUI Library from Omar Cornut.   
Non-kosher wrapper for use with the latest Cinder version.

#####Namespace
For ease of use I added a namespace alias for ImGui, feel free to not use it

#####Initialization
This is the most basic initialization:
```c++
void CinderApp::setup()
{
    ui::initialize();
}
```
You can provide an ui::Options object to the initialize method to setup the ui the way you want:
```c++
void CinderApp::setup()
{
    ui::initialize( ui::Options().font( "font.ttf", 12 ).window( uiWindow ).frameRounding( 0.0f ) );
}
```

#####UI Creation
By default or if you don't specify an empty windowRef, the wrapper will take care of calling ImGui::NewFrame and ImGui::Render, meaning that you don't have to worry about anything else than the actual UI. You can add UI code in any place you want, that's it. The Renderer takes care of setting the matrices and the proper shader to draw the ui through a postDraw signal. (You can disable this behavior through the initialization options).

```c++
void CinderApp::draw()
{
    ui::Combo( "Blending", &blendMode, blendModes, 3 );
    ui::SliderInt( "Circles", &n, 0, 500 );
    ui::SliderFloat( "Min Radius", &minRadius, 1, 499 );
    ui::Image( mFbo->getColorTexture(), mFbo->getSize() );
}
void SomeFunctionCalledSomewhereElse()
{
    ui::Button( "MyButton" );
}
```

#####Scoped Objects
Being a big fan of Cinder's Opengl scoped objects, I decided to experiment with them in this wrapper, we'll see if I keep them or not.
```c++
void SomeWindow()
{
    ui::ScopedWindow window( "Title" );
    ui::ScopedFont font( "Font-Bold" );
    
    ui::Text( "Some Bold Title" );
    ui::Image( mFbo->getColorTexture(), mFbo->getSize() );
}
```

#####Todo
* fix keyboard events handling (modifiers not working for the moment)
* multi-window option


ImGui
=====

ImGui is a bloat-free graphical user interface library for C/C++. It is portable, renderer agnostic and carries minimal amount of dependencies (only 3 files are needed). It is based on an "immediate" graphical user interface paradigm which allows you to build simple user interfaces with ease.

ImGui is designed to allow programmers to create "content creation" or "debug" tools (as opposed to tools for the average end-user). It favors simplicity and thus lacks certain features normally found in more high-level libraries, such as string localisation.

Usage example:

![screenshot of sample code alongside its output with ImGui](https://raw.githubusercontent.com/ocornut/imgui/master/web/code_sample_01.png?raw=true)

ImGui outputs vertex buffers and simple command-lists that you can render in your application. Because it doesn't know or touch graphics state directly, you can call ImGui commands anywhere in your code (e.g. in the middle of a running algorithm, or in the middle of your own rendering process). Refer to the sample applications in the examples/ folder for instructions on how to integrate ImGui with your existing codebase. 


Gallery
-------

![screenshot 1](https://raw.githubusercontent.com/ocornut/imgui/master/web/test_window_01.png?raw=true)
![screenshot 2](https://raw.githubusercontent.com/ocornut/imgui/master/web/test_window_02.png?raw=true)
![screenshot 3](https://raw.githubusercontent.com/ocornut/imgui/master/web/test_window_03.png?raw=true)
![screenshot 4](https://raw.githubusercontent.com/ocornut/imgui/master/web/test_window_04.png?raw=true)

Credits
-------

Developed by [Omar Cornut](http://www.miracleworld.net). The library was developed with the support of [Media Molecule](http://www.mediamolecule.com) and first used internally on the game [Tearaway](http://tearaway.mediamolecule.com). 

Embeds [proggy_clean](http://www.proggyfonts.net/) font by Tristan Grimmer (also MIT license).

Inspiration, feedback, and testing: Casey Muratori, Atman Binstock, Mikko Mononen, Emmanuel Briney, Stefan Kamoda, Matt Willis. Thanks!

License
-------

ImGui is licensed under the MIT License, see LICENSE for more information.

