#include "stdafx.h"
#include "UIInventoryUtilities.h"
#include "WeaponAmmo.h"
#include "Weapon.h"
#include "WeaponRPG7.h"
#include "UIStaticItem.h"
#include "UIStatic.h"
#include "eatable_item.h"
#include "Level.h"
#include "HUDManager.h"
#include "UIGameSP.h"
#include "date_time.h"
#include "string_table.h"
#include "Inventory.h"
#include "InventoryOwner.h"
#include "InventoryBox.h"

#include "InfoPortion.h"
#include "game_base_space.h"
#include "actor.h"
#include "string_table.h"
#include "script_game_object.h"

constexpr auto EQUIPMENT_ICONS = "ui\\ui_icon_equipment";

constexpr LPCSTR relationsLtxSection = "game_relations";
constexpr LPCSTR ratingField = "rating_names";
constexpr LPCSTR reputationgField = "reputation_names";
constexpr LPCSTR goodwillField = "goodwill_names";

static xr_unordered_map<size_t, ui_shader> g_EquipmentIconsShaders;

typedef std::pair<CHARACTER_RANK_VALUE, shared_str> CharInfoStringID;
DEF_MAP(CharInfoStrings, CHARACTER_RANK_VALUE, shared_str);

CharInfoStrings* charInfoReputationStrings = NULL;
CharInfoStrings* charInfoRankStrings = NULL;
CharInfoStrings* charInfoGoodwillStrings = NULL;

void InventoryUtilities::CreateShaders() {}

void InventoryUtilities::DestroyShaders() { g_EquipmentIconsShaders.clear(); }

bool InventoryUtilities::FreeRoom(TIItemContainer& item_list, PIItem _item, int width, int height, bool vertical)
{
    if (!HasFreeSpace(item_list, _item, width, height, vertical))
        return false;

    return vertical ? FreeRoom_byColumns(item_list, _item, width, height) : FreeRoom_byRows(item_list, _item, width, height);
}

bool InventoryUtilities::FreeRoom_byRows(TIItemContainer& item_list, PIItem _item, int width, int height)
{
    bool* ruck_room = (bool*)_malloca(width * height);

    int i, j, k, m;
    int place_row = 0, place_col = 0;
    bool found_place;
    bool can_place;

    for (i = 0; i < height; ++i)
        for (j = 0; j < width; ++j)
            ruck_room[i * width + j] = false;

    item_list.push_back(_item);
    std::stable_sort(item_list.begin(), item_list.end(), [](const auto& a, const auto& b) {
        if (a->GetGridWidth() > b->GetGridWidth())
            return true;
        if (a->GetGridWidth() == b->GetGridWidth())
            return a->GetGridHeight() > b->GetGridHeight();
        return false;
    });

    found_place = true;

    for (const auto& item : item_list)
    {
        int iWidth = item->GetGridWidth();
        int iHeight = item->GetGridHeight();
        // проверить можно ли разместить элемент,
        // проверяем последовательно каждую клеточку
        found_place = false;

        for (i = 0; (i < height - iHeight + 1) && !found_place; ++i)
        {
            for (j = 0; (j < width - iWidth + 1) && !found_place; ++j)
            {
                can_place = true;

                for (k = 0; (k < iHeight) && can_place; ++k)
                {
                    for (m = 0; (m < iHeight) && can_place; ++m)
                    {
                        if (ruck_room[(i + k) * width + (j + m)])
                            can_place = false;
                    }
                }

                if (can_place)
                {
                    found_place = true;
                    place_row = i;
                    place_col = j;
                }

                // разместить элемент на найденном месте
                if (found_place)
                {
                    for (k = 0; k < iHeight; ++k)
                    {
                        for (m = 0; m < iWidth; ++m)
                        {
                            ruck_room[(place_row + k) * width + place_col + m] = true;
                        }
                    }
                }
            }
        }
    }

    // remove
    item_list.erase(std::remove(item_list.begin(), item_list.end(), _item), item_list.end());

    // для какого-то элемента места не нашлось
    if (!found_place)
        return false;

    return true;
}

