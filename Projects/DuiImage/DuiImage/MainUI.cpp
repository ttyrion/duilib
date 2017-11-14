#include "stdafx.h"
#include "MainUI.h"


CMainUI::CMainUI() {

}


CMainUI::~CMainUI() {
    int m = 0;
}

LPCTSTR CMainUI::GetWindowClassName() const {
    return L"DuiImage";
}

LRESULT CMainUI::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
    BOOL handled = FALSE;
    LRESULT result = 0;
    switch (uMsg) {
    case WM_CREATE:
        OnCreate(uMsg, wParam, lParam, result);
        break;
    }

    if (handled) {
        return result;
    }

    if (pntm_.MessageHandler(uMsg, wParam, lParam, result)) {
        return result;
    }

    return CWindowWnd::HandleMessage(uMsg, wParam, lParam);
}

LRESULT CMainUI::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT &result) {
    result = TRUE;
    pntm_.Init(m_hWnd);
    CDialogBuilder builder;
    CControlUI *root = builder.Create(L"MainUI.xml", 0, this, &pntm_, NULL);
    if (root) {
        pntm_.AttachDialog(root);
    }
    
    return 0;
}

CControlUI* CMainUI::CreateControl(LPCTSTR pstrClass) {
    return nullptr;
}