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
	int LocalJrrpGenerate(long long fromQQ, int LocalJrrpVar, int LocalJrrpMin, int LocalJrrpRange);
	string Execute(int intJrrpValue,string strNick);
}
