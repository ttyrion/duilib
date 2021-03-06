#ifndef __UICONTROL_H__
#define __UICONTROL_H__

#pragma once
#include "D3DTypes.h"
#include "Animation.h"

namespace DuiLib {

/*
    目前CControlUI基类会根据mode_来调用GDI或者D3D绘制，实际上更好的方案是从CControlUI派生出CGDIControlUI和CD3DControlUI。
    在解析xml的时候就根据mode来创建CGDIControlUI或者CD3DControlUI。后期打算把Duilib控件类层次改一下。
*/

typedef CControlUI* (CALLBACK* FINDCONTROLPROC)(CControlUI*, LPVOID);

class DUILIB_API CControlUI : public CEventSets
{
public:
    CControlUI();
    virtual void Delete();

protected:
    virtual ~CControlUI();

public:
    virtual CDuiString GetName() const;
    virtual void SetName(LPCTSTR pstrName);
    virtual LPCTSTR GetClass() const;
    virtual LPVOID GetInterface(LPCTSTR pstrName);
    virtual UINT GetControlFlags() const;
	virtual HWND GetNativeWindow() const;

    virtual bool Activate();
    virtual CPaintManagerUI* GetManager() const;
    virtual void SetManager(CPaintManagerUI* pManager, CControlUI* pParent, bool bInit = true);
    virtual CControlUI* GetParent() const;
    virtual CControlUI* GetCover() const;
    virtual void SetCover(CControlUI *pControl);
    virtual bool GetCovering() const;
    virtual void SetCovering(bool cover);

    // 文本相关
    virtual CDuiString GetText() const;
    virtual void SetText(LPCTSTR pstrText);

    // 图形相关
    std::uint8_t GetAlpha() const;
    virtual void SetAlpha(std::uint8_t alpha);
    DWORD GetBkColor() const;
    DWORD GetBlendBkColor() const;  //获取控件背景色，或者父控件背景色，用于背景图片alpha混合
    void SetBkColor(DWORD dwBackColor);
    DWORD GetBkColor2() const;
    void SetBkColor2(DWORD dwBackColor);
    DWORD GetBkColor3() const;
    void SetBkColor3(DWORD dwBackColor);
    LPCTSTR GetBkImage();
    void SetBkImage(LPCTSTR pStrImage);
	DWORD GetFocusBorderColor() const;
	void SetFocusBorderColor(DWORD dwBorderColor);
    bool IsColorHSL() const;
    void SetColorHSL(bool bColorHSL);
    SIZE GetBorderRound() const;
    void SetBorderRound(SIZE cxyRound);
    bool DrawImage(HDC hDC, TDrawInfo& drawInfo);
    bool DrawImage(ImageData& image);
    bool DrawVideoFrame(const VideoFrame& frame);

	//边框相关
	DWORD GetBorderColor() const;
	void SetBorderColor(DWORD dwBorderColor);
	RECT GetBorderSize() const;
	void SetBorderSize(RECT rc);
	void SetBorderSize(int iSize);
	int GetBorderStyle() const;
	void SetBorderStyle(int nStyle);

    // 位置相关
    bool IsPlaceHolder();
    virtual const RECT& GetPos() const;
	virtual RECT GetRelativePos() const; // 相对(父控件)位置
	virtual RECT GetClientPos() const; // 客户区域（除去scrollbar和inset）
	// 只有控件为float的时候，外部调用SetPos和Move才是有效的，位置参数是相对父控件的位置
    virtual void SetPos(RECT rc, bool bNeedInvalidate = true);
	virtual void Move(SIZE szOffset, bool bNeedInvalidate = true);
    virtual int GetWidth() const;
    virtual int GetHeight() const;
    virtual int GetX() const;
    virtual int GetY() const;
    virtual RECT GetPadding() const;
    virtual void SetPadding(RECT rcPadding); // 设置外边距，由上层窗口绘制
    virtual SIZE GetFixedXY() const;         // 实际大小位置使用GetPos获取，这里得到的是预设的参考值
    virtual void SetFixedXY(SIZE szXY);      // 仅float为true时有效
	virtual TPercentInfo GetFloatPercent() const;
	virtual void SetFloatPercent(TPercentInfo piFloatPercent);
    virtual int GetFixedWidth() const;       // 实际大小位置使用GetPos获取，这里得到的是预设的参考值
    virtual void SetFixedWidth(int cx);      // 预设的参考值
    virtual int GetFixedHeight() const;      // 实际大小位置使用GetPos获取，这里得到的是预设的参考值
    virtual void SetFixedHeight(int cy);     // 预设的参考值
    virtual int GetMinWidth() const;
    virtual void SetMinWidth(int cx);
    virtual int GetMaxWidth() const;
    virtual void SetMaxWidth(int cx);
    virtual int GetMinHeight() const;
    virtual void SetMinHeight(int cy);
    virtual int GetMaxHeight() const;
    virtual void SetMaxHeight(int cy);

