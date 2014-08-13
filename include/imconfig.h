//-----------------------------------------------------------------------------
// USER IMPLEMENTATION
//-----------------------------------------------------------------------------

#pragma once

//----- Define your own ImVector<> type if you don't want to use the provided implementation defined in imgui.h
//#include <vector>
//#define ImVector	 std::vector
//#define ImVector	MyVector

//----- Define assertion handler. Default to calling assert().
// #define IM_ASSERT(_EXPR)	MyAssert(_EXPR)

//----- Define implicit cast operators to convert back<>forth from your math types and ImVec2/ImVec4.

#include "cinder/Vector.h"

#define IM_VEC2_CLASS_EXTRA													\
		ImVec2(const ci::Vec2f& f) { x = f.x; y = f.y; }						\
		operator ci::Vec2f() const { return ci::Vec2f(x,y); }

#define IM_VEC4_CLASS_EXTRA													\
		ImVec4(const ci::Vec4f& f) { x = f.x; y = f.y; z = f.z; w = f.w; }		\
		operator ci::Vec4f() const { return ci::Vec4f(x,y,z,w); }


//----- Freely implement extra functions within the ImGui:: namespace.
//----- e.g. you can create variants of the ImGui::Value() helper for your low-level math types.
/*
namespace ImGui
{
	void	Value(const char* prefix, cosnt MyVec2& v, const char* float_format = NULL);
	void	Value(const char* prefix, cosnt MyVec4& v, const char* float_format = NULL);
};
*/
