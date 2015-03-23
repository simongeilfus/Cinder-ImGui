/*
 Cinder-ImGui
 This code is intended for use with Cinder
 and Omar Cornut ImGui C++ libraries.

 http://libcinder.org
 https://github.com/ocornut

 Copyright (c) 2013-2015, Simon Geilfus - All rights reserved.

 Redistribution and use in source and binary forms, with or without modification, are permitted provided that
 the following conditions are met:

 * Redistributions of source code must retain the above copyright notice, this list of conditions and
 the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and
 the following disclaimer in the documentation and/or other materials provided with the distribution.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
 WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 POSSIBILITY OF SUCH DAMAGE.
 */

#include "CinderImGui.h"
#include "cinder/app/App.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Vbo.h"
#include "cinder/gl/Texture.h"
#include "cinder/gl/GlslProg.h"
#include "cinder/DataSource.h"
#include "cinder/Clipboard.h"
#include "cinder/CinderAssert.h"

using namespace std;
using namespace ci;
using namespace ci::app;


namespace ImGui {

	// static variables
	static bool sInitialized = false;

	// same as ImDrawVert
	struct RenderData {
		ci::Vec2f pos;
		ci::Vec2f uv;
		ci::ColorA color;
	};

	ImGui::Options::Options()
		: mWindow(ci::app::getWindow()) {
		mStyle.WindowPadding = ImVec2(10, 10);
		mStyle.WindowMinSize = ImVec2(160, 80);
		mStyle.FramePadding = ImVec2(4, 4);
		mStyle.ItemSpacing = ImVec2(8, 4);
		mStyle.ItemInnerSpacing = ImVec2(6, 4);
		mStyle.WindowFillAlphaDefault = 1.0f;
		mStyle.WindowRounding = 2.0f;
		mStyle.FrameRounding = 2.0f;
		mStyle.IndentSpacing = 6;
		mStyle.ColumnsMinSpacing = 50;
		mStyle.ScrollBarWidth = 12;

		dark();
	}

	ImGui::Options& ImGui::Options::window(const ci::app::WindowRef &window) {
		mWindow = window;
		return *this;
	}

	ImGui::Options& ImGui::Options::font(const ci::fs::path &fontPath, float size) {
		mFonts = { { fontPath, size } };
		return *this;
	}
	ImGui::Options& ImGui::Options::fonts(const std::vector<std::pair<ci::fs::path, float>> &fonts) {
		mFonts = fonts;
		return *this;
	}
	ImGui::Options& ImGui::Options::fontGlyphRanges(const std::string &name, const vector<ImWchar> &glyphRanges) {
		mFontsGlyphRanges[name] = glyphRanges;
		return *this;
	}
	ImGui::Options& ImGui::Options::alpha(float a) {
		mStyle.Alpha = a;
		return *this;
	}
	ImGui::Options& ImGui::Options::windowPadding(const ci::Vec2f &padding) {
		mStyle.WindowPadding = padding;
		return *this;
	}
	ImGui::Options& ImGui::Options::windowMinSize(const ci::Vec2f &minSize) {
		mStyle.WindowMinSize = minSize;
		return *this;
	}
	ImGui::Options& ImGui::Options::windowRounding(float rounding) {
		mStyle.WindowRounding = rounding;
		return *this;
	}
	ImGui::Options& ImGui::Options::framePadding(const ci::Vec2f &padding) {
		mStyle.FramePadding = padding;
		return *this;
	}
	ImGui::Options& ImGui::Options::frameRounding(float rounding) {
		mStyle.FrameRounding = rounding;
		return *this;
	}
	ImGui::Options& ImGui::Options::itemSpacing(const ci::Vec2f &spacing) {
		mStyle.ItemSpacing = spacing;
		return *this;
	}
	ImGui::Options& ImGui::Options::itemInnerSpacing(const ci::Vec2f &spacing) {
		mStyle.ItemInnerSpacing = spacing;
		return *this;
	}
	ImGui::Options& ImGui::Options::touchExtraPadding(const ci::Vec2f &padding) {
		mStyle.TouchExtraPadding = padding;
		return *this;
	}
	ImGui::Options& ImGui::Options::autoFitPadding(const ci::Vec2f &padding) {
		mStyle.AutoFitPadding = padding;
		return *this;
	}
	ImGui::Options& ImGui::Options::windowFillAlphaDefault(float defaultAlpha) {
		mStyle.WindowFillAlphaDefault = defaultAlpha;
		return *this;
	}
	ImGui::Options& ImGui::Options::treeNodeSpacing(float spacing) {
		mStyle.TreeNodeSpacing = spacing;
		return *this;
	}
	ImGui::Options& ImGui::Options::columnsMinSpacing(float minSpacing) {
		mStyle.ColumnsMinSpacing = minSpacing;
		return *this;
	}
	ImGui::Options& ImGui::Options::scrollBarWidth(float width) {
		mStyle.ScrollBarWidth = width;
		return *this;
	}
	const ImWchar* ImGui::Options::getFontGlyphRanges(const std::string &name) const {
		if (mFontsGlyphRanges.count(name))
			return &mFontsGlyphRanges.find(name)->second[0];
		else return NULL;
	}

