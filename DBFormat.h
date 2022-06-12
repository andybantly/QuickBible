// Copyright (C) 2007 Andrew S. Bantly
#pragma once
#include <vector>

class CDBVerse : public CObject
{
	DECLARE_SERIAL(CDBVerse)
public:
	CDBVerse();
	CDBVerse(const CDBVerse & rhs) {*this = rhs;}
	CDBVerse & operator = (const CDBVerse & rhs);
	virtual void Serialize(CArchive & ar);
	std::vector<BYTE> m_vecCryptName;
	std::vector<BYTE> m_vecCryptVerse;
};

class CDBChap : public CObject
{
	DECLARE_SERIAL(CDBChap)
public:
	CDBChap();
	CDBChap(const CDBChap & rhs) {*this = rhs;}
	CDBChap & operator = (const CDBChap & rhs);
	virtual void Serialize(CArchive & ar);
	std::vector<BYTE> m_vecCryptName;
	std::vector<CDBVerse> m_vVerse;
};

class CDBBook : public CObject
{
	DECLARE_SERIAL(CDBBook)
public:
	CDBBook();
	CDBBook(const CDBBook & rhs) {*this = rhs;}
	CDBBook & operator = (const CDBBook & rhs);
	virtual void Serialize(CArchive & ar);
	std::vector<BYTE> m_vecCryptName;
	std::vector<CDBChap> m_vChap;
};

class CDBBible : public CObject
{
	DECLARE_SERIAL(CDBBible)
public:
	CDBBible();
	CDBBible(const CDBBible & rhs) {*this = rhs;}
	CDBBible & operator = (const CDBBible & rhs);
	virtual void Serialize(CArchive & ar);
	std::vector<BYTE> m_vecCryptName;
	std::vector<CDBBook> m_vBook;
};

class CDBTranslation : public CObject
{
	DECLARE_SERIAL(CDBTranslation)
public:
	CDBTranslation();
	CDBTranslation(const CDBTranslation & rhs) {*this = rhs;}
	CDBTranslation & operator = (const CDBTranslation & rhs);
	virtual void Serialize(CArchive & ar);
	std::vector<BYTE> m_vecCryptName;
	std::vector<CDBBible> m_vBible;
};
