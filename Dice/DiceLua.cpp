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

#include "DiceLua.h"

#include <string>
#include <sstream>
#include <regex>
#include <map>
#include "EncodingConvert.h"
#include "CardDeck.h"
#include "DiceEvent.h"
#include "DiceMsgSend.h"
#include "DiceCloud.h"
#include "DiceFile.hpp"
#include "CharacterCard.h"
#include "RD.h"
#include "MD5.h"
#include "json.hpp"
#include "ChineseLocalization.h"

#pragma warning(disable:4244)

int DiceLua_pushMsg(lua_State* L, Dice_Msg_T &Dice_Msg)
{
    lua_newtable(L);
    DiceLua_pushObjInt(L, "msgType", Dice_Msg.msgType);
    DiceLua_pushObjInt(L, "selfId", Dice_Msg.selfId);
    DiceLua_pushObjInt(L, "fromQQ", Dice_Msg.fromQQ);
    DiceLua_pushObjInt(L, "fromGroup", Dice_Msg.fromGroup);
    DiceLua_pushObjInt(L, "tergetId", Dice_Msg.tergetId);
    DiceLua_pushObjInt(L, "fromQQTrust", Dice_Msg.fromQQTrust);
    DiceLua_pushObjInt(L, "fromQQInfo", Dice_Msg.fromQQInfo);
    DiceLua_pushObjString(L, "msg", Dice_Msg.msg);
    DiceLua_pushObjInt(L, "str_max", Dice_Msg.str_max);
    lua_pushstring(L, "str");
    lua_newtable(L);
    int str_count = 0;
    for (unsigned int i = 0; i < Dice_Msg.str_max; i++)
    {
        lua_pushinteger(L, str_count);
        if (i == 0)
        {
            lua_pushstring(L, Dice_Msg.msg);
        }
        else
        {
            lua_pushstring(L, Dice_Msg.str[i]);
        }
        lua_settable(L, -3);
        str_count++;
    }
    lua_settable(L, -3);
    return 0;
}

int Dice_Draw(lua_State* L)
{
    int n = lua_gettop(L);
    std::string strReply;
    std::string str_draw = lua_tostring(L, 1);
    str_draw = UTF8toGBK(str_draw);
    strReply = CardDeck::draw(str_draw);
    strReply = GBKtoUTF8(strReply);
    lua_pushstring(L, strReply.c_str());
    return 1;
}

int Dice_Send(lua_State* L)
{
    int n = lua_gettop(L);
    std::string strReply = lua_tostring(L, 1);
    long long tergetId = lua_tointeger(L, 2);
    int msgTypeNum = lua_tointeger(L, 3);
    CQ::msgtype msgType = CQ::msgtype(msgTypeNum);
    strReply = UTF8toGBK(strReply);
    AddMsgToQueue(strReply, tergetId, msgType);
    return 0;
}

int Dice_Int2String(lua_State* L)
{
    int n = lua_gettop(L);
    long long inputInt = lua_tointeger(L, 1);
    std::string strReply = to_string(inputInt);
    lua_pushstring(L, strReply.c_str());
    return 1;
}

int Dice_Roll(lua_State* L)
{
    int n = lua_gettop(L);
    std::string input = lua_tostring(L, 1);
    RD rv_roll(input);
    rv_roll.Roll();
    lua_pushinteger(L, rv_roll.intTotal);
    return 1;
}

int Dice_MD5(lua_State* L)
{
    int n = lua_gettop(L);
    std::string input = lua_tostring(L, 1);
    lua_pushstring(L, md5(input).c_str());
    return 1;
}

int Dice_DiceDir(lua_State* L)
{
    int n = lua_gettop(L);
    std::string rv = DiceDir;
    rv = GBKtoUTF8(rv);
    lua_pushstring(L, rv.c_str());
    return 1;
}

int Dice_MKDir(lua_State* L)
{
    int n = lua_gettop(L);
    std::string dir_path = lua_tostring(L, 1);
    dir_path = UTF8toGBK(dir_path);
    int rv = mkDir(dir_path);
    lua_pushinteger(L, rv);
    return 1;
}