bool InventoryUtilities::FreeRoom_byColumns(TIItemContainer& item_list, PIItem _item, int width, int height)
{
    bool* ruck_room = (bool*)_malloca(width * height);

    int i, j, k, m;
    int place_row = 0, place_col = 0;
    bool found_place;
    bool can_place;

    for (i = 0; i < width; ++i)
        for (j = 0; j < height; ++j)
            ruck_room[i * height + j] = false;

    item_list.push_back(_item);
    std::stable_sort(item_list.begin(), item_list.end(), [](const auto& a, const auto& b) {
        if (a->GetGridHeight() > b->GetGridHeight())
            return true;
        if (a->GetGridHeight() == b->GetGridHeight())
            return a->GetGridWidth() > b->GetGridWidth();
        return false;
    });

    found_place = true;

    for (const auto& item : item_list)
    {
        int iWidth = item->GetGridWidth();
        int iHeight = item->GetGridHeight();
        // проверить можно ли разместить элемент,
        // проверяем последовательно каждую клеточку
        found_place = false;

        for (i = 0; (i < width - iWidth + 1) && !found_place; ++i)
        {
            for (j = 0; (j < height - iHeight + 1) && !found_place; ++j)
            {
                can_place = true;

                for (k = 0; (k < iWidth) && can_place; ++k)
                {
                    for (m = 0; (m < iWidth) && can_place; ++m)
                    {
                        if (ruck_room[(i + k) * height + (j + m)])
                            can_place = false;
                    }
                }

                if (can_place)
                {
                    found_place = true;
                    place_row = i;
                    place_col = j;
                }

                // разместить элемент на найденном месте
                if (found_place)
                {
                    for (k = 0; k < iWidth; ++k)
                    {
                        for (m = 0; m < iHeight; ++m)
                        {
                            ruck_room[(place_row + k) * height + place_col + m] = true;
                        }
                    }
                }
            }
        }
    }

    // remove
    item_list.erase(std::remove(item_list.begin(), item_list.end(), _item), item_list.end());

    // для какого-то элемента места не нашлось
    if (!found_place)
        return false;

    return true;
}

bool InventoryUtilities::HasFreeSpace(TIItemContainer& item_list, PIItem _item, int width, int height, bool vertical)
{
    Ivector2 r{_item->GetGridWidth(), _item->GetGridHeight()};

    //широкі предмети не можна класти у вертикально-впорядкований масив
    if (vertical && r.x > r.y)
        return false;
    // високі предмети не можна класти у горизонтально-впорядкований масив
    if (!vertical && r.y > r.x)
        return false;

    if (r.x > width || r.y > height)
        return false;

    int list_area = width * height;
    int item_area = _item->GetGridWidth() * _item->GetGridHeight();
    int total_area{};
    for (const auto& item : item_list)
    {
        total_area += item->GetGridWidth() * item->GetGridHeight();
    }
    if ((total_area + item_area) > list_area)
        return false;

    return true;
}

ui_shader& InventoryUtilities::GetEquipmentIconsShader(size_t icon_group)
{
    if (auto it = g_EquipmentIconsShaders.find(icon_group); it == g_EquipmentIconsShaders.end())
    {
        string_path file;
        strcpy_s(file, EQUIPMENT_ICONS);
        if (icon_group > 0)
        {
            strcat_s(file, "_");
            itoa(icon_group, file + strlen(file), 10);
        }

        g_EquipmentIconsShaders[icon_group]->create("hud\\default", file);
    }

    return g_EquipmentIconsShaders.at(icon_group);
}

//////////////////////////////////////////////////////////////////////////

const shared_str InventoryUtilities::GetGameDateAsString(EDatePrecision datePrec, char dateSeparator) { return GetDateAsString(Level().GetGameTime(), datePrec, dateSeparator); }

//////////////////////////////////////////////////////////////////////////

const shared_str InventoryUtilities::GetGameTimeAsString(ETimePrecision timePrec, char timeSeparator) { return GetTimeAsString(Level().GetGameTime(), timePrec, timeSeparator); }

//////////////////////////////////////////////////////////////////////////

const shared_str InventoryUtilities::GetTimeAsString(ALife::_TIME_ID time, ETimePrecision timePrec, char timeSeparator)
{
    string64 bufTime{};

    u32 year = 0, month = 0, day = 0, hours = 0, mins = 0, secs = 0, milisecs = 0;

    split_time(time, year, month, day, hours, mins, secs, milisecs);

    // Time
    switch (timePrec)
    {
    case etpTimeToHours: sprintf_s(bufTime, "%02i", hours); break;
    case etpTimeToMinutes: sprintf_s(bufTime, "%02i%c%02i", hours, timeSeparator, mins); break;
    case etpTimeToSeconds: sprintf_s(bufTime, "%02i%c%02i%c%02i", hours, timeSeparator, mins, timeSeparator, secs); break;
    case etpTimeToMilisecs: sprintf_s(bufTime, "%02i%c%02i%c%02i%c%02i", hours, timeSeparator, mins, timeSeparator, secs, timeSeparator, milisecs); break;
    case etpTimeToSecondsAndDay: {
        int total_day = (int)(time / (1000 * 60 * 60 * 24));
        sprintf_s(bufTime, sizeof(bufTime), "%dd %02i%c%02i%c%02i", total_day, hours, timeSeparator, mins, timeSeparator, secs);
        break;
    }
    default: R_ASSERT(!"Unknown type of date precision");
    }

    return bufTime;
}

