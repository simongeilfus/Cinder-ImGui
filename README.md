Cinder-ImGui
===================
https://github.com/ocornut/imgui/   
Immediate mode GUI Library from Omar Cornut.   
Non-kosher wrapper for use with the last Cinder version.

#####Basic Use
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

#####Todo
* fix keyboard events handling (modifiers not working for the moment)
* issue with combobox( seems to come from one of the last commit from the main repo.)



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
![screenshot 4](https://raw.githubusercontent.com/ocornut/imgui/master/test_window_04.png?raw=true)

Credits
-------

Developed by [Omar Cornut](http://www.miracleworld.net). The library was developed with the support of [Media Molecule](http://www.mediamolecule.com) and first used internally on the game [Tearaway](http://tearaway.mediamolecule.com). 

Embeds [proggy_clean](http://www.proggyfonts.net/) font by Tristan Grimmer (also MIT license).

Inspiration, feedback, and testing: Casey Muratori, Atman Binstock, Mikko Mononen, Emmanuel Briney, Stefan Kamoda, Matt Willis. Thanks!

License
-------

ImGui is licensed under the MIT License, see LICENSE for more information.

