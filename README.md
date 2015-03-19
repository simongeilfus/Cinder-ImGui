Cinder-ImGui
===================
####[Immediate mode GUI Library](https://github.com/ocornut/imgui/) from [Omar Cornut](https://github.com/ocornut) wrapped for use with Cinder([glNext](https://github.com/cinder/Cinder/tree/glNext)).

ImGui is a bloat-free graphical user interface library for C/C++. It is portable, renderer agnostic and carries minimal amount of dependencies (only 3 files are needed). It is based on an "immediate" graphical user interface paradigm which allows you to build simple user interfaces with ease.

ImGui is designed to allow programmers to create "content creation" or "debug" tools (as opposed to tools for the average end-user). It favors simplicity and thus lacks certain features normally found in more high-level libraries, such as string localisation.

#####Namespace
For ease of use I added a namespace alias for ImGui, feel free to disable it by defining CINDER_IMGUI_NO_NAMESPACE_ALIAS

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
For the sake of simplifying a bit more the use of this lib, there's Scoped* objects for most push/pop functions. The state will be pushed when creating the object and poped at the end of its lifespan.
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

Developed by [Omar Cornut](http://www.miracleworld.net) and every direct or indirect contributors to the GitHub. The early version of this library was developed with the support of [Media Molecule](http://www.mediamolecule.com) and first used internally on the game [Tearaway](http://tearaway.mediamolecule.com). 

Embeds [ProggyClean.ttf](http://upperbounds.net) font by Tristan Grimmer (MIT license).

Embeds [stb_textedit.h, stb_truetype.h, stb_rectpack.h](https://github.com/nothings/stb/) by Sean Barrett (public domain).

Inspiration, feedback, and testing for early versions: Casey Muratori, Atman Binstock, Mikko Mononen, Emmanuel Briney, Stefan Kamoda, Anton Mikhailov, Matt Willis. And everybody posting feedback, questions and patches on the GitHub.

ImGui is financially supported on [**Patreon**](http://www.patreon.com/imgui).

Special supporters 
- Jetha Chan

Supporters
- Michel Courtine

And other supporters; thanks!

License
-------

ImGui is licensed under the MIT License, see LICENSE for more information.
