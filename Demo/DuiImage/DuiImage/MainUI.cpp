#include "stdafx.h"
#include "MainUI.h"
#include "define/msg_define.h"
#include <Shlwapi.h>
#include "define/const_define.h"
#include "Dui/UITypedef.h"

#define TIMER_VIDEO_FRAME 1024

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
        result = OnCreate(uMsg, wParam, lParam, handled);
        break;
    case WM_NCHITTEST:
        result = OnNcHitTest(uMsg, wParam, lParam, handled);
        break;
    case WM_FILE_OPENED:
        result = OnOpenFile(uMsg, wParam, lParam, handled);
        break;
    case WM_SYSKEYDOWN:
        // quit on ALT + 1
        if (wParam == 0x31 && (lParam & (1 << 29))) {
            ::PostQuitMessage(0);
        }
        // repaint the window on ALT + 2
        else if (wParam == 0x32 && (lParam & (1 << 29))) {
            ::InvalidateRect(m_hWnd, NULL, FALSE);
        }
        break;
    case WM_TIMER:
        if (wParam == TIMER_VIDEO_FRAME) {
            //::KillTimer(m_hWnd, TIMER_VIDEO_FRAME);
            if (!ifs_.is_open()) {
                ifs_.open("test.video", std::ios::binary | std::ios::in);
            }            
            if (ifs_.is_open() && ifs_.peek() != EOF) {                
                frame_.width = 1920;
                frame_.height = 872;

                //frame.linesize[0] = frame_width;
                //frame.linesize[1] = 448;
                //frame.linesize[2] = 448;

                int count_of_y = frame_.width * frame_.height;
                frame_.yuv[0].resize(count_of_y, 0);
                ifs_.read(frame_.yuv[0].data(), count_of_y);

                frame_.yuv[1].resize(count_of_y / 4, 0);
                ifs_.read(frame_.yuv[1].data(), count_of_y / 4);

                frame_.yuv[2].resize(count_of_y / 4, 0);
                ifs_.read(frame_.yuv[2].data(), count_of_y / 4);

                if (video_) {
                    video_->SetVideoFrame(frame_);
                }
            }
            else {
                if (ifs_.is_open()) {
                    ifs_.close();
                }

                if (video_) {
                    video_->SetVideoFrame(VideoFrame());
                }

                ::KillTimer(m_hWnd, TIMER_VIDEO_FRAME);
                if (play_) {
                    play_->SetTag(0);
                    play_->SetNormalImage(L"file='play.png' source='0,0,38,38'");
                    play_->SetHotImage(L"file='play.png' source='38,0,76,38'");
                    play_->SetPushedImage(L"file='play.png' source='76,0,114,38'");
                }
                
                if (image_) {
                    image_->SetBkImage(L"file='adver.png' source='0,0,730,410' dest='0,125,730,535'");
                }
            }                 
        }
        break;
    default:
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

void CMainUI::Notify(TNotifyUI& msg) {
    if (!msg.pSender) {
        return;
    }

    if (msg.sType == DUI_MSGTYPE_CLICK) {
        //std::wstring name = msg.pSender->GetName();
        //if (name == L"menu") {
        //    RECT pos = msg.pSender->GetPos();
        //    if (::IsWindow(menu_.GetHWND())) {
        //        HWND menu_wnd = menu_.GetHWND();
        //        if (!::IsWindowVisible(menu_wnd)) {
        //            ::SetWindowPos(menu_wnd, NULL, pos.left, pos.bottom, 0, 0, SWP_NOACTIVATE | SWP_NOSIZE | SWP_SHOWWINDOW);
        //        }
        //    }

        //    else {
        //        menu_.Create(m_hWnd, L"DuiImageMenu", WS_CHILD, 0, 0, NULL);
        //        HWND menu_wnd = menu_.GetHWND();
        //        ::SetWindowPos(menu_wnd, NULL, pos.left, pos.bottom, 0, 0, SWP_NOACTIVATE | SWP_NOSIZE | SWP_SHOWWINDOW);
        //    }
        //}
    }
}

LRESULT CMainUI::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &handled) {
    handled = TRUE;

    pntm_.Init(m_hWnd,NULL);
    CDialogBuilder builder;
    CControlUI *root = builder.Create(L"MainUI.xml", 0, this, &pntm_, NULL);
    if (root) {
        pntm_.AttachDialog(root);
        pntm_.AddNotifier(this);
        SetSubControls();
    }
    
    return 0;
}

LRESULT CMainUI::OnOpenFile(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &handled) {
    handled = TRUE;

    if (wParam == 0) { //file opened successfully 
        std::wstring* file = (std::wstring*)lParam;
        if (::PathFileExists(file->c_str()) && index_ && image_) {
            CControlUI* index_image = new CControlUI;
            index_image->SetFixedHeight(index_image_height);
            index_image->SetFixedWidth(index_->GetWidth());
            index_image->SetBkImage(file->c_str());
            index_->Add(index_image);

            //separator
            CControlUI *separator = new CControlUI;
            separator->SetFixedHeight(index_separator_height);
            separator->SetBkColor(index_separator_bkcolor);
            index_->Add(separator);

            image_->SetBkImage(file->c_str());
        }

        delete file;
    }

    return 0;
}

