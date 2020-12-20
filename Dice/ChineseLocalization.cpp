/*
 *  ______   _____  ____  ____  ________
 * |_   _ `.|_   _||_  _||_  _||_   __  |
 *   | | `. \ | |    \ \  / /    | |_ \_|
 *   | |  | | | |     > `' <     |  _| _
 *  _| |_.' /_| |_  _/ /'`\ \_  _| |__/ |
 * |______.'|_____||____||____||________|
 *
 * OlivaDice(DIXE) QQ Dice Robot for TRPG
 * Copyright (C) 2019-2020 lunzhiPenxil仑质
 *
 */

#include <string>
#include <vector>
#include <map>
#include <cassert>
#include <Windows.h>
#include "EncodingConvert.h"
#include "ChineseLocalization.h"
#include "STLExtern.hpp"

#define CP_GBK (936)
#define CP_BIG5 (950)

#define WIN32_LEAN_AND_MEAN

//繁体到简体
string TCNGBKtoSCNGBK(const string _SrcString)
{
    string _DstText;
    char* pszGbt_old = NULL;
    wchar_t* wszUnicode = NULL;
    char* pszGbt = NULL;
    char* pszGbs = NULL;
    string sGb;
    int iLen = 0;
    pszGbt_old = (char*)((LPCSTR)_SrcString.c_str());
    iLen = MultiByteToWideChar(CP_GBK, 0, pszGbt_old, -1, NULL, 0);
    wszUnicode = new wchar_t[iLen + 1];
    MultiByteToWideChar(CP_GBK, 0, pszGbt_old, -1, wszUnicode, iLen);
    iLen = WideCharToMultiByte(CP_GBK, 0, (PWSTR)wszUnicode, -1, NULL, 0, NULL, NULL);
    pszGbt = new char[iLen + 1];
    pszGbs = new char[iLen + 1];
    WideCharToMultiByte(CP_GBK, 0, (PWSTR)wszUnicode, -1, pszGbt, iLen, NULL, NULL);
    LCMapString(0x0804, LCMAP_SIMPLIFIED_CHINESE, pszGbt, -1, pszGbs, iLen);
    sGb = pszGbs;
    delete[] wszUnicode;
    delete[] pszGbt;
    delete[] pszGbs;

    _DstText = string(sGb.c_str());
    return _DstText;
}

//简体到繁体
string SCNGBKtoTCNGBK(const string _SrcString)
{
    string _DstText;
    char* pszGbt_old = NULL;
    wchar_t* wszUnicode = NULL;
    char* pszGbt = NULL;
    char* pszGbs = NULL;
    string sGb;
    int iLen = 0;
    pszGbt_old = (char*)((LPCSTR)_SrcString.c_str());
    iLen = MultiByteToWideChar(CP_GBK, 0, pszGbt_old, -1, NULL, 0);
    wszUnicode = new wchar_t[iLen + 1];
    MultiByteToWideChar(CP_GBK, 0, pszGbt_old, -1, wszUnicode, iLen);
    iLen = WideCharToMultiByte(CP_GBK, 0, (PWSTR)wszUnicode, -1, NULL, 0, NULL, NULL);
    pszGbt = new char[iLen + 1];
    pszGbs = new char[iLen + 1];
    WideCharToMultiByte(CP_GBK, 0, (PWSTR)wszUnicode, -1, pszGbt, iLen, NULL, NULL);
    LCMapString(0x0804, LCMAP_TRADITIONAL_CHINESE, pszGbt, -1, pszGbs, iLen);
    sGb = pszGbs;
    delete[] wszUnicode;
    delete[] pszGbt;
    delete[] pszGbs;

    _DstText = string(sGb.c_str());
    return _DstText;
}


//完整编码转换
//繁体到简体
string TCNBIG5toSCNGBK(const string _SrcString)
{
    string _DstText;
    char* pszBig5 = NULL;
    wchar_t* wszUnicode = NULL;
    char* pszGbt = NULL;
    char* pszGbs = NULL;
    string sGb;
    int iLen = 0;
    pszBig5 = (char*)((LPCSTR)_SrcString.c_str());
    iLen = MultiByteToWideChar(CP_BIG5, 0, pszBig5, -1, NULL, 0);
    wszUnicode = new wchar_t[iLen + 1];
    MultiByteToWideChar(CP_BIG5, 0, pszBig5, -1, wszUnicode, iLen);
    iLen = WideCharToMultiByte(CP_GBK, 0, (PWSTR)wszUnicode, -1, NULL, 0, NULL, NULL);
    pszGbt = new char[iLen + 1];
    pszGbs = new char[iLen + 1];
    WideCharToMultiByte(CP_GBK, 0, (PWSTR)wszUnicode, -1, pszGbt, iLen, NULL, NULL);
    LCMapString(0x0804, LCMAP_SIMPLIFIED_CHINESE, pszGbt, -1, pszGbs, iLen);
    sGb = pszGbs;
    delete[] wszUnicode;
    delete[] pszGbt;
    delete[] pszGbs;

    _DstText = string(sGb.c_str());
    return _DstText;
}

//简体到繁体
string SCNGBKtoTCNBIG5(const string _SrcString)
{
    string _DstText;
    char* pszGbt = NULL;
    char* pszGbs = NULL;
    wchar_t* wszUnicode = NULL;
    char* pszBig5 = NULL;
    string sBig5;
    int iLen = 0;
    pszGbs = (char*)((LPCSTR)_SrcString.c_str());
    iLen = MultiByteToWideChar(CP_GBK, 0, pszGbs, -1, NULL, 0);
    pszGbt = new char[iLen * 2 + 1];
    LCMapString(0x0804, LCMAP_TRADITIONAL_CHINESE, pszGbs, -1, pszGbt, iLen * 2);
    wszUnicode = new wchar_t[iLen + 1];
    MultiByteToWideChar(CP_GBK, 0, pszGbt, -1, wszUnicode, iLen);
    iLen = WideCharToMultiByte(CP_BIG5, 0, (PWSTR)wszUnicode, -1, NULL, 0, NULL, NULL);
    pszBig5 = new char[iLen + 1];
    WideCharToMultiByte(CP_BIG5, 0, (PWSTR)wszUnicode, -1, pszBig5, iLen, NULL, NULL);
    sBig5 = pszBig5;
    delete[] wszUnicode;
    delete[] pszGbt;
    delete[] pszBig5;

    _DstText = string(sBig5.c_str());
    return _DstText;
}