	//! cinder renderer
	class Renderer {
	public:
		Renderer();

		//! renders imgui drawlist
		void renderDrawList(ImDrawList** const cmd_lists, int cmd_lists_count);
		//! sets the font
		ImFont* addFont(ci::DataSourceRef font, float size, const ImWchar* glyph_ranges = NULL);

		//! initializes and returns the font texture
		ci::gl::TextureRef getFontTextureRef();
		//! initializes and returns the vbo
		ci::gl::VboMeshRef getVbo();
		//! initializes and returns the shader
		ci::gl::GlslProgRef getGlslProg();

		//! initializes the font texture
		void initFontTexture();
		//! initializes the vbo mesh
		void initBuffers(int size = 1000);
		//! initializes the shader
		void initGlslProg();

		ImFont* getFont(const std::string &name);

	protected:
		ci::gl::TextureRef    mFontTexture;
		//ci::gl::VaoRef          mVao;
		ci::gl::VboMeshRef      mVbo;
		ci::gl::GlslProgRef     mShader;

		map<string, ImFont*>     mFonts;
	};

	typedef std::shared_ptr<Renderer> RendererRef;
	RendererRef getRenderer() {
		static RendererRef renderer = RendererRef(new Renderer());
		return renderer;
	}

	void initialize(const Options &options) {
		auto renderer = getRenderer();
		auto window = options.getWindow();

		// set style
		const ImGuiStyle& style = options.getStyle();
		ImGuiStyle& imGuiStyle = ImGui::GetStyle();
		imGuiStyle.Alpha = style.Alpha;
		imGuiStyle.WindowPadding = style.WindowPadding;
		imGuiStyle.WindowMinSize = style.WindowMinSize;
		imGuiStyle.FramePadding = style.FramePadding;
		imGuiStyle.FrameRounding = style.FrameRounding;
		imGuiStyle.ItemSpacing = style.ItemSpacing;
		imGuiStyle.ItemInnerSpacing = style.ItemInnerSpacing;
		imGuiStyle.TouchExtraPadding = style.TouchExtraPadding;
		imGuiStyle.AutoFitPadding = style.AutoFitPadding;
		imGuiStyle.WindowFillAlphaDefault = style.WindowFillAlphaDefault;
		imGuiStyle.WindowRounding = style.WindowRounding;
		imGuiStyle.TreeNodeSpacing = style.TreeNodeSpacing;
		imGuiStyle.ColumnsMinSpacing = style.ColumnsMinSpacing;
		imGuiStyle.ScrollBarWidth = style.ScrollBarWidth;
		// set colors
		for (int i = 0; i < ImGuiCol_COUNT; i++)
			imGuiStyle.Colors[i] = style.Colors[i];

		// set io and keymap
		ImGuiIO& io = ImGui::GetIO();
		io.DisplaySize = ImVec2(window->getSize().x, window->getSize().y);
		io.DeltaTime = 1.0f / 60.0f;
		io.KeyMap[ImGuiKey_Tab] = KeyEvent::KEY_TAB;
		io.KeyMap[ImGuiKey_LeftArrow] = KeyEvent::KEY_LEFT;
		io.KeyMap[ImGuiKey_RightArrow] = KeyEvent::KEY_RIGHT;
		io.KeyMap[ImGuiKey_UpArrow] = KeyEvent::KEY_UP;
		io.KeyMap[ImGuiKey_DownArrow] = KeyEvent::KEY_DOWN;
		io.KeyMap[ImGuiKey_Home] = KeyEvent::KEY_HOME;
		io.KeyMap[ImGuiKey_End] = KeyEvent::KEY_END;
		io.KeyMap[ImGuiKey_Delete] = KeyEvent::KEY_DELETE;
		io.KeyMap[ImGuiKey_Backspace] = KeyEvent::KEY_BACKSPACE;
		io.KeyMap[ImGuiKey_Enter] = KeyEvent::KEY_RETURN;
		io.KeyMap[ImGuiKey_Escape] = KeyEvent::KEY_ESCAPE;
		io.KeyMap[ImGuiKey_A] = KeyEvent::KEY_a;
		io.KeyMap[ImGuiKey_C] = KeyEvent::KEY_c;
		io.KeyMap[ImGuiKey_V] = KeyEvent::KEY_v;
		io.KeyMap[ImGuiKey_X] = KeyEvent::KEY_c;
		io.KeyMap[ImGuiKey_Y] = KeyEvent::KEY_y;
		io.KeyMap[ImGuiKey_Z] = KeyEvent::KEY_z;

		// setup config file path
		string path = (getAssetPath("") / "imgui.ini").string();
		io.IniFilename = strdup(path.c_str());

		// setup fonts
		ImFontAtlas* fontAtlas = ImGui::GetIO().Fonts;
		fontAtlas->Clear();
		for (auto font : options.getFonts()) {
			string name = font.first.stem().string();
			renderer->addFont(loadFile(font.first), font.second, options.getFontGlyphRanges(name));
		}
		renderer->initFontTexture();

		// clipboard callbacks
		io.SetClipboardTextFn = [](const char* text) {
			const char* text_end = text + strlen(text);
			char* buf = (char*)malloc(text_end - text + 1);
			memcpy(buf, text, text_end - text);
			buf[text_end - text] = '\0';
			Clipboard::setString(buf);
			free(buf);
		};
		io.GetClipboardTextFn = []() {
			return Clipboard::getString().c_str();
		};

		// renderer callback
		io.RenderDrawListsFn = [](ImDrawList **const cmd_lists, int cmd_lists_count) {
			auto renderer = getRenderer();
			renderer->renderDrawList(cmd_lists, cmd_lists_count);
		};

		// connect window's signals
		connectWindow(window);
		ImGui::NewFrame();

		sInitialized = true;
	}

