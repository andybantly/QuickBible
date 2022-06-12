// BitmapIcon.h : Bitmap based icon header file
// Copyright (C) 2007 Andrew S. Bantly
//
#pragma once

class CBitmapIcon
{
	// Wrapper class for the Icon
	class CIcon
	{
	public:
		CIcon() : m_hIcon(NULL), m_iWidth(0), m_iHeight(0) {}
		~CIcon()
		{
			CleanupIcon();
		}

		// Return a handle to the icon
		operator HICON() {return m_hIcon;}
		
		// Create an Icon from two bitmaps
		void CreateIcon(HBITMAP hMaskBitmap,HBITMAP hColorBitmap)
		{
			// Clean up a previous icon
			CleanupIcon();

			// Create an Icon from the "color" and "mask" bitmaps
			m_IconInfo.fIcon = TRUE; // TRUE is icon, FALSE is cursor
			m_IconInfo.xHotspot = 0; // ignored for icon
			m_IconInfo.yHotspot = 0; // ignored for icon
			m_IconInfo.hbmMask = hMaskBitmap;
			m_IconInfo.hbmColor = hColorBitmap;
			m_hIcon = CreateIconIndirect(&m_IconInfo);
			GetDimensions();
		}

		// Load an Icon from a resource
		void LoadIcon(UINT nID)
		{
			CleanupIcon();
			m_hIcon = (HICON)LoadImage(AfxGetResourceHandle(),MAKEINTRESOURCE(nID),IMAGE_ICON,0,0,LR_DEFAULTCOLOR);
			GetIconInfo(m_hIcon,&m_IconInfo);
			GetDimensions();
		}

		// Clean up the icon
		void CleanupIcon()
		{
			if (m_hIcon)
			{
				DestroyIcon(m_hIcon);
				m_hIcon = NULL;
			}
		}

		void GetDimensions()
		{
			BITMAP BitMap;
			GetObject(m_IconInfo.hbmColor,sizeof(BITMAP),&BitMap);
			m_iWidth = BitMap.bmWidth;
			m_iHeight = BitMap.bmHeight;
		}

		int GetWidth()
		{
			return m_iWidth;
		}

		int GetHeight()
		{
			return m_iHeight;
		}
	private:
		HICON m_hIcon;
		ICONINFO m_IconInfo;
		int m_iWidth,m_iHeight;
	};

public:
	CBitmapIcon();
	CBitmapIcon(UINT nID);
	CBitmapIcon(const CBitmapIcon & rhs);
	~CBitmapIcon();
	CBitmapIcon & operator = (const CBitmapIcon & rhs);

public:
	void Load(UINT nID);
	void LoadIcon(UINT nID);
	UINT GetWidth();
	UINT GetHeight();
	COLORREF GetTransparentColor();
	operator HICON();

protected:
	void Initialize();
	void ConvertBitmap();

private:
	UINT m_nID;
	CBitmap m_Bitmap;
	COLORREF m_crTransparentColor;
	CIcon m_Icon;
};