#pragma once
#include "UIlib.h"
#include "Core/UIBase.h"
#include "menu/MenuUI.h"

using namespace DuiLib;

class CMainUI : public CWindowWnd, IDialogBuilderCallback, public INotifyUI
{
public:
    CMainUI();
    ~CMainUI();

    virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) override;
    virtual LPCTSTR GetWindowClassName() const override;

    //message handle
    LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &handled);
    LRESULT OnNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &handled);

    //IDialogBuilderCallback
    virtual CControlUI* CreateControl(LPCTSTR pstrClass) override;
    //INotifyUI
    virtual void Notify(TNotifyUI& msg) override;

    void SetSubControls();
private:
    CPaintManagerUI pntm_;
    CMenuUI menu_;
    CButtonUI* menu_btn_ = nullptr;
};

