#pragma once
#include "inventory_item_object.h"
class CWarbelt : public CInventoryItemObject
{
private:
    typedef CInventoryItemObject inherited;

public:
    CWarbelt();
    virtual ~CWarbelt();

    virtual void Load(LPCSTR section);

protected:
    Ivector2 m_BeltArray{};
    bool m_BeltVertical{};

public:
    Ivector2 GetBeltArray() { return m_BeltArray; };
    bool GetBeltVertical() { return m_BeltVertical; };

    virtual void OnMoveToSlot(EItemPlace prevPlace);
    virtual void OnMoveToRuck(EItemPlace prevPlace);
};