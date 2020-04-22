# Dear-Imgui is now officially shipped with Cinder. Please use that implementation instead:  

https://github.com/cinder/Cinder/blob/master/include/cinder/CinderImGui.h (See samples update [here](https://github.com/cinder/Cinder/pull/2146/commits/f70f28093c740c7f0247b2d1327030682b8a888a)). This repo will be archived but you can still open issues and pull request [here](https://github.com/cinder/cinder) to help progress the impl. 



Cinder-ImGui
===================
### [Immediate mode GUI Library](https://github.com/ocornut/imgui/) from [Omar Cornut](https://github.com/ocornut) wrapped for use with Cinder 0.9.

<sub>(ImGui is free but Omar needs your support to sustain development and maintenance. If you work for a company, please consider financial support)</sub>

[![Patreon](https://cloud.githubusercontent.com/assets/8225057/5990484/70413560-a9ab-11e4-8942-1a63607c0b00.png)](http://www.patreon.com/imgui) [![PayPal](https://www.paypalobjects.com/en_US/i/btn/btn_donate_LG.gif)](https://www.paypal.com/cgi-bin/webscr?cmd=_s-xclick&hosted_button_id=5Q73FPZ9C526U)


ImGui is a bloat-free graphical user interface library for C++. It outputs vertex buffers that you can render in your 3D-pipeline enabled application. It is portable, renderer agnostic and self-contained (no external dependencies). It is based on an "immediate mode" graphical user interface paradigm which enables you to build user interfaces with ease.

ImGui is designed to enable fast iteration and empower programmers to create content creation tools and visualization/debug tools (as opposed to UI for the average end-user). It favors simplicity and productivity toward this goal, and thus lacks certain features normally found in more high-level libraries.

ImGui is particularly suited to integration in realtime 3D applications, fullscreen applications, embedded applications, games, or any applications on consoles platforms where operating system features are non-standard.

### Getting Started
Download or check out this repository into your `Cinder/blocks` directory, then use Tinderbox to create a new project using ImGui.

Once the project is created you'll need to include the header in your `.cpp` file:
```c++
#include "CinderImGui.h"
```

Initialize ImGui at setup:
```c++
void CinderApp::setup()
{
    ImGui::initialize();
}
```

Then you're ready to start drawing components:
```c++
void CinderApp::draw()
{
    gl::clear( Color( 0, 0, 0 ) );
    ImGui::Text("Hello, world!");
}
```

### Namespace
For ease of use I added a namespace alias for ImGui, feel free to disable it by defining `CINDER_IMGUI_NO_NAMESPACE_ALIAS`.

### Advanced Initialization
You can provide an `ui::Options` object to the initialize method to setup the UI the way you want:
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

### UI Creation
By default or if you don't specify an empty windowRef, the wrapper will take care of calling `ImGui::NewFrame` and `ImGui::Render`, meaning that you don't have to worry about anything else than the actual UI. You can add UI code in any place you want, that's it. The Renderer takes care of setting the matrices and the proper shader to draw the UI through a `postDraw` signal. (You can disable this behavior through the initialization options).

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

### Scoped Objects
For the sake of simplifying a bit more the use of this lib, there's `Scoped*` objects for most push/pop functions. The state will be pushed when creating the object and popped at the end of its lifespan.
```c++
void SomeWindow()
{
    ui::ScopedWindow window( "Title" );
    ui::ScopedFont font( "Font-Bold" );

    ui::Text( "Some Bold Title" );
    ui::Image( mFbo->getColorTexture(), mFbo->getSize() );
}
```

### Todo
* fix keyboard events handling (modifiers not working for the moment)
* multi-window option


ImGui Credits (from [ImGui](https://github.com/ocornut/imgui/) README)
-------

Developed by [Omar Cornut](http://www.miracleworld.net) and every direct or indirect contributors to the GitHub. The early version of this library was developed with the support of [Media Molecule](http://www.mediamolecule.com) and first used internally on the game [Tearaway](http://tearaway.mediamolecule.com).

I first discovered imgui principles at [Q-Games](http://www.q-games.com) where Atman had dropped his own simple imgui implementation in the codebase, which I spent quite some time improving and thinking about. It turned out that Atman was exposed to the concept directly by working with Casey. When I moved to Media Molecule I rewrote a new library trying to overcome the flaws and limitations of the first one I've worked with. It became this library and since then I have spent an unreasonable amount of time iterating on it.

Embeds [ProggyClean.ttf](http://upperbounds.net) font by Tristan Grimmer (MIT license).

Embeds [stb_textedit.h, stb_truetype.h, stb_rectpack.h](https://github.com/nothings/stb/) by Sean Barrett (public domain).

Inspiration, feedback, and testing for early versions: Casey Muratori, Atman Binstock, Mikko Mononen, Emmanuel Briney, Stefan Kamoda, Anton Mikhailov, Matt Willis. And everybody posting feedback, questions and patches on the GitHub.

ImGui development is financially supported on [**Patreon**](http://www.patreon.com/imgui).

Special supporters:
- Jetha Chan, Wild Sheep Studio, Pastagames, Mārtiņš Možeiko, Daniel Collin, Stefano Cristiano.

And:
- Michel Courtine, César Leblic, Dale Kim, Alex Evans, Rui Figueira, Paul Patrashcu, Jerome Lanquetot, Ctrl Alt Ninja, Paul Fleming, Neil Henning, Stephan Dilly, Neil Blakey-Milner, Aleksei.

And other supporters; thanks!

ImGui License
-------

ImGui is licensed under the MIT License, see [LICENSE](https://github.com/ocornut/imgui/blob/master/LICENSE) for more information.
