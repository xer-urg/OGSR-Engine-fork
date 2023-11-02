#pragma once

class CInventory;

#include "UIDialogWnd.h"
#include "UIStatic.h"

#include "UIProgressBar.h"

#include "UIPropertiesBox.h"
#include "UIOutfitSlot.h"

#include "UIOutfitInfo.h"
#include "UIItemInfo.h"
#include "../inventory_space.h"
#include "../actor_flags.h"

class CArtefact;
class CUI3tButton;
class CUIDragDropListEx;
class CUICellItem;

class CUIInventoryWnd : public CUIDialogWnd
{
private:
    typedef CUIDialogWnd inherited;
    bool m_b_need_reinit{};
    bool m_b_need_update_stats{};

public:
    CUIInventoryWnd();
    virtual ~CUIInventoryWnd();

    virtual void Init();

    void InitInventory();
    void InitInventory_delayed();
    virtual bool StopAnyMove() { return false; }

    virtual void SendMessage(CUIWindow* pWnd, s16 msg, void* pData);
    virtual bool OnMouse(float x, float y, EUIMessages mouse_action);
    virtual bool OnKeyboard(int dik, EUIMessages keyboard_action);

    IC CInventory* GetInventory() { return m_pInv; }

    virtual void Update();
    virtual void Draw();

    virtual void Show();
    virtual void Hide();

    void HideSlotsHighlight();
    void ShowSlotsHighlight(PIItem InvItem);

    void AddItemToBag(PIItem pItem);

protected:
    enum eInventorySndAction
    {
        eInvSndOpen = 0,
        eInvSndClose,
        eInvItemToSlot,
        eInvItemToBelt,
        eInvItemToVest,
        eInvItemToRuck,
        eInvProperties,
        eInvDropItem,
        eInvAttachAddon,
        eInvDetachAddon,
        eInvSndMax
    };

    ref_sound sounds[eInvSndMax];
    void PlaySnd(eInventorySndAction a);

    CUIDragDropListEx* m_pUIBagList{};
    CUIDragDropListEx* m_pUIBeltList{};
    CUIDragDropListEx* m_pUIVestList{};

    CUIOutfitDragDropList* m_pUIOutfitList{};
    CUIDragDropListEx* m_pUIHelmetList{};
    CUIDragDropListEx* m_pUIGasMaskList{};
    CUIDragDropListEx* m_pUIWarBeltList{};
    CUIDragDropListEx* m_pUIBackPackList{};
    CUIDragDropListEx* m_pUITacticalVestList{};

    CUIDragDropListEx* m_pUIKnifeList{};
    CUIDragDropListEx* m_pUIFirstWeaponList{};
    CUIDragDropListEx* m_pUISecondWeaponList{};
    CUIDragDropListEx* m_pUIBinocularList{};

    CUIDragDropListEx* m_pUIGrenadeList{};
    CUIDragDropListEx* m_pUIArtefactList{};
    CUIDragDropListEx* m_pUIBoltList{};

    CUIDragDropListEx* m_pUIDetectorList{};
    CUIDragDropListEx* m_pUIOnHeadList{};
    CUIDragDropListEx* m_pUIPdaList{};

    //окремо для маркованих предметів
    CUIDragDropListEx* m_pUIMarkedList{};

    // alpet: для индексированного доступа
    CUIDragDropListEx* m_slots_array[SLOTS_TOTAL];

    void ClearAllLists();
    void BindDragDropListEvents(CUIDragDropListEx* lst);

    EListType GetType(CUIDragDropListEx* l);
    CUIDragDropListEx* GetSlotList(u8 slot_idx);

    bool OnItemDrop(CUICellItem* itm);
    bool OnItemStartDrag(CUICellItem* itm);
    bool OnItemDbClick(CUICellItem* itm);
    bool OnItemSelected(CUICellItem* itm);
    bool OnItemRButtonClick(CUICellItem* itm);

    CUIPropertiesBox UIPropertiesBox;

    // информация о персонаже
    CUIOutfitInfo UIOutfitInfo;

    CInventory* m_pInv;

public:
    CUICellItem* m_pCurrentCellItem;

protected:
    //bool DropItem(PIItem itm, CUIDragDropListEx* lst);
    bool TryUseItem(PIItem itm);

    void ProcessPropertiesBoxClicked();
    void ActivatePropertiesBox();

    void DropCurrentItem(bool b_all);
    void EatItem(PIItem itm);

    bool ToSlot(CUICellItem* itm, u8 _slot_id, bool force_place);
    bool ToSlot(CUICellItem* itm, bool force_place);
    bool ToBag(CUICellItem* itm, bool b_use_cursor_pos);
    bool ToBelt(CUICellItem* itm, bool b_use_cursor_pos);
    bool ToVest(CUICellItem* itm, bool b_use_cursor_pos);

    bool CanMoveToMarked(PIItem itm);
    bool OnToMarked(CUICellItem* itm, bool b_use_cursor_pos);
    void OnFromMarked(PIItem itm);

    void AttachAddon(PIItem item_to_upgrade);
    void DetachAddon(const char* addon_name, bool);

    void SetCurrentItem(CUICellItem* itm);
    CUICellItem* CurrentItem();

    TIItemContainer ruck_list;
    u32 m_iCurrentActiveSlot{NO_ACTIVE_SLOT};

public:
    PIItem CurrentIItem();
    // обновление отрисовки сетки пояса
    void UpdateCustomDraw();
    void ReinitBeltList();
    void ReinitVestList();
    void ReinitMarkedList();
    void ReinitSlotList(u32);
    void TryReinitLists(PIItem);
};