int Dice_TCNtoSCN(lua_State* L)
{
    int n = lua_gettop(L);
    std::string rv = lua_tostring(L, 1);
    rv = UTF8toGBK(rv);
    rv = TCNGBKtoSCNGBK(rv);
    rv = GBKtoUTF8(rv);
    lua_pushstring(L, rv.c_str());
    return 1;
}

int Dice_SCNtoTCN(lua_State* L)
{
    int n = lua_gettop(L);
    std::string rv = lua_tostring(L, 1);
    rv = UTF8toGBK(rv);
    rv = SCNGBKtoTCNGBK(rv);
    rv = GBKtoUTF8(rv);
    lua_pushstring(L, rv.c_str());
    return 1;
}

int Dice_GBKtoUTF8(lua_State* L)
{
    int n = lua_gettop(L);
    std::string rv = lua_tostring(L, 1);
    rv = GBKtoUTF8(rv);
    lua_pushstring(L, rv.c_str());
    return 1;
}

int Dice_UTF8toGBK(lua_State* L)
{
    int n = lua_gettop(L);
    std::string rv = lua_tostring(L, 1);
    rv = UTF8toGBK(rv);
    lua_pushstring(L, rv.c_str());
    return 1;
}

int Dice_UrlEncode(lua_State* L)
{
    int n = lua_gettop(L);
    std::string rv = lua_tostring(L, 1);
    rv = UrlEncode(rv);
    lua_pushstring(L, rv.c_str());
    return 1;
}

int Dice_UrlDecode(lua_State* L)
{
    int n = lua_gettop(L);
    std::string rv = lua_tostring(L, 1);
    rv = UrlDecode(rv);
    lua_pushstring(L, rv.c_str());
    return 1;
}

int Dice_GetPcSkill(lua_State* L)
{
    int n = lua_gettop(L);
    long long fromQQ = lua_tointeger(L, 1);
    long long fromGroup = lua_tointeger(L, 2);
    std::string skill = lua_tostring(L, 3);
    skill = UTF8toGBK(skill);
    std::string skillVal = "0";
    CharaCard& pc = getPlayer(fromQQ)[fromGroup];
    int rv_int = pc.show(skill, skillVal);
    if (skill == "note")
    {
        skillVal = "0";
    }
    int rv = stoi(skillVal);
    lua_pushinteger(L, rv);
    return 1;
}

int Dice_SetPcSkill(lua_State* L)
{
    int n = lua_gettop(L);
    long long fromQQ = lua_tointeger(L, 1);
    long long fromGroup = lua_tointeger(L, 2);
    std::string skill = lua_tostring(L, 3);
    skill = UTF8toGBK(skill);
    int skillVal = lua_tointeger(L, 4);
    CharaCard& pc = getPlayer(fromQQ)[fromGroup];
    pc.set(skill, skillVal);
    return 0;
}

int Dice_GetPcName(lua_State* L)
{
    int n = lua_gettop(L);
    long long fromQQ = lua_tointeger(L, 1);
    long long fromGroup = lua_tointeger(L, 2);
    std::string name = getName(fromQQ, fromGroup);
    name = GBKtoUTF8(name);
    lua_pushstring(L, name.c_str());
    return 1;
}

int Dice_SetPcName(lua_State* L)
{
    int n = lua_gettop(L);
    long long fromQQ = lua_tointeger(L, 1);
    long long fromGroup = lua_tointeger(L, 2);
    std::string name_set = lua_tostring(L, 3);
    name_set = UTF8toGBK(name_set);
    getUser(fromQQ).setNick(fromGroup, name_set);
    return 0;
}