    // 鼠标提示
    virtual CDuiString GetToolTip() const;
    virtual void SetToolTip(LPCTSTR pstrText);
	virtual void SetToolTipWidth(int nWidth);
	virtual int	  GetToolTipWidth(void);	// 多行ToolTip单行最长宽度

    // 快捷键
    virtual TCHAR GetShortcut() const;
    virtual void SetShortcut(TCHAR ch);

    // 菜单
    virtual bool IsContextMenuUsed() const;
    virtual void SetContextMenuUsed(bool bMenuUsed);

    // 用户属性
    virtual const CDuiString& GetUserData(); // 辅助函数，供用户使用
    virtual void SetUserData(LPCTSTR pstrText); // 辅助函数，供用户使用
    virtual UINT_PTR GetTag() const; // 辅助函数，供用户使用
    virtual void SetTag(UINT_PTR pTag); // 辅助函数，供用户使用

    // 一些重要的属性
    virtual bool IsVisible() const;
    virtual void SetVisible(bool bVisible = true);
    virtual void SetInternVisible(bool bVisible = true); // 仅供内部调用，有些UI拥有窗口句柄，需要重写此函数
    virtual bool IsEnabled() const;
    virtual void SetEnabled(bool bEnable = true);
    virtual bool IsMouseEnabled() const;
    virtual void SetMouseEnabled(bool bEnable = true);
    virtual bool IsKeyboardEnabled() const;
    virtual void SetKeyboardEnabled(bool bEnable = true);
    virtual bool IsFocused() const;
    virtual void SetFocus();
    void SetFloat(bool bFloat = true);
    bool IsFloat() const;
    bool IsRealFloat() const; //控件本身或者父、祖先控件是否float: 判断控件是否会覆盖到其它无父子关系的控件上

	// 自定义(未处理的)属性
	void AddCustomAttribute(LPCTSTR pstrName, LPCTSTR pstrAttr);
	LPCTSTR GetCustomAttribute(LPCTSTR pstrName) const;
	bool RemoveCustomAttribute(LPCTSTR pstrName);
	void RemoveAllCustomAttribute();

    virtual CControlUI* FindControl(FINDCONTROLPROC Proc, LPVOID pData, UINT uFlags);

    void Invalidate();
    bool IsUpdateNeeded() const;
    void NeedUpdate();
    void NeedParentUpdate();
    DWORD GetAdjustColor(DWORD dwColor);

    virtual void Init();
    virtual void DoInit();

    virtual void Event(TEventUI& event);
    virtual void DoEvent(TEventUI& event);
    virtual bool OnEventInternal(void* event);

    virtual CDuiString GetAttribute(LPCTSTR pstrName);
    virtual void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
	virtual CDuiString GetAttributeList(bool bIgnoreDefault = true);
    virtual void SetAttributeList(LPCTSTR pstrList);

    virtual SIZE EstimateSize(SIZE szAvailable);

	virtual bool Paint(HDC hDC, const RECT& rcPaint, CControlUI* pStopControl=NULL); // 返回要不要继续绘制
    virtual bool DoPaint(HDC hDC, const RECT& rcPaint, CControlUI* pStopControl);
    virtual void PaintBkColor(HDC hDC);
    virtual void PaintBkImage(HDC hDC);
    virtual void PaintStatusImage(HDC hDC);
    virtual void PaintText(HDC hDC);
    virtual void PaintBorder(HDC hDC);

