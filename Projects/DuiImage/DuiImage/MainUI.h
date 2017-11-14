#pragma once
#include "../../../duilib/Dui/UIlib.h"
#include "Core/UIBase.h"

using namespace DuiLib;

class CMainUI : public CWindowWnd, IDialogBuilderCallback
{
public:
    CMainUI();
    ~CMainUI();

    virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) override;
    virtual LPCTSTR GetWindowClassName() const override;

    //message handle
    LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT &result);

    //IDialogBuilderCallback
    virtual CControlUI* CreateControl(LPCTSTR pstrClass) override;
private:
    CPaintManagerUI pntm_;
};

