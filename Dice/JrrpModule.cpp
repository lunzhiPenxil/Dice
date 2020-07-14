#include "JrrpModule.h"
#include "json.hpp"
#include "Jsonio.h"
#include "GlobalVar.h"
#include "RD.h"
#include "CardDeck.h"
#include "MD5.h"
namespace JrrpModule
{
	using std::unique;
	using std::replace;
	using std::find;

	int LocalJrrpGenerate(long long fromQQ, int LocalJrrpVar, int LocalJrrpMin, int LocalJrrpRange)
	{
		int md5_1 = 6;
		struct tm* LocalTime;
		const time_t TimeStamp = time(NULL);
		LocalTime = localtime(&TimeStamp);
		string md5_str;
		switch (LocalJrrpVar)
		{
			case 1:
			{
				md5_str = md5(to_string(fromQQ) + to_string(LocalTime->tm_year) + to_string(LocalTime->tm_mon) + to_string(LocalTime->tm_mday));
				break;
			}
			case 2:
			{
				md5_str = md5(to_string(console.DiceMaid) + to_string(fromQQ) + to_string(LocalTime->tm_year) + to_string(LocalTime->tm_mon) + to_string(LocalTime->tm_mday));
				break;
			}
		}
		int res_num = 0;
		for (int str_i = 0; str_i < 4; str_i++)
		{
			if ('0' <= md5_str[str_i + md5_1] && '9' >= md5_str[str_i + md5_1])
			{
				res_num = md5_str[str_i + md5_1] - '0' + res_num * 16;
			}
			else if ('a' <= md5_str[str_i + md5_1] && 'z' >= md5_str[str_i + md5_1])
			{
				res_num = md5_str[str_i + md5_1] - 'a' + 10 + res_num * 16;
			}
			else if ('A' <= md5_str[str_i + md5_1] && 'Z' >= md5_str[str_i + md5_1])
			{
				res_num = md5_str[str_i + md5_1] - 'A' + 10 + res_num * 16;
			}
			else
			{
				res_num = res_num * 16;
			}
		}
		res_num %= LocalJrrpRange;
		return res_num + LocalJrrpMin;
	}

	string Execute(int intJrrpValue,string strNick)
	{
		std::map<string, int> console_this =
		{
			{"LocalJrrp", console["LocalJrrp"]},
			{"LocalJrrpMin", console["LocalJrrpMin"]},
			{"LocalJrrpRange", console["LocalJrrpRange"]},
			{"JrrpDashesRange", console["JrrpDashesRange"]}
		};
		if (console_this["LocalJrrp"] == 0)
		{
			console_this["LocalJrrpMin"] = 1;
			console_this["LocalJrrpRange"] = 100;
		}
		try
		{
			SYSTEMTIME stNow;
			GetLocalTime(&stNow);
			if (console_this["LocalJrrpMin"] <= intJrrpValue && console_this["LocalJrrpMin"] + console_this["LocalJrrpRange"] - 1 >= intJrrpValue)
			{
				string strReply = GlobalMsg["strJrrp"];
				string strDashes = string((intJrrpValue + 1 - console_this["LocalJrrpMin"]) * console_this["JrrpDashesRange"] / console_this["LocalJrrpRange"], '|') + string(console_this["JrrpDashesRange"] - (intJrrpValue + 1 - console_this["LocalJrrpMin"]) * console_this["JrrpDashesRange"] / console_this["LocalJrrpRange"], ' ');
				while (strReply.find("{nick}") != string::npos)
				{
					strReply.replace(strReply.find("{nick}"), 6, strNick);
				}
				while (strReply.find("{res}") != string::npos)
				{
					strReply.replace(strReply.find("{res}"), 5, to_string(intJrrpValue));
				}
				while (strReply.find("{dashes}") != string::npos)
				{
					strReply.replace(strReply.find("{dashes}"), 8, strDashes);
				}
				return CardDeck::draw(strReply);
			}
		}
		catch (...)
		{
			return GlobalMsg["strJrrp"];
		}
		return GlobalMsg["strJrrp"];
	}
}
