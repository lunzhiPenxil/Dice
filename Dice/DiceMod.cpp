#include <set>
#include "DiceMod.h"
#include "GlobalVar.h"
#include "ManagerSystem.h"
#include "Jsonio.h"
#include "DiceFile.hpp"
using std::set;

DiceModManager::DiceModManager() : helpdoc(HelpDoc)
{
}

string DiceModManager::format(string s, const map<string, string, less_ci>& dict, const char* mod_name = "") const
{
	//直接重定向
	if (s[0] == '&')
	{
		const string key = s.substr(1);
		const auto it = dict.find(key);
		if (it != dict.end())
		{
			return format(it->second, dict, mod_name);
		}
		//调用本mod词条
	}
	int l = 0, r = 0;
	int len = s.length();
	while ((l = s.find('{', r)) != string::npos && (r = s.find('}', l)) != string::npos)
	{
		if (s[l - 1] == 0x5c)
		{
			s.replace(l - 1, 1, "");
			continue;
		}
		string key = s.substr(l + 1, r - l - 1), val;
		auto it = dict.find(key);
		if (it != dict.end())
		{
			val = format(it->second, dict, mod_name);
		}
		else if (auto func = strFuncs.find(key); func != strFuncs.end())
		{
			val = func->second();
		}
		else continue;
		s.replace(l, r - l + 1, val);
		r = l + val.length();
		//调用本mod词条
	}
	return s;
}

vector<pair<char, char>> DiceModManager::makeConsult(string word) const
{
	vector<pair<char, char>> vResult;
	for (unsigned int i = 0; i < word.size(); i++)
	{
		if (word[i] & 0x80) 
		{
			vResult.push_back(std::make_pair(word[i], word[i + 1]));
			i++;
		}
		else 
		{
			vResult.push_back(std::make_pair(0x00, word[i]));
		}
	}
	return vResult;
}

int DiceModManager::getRecommendRank(string word1_in, string word2_in) const
{
	int iRank = 0;
	bool find_flag = 1;
	vector<pair<char, char>> word1, word2;
	word1 = makeConsult(word1_in);
	word2 = makeConsult(word2_in);
	if (word1.size() <= 0 || word2.size() <= 0)return 1000;
	if (word1_in.length() > word2_in.length())swap(word1_in, word2_in);
	if (word2_in.find(word1_in) != string::npos) 
	{
		find_flag = 0;
	}
	//LCS
	vector<vector<int>> dp1(word1.size() + 1, vector<int>(word2.size() + 1, 0));
	for (unsigned int i = 0; i < word1.size() + 1; i++) 
	{
		dp1[i][0] = 0;
	}
	for (unsigned int i = 0; i < word2.size() + 1; i++)
	{
		dp1[0][i] = 0;
	}
	for (unsigned int i = 1; i < word1.size() + 1; i++) 
	{
		for (unsigned int j = 1; j < word2.size() + 1; j++) 
		{
			if (word1[i - 1].second == word2[j - 1].second && word1[i - 1].first == word2[j - 1].first)
			{
				dp1[i][j] = dp1[i - 1][j - 1] + 1;
			}
			else 
			{
				dp1[i][j] = max(dp1[i][j - 1], dp1[i - 1][j]);
			}
		}
	}
	//minDistance
	vector<vector<int>> dp2(word1.size() + 1, vector<int>(word2.size() + 1, 0));
	for (unsigned int i = 0; i < word1.size() + 1; i++) 
	{
		dp2[i][0] = i;
	}
	for (unsigned int i = 0; i < word2.size() + 1; i++)
	{
		dp2[0][i] = i;
	}
	for (unsigned int i = 1; i < word1.size() + 1; i++) 
	{
		for (unsigned int j = 1; j < word2.size() + 1; j++)
		{
			if (word1[i - 1].second == word2[j - 1].second && word1[i - 1].first == word2[j - 1].first)
			{
				dp2[i][j] = dp2[i - 1][j - 1];
			}
			else {
				dp2[i][j] = min(dp2[i - 1][j - 1], min(dp2[i][j - 1], dp2[i - 1][j])) + 1;
			}
		}
	}
	iRank = (find_flag) * (word1.size() * (word2.size() - dp1[word1.size()][word2.size()]) + dp2[word1.size()][word2.size()] + 1);
	iRank = iRank * iRank / word1.size() / word2.size();
	if ((unsigned int)abs(iRank) >= word1.size() * word2.size()) 
	{
		iRank += 1000;
	}
	return iRank;
}