    //paint with Direct3D
    virtual bool Paint(const RECT& rcPaint, CControlUI* pStopControl = NULL);
    virtual bool DoPaint(const RECT& rcPaint, CControlUI* pStopControl);
    virtual void PaintBkColor();
    virtual void PaintBkImage();
    virtual void PaintStatusImage();
    virtual void PaintVideoFrame();
    virtual void PaintText();
    virtual void PaintBorder();

    void PaintIntersectFloats();
    void SetVideoFrame(const VideoFrame& frame);
    void SetVideoAttribute(bool video_control);
    bool GetVideoAttribute();
    void SetVideoPlaying(bool playing);
    bool GetVideoPlaying();
    virtual void DoPostPaint(HDC hDC, const RECT& rcPaint);

	//虚拟窗口参数
	void SetVirtualWnd(LPCTSTR pstrValue);
	CDuiString GetVirtualWnd() const;
    
    //动画
    void EnableAnimator(anim::ANIM_SLOT slot);
    void DisableAnimator(anim::ANIM_SLOT slot);
    std::shared_ptr<anim::StoryBoard>& GetStoryBoard();
    std::shared_ptr<anim::IAnimator>& GetAnimator(anim::ANIM_SLOT slot);
public:
    CEventSource OnInit;
    CEventSource OnDestroy;
    CEventSource OnSize;
    CEventSource OnEvent;
    CEventSource OnNotify;
	CEventSource OnPaint;
	CEventSource OnPostPaint;

protected:
        DrawMode mode_;

protected:
    CPaintManagerUI* m_pManager;
    CControlUI* m_pParent;
    CControlUI* m_pCover;
    bool covering_ = false; //此控件是否配置了cover=true，与m_pCover意义不同
	CDuiString m_sVirtualWnd;
    CDuiString m_sName;
    bool m_bUpdateNeeded;
    bool m_bMenuUsed;
	bool m_bAsyncNotify;
    RECT m_rcItem;
    RECT m_rcPadding;
    SIZE m_cXY;  //fixed x/y
    SIZE m_cxyFixed; //fixed h/w
    SIZE m_cxyMin;
    SIZE m_cxyMax;
    bool m_bVisible;
    bool m_bInternVisible;
    bool m_bEnabled;
    bool m_bMouseEnabled;
	bool m_bKeyboardEnabled ;
    bool m_bFocused;
    bool m_bFloat;
	TPercentInfo m_piFloatPercent;
    bool m_bSetPos; // 防止SetPos循环调用

    CDuiString m_sText;
    CDuiString m_sToolTip;
    TCHAR m_chShortcut;
    CDuiString m_sUserData;
    UINT_PTR m_pTag;

    DWORD m_dwBackColor;
    DWORD m_dwBackColor2;
    DWORD m_dwBackColor3;
    TDrawInfo m_diBk;
	TDrawInfo m_diFore;

    //for d3d rendering
    bool m_bVideoControl = false;
    bool m_bPlayingVideo = false;
    VideoFrame frame_;
    ImageData back_image_data_;
    ImageData fore_image_data_;
    

    DWORD m_dwBorderColor;
	DWORD m_dwFocusBorderColor;
    bool m_bColorHSL;
	int m_nBorderStyle;
	int m_nTooltipWidth;
    SIZE m_cxyBorderRound;
    RECT m_rcPaint;
	RECT m_rcBorderSize;
	CDuiStringPtrMap m_mCustomAttrHash;

private:
    std::shared_ptr<anim::StoryBoard> story_board_;
    std::vector<std::shared_ptr<anim::IAnimator>> animators_ = { std::shared_ptr<anim::IAnimator>(), std::shared_ptr<anim::IAnimator>() };
    std::uint8_t ui_alpha_ = 255;

    bool m_bPlaceHolder = false;
};

} // namespace DuiLib

#endif // __UICONTROL_H__
