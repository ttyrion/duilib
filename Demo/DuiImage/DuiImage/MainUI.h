#pragma once
#include <fstream>
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
    LRESULT OnOpenFile(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &handled);

    //IDialogBuilderCallback
    virtual CControlUI* CreateControl(LPCTSTR pstrClass) override;
    //INotifyUI
    virtual void Notify(TNotifyUI& msg) override;

    bool OnMenuClick(void *p);
    bool OnMenuExit(void* p);
    bool OnPlayClick(void* p);
    bool OnCloseAdverClick(void* p);

    void SetSubControls();
private:
    CPaintManagerUI pntm_;
    CMenuUI sub_wnd_;
    CButtonUI* menu_btn_ = nullptr;
    CVerticalLayoutUI* menu_ = nullptr;
    CContainerUI* image_ = nullptr;
    CVerticalLayoutUI* index_ = nullptr;
    CHorizontalLayoutUI* video_ = nullptr;
    CButtonUI* play_ = nullptr;
    CLabelUI* water_mark_ = nullptr;
    CVerticalLayoutUI* adver_ = nullptr;
    CButtonUI* close_adver_ = nullptr;
    VideoFrame frame_;

    std::ifstream ifs_;
};