const shared_str InventoryUtilities::GetDateAsString(ALife::_TIME_ID date, EDatePrecision datePrec, char dateSeparator)
{
    string32 bufDate{};

    u32 year = 0, month = 0, day = 0, hours = 0, mins = 0, secs = 0, milisecs = 0;

    split_time(date, year, month, day, hours, mins, secs, milisecs);

    // Date
    switch (datePrec)
    {
    case edpDateToYear: sprintf_s(bufDate, "%04i", year); break;
    case edpDateToMonth: sprintf_s(bufDate, "%02i%c%04i", month, dateSeparator, year); break;
    case edpDateToDay: sprintf_s(bufDate, "%02i%c%02i%c%04i", day, dateSeparator, month, dateSeparator, year); break;
    default: R_ASSERT(!"Unknown type of date precision");
    }

    return bufDate;
}

LPCSTR InventoryUtilities::GetTimePeriodAsString(LPSTR _buff, u32 buff_sz, ALife::_TIME_ID _from, ALife::_TIME_ID _to)
{
    u32 year1, month1, day1, hours1, mins1, secs1, milisecs1;
    u32 year2, month2, day2, hours2, mins2, secs2, milisecs2;

    split_time(_from, year1, month1, day1, hours1, mins1, secs1, milisecs1);
    split_time(_to, year2, month2, day2, hours2, mins2, secs2, milisecs2);

    int cnt = 0;
    _buff[0] = 0;

    if (month1 != month2)
        cnt = sprintf_s(_buff + cnt, buff_sz - cnt, "%d %s ", month2 - month1, *CStringTable().translate("ui_st_months"));

    if (!cnt && day1 != day2)
        cnt = sprintf_s(_buff + cnt, buff_sz - cnt, "%d %s", day2 - day1, *CStringTable().translate("ui_st_days"));

    if (!cnt && hours1 != hours2)
        cnt = sprintf_s(_buff + cnt, buff_sz - cnt, "%d %s", hours2 - hours1, *CStringTable().translate("ui_st_hours"));

    if (!cnt && mins1 != mins2)
        cnt = sprintf_s(_buff + cnt, buff_sz - cnt, "%d %s", mins2 - mins1, *CStringTable().translate("ui_st_mins"));

    if (!cnt && secs1 != secs2)
        cnt = sprintf_s(_buff + cnt, buff_sz - cnt, "%d %s", secs2 - secs1, *CStringTable().translate("ui_st_secs"));

    return _buff;
}

//////////////////////////////////////////////////////////////////////////

void InventoryUtilities::UpdateWeight(CUIStatic& wnd, bool withPrefix)
{
    CInventoryOwner* pInvOwner = smart_cast<CInventoryOwner*>(Level().CurrentEntity());
    R_ASSERT(pInvOwner);
    string128 buf{};

    float total = pInvOwner->GetCarryWeight();
    float max = pInvOwner->MaxCarryWeight();

    string16 cl{};

    if (total > max)
    {
        strcpy_s(cl, "%c[red]");
    }
    else
    {
        strcpy_s(cl, "%c[UI_orange]");
    }

    string32 prefix{};

    if (withPrefix)
    {
        sprintf_s(prefix, "%%c[default]%s ", CStringTable().translate("ui_inv_weight").c_str());
    }
    else
    {
        strcpy_s(prefix, "");
    }

    sprintf_s(buf, "%s%s%.1f%s/%.1f %s", prefix, cl, total, "%c[UI_orange]", max, CStringTable().translate("st_kg").c_str());
    wnd.SetText(buf);
    //	UIStaticWeight.ClipperOff();
}

//////////////////////////////////////////////////////////////////////////

void InventoryUtilities::UpdateLimit(CUIStatic& wnd, CGameObject* object)
{
    auto box = smart_cast<IInventoryBox*>(object);
    if (!box)
        return;
    string128 buf{};

    int total = box->GetItems().size();
    int max = box->GetItemsLimit();

    string16 cl{};

    if (total > max)
    {
        strcpy_s(cl, "%c[red]");
    }
    else
    {
        strcpy_s(cl, "%c[UI_orange]");
    }

    string32 prefix{};
    sprintf_s(prefix, "%%c[default]%s ", CStringTable().translate("st_items_limit").c_str());

    sprintf_s(buf, "%s%s%u%s/%u", prefix, cl, total, "%c[UI_orange]", max);
    wnd.SetText(buf);
    //	UIStaticWeight.ClipperOff();
}