string DiceModManager::get_help(const string& key) const
{
	if (auto it = helpdoc.find(key); it != helpdoc.end())
	{
		return format(it->second, helpdoc);
	}
	string strAns = "{strHlpNotFound}";
	vector<pair<int, string>> vResult;
	for (auto it = helpdoc.begin(); it != helpdoc.end(); ++it)
	{
		vResult.push_back(std::make_pair(getRecommendRank(it->first, key), it->first));
	}
	sort(vResult.begin(), vResult.end());
	bool extInfo = false;
	for (unsigned int u = 0; u < 8 && u < vResult.size() && vResult[u].first < (int)(makeConsult(vResult[u].second).size() * makeConsult(key).size()); ++u)
	{
		if (!extInfo)
		{
			extInfo = true;
			strAns += "\n{strHlpRecommend}";
		}
		strAns += "\n[.help " + vResult[u].second + "]";
	}
	return strAns;
}

struct help_sorter {
	bool operator()(const string& _Left, const string& _Right) const {
		if (fmt->cntHelp.count(_Right) && !fmt->cntHelp.count(_Left))
			return true;
		else if (fmt->cntHelp.count(_Left) && !fmt->cntHelp.count(_Right))
			return false;
		else if (fmt->cntHelp.count(_Left) && fmt->cntHelp.count(_Right) && fmt->cntHelp[_Left] != fmt->cntHelp[_Right])
			return fmt->cntHelp[_Left] < fmt->cntHelp[_Right];
		else if (_Left.length() != _Right.length()) {
			return _Left.length() > _Right.length();
		}
		else return _Left > _Right;
	}
};

void DiceModManager::get_help(DiceJobDetail* job) {
	if ((*job)["help_word"].empty()) {
		job->reply(string(Dice_Short_Ver) + "\n" + GlobalMsg["strHlpMsg"]);
		return;
	}
	else if (const auto it = helpdoc.find((*job)["help_word"]); it != helpdoc.end()) {
		job->reply(format(it->second, helpdoc));
	}
	else if (unordered_set<string> keys = querier.search((*job)["help_word"]);!keys.empty()) {
		std::priority_queue<string, vector<string>, help_sorter> qKey;
		for (auto key : keys) {
			qKey.emplace(key);
		}
		ResList res;
		while (!qKey.empty()) {
			res << qKey.top();
			qKey.pop();
			if (res.size() > 20)break;
		}
		(*job)["res"] = res.dot("/").show(1);
		job->reply("{strHelpSuggestion}");
	}
	else job->reply("{strHelpNotFound}");
	cntHelp[(*job)["help_word"]] += 1;
	saveJMap(DiceDir + "\\user\\HelpStatic.json",cntHelp);
}

void DiceModManager::set_help(const string& key, const string& val)
{
	if (!helpdoc.count(key))querier.insert(key);
	helpdoc[key] = val;
}

void DiceModManager::rm_help(const string& key)
{
	helpdoc.erase(key);
}

int DiceModManager::load(string& strLog) 
{
	vector<std::filesystem::path> sFile;
	vector<string> sFileErr;
	int cntFile = listDir(DiceDir + "\\mod\\", sFile, true);
	int cntItem{0};
	if (cntFile <= 0)return cntFile;
	for (auto& filename : sFile) 
	{
		nlohmann::json j = freadJson(filename);
		if (j.is_null())
		{
			sFileErr.push_back(filename.filename().string());
			continue;
		}
		if (j.count("dice_build")) {
			if (j["dice_build"] > Dice_Build) {
				sFileErr.push_back(filename.filename().string()+"(Dice版本过低)");
				continue;
			}
		}
		if (j.count("helpdoc"))
		{
			cntItem += readJMap(j["helpdoc"], helpdoc);
		}
		if (j.count("global_char"))
		{
			cntItem += readJMap(j["global_char"], GlobalChar);
		}
	}
	strLog += "读取" + DiceDir + "\\mod\\中的" + std::to_string(cntFile) + "个文件, 共" + std::to_string(cntItem) + "个条目\n";
	if (!sFileErr.empty())
	{
		strLog += "读取失败" + std::to_string(sFileErr.size()) + "个:\n";
		for (auto& it : sFileErr)
		{
			strLog += it + "\n";
		}
	}
	std::thread factory(&DiceModManager::init,this);
	factory.detach();
	cntHelp.reserve(helpdoc.size());
	loadJMap(DiceDir + "\\user\\HelpStatic.json", cntHelp);
	return cntFile;
}
void DiceModManager::init() {
	isIniting = true;
	for (auto& [key, word] : helpdoc) {
		querier.insert(key);
	}
	isIniting = false;
}
void DiceModManager::clear()
{
	helpdoc.clear();
}
