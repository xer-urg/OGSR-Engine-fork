#include "stdafx.h"
#include "uizonemap.h"

#include "hudmanager.h"

#include "InfoPortion.h"
#include "Pda.h"

#include "Grenade.h"
#include "level.h"
#include "game_cl_base.h"

#include "actor.h"
#include "ai_space.h"
#include "game_graph.h"

#include "ui/UIMap.h"
#include "ui/UIXmlInit.h"

#include "ui/UIInventoryUtilities.h"
//////////////////////////////////////////////////////////////////////////

constexpr auto ZONE_MAP = "zone_map.xml";

CUIZoneMap::CUIZoneMap() 
{
    m_background = xr_new<CUIStatic>();
    m_clipFrame = xr_new<CUIStatic>();
    m_center = xr_new<CUIStatic>();
    m_activeMap = xr_new<CUIMiniMap>();
}

CUIZoneMap::~CUIZoneMap() 
{
    xr_delete(m_center);
    xr_delete(m_clipFrame);
    xr_delete(m_background);
}

void CUIZoneMap::Init()
{
    CUIXml uiXml;
    const bool xml_result = uiXml.Init(CONFIG_PATH, UI_PATH, "zone_map.xml");
    R_ASSERT(xml_result, "xml file not found", "zone_map.xml");

    // load map background
    CUIXmlInit xml_init;

    xml_init.InitStatic(uiXml, "minimap:background", 0, m_background);

    xml_init.InitStatic(uiXml, "minimap:level_frame", 0, m_clipFrame);

    xml_init.InitStatic(uiXml, "minimap:center", 0, m_center);

    m_rounded = uiXml.ReadAttribInt("minimap", 0, "rounded", 0) == 1;
    m_alpha = uiXml.ReadAttribInt("minimap", 0, "alpha", 127);

    m_clipFrame->AttachChild(m_activeMap);

    m_activeMap->SetAutoDelete(true);
    m_activeMap->EnableHeading(true);
    m_activeMap->SetWindowName("minimap");
    m_activeMap->SetRounded(m_rounded);

    m_activeMap->SetTextureColor(color_argb(m_alpha, 255, 255, 255));

    m_clipFrame->AttachChild(m_center);
    m_center->SetWndPos(m_clipFrame->GetWidth() / 2, m_clipFrame->GetHeight() / 2);
}

void CUIZoneMap::Render()
{
    m_background->Draw();

    auto pda = Actor()->GetPDA();
    if (!pda || !pda->IsPowerOn())
        return;
    m_clipFrame->Draw();
}

void CUIZoneMap::SetHeading(float angle) const { m_activeMap->SetHeading(angle); };

void CUIZoneMap::UpdateRadar(Fvector pos) const
{
    m_clipFrame->Update();
    m_background->Update();
    m_activeMap->SetActivePoint(pos);
}

bool CUIZoneMap::ZoomIn() 
{
    m_fScale = m_fScale + m_fScale * 0.25f;
    clamp(m_fScale, 0.5f, 2.f);
    ApplyZoom();
    return true; 
}

bool CUIZoneMap::ZoomOut() 
{ 
    m_fScale = m_fScale - m_fScale * 0.25f;
    clamp(m_fScale, 0.5f, 2.f);
    ApplyZoom();
    return true; 
}

void CUIZoneMap::SetupCurrentMap()
{
    CInifile* pLtx = pGameIni;

    if (!pLtx->section_exist(Level().name()))
        pLtx = Level().pLevel;

    // dsh: очередной костыль. Если не создавать новый CUIMiniMap, то после
    // перехода с локации, на которой нет текстуры миникарты, на локацию,
    // где эта текстура есть (например, из X-10 на Радар), миникарта
    // перестает показываться.
    m_clipFrame->DetachChild(m_activeMap);
    m_clipFrame->DetachChild(m_center);

    m_activeMap = xr_new<CUIMiniMap>();
    m_clipFrame->AttachChild(m_activeMap);

    m_activeMap->SetAutoDelete(true);
    m_activeMap->EnableHeading(true);
    m_activeMap->SetRounded(m_rounded);

    m_activeMap->Init(Level().name(), *pLtx, "hud\\default");
    m_activeMap->SetWindowName("minimap"); // имя нужно задавать позже чем Init

    m_activeMap->SetTextureColor(color_argb(m_alpha, 255, 255, 255));

    m_clipFrame->AttachChild(m_center);

    Frect r;
    m_clipFrame->GetAbsoluteRect(r);
    m_activeMap->SetClipRect(r);
    m_activeMap->WorkingArea().set(r);

    ApplyZoom();
}

void CUIZoneMap::ApplyZoom() const
{
    Fvector2 wnd_size{};
    const float zoom_factor = float(m_clipFrame->GetWndRect().width()) / 100.0f;
    wnd_size.x = m_activeMap->BoundRect().width() * zoom_factor * m_fScale;
    wnd_size.y = m_activeMap->BoundRect().height() * zoom_factor * m_fScale;
    m_activeMap->SetWndSize(wnd_size);
}