int Dice_FReadJson(lua_State* L)
{
    int n = lua_gettop(L);
    std::string rv = "";
    std::string rv_json_path = "jsonbase";
    std::string dot = ".";
    std::string file_path = lua_tostring(L, 1);

    file_path = UTF8toGBK(file_path);

    std::stringstream json_get;

    std::ifstream fin(file_path);
    nlohmann::json obj_json;
    if (!fin)
    {
        rv = "fReadJson#file not there";
    }
    else
    {
        try
        {
            fin >> obj_json;
            for (int i = 2; i <= n; i++)
            {
                if (lua_isinteger(L, i))
                {
                    int this_obj = lua_tointeger(L, i);
                    rv_json_path += dot + "[" + to_string(this_obj) + "]";
                    obj_json = obj_json[this_obj];
                }
                else if (lua_isstring(L, i))
                {
                    std::string this_obj = lua_tostring(L, i);
                    rv_json_path += dot + this_obj;
                    obj_json = obj_json[this_obj];
                }
            }
            json_get << obj_json;
            rv = json_get.str();
        }
        catch (...)
        {
            rv = "fReadJson#can not reach:" + rv_json_path;
        }
    }

    lua_pushstring(L, rv.c_str());
    return 1;
}

int Dice_FGetJson(lua_State* L)
{
    int n = lua_gettop(L);
    std::string rv = "";
    std::string rv_json_path = "jsonbase";
    std::string dot = ".";
    std::string file_path = lua_tostring(L, 1);
    std::string str_lobby = lua_tostring(L, 2);

    bool flag_not_in_law = false;

    file_path = UTF8toGBK(file_path);

    std::stringstream json_in;
    std::stringstream json_get;

    std::ifstream fin(file_path);
    nlohmann::json obj_json;
    if (!fin)
    {
        rv = str_lobby;
    }
    else
    {
        try
        {
            json_in << fin.rdbuf();
            if (nlohmann::json::accept(json_in.str()))
            {
                obj_json = nlohmann::json::parse(json_in.str());
                for (int i = 3; i <= n; i++)
                {
                    if (lua_isinteger(L, i))
                    {
                        unsigned int this_obj = lua_tointeger(L, i);
                        rv_json_path += dot + "[" + to_string(this_obj) + "]";
                        if (obj_json.size() < this_obj)
                        {
                            flag_not_in_law = true;
                        }
                        obj_json = obj_json[this_obj];
                    }
                    else if (lua_isstring(L, i))
                    {
                        std::string this_obj = lua_tostring(L, i);
                        rv_json_path += dot + this_obj;
                        obj_json = obj_json[this_obj];
                    }
                }
                if (obj_json.is_structured())
                {
                    json_get << obj_json;
                }
                else if (obj_json.is_string())
                {
                    json_get << obj_json.get<std::string>();
                }
                else
                {
                    json_get << obj_json;
                }
                rv = json_get.str();
                if (flag_not_in_law || rv == "null" || rv == "")
                {
                    rv = str_lobby;
                }
            }
            else
            {
                rv = str_lobby;
            }
        }
        catch (...)
        {
            rv = str_lobby;
        }
    }

    lua_pushstring(L, rv.c_str());
    return 1;
}

