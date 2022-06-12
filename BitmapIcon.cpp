// BitmapIcon.cpp : Bitmap based icon implementation file
// Copyright (C) 2007 Andrew S. Bantly
//
#include "stdafx.h"
#include "BitmapIcon.h"

CBitmapIcon::CBitmapIcon() : m_nID(0), m_crTransparentColor(RGB(0,0,0))
{
}

CBitmapIcon::CBitmapIcon(UINT nID) : m_nID(nID), m_crTransparentColor(RGB(0,0,0))
{
	Initialize();
}

CBitmapIcon::CBitmapIcon(const CBitmapIcon & rhs)
{
	*this = rhs;
}

CBitmapIcon::~CBitmapIcon()
{
}

CBitmapIcon & CBitmapIcon::operator = (const CBitmapIcon & rhs)
{
	if (this != &rhs)
	{
		m_nID = rhs.m_nID;
		Initialize();
	}
	return *this;
}

void CBitmapIcon::Load(UINT nID)
{
	m_nID = nID;
	Initialize();
}

void CBitmapIcon::LoadIcon(UINT nID)
{
	m_Icon.LoadIcon(nID);
}

UINT CBitmapIcon::GetWidth()
{
	return m_Icon.GetWidth();
}

UINT CBitmapIcon::GetHeight()
{
	return m_Icon.GetHeight();
}

COLORREF CBitmapIcon::GetTransparentColor()
{
	return m_crTransparentColor;
}

CBitmapIcon::operator HICON()
{
	return m_Icon;
}

void CBitmapIcon::Initialize()
{
	m_Bitmap.DeleteObject();
	if (m_Bitmap.LoadBitmap(m_nID))
		ConvertBitmap();
}

void CBitmapIcon::ConvertBitmap()
{
	// Create a memory DC that contains the loaded up bitmap
	CDC ImageDC;
	ImageDC.CreateCompatibleDC(NULL);
	CBitmap * pOldImageBitmap = ImageDC.SelectObject(&m_Bitmap);

	// Get the transparent pixel from the upper left of the bitmap
	m_crTransparentColor = ImageDC.GetPixel(0,0);

	// Get the bitmap information
	BITMAP bm;
	m_Bitmap.GetBitmap(&bm);

	// Create a memory DC for the mask monochrome bitmap
	CDC MaskDC;
	MaskDC.CreateCompatibleDC(NULL);

	// Initialize the "mask" monochrome bitmap
	CBitmap MaskBitmap;
	MaskBitmap.CreateBitmap(bm.bmWidth,bm.bmHeight,1,1,NULL);
	CBitmap * pOldMaskBitmap = MaskDC.SelectObject(&MaskBitmap);

	// Create the "mask" bitmap
	ImageDC.SetBkColor(m_crTransparentColor);
	MaskDC.BitBlt(0,0,bm.bmWidth,bm.bmHeight,&ImageDC,0,0,SRCCOPY);

	// Create a memory DC for the color bitmap
	CDC ColorDC;
	ColorDC.CreateCompatibleDC(NULL);

	// Initialize the "color" bitmap
	CBitmap ColorBitmap;
	ColorBitmap.CreateCompatibleBitmap(&ImageDC,bm.bmWidth,bm.bmHeight);
	CBitmap * pOldOrBitmap = ColorDC.SelectObject(&ColorBitmap);

	// Create the "color" bitmap
	ColorDC.BitBlt(0,0,bm.bmWidth,bm.bmHeight,&ImageDC,0,0,SRCCOPY);
	ColorDC.BitBlt(0,0,bm.bmWidth,bm.bmHeight,&MaskDC,0,0,0x220326);

	// Clean up the image bitmap
	ImageDC.SelectObject(pOldImageBitmap);

	// Clean up the "color" bitmap
	ColorDC.SelectObject(pOldOrBitmap);

	// Clean up the "mask" bitmap
	MaskDC.SelectObject(pOldMaskBitmap);

	// Create the Icon
	m_Icon.CreateIcon((HBITMAP)MaskBitmap,(HBITMAP)ColorBitmap);
}