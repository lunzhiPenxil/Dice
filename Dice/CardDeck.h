/*
 * 牌堆抽卡功能
 * 因为发展方向与Dice！插件并不相合，所以更新会止步在某种程度
 * Copyright (C) 2019 String.Empty
 * 然而一年之后，上面这句话变成了笑话
 * Copyright (C) 2019-2020 lunzhiPenxil仑质
 */
#pragma once
#ifndef CARD_DECK
#define CARD_DECK
#include <string>
#include <vector>
#include <map>
#include "STLExtern.hpp"

namespace CardDeck
{
	extern std::map<std::string, std::vector<std::string>, less_ci> mPublicDeck;
	extern std::map<std::string, std::vector<std::string>, less_ci> mExternPublicDeck;
	extern std::map<std::string, std::vector<std::string>, less_ci> mReplyDeck;
	extern std::map<std::string, std::string> PublicComplexDeck;
	bool isRegexMatch(std::string strRegex, std::string strInput);
	std::string doRegexReplace(std::string strRegex, std::string strInput, std::string strfmt);
	int findDeck(std::string strDeckName);
	std::string drawCard(std::vector<std::string>& TempDeck, bool boolBack = false);
	std::string draw(std::string strDeckName);
};
#endif /*CARD_DECK*/