int Dice_FSetJson(lua_State* L)
{
    int n = lua_gettop(L);
    std::string rv = "";
    std::string rv_json_path = "jsonbase";
    std::string dot = ".";
    std::string file_path = lua_tostring(L, 1);
    std::string str_set = lua_tostring(L, 2);

    file_path = UTF8toGBK(file_path);

    std::stringstream json_in;
    std::stringstream json_get;

    std::ifstream fin(file_path);
    nlohmann::json obj_json;
    std::vector<std::pair<nlohmann::json, std::string>> json_path_list;
    if (!fin)
    {
        rv = "fSetJson#file not there";
    }
    else
    {
        try
        {
            json_in << fin.rdbuf();
            if (nlohmann::json::accept(json_in.str()))
            {
                obj_json = nlohmann::json::parse(json_in.str());
                for (int i = 3; i <= n; i++)
                {
                    if (lua_isstring(L, i))
                    {
                        std::string this_obj = lua_tostring(L, i);
                        rv_json_path += dot + this_obj;
                        json_path_list.push_back(pair(obj_json, this_obj));
                        obj_json = obj_json[this_obj];
                    }
                }
                json_path_list.push_back(pair(obj_json, ""));
            }
            else
            {
                rv = "fSetJson#is not json";
            }
        }
        catch (...)
        {
            rv = "fSetJson#can not reach:" + rv_json_path;
        }
    }

    std::ofstream fout(file_path);
    if (!fout)
    {
        rv = "fSetJson#file not there";
    }
    else
    {
        try
        {
            for (auto json_path_list_it = json_path_list.rbegin(); json_path_list_it != json_path_list.rend(); ++json_path_list_it)
            {
                if (json_path_list_it->second == "")
                {
                    obj_json = str_set;
                }
                else
                {
                    json_path_list_it->first[json_path_list_it->second] = obj_json;
                    obj_json = json_path_list_it->first;
                }
            }

            fout << std::setw(2) << obj_json;
            rv = "fSetJson#done";
        }
        catch (...)
        {
            rv = "fSetJson#can not reach:" + rv_json_path;
        }
    }
    

    lua_pushstring(L, rv.c_str());
    return 1;
}

int Dice_FDownWebPage(lua_State* L)
{
    int n = lua_gettop(L);
    std::string dot = ".";
    std::string url = "";
    std::string file_path = "";
    
    int rv = -1;

    try
    {
        url = lua_tostring(L, 1);
        file_path = lua_tostring(L, 2);
    }
    catch (...)
    {
        url = "";
        file_path = "";
    }

    file_path = UTF8toGBK(file_path);
    try
    {
        rv = Cloud::DownloadFile(url.c_str(), file_path.c_str());
    }
    catch (...)
    {
        rv = -1;
    }

    lua_pushinteger(L, rv);
    return 1;
}

static const luaL_Reg diceLualib[] = {
    {"draw", Dice_Draw},
    {"send", Dice_Send},
    {"int2string", Dice_Int2String},
    {"rd", Dice_Roll},
    {"md5", Dice_MD5},
    {"DiceDir", Dice_DiceDir},
    {"mkDir", Dice_MKDir},
    {"TCNtoSCN", Dice_TCNtoSCN},
    {"SCNtoTCN", Dice_SCNtoTCN},
    {"GBKtoUTF8", Dice_GBKtoUTF8},
    {"UTF8toGBK", Dice_UTF8toGBK},
    {"UrlEncode", Dice_UrlEncode},
    {"UrlDecode", Dice_UrlDecode},
    {"getPcSkill", Dice_GetPcSkill},
    {"setPcSkill", Dice_SetPcSkill},
    {"getPcName", Dice_GetPcName},
    {"setPcName", Dice_SetPcName},
    {"fReadJson", Dice_FReadJson},
    {"fGetJson", Dice_FGetJson},
    {"fSetJson", Dice_FSetJson},
    {"fDownWebPage", Dice_FDownWebPage},
    {NULL, NULL}
};

/*
** Open DiceLua library
*/
LUAMOD_API int luaopen_diceLua(lua_State* L) {
    luaL_newlib(L, diceLualib);
    return 1;
}

LUALIB_API void DiceLua_openlibs(lua_State* L)
{
    std::string lib_path;
    lua_getglobal(L, "package");
    lua_getfield(L, -1, "path");
    lib_path = lua_tostring(L, -1);
    lib_path += ";" + DiceDir + "\\plugin\\lib\\?.lua";
    lua_getglobal(L, "package");
    lua_pushstring(L, lib_path.c_str());
    lua_setfield(L, -2, "path");
    luaL_requiref(L, LUA_DICELUALIBNAME, luaopen_diceLua, 1);
    lua_pop(L, 1);
}

int DiceLua_init(lua_State* L)
{
    const luaL_Reg* lib;
    for (lib = diceLualib; lib->func; lib++)
    {
        lua_register(L, lib->name, lib->func);
    }
    return 1;
}


