#pragma once

#include "UIDialogWnd.h"
#include "UIEditBox.h"
#include "../inventory_space.h"

class CUIDragDropListEx;
class CUIItemInfo;
class CUICharacterInfo;
class CUIPropertiesBox;
class CUI3tButton;
class CUICheckButton;
class CUICellItem;
class IInventoryBox;
class CInventoryOwner;
class CGameObject;

class CUICarBodyWnd : public CUIDialogWnd
{
private:
    typedef CUIDialogWnd inherited;
    bool m_b_need_update;

public:
    CUICarBodyWnd();
    virtual ~CUICarBodyWnd();

    virtual void Init();
    virtual bool StopAnyMove() { return true; }

    virtual void SendMessage(CUIWindow* pWnd, s16 msg, void* pData);

    void InitCarBody(CInventoryOwner* pOurInv, CInventoryOwner* pOthersInv);
    void InitCarBody(CInventoryOwner* pOur, IInventoryBox* pInvBox);
    virtual void Draw();
    virtual void Update();

    virtual void Show();
    virtual void Hide();

    virtual bool OnKeyboard(int dik, EUIMessages keyboard_action);
    virtual bool OnMouse(float x, float y, EUIMessages mouse_action);

    void UpdateLists_delayed();
    void RepackAmmo();

protected:
    CInventoryOwner* m_pActorInventoryOwner{};

    CUIDragDropListEx* m_pUIOurBagList;
    CUIDragDropListEx* m_pUIOthersBagList;

    CUIItemInfo* m_pUIItemInfo;

    CUIPropertiesBox* m_pUIPropertiesBox;

public:
    CUICellItem* m_pCurrentCellItem;

    CInventoryOwner* m_pOtherInventoryOwner{};
    IInventoryBox* m_pOtherInventoryBox{};

    CGameObject* m_pActorGO{};
    CGameObject* m_pOtherGO{};

protected:
    void UpdateLists();

    void ActivatePropertiesBox();
    void EatItem();

    void SetCurrentItem(CUICellItem* itm);
    CUICellItem* CurrentItem();
    PIItem CurrentIItem();

    // Взять все
    void TakeAll();
    void MoveItems(CUICellItem* itm, bool b_all);
    void DropItems(bool b_all);
    void MoveAllFromRuck();
    void DetachAddon(const char* addon_name, bool);

    bool OnItemDrop(CUICellItem* itm);
    bool OnItemStartDrag(CUICellItem* itm);
    bool OnItemDbClick(CUICellItem* itm);
    bool OnItemSelected(CUICellItem* itm);
    bool OnItemRButtonClick(CUICellItem* itm);
    //
    bool OnItemFocusReceived(CUICellItem* itm);
    bool OnItemFocusLost(CUICellItem* itm);
    bool OnItemFocusedUpdate(CUICellItem* itm);

    bool TransferItem(PIItem itm, CGameObject* owner_from, CGameObject* owner_to);
    void BindDragDropListEvents(CUIDragDropListEx* lst);

    enum eInventorySndAction
    {
        eInvSndOpen = 0,
        eInvSndClose,
        eInvProperties,
        eInvDropItem,
        eInvDetachAddon,
        eInvMoveItem,
        eInvSndMax
    };

    ref_sound sounds[eInvSndMax];
    void PlaySnd(eInventorySndAction a);

    bool CanMoveToOther(PIItem pItem, CGameObject* owner_to) const;

    CUICellItem* itm_to_descr{};
};