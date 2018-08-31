#include "stdafx.h"
#include "UIOption.h"

namespace DuiLib
{
	COptionUI::COptionUI() : m_bSelected(false), m_dwSelectedBkColor(0), m_dwSelectedTextColor(0)
	{
	}

	COptionUI::~COptionUI()
	{
		if( !m_sGroupName.IsEmpty() && m_pManager ) m_pManager->RemoveOptionGroup(m_sGroupName, this);
	}

	LPCTSTR COptionUI::GetClass() const
	{
		return DUI_CTR_OPTION;
	}

	LPVOID COptionUI::GetInterface(LPCTSTR pstrName)
	{
		if( _tcscmp(pstrName, DUI_CTR_OPTION) == 0 ) return static_cast<COptionUI*>(this);
		return CButtonUI::GetInterface(pstrName);
	}

	void COptionUI::SetManager(CPaintManagerUI* pManager, CControlUI* pParent, bool bInit)
	{
		CControlUI::SetManager(pManager, pParent, bInit);
		if( bInit && !m_sGroupName.IsEmpty() ) {
			if (m_pManager) m_pManager->AddOptionGroup(m_sGroupName, this);
		}
	}

	LPCTSTR COptionUI::GetGroup() const
	{
		return m_sGroupName;
	}

	void COptionUI::SetGroup(LPCTSTR pStrGroupName)
	{
		if( pStrGroupName == NULL ) {
			if( m_sGroupName.IsEmpty() ) return;
			m_sGroupName.Empty();
		}
		else {
			if( m_sGroupName == pStrGroupName ) return;
			if (!m_sGroupName.IsEmpty() && m_pManager) m_pManager->RemoveOptionGroup(m_sGroupName, this);
			m_sGroupName = pStrGroupName;
		}

		if( !m_sGroupName.IsEmpty() ) {
			if (m_pManager) m_pManager->AddOptionGroup(m_sGroupName, this);
		}
		else {
			if (m_pManager) m_pManager->RemoveOptionGroup(m_sGroupName, this);
		}

		Selected(m_bSelected);
	}

	bool COptionUI::IsSelected() const
	{
		return m_bSelected;
	}

	void COptionUI::Selected(bool bSelected, bool bTriggerEvent)
	{
		if( m_bSelected == bSelected ) return;
		m_bSelected = bSelected;
		if( m_bSelected ) m_uButtonState |= UISTATE_SELECTED;
		else m_uButtonState &= ~UISTATE_SELECTED;

		if( m_pManager != NULL ) {
			if( !m_sGroupName.IsEmpty() ) {
				if( m_bSelected ) {
					CDuiPtrArray* aOptionGroup = m_pManager->GetOptionGroup(m_sGroupName);
					for( int i = 0; i < aOptionGroup->GetSize(); i++ ) {
						COptionUI* pControl = static_cast<COptionUI*>(aOptionGroup->GetAt(i));
						if( pControl != this ) {
							pControl->Selected(false, bTriggerEvent);
						}
					}
					if (bTriggerEvent) m_pManager->SendNotify(this, DUI_MSGTYPE_SELECTCHANGED);
				}
			}
			else {
				if (bTriggerEvent) m_pManager->SendNotify(this, DUI_MSGTYPE_SELECTCHANGED);
			}
		}

		Invalidate();
	}

	bool COptionUI::Activate()
	{
		if( !CButtonUI::Activate() ) return false;
		if( !m_sGroupName.IsEmpty() ) Selected(true);
		else Selected(!m_bSelected);

		return true;
	}

	void COptionUI::SetEnabled(bool bEnable)
	{
		CControlUI::SetEnabled(bEnable);
		if( !IsEnabled() ) {
			if( m_bSelected ) m_uButtonState = UISTATE_SELECTED;
			else m_uButtonState = 0;
		}
	}

	LPCTSTR COptionUI::GetSelectedImage()
	{
        if (mode_ == DrawMode_GDI) {
            return m_diSelected.sDrawString;
        }
        else {
            return selected_image_data_.sDrawString;
        }
	}

