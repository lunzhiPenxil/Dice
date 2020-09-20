/*
 * 骰娘网络
 * Copyright (C) 2019 String.Empty
 */
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <WinInet.h>
#include <urlmon.h>
#include "json.hpp"
#include "DiceCloud.h"
#include "GlobalVar.h"
#include "EncodingConvert.h"
#include "CQAPI_EX.h"
#include "DiceNetwork.h"
#include "DiceConsole.h"
#include "DiceMsgSend.h"
#include "DiceEvent.h"

#pragma comment(lib, "urlmon.lib")

using namespace std;
using namespace nlohmann;

namespace Cloud
{
	void update()
	{
		//Shiki心跳
		if (console["PulseMode"] & (0x001 << 0))
		{
			const string strVer = GBKtoUTF8(string(Dice_Ver));
			const string data = "DiceQQ=" + to_string(console.DiceMaid) + "&masterQQ=" + to_string(console.master()) + "&Ver=" +
				strVer + "&isGlobalOn=" + to_string(!console["DisabledGlobal"]) + "&isPublic=" +
				to_string(!console["Private"]) + "&isVisible=" + to_string(console["CloudVisible"]);
			char* frmdata = new char[data.length() + 1];
			strcpy_s(frmdata, data.length() + 1, data.c_str());
			string temp;
			Network::POST("shiki.stringempty.xyz", "/DiceCloud/update.php", 80, frmdata, temp);
			//AddMsgToQueue(temp, masterQQ);
			delete[] frmdata;
		}
		//自定义心跳标准
		if (console["PulseMode"] & (0x001 << 1) && GlobalMsg["strConfigPulseToken"] != "NULL")
		{
			const time_t TimeStamp = time(NULL);
			const string strVer = GBKtoUTF8(string(Dice_Ver));
			const string data =
				"token=" + GlobalMsg["strConfigPulseToken"] +
				"&user_id=" + to_string(console.DiceMaid) +
				"&time_ts=" + to_string(TimeStamp) + "000" +
				"&version=Dice! " + strVer +
				"&name=" + UrlEncode(GBKtoUTF8(GlobalMsg["strSelfName"])) +
				"&masterid=" + to_string(console.master()) +
				"&interval=330" +
				"&isGlobalOn=" + to_string(!console["DisabledGlobal"]) +
				"&isPublic=" + to_string(!console["Private"]) +
				"&isVisible=" + to_string(console["CloudVisible"]);
			char* frmdata = new char[data.length() + 1];
			strcpy_s(frmdata, data.length() + 1, data.c_str());
			int port = 80;
			if (GlobalMsg["strConfigPulsePort"].length() <= 5)
			{
				port = stoi(GlobalMsg["strConfigPulsePort"]);
			}
			string temp;
			Network::POST(GlobalMsg["strConfigPulseHost"].c_str(), GlobalMsg["strConfigPulseXpath"].c_str(), port, frmdata, temp);
			//AddMsgToQueue(temp, console.master());
			delete[] frmdata;
		}
		
	}

	int checkWarning(const char* warning)
	{
		char* frmdata = new char[strlen(warning) + 1];
		strcpy_s(frmdata, strlen(warning) + 1, warning);
		string temp;
		Network::POST("shiki.stringempty.xyz", "/DiceCloud/warning_check.php", 80, frmdata, temp);
		delete[] frmdata;
		if (temp == "exist")
		{
			return 1;
		}
		if (temp == "erased")
		{
			return -1;
		}
		return 0;
	}

	int DownloadFile(const char* url, const char* downloadPath)
	{
		DeleteUrlCacheEntryA(url);
		if (URLDownloadToFileA(nullptr, url, downloadPath, 0, nullptr) != S_OK) return -1;
		if (_access(downloadPath, 0))return -2;
		return 0;
	}

	int checkUpdate(FromMsg* msg)
	{
		std::string strVerInfo;
		if (!Network::GET("shiki.stringempty.xyz", "/DiceVer/update", 80, strVerInfo))
		{
			msg->reply("{self}获取版本信息时遇到错误: \n" + strVerInfo);
			return -1;
		}
		try
		{
			json jInfo(json::parse(strVerInfo, nullptr, false));
			unsigned short nBuild = jInfo["release"]["build"];
			if (nBuild > Dice_Build)
			{
				msg->note(
					"发现Dice!的发布版更新:" + jInfo["release"]["ver"].get<string>() + "(" + to_string(nBuild) + ")\n更新说明：" +
					UTF8toGBK(jInfo["release"]["changelog"].get<string>()), 1);
				return 1;
			}
			if (nBuild = jInfo["dev"]["build"]; nBuild > Dice_Build)
			{
				msg->note(
					"发现Dice!的开发版更新:" + jInfo["dev"]["ver"].get<string>() + "(" + to_string(nBuild) + ")\n更新说明：" +
					UTF8toGBK(jInfo["dev"]["changelog"].get<string>()), 1);
				return 2;
			}
			msg->reply("未发现版本更新。");
			return 0;
		}
		catch (...)
		{
			msg->reply("{self}解析json失败！");
			return -2;
		}
		return 0;
	}
}
