Cinder-ImGui
===================
####Immediate mode GUI Library from Omar Cornut wrapped for use with Cinder.

ImGui is a bloat-free graphical user interface library for C/C++. It is portable, renderer agnostic and carries minimal amount of dependencies (only 3 files are needed). It is based on an "immediate" graphical user interface paradigm which allows you to build simple user interfaces with ease.

ImGui is designed to allow programmers to create "content creation" or "debug" tools (as opposed to tools for the average end-user). It favors simplicity and thus lacks certain features normally found in more high-level libraries, such as string localisation.

https://github.com/ocornut/imgui/   

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
    ui::initialize( ui::Options().font( ttfFontPath, 12 ).window( uiWindow ).frameRounding( 0.0f ) );
}
```
Multiple fonts and special glyphs are specified the same way (see ImGui docs for more info):
```c++
void CinderApp::setup()
{
    ui::initialize( ui::Options()
                    .fonts( {
                        { getAssetPath( "Kontrapunkt Bob Light.ttf" ), 12 },
                        { getAssetPath( "Kontrapunkt Bob Bold.ttf" ), 20 },
                        { getAssetPath( "FontAwesome.ttf" ), 12 }
                    } )
                    .fontGlyphRanges( "FontAwesome", { 0xf000, 0xf06e, 0 } )
                );
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
I'm a big fan of Cinder's Opengl scoped objects and for the sake of simplifying a bit more the use of this lib, there's Scoped* objects for most push/pop functions. The state will be pushed when creating the object and poped at the end of its lifespan.
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


Credits (from [ImGui](https://github.com/ocornut/imgui/) README)
-------

Developed by [Omar Cornut](http://www.miracleworld.net). The library was developed with the support of [Media Molecule](http://www.mediamolecule.com) and first used internally on the game [Tearaway](http://tearaway.mediamolecule.com). 

Embeds [proggy_clean](http://www.proggyfonts.net/) font by Tristan Grimmer (also MIT license).

Inspiration, feedback, and testing: Casey Muratori, Atman Binstock, Mikko Mononen, Emmanuel Briney, Stefan Kamoda, Matt Willis. Thanks!

License
-------

ImGui is licensed under the MIT License, see LICENSE for more information.

