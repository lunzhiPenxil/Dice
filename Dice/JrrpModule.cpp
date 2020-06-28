#include "JrrpModule.h"
#include "json.hpp"
#include "Jsonio.h"
#include "GlobalVar.h"
#include "RD.h"
#include "CardDeck.h"
namespace JrrpModule
{
	using std::unique;
	using std::replace;
	using std::find;
	vector<JrrpConfigure> Configure;
	void SetDefault()
	{
		Configure.clear();
		Configure.push_back(JrrpConfigure(1, 100, { GlobalMsg["strJrrp"] }));
	}
	void LoadConfigure(string strPath)
	{
		Configure.clear();
		nlohmann::json content = freadJson(strPath);
		if (content.empty() || !content.is_array())
		{
			SetDefault();
			return;
		}
		try
		{
			for (auto x : content)
			{
				JrrpConfigure cfg(-1, -1, {});
				cfg.intJrrpMax = x["MaxJrrp"];
				cfg.intJrrpMin = x["MinJrrp"];
				cfg.vReplies = x["Reply"].get<vector<string>>();
				for (auto& s : cfg.vReplies)
				{
					s = UTF8toGBK(s);
				}
				if (x.count("WeekDays"))
					cfg.vWeekDays = x["WeekDays"].get<vector<int>>();
				replace(cfg.vWeekDays.begin(), cfg.vWeekDays.end(), 7, 0);
				sort(cfg.vWeekDays.begin(), cfg.vWeekDays.end());
				cfg.vWeekDays.erase(unique(cfg.vWeekDays.begin(), cfg.vWeekDays.end()),cfg.vWeekDays.end());
				for (auto it = cfg.vWeekDays.begin();it != cfg.vWeekDays.end();++it)
					if (*it < 0 || *it > 6)
						it = cfg.vWeekDays.erase(it);
				if(!cfg.vWeekDays.empty())
					Configure.push_back(cfg);
			}
			Configure.push_back(JrrpConfigure(1, 100, { GlobalMsg["strJrrp"] }));
			return;
		}
		catch (...)
		{
			SetDefault();
			return;
		}
	}
	string Execute(int intJrrpValue,string strNick)
	{
		try
		{
			SYSTEMTIME stNow;
			GetLocalTime(&stNow);
			for (auto x : Configure)
			{
				if (x.intJrrpMin <= intJrrpValue && x.intJrrpMax >= intJrrpValue && !x.vReplies.empty() && find(x.vWeekDays.begin(),x.vWeekDays.end(),stNow.wDayOfWeek)!=x.vWeekDays.end())
				{
					int intPick = RandomGenerator::Randint(0, (signed)x.vReplies.size() - 1);
					string strReply = x.vReplies[intPick];
					string strDashes = string(intJrrpValue, '|');
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
		}
		catch (...)
		{
			return GlobalMsg["strJrrp"];
		}
		return GlobalMsg["strJrrp"];
	}
}