std::map<std::string, std::string> DiceLua_LoadList = {};

namespace DiceLua
{
    void GetFilesFromDirectory(std::vector<std::string>& files, const char* directoryPath)
    {
        struct _finddata_t fileinfo;
        long hFile = 0;
        char tmpPath[MAX_PATH] = { 0 };
        sprintf_s(tmpPath, "%s\\*", directoryPath);
        if ((hFile = _findfirst(tmpPath, &fileinfo)) == -1) { return; }
        do
        {
            if ((fileinfo.attrib & _A_SUBDIR))
            {
                if (strcmp(fileinfo.name, ".") != 0 && 
                    strcmp(fileinfo.name, "..") != 0 &&
                    strcmp(fileinfo.name, "lib") != 0)
                {
                    sprintf_s(tmpPath, "%s\\%s", directoryPath, fileinfo.name);
                    GetFilesFromDirectory(files, tmpPath);
                }
            }
            else
            {
                sprintf_s(tmpPath, "%s\\%s", directoryPath, fileinfo.name);
                files.push_back(tmpPath);
            }
        } while (_findnext(hFile, &fileinfo) == 0);
        _findclose(hFile);
    }
    
    void DiceLua_LoadCommandList(std::map<std::string, std::string> &DiceLua_LoadList, std::string &strLog, std::string DiceLua_DirPath)
    {
        std::vector<std::string> DiceLua_FileList_tmp = {};
        GetFilesFromDirectory(DiceLua_FileList_tmp, DiceLua_DirPath.c_str());
        for (const auto DiceLua_FileList_this : DiceLua_FileList_tmp)
        {
            if (DiceLua_FileList_this.size() <= 4 || 
                DiceLua_FileList_this.substr(DiceLua_FileList_this.size() - 4, DiceLua_FileList_this.size()) != ".lua")
            {
                continue;
            }

            lua_State* L_load = luaL_newstate();
            luaL_openlibs(L_load);
            DiceLua_openlibs(L_load);
            int bRet = luaL_loadfile(L_load, DiceLua_FileList_this.c_str());
            if (bRet)
            {
                continue;
            }
            bRet = lua_pcall(L_load, 0, 0, 0);;
            if (bRet)
            {
                continue;
            }
            try
            {
                lua_getglobal(L_load, LUA_DICELUACOMMANDLIST);
                if (lua_istable(L_load, -1))
                {
                    lua_pushnil(L_load);
                    while (lua_next(L_load, -2) != 0)
                    {
                        string key;
                        string value;
                        key = lua_tostring(L_load, -2);
                        value = lua_tostring(L_load, -1);
                        lua_pop(L_load, 1);
                        DiceLua_LoadList[UTF8toGBK(key)] = DiceLua_FileList_this;
                    }
                }
            }
            catch (...)
            {

            }
        }
        if (!DiceLua_LoadList.empty())
        {
            strLog += "读取" + DiceDir + "\\plugin\\中的" + to_string(DiceLua_FileList_tmp.size()) + "个文件, 共" + to_string(DiceLua_LoadList.size()) + "个条目\n";
        }
    }

    bool isRegexMatch(std::string strRegex, std::string strInput)
    {
        std::regex regStrRegex(strRegex);
        return std::regex_match(strInput, regStrRegex);
    }

    std::string doRegexReplace(std::string strRegex, std::string strInput, std::string strfmt)
    {
        std::regex regStrRegex(strRegex);
        return std::regex_replace(strInput, regStrRegex, strfmt);
    }

    bool getRegexMatchResult(const std::string &strRegex, const std::string &strInput, std::smatch &result)
    {
        const std::regex regStrRegex(strRegex);
        return std::regex_match(strInput, result, regStrRegex);
    }

    int deletePlist(char*** in, unsigned int size)
    {
        for (unsigned int i = 0; i < size; i++)
        {
            delete[] *in[i];
        }
        delete[] *in;
        return 1;
    }
}