void CMainUI::SetSubControls() {
    menu_btn_ = static_cast<CButtonUI*>(pntm_.FindControl(L"menu_btn"));
    if (menu_btn_) {
        menu_btn_->Subscribe(CEventSets::EventClick, MakeDelegate(this, &CMainUI::OnMenuClick));
    }
    image_ = static_cast<CContainerUI*>(pntm_.FindControl(L"image"));
    index_ = static_cast<CVerticalLayoutUI*>(pntm_.FindControl(L"index"));
    video_ = static_cast<CHorizontalLayoutUI*>(pntm_.FindControl(L"video"));
    video_->SetVideoAttribute(true);
    menu_ = static_cast<CVerticalLayoutUI*>(pntm_.FindControl(L"menu"));


    CButtonUI* exit = static_cast<CButtonUI*>(pntm_.FindControl(L"exit"));
    if (exit) {
        exit->Subscribe(CEventSets::EventClick, MakeDelegate(this, &CMainUI::OnMenuExit));
    }

    play_ = static_cast<CButtonUI*>(pntm_.FindControl(L"play"));
    if (play_) {
        play_->SetTag(0);
        play_->Subscribe(CEventSets::EventClick, MakeDelegate(this, &CMainUI::OnPlayClick));
    }
}

LRESULT CMainUI::OnNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &handled) {
    handled = TRUE;

    //lParam specifys the coordinates of the cursor
    //Do not use the LOWORD or HIWORD
    short xPos = GET_X_LPARAM(lParam);
    short yPos = GET_Y_LPARAM(lParam);

    RECT main_area = { 0 };
    ::GetWindowRect(m_hWnd, &main_area);

    RECT menu_area = { 0 };
    if (menu_btn_) {
        menu_area = menu_btn_->GetPos();
    }

    int x = xPos - main_area.left;
    int y = yPos - main_area.top;
    POINT pt = { x,y };

    RECT caption = { 0 };
    caption.left = menu_area.right;
    caption.right = main_area.right - menu_area.right;
    caption.bottom = menu_area.bottom;

    CControlUI* ctrl = pntm_.FindSubControlByPoint(NULL, pt);
    std::wstring ctrl_class_name = ctrl->GetClass();
    if (::PtInRect(&caption, pt) && ctrl_class_name != DUI_CTR_BUTTON && ctrl_class_name != DUI_CTR_CHECKBOX) {
        //std::wstringstream ws;
        //ws << L"DuiImage HTCAPTION." << ctrl_class_name;
        //::OutputDebugString(ws.str().c_str());
        return HTCAPTION;  //return HTCAPTION to make system treat this area as a titlebar
    }
    else {
        //std::wstringstream ws;
        //ws << L"DuiImage HTCLIENT." << ctrl_class_name;
        //::OutputDebugString(ws.str().c_str());
        return HTCLIENT;
    }

    //how windows system repsone to a mouse pressed?
    /*
        1¡¢find the window which now is being clicked
        2¡¢find the clicked position of the window by sending WM_NCHITTEST to query
        3¡¢according to the position, send corresponding msg to the window ,such as:
           WM_LBUTTONDOWN on HTCLIENT;
           WM_NCLBUTTONDOWN on the other.
    */
}

CControlUI* CMainUI::CreateControl(LPCTSTR pstrClass) {
    return nullptr;
}

bool CMainUI::OnMenuClick(void *p) {
    TNotifyUI *notify = (TNotifyUI*)p;
    if (notify) {
        CControlUI* sender = notify->pSender;
        if (sender) {
            //RECT pos = sender->GetPos();
            //if (::IsWindow(menu_.GetHWND())) {
            //    HWND menu_wnd = menu_.GetHWND();
            //    if (!::IsWindowVisible(menu_wnd)) {
            //        ::SetWindowPos(menu_wnd, NULL, pos.left, pos.bottom, 0, 0, SWP_NOACTIVATE | SWP_NOSIZE | SWP_SHOWWINDOW);
            //    }
            //}
            //else {
            //    menu_.Create(m_hWnd, L"DuiImageMenu", WS_CHILD, 0, 0, NULL);
            //    HWND menu_wnd = menu_.GetHWND();
            //    ::SetWindowPos(menu_wnd, NULL, pos.left, pos.bottom, 0, 0, SWP_NOACTIVATE | SWP_NOSIZE | SWP_SHOWWINDOW);
            //}
            if (!menu_->IsVisible()) {
                menu_->SetVisible(true);
            }
        }
    }
    
    return true;
}

bool CMainUI::OnMenuExit(void* p) {
    TNotifyUI *notify = (TNotifyUI*)p;
    if (notify) {
        CControlUI* sender = notify->pSender;
        if (sender) {            
            ::PostQuitMessage(0);
        }
    }

    return true;
}

bool CMainUI::OnPlayClick(void* p) {
    TNotifyUI *notify = (TNotifyUI*)p;
    if (notify) {
        CButtonUI* sender = static_cast<CButtonUI*>(notify->pSender);
        if (sender) {
            UINT_PTR tag = sender->GetTag();
            if (tag == 0) {
                sender->SetNormalImage(L"file='pause.png' source='0,0,38,38'");
                sender->SetHotImage(L"file='pause.png' source='38,0,76,38'");
                sender->SetPushedImage(L"file='pause.png' source='76,0,114,38'");

                if (image_) {
                    image_->SetBkImage(L"");
                }
                
                //start play
                ::SetTimer(m_hWnd, TIMER_VIDEO_FRAME, 40, NULL);

                sender->SetTag(1);
            }
            else {
                sender->SetNormalImage(L"file='play.png' source='0,0,38,38'");
                sender->SetHotImage(L"file='play.png' source='38,0,76,38'");
                sender->SetPushedImage(L"file='play.png' source='76,0,114,38'");

                //pause play
                ::KillTimer(m_hWnd, TIMER_VIDEO_FRAME);

                sender->SetTag(0);
            }
        }
    }

    return true;
}