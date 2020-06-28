#pragma once
#include <map>
#include <string>
#include <vector>
#include "CardDeck.h"
#include "RandomGenerator.h"
#include "EncodingConvert.h"
#include "DiceEvent.h"
namespace JrrpModule
{
	using std::vector;
	using std::string;
	struct JrrpConfigure
	{
		int intJrrpMax;
		int intJrrpMin;
		vector<string> vReplies;
		vector<int> vWeekDays;
		JrrpConfigure(int mx, int mn, vector<string> rep, vector<int> wk = { 0,1,2,3,4,5,6 }) :intJrrpMax(mx), intJrrpMin(mn), vReplies(rep),vWeekDays(wk) {}
	};
	extern vector<JrrpConfigure> Configure;
	void LoadConfigure(string strPath);
	string Execute(int intJrrpValue,string strNick);
}
