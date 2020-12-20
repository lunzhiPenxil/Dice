/*
 *  ______   _____  ____  ____  ________
 * |_   _ `.|_   _||_  _||_  _||_   __  |
 *   | | `. \ | |    \ \  / /    | |_ \_|
 *   | |  | | | |     > `' <     |  _| _
 *  _| |_.' /_| |_  _/ /'`\ \_  _| |__/ |
 * |______.'|_____||____||____||________|
 *
 * OlivaDice(DIXE) QQ Dice Robot for TRPG
 * Copyright (C) 2019-2020 lunzhiPenxilÂØÖÊ
 *
 */

#include "Lua/lua.hpp"

#include <string>
#include <regex>
#include "DiceEvent.h"

#ifdef LUA_DICELUALIBNAME
#undef LUA_DICELUALIBNAME
#endif

#define LUA_DICELUALIBNAME "dice"

#define LUA_DICELUACOMMANDLIST "command"

#define LUA_DICELUAREGRANGE (1024)

#define DiceLua_pushObjInt(L,NAME,OBJ) \
            lua_pushinteger(L, OBJ);\
            lua_setfield(L, (-2), NAME);

#define DiceLua_pushObjString(L,NAME,OBJ) \
            lua_pushstring(L, OBJ);\
            lua_setfield(L, (-2), NAME);

struct Dice_Msg_T
{
    int msgType = 0;
    long long selfId = 0;
    long long fromQQ = 0;
    long long fromGroup = 0;
    long long tergetId = 0;
    int fromQQTrust = 0;
    int fromQQInfo = 1;
    const char* msg = NULL;
    unsigned int str_max = 1;
    char** str = NULL;
};

int DiceLua_pushMsg(lua_State* L, Dice_Msg_T &Dice_Msg);

LUAMOD_API int luaopen_diceLua(lua_State* L);
LUALIB_API void DiceLua_openlibs(lua_State* L);

int DiceLua_init(lua_State* L);

extern std::map<std::string, std::string> DiceLua_LoadList;

namespace DiceLua
{
    void GetFilesFromDirectory(std::vector<std::string>& files, const char* directoryPath);
    void DiceLua_LoadCommandList(std::map<std::string, std::string>& DiceLua_LoadList, std::string& strLog, std::string DiceLua_DirPath);
    bool isRegexMatch(std::string strRegex, std::string strInput);
    std::string doRegexReplace(std::string strRegex, std::string strInput, std::string strfmt);
    bool getRegexMatchResult(const std::string& strRegex, const std::string& strInput, std::smatch& result);
    int deletePlist(char*** in, unsigned int size);
}