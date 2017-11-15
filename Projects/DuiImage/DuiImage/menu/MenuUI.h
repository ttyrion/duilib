#pragma once
#include "UIlib.h"
#include "Core\UIBase.h"

using namespace DuiLib;

class CMenuUI : public CWindowWnd, public INotifyUI
{
public:
    CMenuUI();
    ~CMenuUI();

    virtual LPCTSTR GetWindowClassName() const override;
    virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) override;
    LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT &result);

    virtual void Notify(TNotifyUI& msg) override;
private:
    void SetSubControls();

    CPaintManagerUI pntm_;
    CButtonUI* about_ = nullptr;
    CButtonUI* exit_ = nullptr;
};

