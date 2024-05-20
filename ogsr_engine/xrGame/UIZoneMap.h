#pragma once

#include "ui/UIStatic.h"

class CActor;
class CUICustomMap;
//////////////////////////////////////////////////////////////////////////

class CUIZoneMap
{
    CUICustomMap* m_activeMap{};
    float m_fScale{1.f};

    CUIStatic m_background;
    CUIStatic m_center;
    CUIStatic m_clipFrame;
    CUIStatic m_pointerDistanceText;

public:
    CUIZoneMap();
    virtual ~CUIZoneMap();

    void SetHeading(float angle);
    void Init();

    void Render();
    void UpdateRadar(Fvector pos);

    void SetScale(float s) { m_fScale = s; }
    float GetScale() { return m_fScale; }

    bool ZoomIn();
    bool ZoomOut();

    void ApplyZoom();

    CUIStatic& Background() { return m_background; };
    CUIStatic& ClipFrame() { return m_clipFrame; }; // alpet: для экспорта в скрипты

    void SetupCurrentMap();
};