	void COptionUI::SetSelectedImage(LPCTSTR pStrImage)
	{
        switch (mode_)
        {
        case DuiLib::DrawMode_GDI: {
            if (m_diSelected.sDrawString == pStrImage && m_diSelected.pImageInfo != NULL) return;
            m_diSelected.Clear();
            m_diSelected.sDrawString = pStrImage;
        }
                                   break;
        case DuiLib::DrawMode_Direct3D_11: {
            if (selected_image_data_.sDrawString == pStrImage && !selected_image_data_.empty()) return;
            selected_image_data_.clear();
            selected_image_data_.sDrawString = pStrImage;
            Direct3DRender::LoadImage(selected_image_data_);
        }
        default:
            break;
        }

		Invalidate();
	}

	LPCTSTR COptionUI::GetSelectedHotImage()
	{
        if (mode_ == DrawMode_GDI) {
            return m_diSelectedHot.sDrawString;
        }
        else {
            return selected_hot_image_data_.sDrawString;
        }
	}

	void COptionUI::SetSelectedHotImage( LPCTSTR pStrImage )
	{
        switch (mode_)
        {
        case DuiLib::DrawMode_GDI: {
            if (m_diSelectedHot.sDrawString == pStrImage && m_diSelectedHot.pImageInfo != NULL) return;
            m_diSelectedHot.Clear();
            m_diSelectedHot.sDrawString = pStrImage;
        }
                                   break;
        case DuiLib::DrawMode_Direct3D_11: {
            if (selected_hot_image_data_.sDrawString == pStrImage && !selected_hot_image_data_.empty()) return;
            selected_hot_image_data_.clear();
            selected_hot_image_data_.sDrawString = pStrImage;
            Direct3DRender::LoadImage(selected_hot_image_data_);
        }
        default:
            break;
        }

		Invalidate();
	}

	void COptionUI::SetSelectedTextColor(DWORD dwTextColor)
	{
		m_dwSelectedTextColor = dwTextColor;
	}

	DWORD COptionUI::GetSelectedTextColor()
	{
		if (m_dwSelectedTextColor == 0) m_dwSelectedTextColor = m_pManager->GetDefaultFontColor();
		return m_dwSelectedTextColor;
	}

	void COptionUI::SetSelectedBkColor( DWORD dwBkColor )
	{
		m_dwSelectedBkColor = dwBkColor;
	}

	DWORD COptionUI::GetSelectBkColor()
	{
		return m_dwSelectedBkColor;
	}

	LPCTSTR COptionUI::GetForeImage()
	{
		return m_diFore.sDrawString;
	}

	void COptionUI::SetForeImage(LPCTSTR pStrImage)
	{
		if( m_diFore.sDrawString == pStrImage && m_diFore.pImageInfo != NULL ) return;
		m_diFore.Clear();
		m_diFore.sDrawString = pStrImage;
		Invalidate();
	}

	SIZE COptionUI::EstimateSize(SIZE szAvailable)
	{
		if( m_cxyFixed.cy == 0 ) return CDuiSize(m_cxyFixed.cx, m_pManager->GetFontInfo(GetFont())->tm.tmHeight + 8);
		return CControlUI::EstimateSize(szAvailable);
	}

	void COptionUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
	{
		if( _tcscmp(pstrName, _T("group")) == 0 ) SetGroup(pstrValue);
		else if( _tcscmp(pstrName, _T("selected")) == 0 ) Selected(_tcscmp(pstrValue, _T("true")) == 0);
		else if( _tcscmp(pstrName, _T("selectedimage")) == 0 ) SetSelectedImage(pstrValue);
		else if( _tcscmp(pstrName, _T("selectedhotimage")) == 0 ) SetSelectedHotImage(pstrValue);
		else if( _tcscmp(pstrName, _T("foreimage")) == 0 ) SetForeImage(pstrValue);
		else if( _tcscmp(pstrName, _T("selectedbkcolor")) == 0 ) {
			if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
			LPTSTR pstr = NULL;
			DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
			SetSelectedBkColor(clrColor);
		}
		else if( _tcscmp(pstrName, _T("selectedtextcolor")) == 0 ) {
			if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
			LPTSTR pstr = NULL;
			DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
			SetSelectedTextColor(clrColor);
		}
		else CButtonUI::SetAttribute(pstrName, pstrValue);
	}