	Options& Options::light() {
		mStyle.Colors[ImGuiCol_Text] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
		mStyle.Colors[ImGuiCol_WindowBg] = ImVec4(0.76f, 0.76f, 0.76f, 1.00f);
		mStyle.Colors[ImGuiCol_Border] = ImVec4(0.76f, 0.76f, 0.76f, 1.00f);
		mStyle.Colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		mStyle.Colors[ImGuiCol_FrameBg] = ImVec4(0.71f, 0.71f, 0.71f, 1.00f);
		mStyle.Colors[ImGuiCol_TitleBg] = ImVec4(0.71f, 0.71f, 0.71f, 1.00f);
		mStyle.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.57f, 0.57f, 0.57f, 1.00f);
		mStyle.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.71f, 0.71f, 0.71f, 1.00f);
		mStyle.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.57f, 0.57f, 0.57f, 1.00f);
		mStyle.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.76f, 0.76f, 0.76f, 1.00f);
		mStyle.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
		mStyle.Colors[ImGuiCol_ComboBg] = ImVec4(0.71f, 0.71f, 0.71f, 1.00f);
		mStyle.Colors[ImGuiCol_CheckHovered] = ImVec4(0.57f, 0.57f, 0.57f, 1.00f);
		mStyle.Colors[ImGuiCol_CheckActive] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
		mStyle.Colors[ImGuiCol_CheckMark] = ImVec4(0.22f, 0.22f, 0.22f, 0.50f);
		mStyle.Colors[ImGuiCol_SliderGrab] = ImVec4(0.57f, 0.57f, 0.57f, 1.00f);
		mStyle.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
		mStyle.Colors[ImGuiCol_Button] = ImVec4(0.71f, 0.71f, 0.71f, 1.00f);
		mStyle.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.57f, 0.57f, 0.57f, 1.00f);
		mStyle.Colors[ImGuiCol_ButtonActive] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
		mStyle.Colors[ImGuiCol_Header] = ImVec4(0.71f, 0.71f, 0.71f, 1.00f);
		mStyle.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.57f, 0.57f, 0.57f, 1.00f);
		mStyle.Colors[ImGuiCol_HeaderActive] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
		mStyle.Colors[ImGuiCol_Column] = ImVec4(0.71f, 0.71f, 0.71f, 1.00f);
		mStyle.Colors[ImGuiCol_ColumnHovered] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
		mStyle.Colors[ImGuiCol_ColumnActive] = ImVec4(0.57f, 0.57f, 0.57f, 1.00f);
		mStyle.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.71f, 0.71f, 0.71f, 1.00f);
		mStyle.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
		mStyle.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.57f, 0.57f, 0.57f, 1.00f);
		mStyle.Colors[ImGuiCol_CloseButton] = ImVec4(0.57f, 0.57f, 0.57f, 1.00f);
		mStyle.Colors[ImGuiCol_CloseButtonHovered] = ImVec4(0.71f, 0.71f, 0.71f, 1.00f);
		mStyle.Colors[ImGuiCol_CloseButtonActive] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
		mStyle.Colors[ImGuiCol_PlotLines] = ImVec4(0.57f, 0.57f, 0.57f, 1.00f);
		mStyle.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
		mStyle.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.57f, 0.57f, 0.57f, 1.00f);
		mStyle.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
		mStyle.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.57f, 0.57f, 0.57f, 1.00f);
		mStyle.Colors[ImGuiCol_TooltipBg] = ImVec4(0.76f, 0.76f, 0.76f, 1.00f);

		return *this;
	}

	Options& Options::dark() {
		mStyle.Colors[ImGuiCol_Text] = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
		mStyle.Colors[ImGuiCol_WindowBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
		mStyle.Colors[ImGuiCol_Border] = ImVec4(0.21f, 0.21f, 0.21f, 1.00f);
		mStyle.Colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.38f);
		mStyle.Colors[ImGuiCol_FrameBg] = ImVec4(0.11f, 0.11f, 0.11f, 1.00f);
		mStyle.Colors[ImGuiCol_TitleBg] = ImVec4(0.16f, 0.16f, 0.16f, 1.00f);
		mStyle.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.17f, 0.17f, 0.17f, 1.00f);
		mStyle.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
		mStyle.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
		mStyle.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.26f, 0.26f, 0.26f, 1.00f);
		mStyle.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.26f, 0.26f, 0.26f, 1.00f);
		mStyle.Colors[ImGuiCol_ComboBg] = ImVec4(0.13f, 0.13f, 0.13f, 1.00f);
		mStyle.Colors[ImGuiCol_CheckHovered] = ImVec4(0.27f, 0.27f, 0.27f, 1.00f);
		mStyle.Colors[ImGuiCol_CheckActive] = ImVec4(0.36f, 0.36f, 0.36f, 1.00f);
		mStyle.Colors[ImGuiCol_CheckMark] = ImVec4(0.22f, 0.22f, 0.22f, 0.50f);
		mStyle.Colors[ImGuiCol_SliderGrab] = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);
		mStyle.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.30f, 0.30f, 0.30f, 1.00f);
		mStyle.Colors[ImGuiCol_Button] = ImVec4(0.17f, 0.17f, 0.17f, 1.00f);
		mStyle.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.27f, 0.27f, 0.27f, 1.00f);
		mStyle.Colors[ImGuiCol_ButtonActive] = ImVec4(0.38f, 0.38f, 0.38f, 1.00f);
		mStyle.Colors[ImGuiCol_Header] = ImVec4(0.11f, 0.11f, 0.11f, 1.00f);
		mStyle.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
		mStyle.Colors[ImGuiCol_HeaderActive] = ImVec4(0.27f, 0.27f, 0.27f, 1.00f);
		mStyle.Colors[ImGuiCol_Column] = ImVec4(0.04f, 0.04f, 0.04f, 0.22f);
		mStyle.Colors[ImGuiCol_ColumnHovered] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
		mStyle.Colors[ImGuiCol_ColumnActive] = ImVec4(0.27f, 0.27f, 0.27f, 1.00f);
		mStyle.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
		mStyle.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.49f, 0.49f, 0.49f, 1.00f);
		mStyle.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.27f, 0.27f, 0.27f, 1.00f);
		mStyle.Colors[ImGuiCol_CloseButton] = ImVec4(0.28f, 0.28f, 0.28f, 1.00f);
		mStyle.Colors[ImGuiCol_CloseButtonHovered] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
		mStyle.Colors[ImGuiCol_CloseButtonActive] = ImVec4(0.49f, 0.49f, 0.49f, 1.00f);
		mStyle.Colors[ImGuiCol_PlotLines] = ImVec4(0.22f, 0.22f, 0.22f, 1.00f);
		mStyle.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.42f, 0.42f, 0.42f, 1.00f);
		mStyle.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.16f, 0.16f, 0.16f, 1.00f);
		mStyle.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.49f, 0.49f, 0.49f, 1.00f);
		mStyle.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.24f, 0.24f, 0.24f, 1.00f);
		mStyle.Colors[ImGuiCol_TooltipBg] = ImVec4(0.16f, 0.16f, 0.16f, 1.00f);

		return *this;
	}

	Options& Options::color(ImGuiCol option, const ci::ColorA &color)
	{
		mStyle.Colors[option] = color;
		return *this;
	}


	Renderer::Renderer() {
		initFontTexture();
		initGlslProg();
		initBuffers();
	}

	//! renders imgui drawlist
	void Renderer::renderDrawList(ImDrawList** const cmd_lists, int cmd_lists_count) {
		size_t total_vtx_count = 0;
		for (int n = 0; n < cmd_lists_count; n++)
			total_vtx_count += cmd_lists[n]->vtx_buffer.size();
		if (total_vtx_count == 0)
			return;

		if (getVbo()->getNumVertices() < total_vtx_count) {
			gl::VboMesh::Layout layout;
			layout.setDynamicColorsRGBA();
			layout.setDynamicPositions();
			layout.setDynamicTexCoords2d();

			mVbo = gl::VboMesh::create(total_vtx_count, 0, layout, GL_TRIANGLES);
		}

		int vtx_consumed = 0;
		{
			gl::VboMesh::VertexIter iter = getVbo()->mapVertexBuffer();


			for (int n = 0; n < cmd_lists_count; n++) {
				const ImDrawList* cmd_list = cmd_lists[n];
				if (!cmd_list->vtx_buffer.empty()) {
					vtx_consumed += cmd_list->vtx_buffer.size();
					int count = cmd_list->vtx_buffer.size();
					for (int i = 0; i < count; i++) {

						iter.setPosition(Vec3f(cmd_list->vtx_buffer[i].pos, 0.0f));
						iter.setTexCoord2d0(cmd_list->vtx_buffer[i].uv);

						ImU32 color = cmd_list->vtx_buffer[i].col;

						uint32_t a = color >> 24 & 255;
						uint32_t b = color >> 16 & 255;
						uint32_t g = color >> 8 & 255;
						uint32_t r = color >> 0 & 255;
						iter.setColorRGBA(ColorA(r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f));
						++iter;
					}
				}
			}
		}

		getGlslProg()->bind();

		float windowHeight = GetIO().DisplaySize.y;
		getGlslProg()->uniform("height", windowHeight);

		gl::enableAlphaBlending();
		gl::disableDepthRead();
		glDisable(GL_CULL_FACE);

		getFontTextureRef()->enableAndBind();

		vtx_consumed = 0;						// offset in vertex buffer. each command consume ImDrawCmd::vtx_count of those

		getVbo()->enableClientStates();
		getVbo()->bindAllData();

		for (int n = 0; n < cmd_lists_count; n++) {
			const ImDrawList* cmd_list = cmd_lists[n];
			if (cmd_list->commands.empty() || cmd_list->vtx_buffer.empty())
				continue;
			const ImDrawCmd* pcmd = &cmd_list->commands.front();
			const ImDrawCmd* pcmd_end = &cmd_list->commands.back();
			while (pcmd <= pcmd_end) {
				const ImDrawCmd& cmd = *pcmd++;
				glBindTexture(GL_TEXTURE_2D, (GLuint)(intptr_t)cmd.texture_id);
				getGlslProg()->uniform("ClipRect", Vec4f(cmd.clip_rect));
				glDrawArrays(GL_TRIANGLES, vtx_consumed, cmd.vtx_count);
				vtx_consumed += cmd.vtx_count;
			}
		}

		getVbo()->unbindBuffers();
		getVbo()->disableClientStates();

		getFontTextureRef()->unbind();
		getGlslProg()->unbind();
	}

	//! initializes and returns the font texture
	gl::TextureRef Renderer::getFontTextureRef() {
		if (!mFontTexture) {
			initFontTexture();
		}
		return mFontTexture;
	}

	//! initializes and returns the vbo
	gl::VboMeshRef Renderer::getVbo() {
		if (!mVbo) {
			initBuffers();
		}
		return mVbo;
	}

	//! initializes the vbo mesh
	void Renderer::initBuffers(int size) {
		gl::VboMesh::Layout layout;
		layout.setDynamicColorsRGBA();
		layout.setDynamicPositions();
		layout.setDynamicTexCoords2d();

		mVbo = gl::VboMesh::create(1000, 0, layout, GL_TRIANGLES);

	}

	//! initializes the font texture
	void Renderer::initFontTexture() {
		gl::Texture::Format format;
		format.setMagFilter(GL_LINEAR);
		format.setMinFilter(GL_LINEAR);

		unsigned char* pixels;
		int width, height;
		ImGui::GetIO().Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);
		mFontTexture = gl::Texture::create(pixels, GL_RGBA, width, height, format);
		ImGui::GetIO().Fonts->ClearTexData();
		ImGui::GetIO().Fonts->TexID = (void *)(intptr_t)mFontTexture->getId();
	}

	ImFont* Renderer::addFont(ci::DataSourceRef font, float size, const ImWchar* glyph_ranges) {
		ImFontAtlas* fontAtlas = ImGui::GetIO().Fonts;


		Font ciFont(font, size);

		Buffer &buffer = font->getBuffer();
		void* bufferCopy = (void*)malloc(buffer.getDataSize());
		memcpy(bufferCopy, buffer.getData(), buffer.getDataSize());

		ImFont* newFont = fontAtlas->AddFontFromMemoryTTF(bufferCopy, buffer.getDataSize(), size, glyph_ranges);

		mFonts.insert(make_pair(font->getFilePath().stem().string(), newFont));
		return newFont;
	}

	//! initializes and returns the shader
	gl::GlslProgRef Renderer::getGlslProg() {
		if (!mShader) {
			initGlslProg();
		}
		return mShader;
	}

	//! initializes the shader
	void Renderer::initGlslProg() {
		try {
			mShader = gl::GlslProg::create(
				"void main() {"
				"   gl_FrontColor = gl_Color;"
				"   gl_TexCoord[0] = gl_MultiTexCoord0;"
				"   gl_Position = ftransform();"
				"}",

				"uniform sampler2D Tex;"
				"uniform vec4 ClipRect;"
				"uniform float height;"
				"void main() {"
				"   vec4 color = texture2D( Tex, gl_TexCoord[0].st ) * gl_Color;"
				"   vec2 pixel_pos = gl_FragCoord.st;"
				"   pixel_pos.y = height - pixel_pos.y;"
				// Clipping: using discard
				//"   if (pixel_pos.x < ClipRect.x || pixel_pos.y < ClipRect.y || pixel_pos.x > ClipRect.z || pixel_pos.y > ClipRect.w) discard;"
				// Clipping: using discard and step
				//"   if (step(ClipRect.x,pixel_pos.x) * step(ClipRect.y,pixel_pos.y) * step(pixel_pos.x,ClipRect.z) * step(pixel_pos.y,ClipRect.w) < 1.0f) discard;"
				// Clipping: branch-less
				"   color.w *= (step(ClipRect.x,pixel_pos.x) * step(ClipRect.y,pixel_pos.y) * step(pixel_pos.x,ClipRect.z) * step(pixel_pos.y,ClipRect.w));"
				"   gl_FragColor = color;"
				"}"
				);
		}
		catch (gl::GlslProgCompileExc exc) {
			cout << "Problem Compiling ImGui::Renderer shader" << endl << exc.what() << endl;
		}
	}


	ImFont* Renderer::getFont(const std::string &name) {
		if (!mFonts.count(name)) {
			return nullptr;
		}
		else {
			return mFonts[name];
		}
	}

	void Image(const ci::gl::TextureRef &texture, const ImVec2& size, const ImVec2& uv0, const ImVec2& uv1, const ImVec4& tint_col, const ImVec4& border_col) {
		Image((void*)(intptr_t)texture->getId(), size, uv0, uv1, tint_col, border_col);
	}

	bool ImageButton(const ci::gl::TextureRef &texture, const ImVec2& size, const ImVec2& uv0, const ImVec2& uv1, int frame_padding, const ImVec4& bg_col, const ImVec4& tint_col) {
		return ImageButton((void*)(intptr_t)texture->getId(), size, uv0, uv1, frame_padding, bg_col, tint_col);
	}

	void PushFont(const std::string& name) {
		auto renderer = getRenderer();
		ImFont* font = renderer->getFont(name);
		CI_ASSERT(font != nullptr);
		PushFont(font);
	}


	//! sets the right mouseDown IO values in imgui
	void mouseDown(ci::app::MouseEvent& event) {
		ImGuiIO& io = ImGui::GetIO();
		io.MousePos = ImVec2(event.getPos().x, event.getPos().y);
		if (event.isLeftDown()) {
			io.MouseDown[0] = true;
			io.MouseDown[1] = false;
		}
		else if (event.isLeftDown()) {
			io.MouseDown[0] = false;
			io.MouseDown[1] = true;
		}
		
		event.setHandled(io.WantCaptureMouse);
	}
	//! sets the right mouseMove IO values in imgui
	void mouseMove(ci::app::MouseEvent& event) {
		ImGuiIO& io = ImGui::GetIO();
		io.MousePos = ImVec2(event.getPos().x, event.getPos().y);

		event.setHandled(io.WantCaptureMouse);
	}
	//! sets the right mouseDrag IO values in imgui
	void mouseDrag(ci::app::MouseEvent& event) {
		ImGuiIO& io = ImGui::GetIO();
		io.MousePos = ImVec2(event.getPos().x, event.getPos().y);

		event.setHandled(io.WantCaptureMouse);
	}
	//! sets the right mouseDrag IO values in imgui
	void mouseUp(ci::app::MouseEvent& event) {
		ImGuiIO& io = ImGui::GetIO();
		io.MouseDown[0] = false;
		io.MouseDown[1] = false;

		event.setHandled(io.WantCaptureMouse);
	}
	//! sets the right mouseWheel IO values in imgui
	void mouseWheel(ci::app::MouseEvent& event) {
		ImGuiIO& io = ImGui::GetIO();
		io.MouseWheel += event.getWheelIncrement();

		event.setHandled(io.WantCaptureMouse);
	}
	//! sets the right keyDown IO values in imgui
	void keyDown(ci::app::KeyEvent& event) {
		ImGuiIO& io = ImGui::GetIO();

		io.KeyCtrl = event.isControlDown();
		io.KeyShift = event.isShiftDown();

		switch (event.getCode()) {
		default:
			io.KeysDown[event.getCode()] = true;

			uint32_t character = event.getCharUtf32();
			if (character > 0 && character <= 255) {
				io.AddInputCharacter((char)character);
			}
			break;
		}

		event.setHandled(io.WantCaptureKeyboard);
	}
	//! sets the right keyUp IO values in imgui
	void keyUp(ci::app::KeyEvent& event) {
		ImGuiIO& io = ImGui::GetIO();

		io.KeyCtrl = event.isControlDown();
		io.KeyShift = event.isShiftDown();

		switch (event.getCode()) {

		default:
			io.KeysDown[event.getCode()] = false;
			break;
		}

		event.setHandled(io.WantCaptureKeyboard);
	}
	void resize() {
		ImGuiIO& io = ImGui::GetIO();
		io.DisplaySize = getWindowSize();
	}

	static bool sNewFrame = false;
	static uint32_t sLastFrame = -1;
	void render() {
		if (getElapsedFrames() == sLastFrame) return;

		gl::setMatricesWindow(getWindowSize());
		ImGui::Render();

		static float elapsedTime = 0.0f;
		float currentElapsedTime = getElapsedSeconds();
		ImGuiIO& io = ImGui::GetIO();
		io.DeltaTime = (currentElapsedTime - elapsedTime);
		elapsedTime = currentElapsedTime;
		sLastFrame = getElapsedFrames();
		sNewFrame = false;
		App::get()->dispatchAsync([]() { ImGui::NewFrame(); sNewFrame = true; });
	}
	void newFrameGuard() {
		if (!sNewFrame) {
			ImGui::NewFrame();
		}
	}

	void connectWindow(ci::app::WindowRef window) {
		window->getSignalMouseDown().connect(mouseDown);
		window->getSignalMouseUp().connect(mouseUp);
		window->getSignalMouseDrag().connect(mouseDrag);
		window->getSignalMouseMove().connect(mouseMove);
		window->getSignalMouseWheel().connect(mouseWheel);
		window->getSignalKeyDown().connect(keyDown);
		window->getSignalKeyUp().connect(keyUp);
		window->getSignalResize().connect(resize);
		window->getSignalDraw().connect(newFrameGuard);
		window->getSignalPostDraw().connect(render);
	}
	void disconnectWindow(ci::app::WindowRef window) {
		window->getSignalMouseDown().disconnect(mouseDown);
		window->getSignalMouseUp().disconnect(mouseUp);
		window->getSignalMouseDrag().disconnect(mouseDrag);
		window->getSignalMouseMove().disconnect(mouseMove);
		window->getSignalMouseWheel().disconnect(mouseWheel);
		window->getSignalKeyDown().disconnect(keyDown);
		window->getSignalKeyUp().disconnect(keyUp);
		window->getSignalResize().disconnect(resize);
		window->getSignalDraw().disconnect(newFrameGuard);
		window->getSignalPostDraw().disconnect(render);
	}

	ScopedWindow::ScopedWindow(const std::string &name, bool* opened, ci::Vec2f size, float fillAlpha, ImGuiWindowFlags flags) {
		ImGui::Begin(name.c_str(), opened, size, fillAlpha, flags);
	}
	ScopedWindow::~ScopedWindow() {
		ImGui::End();
	}
	ScopedChild::ScopedChild(const std::string &name, ci::Vec2f size, bool border, ImGuiWindowFlags extraFlags) {
		ImGui::BeginChild(name.c_str(), size, border, extraFlags);
	}
	ScopedChild::~ScopedChild() {
		ImGui::EndChild();
	}

	ScopedFont::ScopedFont(const std::string &name) {
		ImGui::PushFont(name);
	}
	ScopedFont::~ScopedFont() {
		ImGui::PopFont();
	}
	ScopedStyleColor::ScopedStyleColor(ImGuiCol idx, const ImVec4& col) {
		ImGui::PushStyleColor(idx, col);
	}
	ScopedStyleColor::~ScopedStyleColor() {
		ImGui::PopStyleColor();
	}
	ScopedStyleVar::ScopedStyleVar(ImGuiStyleVar idx, float val) {
		ImGui::PushStyleVar(idx, val);
	}
	ScopedStyleVar::ScopedStyleVar(ImGuiStyleVar idx, const ImVec2 &val) {
		ImGui::PushStyleVar(idx, val);
	}
	ScopedStyleVar::~ScopedStyleVar() {
		ImGui::PopStyleVar();
	}

	ScopedItemWidth::ScopedItemWidth(float itemWidth) {
		ImGui::PushItemWidth(itemWidth);
	}
	ScopedItemWidth::~ScopedItemWidth() {
		ImGui::PopItemWidth();
	}

	ScopedTextWrapPos::ScopedTextWrapPos(float wrapPosX) {
		ImGui::PushTextWrapPos(wrapPosX);
	}
	ScopedTextWrapPos::~ScopedTextWrapPos() {
		ImGui::PopTextWrapPos();
	}

	ScopedId::ScopedId(const std::string &name) {
		ImGui::PushID(name.c_str());
	}
	ScopedId::ScopedId(const void *ptrId) {
		ImGui::PushID(ptrId);
	}
	ScopedId::ScopedId(const int intId) {
		ImGui::PushID(intId);
	}
	ScopedId::~ScopedId() {
		ImGui::PopID();
	}
}