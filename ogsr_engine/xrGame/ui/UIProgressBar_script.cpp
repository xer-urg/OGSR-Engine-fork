#include "stdafx.h"
#include "UIProgressBar.h"
#include "UIProgressShape.h"

using namespace luabind;

#pragma optimize("s", on)
void CUIProgressBar::script_register(lua_State* L)
{
    module(L)[class_<CUIProgressBar, CUIWindow>("CUIProgressBar")
                  .def(constructor<>())
                  .def("SetProgressPos", &CUIProgressBar::SetProgressPos)
                  .def("GetProgressPos", &CUIProgressBar::GetProgressPos)

                  .def("GetRange_min", &CUIProgressBar::GetRange_min)
                  .def("GetRange_max", &CUIProgressBar::GetRange_max),

            class_<CUIProgressShape, CUIWindow>("CUIProgressShape")
                  .def(constructor<>())
                  .def("SetPos", &CUIProgressShape::SetPos)
                  .def("SetPosWMax", &CUIProgressShape::SetPosWMax)

    ];
}