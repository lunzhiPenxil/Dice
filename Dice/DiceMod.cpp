#include <set>
#include "DiceMod.h"
#include "GlobalVar.h"
#include "ManagerSystem.h"
#include "Jsonio.h"
#include "DiceFile.hpp"
using std::set;

DiceModManager::DiceModManager() : helpdoc(HelpDoc) {

}

string DiceModManager::format(string s, const map<string, string, less_ci>& dict, const char* mod_name = "") const {
	//直接重定向
	if (s[0] == '&') {
		string key = s.substr(1);
		auto it = dict.find(key);
		if (it != dict.end()) {
			return format(it->second, dict, mod_name);
		}
		//调用本mod词条
	}
	int l = 0, r = 0;
	int len = s.length();
	while ((l = s.find('{', r)) != string::npos && (r = s.find('}', l)) != string::npos) {
		if (s[l - 1] == 0x5c) {
			s.replace(l - 1, 1, "");
			continue;
		}
		string key = s.substr(l + 1, r - l - 1);
		auto it = dict.find(key);
		if (it != dict.end()) {
			s.replace(l, r - l + 1, format(it->second, dict, mod_name));
			r += s.length() - len + 1;
			len = s.length();
		}
		//调用本mod词条
	}
	return s;
}

vector<pair<char, char>> DiceModManager::makeConsult(string word) const
{
	vector<pair<char, char>> vResult;
	for (unsigned int i = 0; i < word.size(); i++) {
		if (word[i] & 0x80) {
			vResult.push_back(std::make_pair(word[i], word[i+1]));
			i++;
		}
		else {
			vResult.push_back(std::make_pair(0x00, word[i]));
		}
	}
	return vResult;
}

int DiceModManager::minDistance(string word1_in, string word2_in) const
{
	if (word1_in.length() > word2_in.length())swap(word1_in, word2_in);
	if (word2_in.find(word1_in) != string::npos)return 1;
	vector<pair<char, char>> word1, word2;
	word1 = makeConsult(word1_in);
	word2 = makeConsult(word2_in);
	vector<vector<int>> dp(word1.size() + 1, vector<int>(word2.size() + 1, 0));
	for (unsigned int i = 0; i < word1.size() + 1; i++) {
		dp[i][0] = i;
	}
	for (unsigned int i = 0; i < word2.size() + 1; i++) {
		dp[0][i] = i;
	}
	for (unsigned int i = 1; i < word1.size() + 1; i++) {
		for (unsigned int j = 1; j < word2.size() + 1; j++) {
			if (word1[i - 1].second == word2[j - 1].second && word1[i - 1].first == word2[j - 1].first) {
				dp[i][j] = dp[i - 1][j - 1];
			}
			else {
				dp[i][j] = min(dp[i - 1][j - 1], min(dp[i][j - 1], dp[i - 1][j])) + 1;
			}
		}
	}
	return dp[word1.size()][word2.size()];
}

string DiceModManager::get_help(const string& key) const {
	if (auto it = helpdoc.find(key); it != helpdoc.end()) {
		return format(it->second, helpdoc);
	}
	string strAns = "{strHlpNotFound}";
	vector<pair<int, string>> vResult;
	for (auto it = helpdoc.begin(); it != helpdoc.end(); ++it)
	{
		vResult.push_back(std::make_pair(minDistance(it->first, key), it->first));
	}
	sort(vResult.begin(), vResult.end());
	bool extInfo = false;
	for (unsigned int u = 0; u < 10 && u < vResult.size() && vResult[u].first <= (int)makeConsult(key).size() / 2; ++u)
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

void DiceModManager::set_help(string key, string val) {
	helpdoc[key] = val;
}
void DiceModManager::rm_help(string key) {
	helpdoc.erase(key);
}

int DiceModManager::load(string& strLog) {
	vector<std::filesystem::path> sFile;
	vector<string> sFileErr;
	int cntFile = listDir(DiceDir + "\\mod\\", sFile, true);
	int cntItem{ 0 };
	if (cntFile <= 0)return cntFile;
	for (auto& filename : sFile) {
		nlohmann::json j = freadJson(filename);
		if (j.is_null()) {
			sFileErr.push_back(filename.filename().string());
			continue;
		}
		if (j.count("helpdoc")) {
			cntItem += readJMap(j["helpdoc"], helpdoc);
		}
	}
	strLog += "读取" + DiceDir + "mod\\中的" + std::to_string(cntFile) + "个文件, 共" + std::to_string(cntItem) + "个条目\n";
	if (!sFileErr.empty()) {
		strLog += "读取失败" + std::to_string(sFileErr.size()) + "个:\n";
		for (auto &it : sFileErr) {
			strLog += it + "\n";
		}
	}
	return cntFile;
}
void DiceModManager::clear() {
	helpdoc.clear();
}