void LoadStrings(CharInfoStrings* container, LPCSTR section, LPCSTR field)
{
    R_ASSERT(container);

    LPCSTR cfgRecord = pSettings->r_string(section, field);
    u32 count = _GetItemCount(cfgRecord);
    R_ASSERT3(count % 2, "there're must be an odd number of elements", field);
    string64 singleThreshold{};
    int upBoundThreshold = 0;
    CharInfoStringID id;

    for (u32 k = 0; k < count; k += 2)
    {
        _GetItem(cfgRecord, k, singleThreshold);
        id.second = singleThreshold;

        _GetItem(cfgRecord, k + 1, singleThreshold);
        if (k + 1 != count)
            sscanf(singleThreshold, "%i", &upBoundThreshold);
        else
            upBoundThreshold += 1;

        id.first = upBoundThreshold;

        container->insert(id);
    }
}

//////////////////////////////////////////////////////////////////////////

void InitCharacterInfoStrings()
{
    if (charInfoReputationStrings && charInfoRankStrings)
        return;

    if (!charInfoReputationStrings)
    {
        // Create string->Id DB
        charInfoReputationStrings = xr_new<CharInfoStrings>();
        // Reputation
        LoadStrings(charInfoReputationStrings, relationsLtxSection, reputationgField);
    }

    if (!charInfoRankStrings)
    {
        // Create string->Id DB
        charInfoRankStrings = xr_new<CharInfoStrings>();
        // Ranks
        LoadStrings(charInfoRankStrings, relationsLtxSection, ratingField);
    }

    if (!charInfoGoodwillStrings)
    {
        // Create string->Id DB
        charInfoGoodwillStrings = xr_new<CharInfoStrings>();
        // Goodwills
        LoadStrings(charInfoGoodwillStrings, relationsLtxSection, goodwillField);
    }
}

//////////////////////////////////////////////////////////////////////////

void InventoryUtilities::ClearCharacterInfoStrings()
{
    xr_delete(charInfoReputationStrings);
    xr_delete(charInfoRankStrings);
    xr_delete(charInfoGoodwillStrings);
}

//////////////////////////////////////////////////////////////////////////

LPCSTR InventoryUtilities::GetRankAsText(CHARACTER_RANK_VALUE rankID)
{
    InitCharacterInfoStrings();
    CharInfoStrings::const_iterator cit = charInfoRankStrings->upper_bound(rankID);
    if (charInfoRankStrings->end() == cit)
        return charInfoRankStrings->rbegin()->second.c_str();
    return cit->second.c_str();
}

//////////////////////////////////////////////////////////////////////////

LPCSTR InventoryUtilities::GetReputationAsText(CHARACTER_REPUTATION_VALUE rankID)
{
    InitCharacterInfoStrings();

    CharInfoStrings::const_iterator cit = charInfoReputationStrings->upper_bound(rankID);
    if (charInfoReputationStrings->end() == cit)
        return charInfoReputationStrings->rbegin()->second.c_str();

    return cit->second.c_str();
}

//////////////////////////////////////////////////////////////////////////

LPCSTR InventoryUtilities::GetGoodwillAsText(CHARACTER_GOODWILL goodwill)
{
    InitCharacterInfoStrings();

    CharInfoStrings::const_iterator cit = charInfoGoodwillStrings->upper_bound(goodwill);
    if (charInfoGoodwillStrings->end() == cit)
        return charInfoGoodwillStrings->rbegin()->second.c_str();

    return cit->second.c_str();
}

//////////////////////////////////////////////////////////////////////////
// специальная функция для передачи info_portions при нажатии кнопок UI
// (для tutorial)
void InventoryUtilities::SendInfoToActor(LPCSTR info_id)
{
    CActor* actor = smart_cast<CActor*>(Level().CurrentEntity());
    if (actor)
    {
        actor->TransferInfo(info_id, true);
    }
}

u32 InventoryUtilities::GetGoodwillColor(CHARACTER_GOODWILL gw)
{
    if (gw >= charInfoGoodwillStrings->rbegin()->first)
        return 0xff00ff00;
    if (gw < charInfoGoodwillStrings->begin()->first)
        return 0xffff0000;
    return 0xffc0c0c0;
}

u32 InventoryUtilities::GetReputationColor(CHARACTER_REPUTATION_VALUE rv)
{
    if (rv >= charInfoReputationStrings->rbegin()->first)
        return 0xff00ff00;
    if (rv < charInfoReputationStrings->begin()->first)
        return 0xffff0000;
    return 0xffc0c0c0;
}

u32 InventoryUtilities::GetRelationColor(ALife::ERelationType relation)
{
    switch (relation)
    {
    case ALife::eRelationTypeFriend: return 0xff00ff00; break;
    case ALife::eRelationTypeNeutral: return 0xffc0c0c0; break;
    case ALife::eRelationTypeEnemy: return 0xffff0000; break;
    default: NODEFAULT;
    }
#ifdef DEBUG
    return 0xffffffff;
#endif
}