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
	string Execute(int intJrrpValue,string strNick)
	{
		try
		{
			SYSTEMTIME stNow;
			GetLocalTime(&stNow);
			if (1 <= intJrrpValue && 100 >= intJrrpValue)
			{
				string strReply = GlobalMsg["strJrrp"];
				string strDashes = ">" + string(intJrrpValue/4, '|') + string(25 - intJrrpValue / 4, ' ') + "<";
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