	void COptionUI::PaintStatusImage(HDC hDC)
	{
		if( (m_uButtonState & UISTATE_SELECTED) != 0 ) {
			if ((m_uButtonState & UISTATE_HOT) != 0)
			{
				if (DrawImage(hDC, m_diSelectedHot)) goto Label_ForeImage;
			}

			if( DrawImage(hDC, m_diSelected) ) goto Label_ForeImage;
			else if(m_dwSelectedBkColor != 0) {
				CRenderEngine::DrawColor(hDC, m_rcPaint, GetAdjustColor(m_dwSelectedBkColor));
				goto Label_ForeImage;
			}	
		}

		UINT uSavedState = m_uButtonState;
		m_uButtonState &= ~UISTATE_PUSHED;
		CButtonUI::PaintStatusImage(hDC);
		m_uButtonState = uSavedState;

Label_ForeImage:
		DrawImage(hDC, m_diFore);
	}

    void COptionUI::PaintStatusImage()
    {
        auto draw_fore_image = [this]() {
            DrawImage(fore_image_data_);
        };

        if ((m_uButtonState & UISTATE_SELECTED) != 0) {
            if ((m_uButtonState & UISTATE_HOT) != 0)
            {
                if (DrawImage(selected_hot_image_data_)) {
                    draw_fore_image();
                    return;
                }
            }

            if (DrawImage(selected_image_data_)) {
                draw_fore_image();
                return;
            }
            else if (m_dwSelectedBkColor != 0) {
                if (m_pManager) {
                    m_pManager->DrawColor(m_rcPaint, m_dwSelectedBkColor);
                }
                draw_fore_image();
                return;
            }
        }

        UINT uSavedState = m_uButtonState;
        m_uButtonState &= ~UISTATE_PUSHED;
        CButtonUI::PaintStatusImage();
        m_uButtonState = uSavedState;
        draw_fore_image();
    }

	void COptionUI::PaintText(HDC hDC)
	{
		if( (m_uButtonState & UISTATE_SELECTED) != 0 )
		{
			DWORD oldTextColor = m_dwTextColor;
			if( m_dwSelectedTextColor != 0 ) m_dwTextColor = m_dwSelectedTextColor;

			if( m_dwTextColor == 0 ) m_dwTextColor = m_pManager->GetDefaultFontColor();
			if( m_dwDisabledTextColor == 0 ) m_dwDisabledTextColor = m_pManager->GetDefaultDisabledColor();

			if( m_sText.IsEmpty() ) return;
			int nLinks = 0;
			RECT rc = m_rcItem;
			rc.left += m_rcTextPadding.left;
			rc.right -= m_rcTextPadding.right;
			rc.top += m_rcTextPadding.top;
			rc.bottom -= m_rcTextPadding.bottom;

			if( m_bShowHtml )
				CRenderEngine::DrawHtmlText(hDC, m_pManager, rc, m_sText, IsEnabled()?m_dwTextColor:m_dwDisabledTextColor, \
				NULL, NULL, nLinks, m_iFont, m_uTextStyle);
			else
				CRenderEngine::DrawText(hDC, m_pManager, rc, m_sText, IsEnabled()?m_dwTextColor:m_dwDisabledTextColor, \
				m_iFont, m_uTextStyle);

			m_dwTextColor = oldTextColor;
		}
		else
		{
			UINT uSavedState = m_uButtonState;
			m_uButtonState &= ~UISTATE_PUSHED;
			CButtonUI::PaintText(hDC);
			m_uButtonState = uSavedState;
		}
	}

    void COptionUI::PaintText() {
        if ((m_uButtonState & UISTATE_SELECTED) != 0)
        {
            DWORD oldTextColor = m_dwTextColor;
            if (m_dwSelectedTextColor != 0) m_dwTextColor = m_dwSelectedTextColor;

            if (m_dwTextColor == 0) m_dwTextColor = m_pManager->GetDefaultFontColor();
            if (m_dwDisabledTextColor == 0) m_dwDisabledTextColor = m_pManager->GetDefaultDisabledColor();

            if (m_sText.IsEmpty()) return;
            int nLinks = 0;
            RECT rc = m_rcItem;
            rc.left += m_rcTextPadding.left;
            rc.right -= m_rcTextPadding.right;
            rc.top += m_rcTextPadding.top;
            rc.bottom -= m_rcTextPadding.bottom;

            if (m_pManager) {
                m_pManager->DrawText(rc, m_sText, m_iFont, IsEnabled() ? m_dwTextColor : m_dwDisabledTextColor, m_uTextStyle);
            }

            m_dwTextColor = oldTextColor;
        }
        else
        {
            UINT uSavedState = m_uButtonState;
            m_uButtonState &= ~UISTATE_PUSHED;
            CButtonUI::PaintText();
            m_uButtonState = uSavedState;
        }
    }
}