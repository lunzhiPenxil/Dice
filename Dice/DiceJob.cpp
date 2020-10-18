#pragma once
#include "DiceJob.h"
#include "DiceConsole.h"
#include <TlHelp32.h>
#include <Psapi.h>
#include "StrExtern.hpp"
#include "CQAPI.h"
#include "ManagerSystem.h"
#include "DiceCloud.h"
#include "BlackListManager.h"
#include "GlobalVar.h"
#include "CardDeck.h"
#include "DiceMod.h"
#include "DiceNetwork.h"
#include "Jsonio.h"
#include "S3PutObject.h"
#include "RD.h"
#include "GlobalVar.h"

#pragma warning(disable:28159)

using namespace std;
using namespace CQ;

int sendSelf(const string msg) {
	static long long selfQQ = CQ::getLoginQQ();
	return CQ::sendPrivateMsg(selfQQ, msg);
}

void cq_exit(DiceJob& job) {
	job.note("已令" + getMsg("self") + "在5秒后自杀", 1);
	if (frame == QQFrame::CoolQ) {
		int pid = _getpid();
		PROCESSENTRY32 pe32;
		pe32.dwSize = sizeof(pe32);
		HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		if (hProcessSnap == INVALID_HANDLE_VALUE) {
			job.note("重启失败：进程快照创建失败！", 1);
		}
		BOOL bResult = Process32First(hProcessSnap, &pe32);
		int ppid(0);
		while (bResult) {
			if (pe32.th32ProcessID == pid) {
				ppid = pe32.th32ParentProcessID;
				break;
			}
			bResult = Process32Next(hProcessSnap, &pe32);
		}
		if (!ppid) {
			job.note("重启失败：未找到进程！", 1);
		}
		string strCMD("taskkill /f /pid " + to_string(ppid));
		std::this_thread::sleep_for(5s);
		job.echo(strCMD);
		Enabled = false;
		dataBackUp();
		system(strCMD.c_str());
	}
	else if (frame == QQFrame::Mirai) {
		std::this_thread::sleep_for(5s);
		//Enabled = false;
		dataBackUp();
		cout << "stop" << endl;
		string cmd = "stop";
		for (auto key : cmd) {
			keybd_event(key, 0, 0, 0);
		}
		keybd_event(VK_RETURN, MapVirtualKey(VK_RETURN, 0), 0, 0);
	}
}

inline PROCESSENTRY32 getProcess(int pid) {
	PROCESSENTRY32 pe32;
	pe32.dwSize = sizeof(pe32);
	HANDLE hParentProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);
	Process32First(hParentProcess, &pe32);
	return pe32;
}
void frame_restart(DiceJob& job) {
	if (!job.fromQQ) {
		if (console["AutoFrameRemake"] <= 0) {
			sch.add_job_for(60 * 60, job);
			return;
		}
		else if (int tWait = console["AutoFrameRemake"] * 60 * 60 - (clock() - llStartTime) / 1000; tWait > 0) {
			sch.add_job_for(tWait, job);
			return;
		}
	}
	string strSelfName;
	PROCESSENTRY32 pe32;
	pe32.dwSize = sizeof(pe32);
	HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hProcessSnap == INVALID_HANDLE_VALUE) {
		job.note("重启失败：进程快照创建失败！", 1);
	}
	BOOL bResult = Process32First(hProcessSnap, &pe32);
	int ppid(0);
	if (frame == QQFrame::Mirai) {
		strSelfName = "MiraiOK.exe";
		char buffer[MAX_PATH];
		const DWORD length = GetModuleFileNameA(nullptr, buffer, sizeof buffer);
		std::string pathSelf(buffer, length);
		pathSelf = pathSelf.substr(0, pathSelf.find("jre\\bin\\java.exe")) + strSelfName;
		char pathFull[MAX_PATH];
		while (bResult) {
			if (strSelfName == pe32.szExeFile) {
				HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe32.th32ProcessID);
				GetModuleFileNameEx(hProcess, NULL, pathFull, sizeof(pathFull));
				if (pathSelf != pathFull)continue;
				ppid = pe32.th32ProcessID;
				job.echo("确认进程" + pathSelf + "\n进程id:" + to_string(ppid));
				break;
			}
			bResult = Process32Next(hProcessSnap, &pe32);
		}
		if (!ppid) {
			job.echo("未找到进程" + pathSelf);
			return;
		}
	}
	else if (frame == QQFrame::CoolQ) {
		int pid = _getpid();
		while (bResult) {
			if (pe32.th32ProcessID == pid) {
				ppid = pe32.th32ParentProcessID;
				PROCESSENTRY32 pp32 = getProcess(ppid);
				strSelfName = pp32.szExeFile;
				job.echo("确认进程" + strSelfName + "\n本进程id:" + to_string(pe32.th32ProcessID) + "\n父进程id:" + to_string(ppid));
				break;
			}
			bResult = Process32Next(hProcessSnap, &pe32);
		}
	}
	else {
		ppid = _getpid();
		while (bResult) {
			if (pe32.th32ProcessID == ppid) {
				strSelfName = pe32.szExeFile;
				job.echo("确认进程" + strSelfName + "\n本进程id:" + to_string(pe32.th32ProcessID));
				break;
			}
			bResult = Process32Next(hProcessSnap, &pe32);
		}
	}
	if (!ppid) {
		job.note("重启失败：未找到进程！", 1);
		return;
	}
	string command = "taskkill /f /pid " + to_string(ppid) + " /t\nstart " + strSelfName;
	if (frame == QQFrame::CoolQ) command += " /account " + to_string(console.DiceMaid);
	//else if (frame == QQFrame::XianQu) command = "start .\\remake.exe " + to_string(ppid) + " " + strSelfName;
	else if (frame == QQFrame::XianQu) command = "start .\\先驱.exe\ntaskkill /f /pid " + to_string(ppid);
	ofstream fout("remake.bat");
	fout << command << std::endl;
	fout.close();
	job.note(command, 0);
	std::this_thread::sleep_for(3s);
	Enabled = false;
	dataBackUp();
	std::this_thread::sleep_for(3s);
	//if (frame == QQFrame::Mirai) {
	//	WinExec(("remake.exe " + to_string(ppid) + " " + strSelfName).c_str(), SW_SHOW);
	//	return;
	//}
	/*
	switch (UINT res = -1; res = WinExec(".\\remake.bat", SW_SHOW)) {
	case 0:
		job.note("重启失败：内存或资源已耗尽！", 1);
		break;
	case ERROR_FILE_NOT_FOUND:
		job.note("重启失败：指定的文件未找到！", 1);
		break;
	case ERROR_PATH_NOT_FOUND:
		job.note("重启失败：指定的路径未找到！", 1);
		break;
	default:
		if (res > 31)job.note("重启成功√", 0);
		else job.note("重启失败：未知错误" + to_string(res), 0);
		break;
	}
	*/
	ShellExecute(NULL, "open", "remake.bat", NULL, NULL, SW_SHOWNORMAL);
}

void frame_reload(DiceJob& job){
	using cq_reload_type = int(__stdcall*)(int32_t);
	HMODULE hModule = GetModuleHandleA("CQP.dll");
	cq_reload_type cq_reload = (cq_reload_type)GetProcAddress(hModule, "CQ_reload");
	if (!cq_reload) {
		if (frame == QQFrame::Mirai)
			job.note("重载MiraiNative失败×\n使用了过旧或不适配的CQP.dll\n请保证更新适配版本的MiraiNative并删除旧CQP.dll", 0b10);
		else if (frame == QQFrame::XianQu)
			job.note("重载CQXQ失败×\n版本过旧，请保证更新适配版本的CQXQ", 0b10);
		return;
	}
	if(cq_reload(getAuthCode()))
		job.note("重载" + getMsg("self") + "完成√", 1);
	else
		job.note("重载" + getMsg("self") + "失败×", 0b10);
}

void auto_save(DiceJob& job) {
	if (sch.is_job_cold("autosave"))return;
	dataBackUp();
	console.log(GlobalMsg["strSelfName"] + "已自动保存", 0, printSTNow());
	if (console["AutoSaveInterval"] > 0) {
		sch.refresh_cold("autosave", time(NULL) + console["AutoSaveInterval"]);
		sch.add_job_for(console["AutoSaveInterval"] * 60, "autosave");
	}
}

void check_system(DiceJob& job) {
	static int perRAM(0), perLastRAM(0);
	static double  perLastCPU(0), perLastDisk(0),
		 perCPU(0), perDisk(0);
	static bool isAlarmRAM(false), isAlarmCPU(false), isAlarmDisk(false);
	static double mbFreeBytes = 0, mbTotalBytes = 0;
	//内存检测
	if (console["SystemAlarmRAM"] > 0) {
		perRAM = getRamPort();
		if (perRAM > console["SystemAlarmRAM"] && perRAM > perLastRAM) {
			console.log("警告：" + GlobalMsg["strSelfName"] + "所在系统内存占用达" + to_string(perRAM) + "%", 0b1000, printSTime(stNow));
			perLastRAM = perRAM;
			isAlarmRAM = true;
		}
		else if (perLastRAM > console["SystemAlarmRAM"] && perRAM < console["SystemAlarmRAM"]) {
			console.log("提醒：" + GlobalMsg["strSelfName"] + "所在系统内存占用降至" + to_string(perRAM) + "%", 0b10, printSTime(stNow));
			perLastRAM = perRAM;
			isAlarmRAM = false;
		}
	}
	//CPU检测
	if (console["SystemAlarmCPU"] > 0) {
		perCPU = getWinCpuUsage() / 10.0;
		if (perCPU > console["SystemAlarmCPU"] && (!isAlarmCPU || perCPU > perLastCPU + 1)) {
			console.log("警告：" + GlobalMsg["strSelfName"] + "所在系统CPU占用达" + toString(perCPU) + "%", 0b1000, printSTime(stNow));
			perLastCPU = perCPU;
			isAlarmCPU = true;
		}
		else if (perLastCPU > console["SystemAlarmCPU"] && perCPU < console["SystemAlarmCPU"]) {
			console.log("提醒：" + GlobalMsg["strSelfName"] + "所在系统CPU占用降至" + toString(perCPU) + "%", 0b10, printSTime(stNow));
			perLastCPU = perCPU;
			isAlarmCPU = false;
		}
	}
	//硬盘检测
	if (console["SystemAlarmRAM"] > 0) {
		perDisk = getDiskUsage(mbFreeBytes, mbTotalBytes) / 10.0;
		if (perDisk > console["SystemAlarmDisk"] && (!isAlarmDisk || perDisk > perLastDisk + 1)) {
			console.log("警告：" + GlobalMsg["strSelfName"] + "所在系统硬盘占用达" + toString(perDisk) + "%", 0b1000, printSTime(stNow));
			perLastDisk = perDisk;
			isAlarmDisk = true;
		}
		else if (perLastDisk > console["SystemAlarmDisk"] && perDisk < console["SystemAlarmDisk"]) {
			console.log("提醒：" + GlobalMsg["strSelfName"] + "所在系统硬盘占用降至" + toString(perDisk) + "%", 0b10, printSTime(stNow));
			perLastDisk = perDisk;
			isAlarmDisk = false;
		}
	}
	if (isAlarmRAM || isAlarmCPU || isAlarmDisk) {
		sch.add_job_for(5 * 60, job);
	}
	else {
		sch.add_job_for(30 * 60, job);
	}
}

//被引用的图片列表
void clear_image(DiceJob& job) {
	if (!job.fromQQ) {
		if (sch.is_job_cold("clrimage"))return;
		if (console["AutoClearImage"] <= 0) {
			sch.add_job_for(60 * 60, job);
			return;
		}
	}
	scanImage(GlobalMsg, sReferencedImage);
	scanImage(HelpDoc, sReferencedImage);
	scanImage(CardDeck::mPublicDeck, sReferencedImage);
	scanImage(CardDeck::mReplyDeck, sReferencedImage);
	for (auto it : ChatList) {
		scanImage(it.second.strConf, sReferencedImage);
	}
	job.note("整理" + GlobalMsg["strSelfName"] + "被引用图片" + to_string(sReferencedImage.size()) + "项", 0b0);
	int cnt = clrDir("data\\image\\", sReferencedImage);
	job.note("已清理image文件"+ to_string(cnt) + "项", 1);
	if (console["AutoClearImage"] > 0) {
		sch.refresh_cold("clrimage", time(NULL) + console["AutoClearImage"]);
		sch.add_job_for(console["AutoClearImage"] * 60 * 60, "clrimage");
	}
}

void clear_group(DiceJob& job) {
	int intCnt = 0;
	ResList res;
	if (job.strVar["clear_mode"] == "unpower") {
		for (auto& [id, grp] : ChatList) {
			if (grp.isset("忽略") || grp.isset("已退") || grp.isset("未进") || grp.isset("免清") || grp.isset("协议无效"))continue;
			if (grp.isGroup && getGroupMemberInfo(id, console.DiceMaid).permissions == 1) {
				res << printGroup(id);
				grp.leave(getMsg("strLeaveNoPower"));
				intCnt++;
				this_thread::sleep_for(3s);
			}
		}
		console.log(GlobalMsg["strSelfName"] + "筛除无群权限群聊" + to_string(intCnt) + "个:" + res.show(), 0b10, printSTNow());
	}
	else if (isdigit(static_cast<unsigned char>(job.strVar["clear_mode"][0]))) {
		int intDayLim = stoi(job.strVar["clear_mode"]);
		string strDayLim = to_string(intDayLim);
		time_t tNow = time(NULL);
		for (auto& [id, grp] : ChatList) {
			if (grp.isset("忽略") || grp.isset("已退") || grp.isset("未进") || grp.isset("免清") || grp.isset("协议无效"))continue;
			time_t tLast = grp.tUpdated;
			if (int tLMT; grp.isGroup && (tLMT = getGroupMemberInfo(id, console.DiceMaid).LastMsgTime) > 0 && tLMT > tLast)tLast = tLMT;
			if (!tLast)continue;
			int intDay = (int)(tNow - tLast) / 86400;
			if (intDay > intDayLim) {
				job["day"] = to_string(intDay);
				res << printGroup(id) + ":" + to_string(intDay) + "天\n";
				grp.leave(getMsg("strLeaveUnused", job.strVar));
				intCnt++;
				this_thread::sleep_for(2s);
			}
		}
		console.log(GlobalMsg["strSelfName"] + "已筛除潜水" + strDayLim + "天群聊" + to_string(intCnt) + "个√" + res.show(), 0b10, printSTNow());
	}
	else if (job.strVar["clear_mode"] == "black") {
		try {
			for (auto& [id, grp_name] : getGroupList()) {
				Chat& grp = chat(id).group().name(grp_name);
				if (grp.isset("忽略") || grp.isset("已退") || grp.isset("未进") || grp.isset("免清") || grp.isset("免黑") || grp.isset("协议无效"))continue;
				if (blacklist->get_group_danger(id)) {
					res << printGroup(id) + "：" + "黑名单群";
					if (console["LeaveBlackGroup"])grp.leave(getMsg("strBlackGroup"));
				}
				vector<GroupMemberInfo> MemberList = getGroupMemberList(id);
				for (auto eachQQ : MemberList) {
					if (blacklist->get_qq_danger(eachQQ.QQID) > 1) {
						if (eachQQ.permissions < getGroupMemberInfo(id, getLoginQQ()).permissions) {
							continue;
						}
						else if (eachQQ.permissions > getGroupMemberInfo(id, getLoginQQ()).permissions) {
							res << printChat(grp) + "：" + printQQ(eachQQ.QQID) + "对方群权限较高";
							grp.leave("发现黑名单管理员" + printQQ(eachQQ.QQID) + "\n" + GlobalMsg["strSelfName"] + "将预防性退群");
							intCnt++;
							break;
						}
						else if (console["LeaveBlackQQ"]) {
							res << printChat(grp) + "：" + printQQ(eachQQ.QQID);
							grp.leave("发现黑名单成员" + printQQ(eachQQ.QQID) + "\n" + GlobalMsg["strSelfName"] + "将预防性退群");
							intCnt++;
							break;
						}
					}
				}
			}
		}
		catch (...) {
			console.log("提醒：" + GlobalMsg["strSelfName"] + "清查黑名单群聊时出错！", 0b10, printSTNow());
		}
		if (intCnt) {
			job.note("已按" + getMsg("strSelfName") + "黑名单清查群聊" + to_string(intCnt) + "个：" + res.show(), 0b10);
		}
		else if (job.fromQQ) {
			job.echo(getMsg("strSelfName") + "按黑名单未发现待清查群聊");
		}
	}
	else if (job["clear_mode"] == "preserve") {
		for (auto& [id, grp] : ChatList) {
			if (grp.isset("忽略") || grp.isset("已退") || grp.isset("未进") || grp.isset("使用许可") || grp.isset("免清") || grp.isset("协议无效"))continue;
			if (grp.isGroup && getGroupMemberInfo(id, console.master()).permissions) {
				grp.set("使用许可");
				continue;
			}
			res << printChat(grp);
			grp.leave(getMsg("strPreserve"));
			intCnt++;
			this_thread::sleep_for(3s);
		}
		console.log(GlobalMsg["strSelfName"] + "筛除无许可群聊" + to_string(intCnt) + "个：" + res.show(), 1, printSTNow());
	}
	else
		job.echo("无法识别筛选参数×");
}
void list_group(DiceJob& job) {
	if (job["list_mode"].empty()) {
		job.reply(fmt->get_help("groups_list"));
	}
	if (mChatConf.count(job["list_mode"])) {
		ResList res;
		for (auto& [id, grp] : ChatList) {
			if (grp.isset(job["list_mode"])) {
				res << printChat(grp);
			}
		}
		job.reply("{self}含词条" + job["list_mode"] + "群记录" + to_string(res.size()) + "条" + res.head(":").show());
	}
	else if (job["list_mode"] == "idle") {
		std::priority_queue<std::pair<time_t, string>> qDiver;
		time_t tNow = time(NULL);
		for (auto& [id, grp] : ChatList) {
			if (grp.isset("已退") || grp.isset("未进"))continue;
			time_t tLast = grp.tUpdated;
			if (int tLMT; grp.isGroup && (tLMT = getGroupMemberInfo(id, console.DiceMaid).LastMsgTime) > 0 && tLMT > tLast)tLast = tLMT;
			if (!tLast)continue;
			int intDay = (int)(tNow - tLast) / 86400;
			qDiver.emplace(intDay, printGroup(id));
		}
		if (qDiver.empty()) {
			job.reply("{self}无群聊或群信息加载失败！");
		}
		size_t intCnt(0);
		ResList res;
		while (!qDiver.empty()) {
			res << qDiver.top().second + to_string(qDiver.top().first) + "天";
			qDiver.pop();
			if (++intCnt > 32 || qDiver.top().first < 7)break;
		}
		job.reply("{self}所在闲置群列表:" + res.show(1));
	}
	else if (job["list_mode"] == "size") {
		std::priority_queue<std::pair<time_t, string>> qSize;
		time_t tNow = time(NULL);
		for (auto& [id, grp] : ChatList) {
			if (grp.isset("已退") || grp.isset("未进") || !grp.isGroup)continue;
			GroupInfo ginfo(id);
			if (!ginfo.nGroupSize)continue;
			qSize.emplace(ginfo.nGroupSize, printGroup(id));
		}
		if (qSize.empty()) {
			job.reply("{self}无群聊或群信息加载失败！");
		}
		size_t intCnt(0);
		ResList res;
		while (!qSize.empty()) {
			res << qSize.top().second + "[" + to_string(qSize.top().first) + "]";
			qSize.pop();
			if (++intCnt > 32 || qSize.top().first < 7)break;
		}
		job.reply("{self}所在大群列表:" + res.show(1));
	}
}

//心跳检测
void cloud_beat(DiceJob& job) {
	Cloud::update();
	sch.add_job_for(5 * 60, job);
}

void dice_update(DiceJob& job) {
	job.note("开始更新Dice\n版本:" + job.strVar["ver"], 1);
	if (frame == QQFrame::Mirai) {
		mkDir(dirExe + "plugins/MiraiNative/pluginsnew");
		char pathDll[] = "plugins/MiraiNative/pluginsnew/com.w4123.dice.dll";
		char pathJson[] = "plugins/MiraiNative/pluginsnew/com.w4123.dice.json";
		string urlDll("https://shiki.stringempty.xyz/DiceVer/" + job.strVar["ver"] + "/com.w4123.dice.dll?" + to_string(job.fromTime));
		string urlJson("https://shiki.stringempty.xyz/DiceVer/" + job.strVar["ver"] + "/com.w4123.dice.json?" + to_string(job.fromTime));
		switch (Cloud::DownloadFile(urlDll.c_str(), pathDll)) {
		case -1:
			job.echo("更新失败:" + urlDll);
			break;
		case -2:
			job.note("更新Dice失败!dll文件未下载到指定位置", 0b1);
			break;
		case 0:
		default:
			switch (Cloud::DownloadFile(urlJson.c_str(), pathJson)) {
			case -1:
				job.echo("更新失败:" + urlJson);
				break;
			case -2:
				job.note("更新Dice失败!json文件未下载到指定位置", 0b1);
				break;
			case 0:
			default:
				job.note("更新Dice!" + job.strVar["ver"] + "版成功√", 1);
			}
		}
	}
	else if (frame == QQFrame::XianQu) {
		mkDir(dirExe + "CQPlugins/");
		char pathDll[] = "CQPlugins/com.w4123.dice.dll";
		string urlDll("https://shiki.stringempty.xyz/DiceVer/" + job.strVar["ver"] + "/com.w4123.dice.dll?" + to_string(job.fromTime));
		switch (Cloud::DownloadFile(urlDll.c_str(), pathDll)) {
		case -1:
			job.echo("更新失败:" + urlDll);
			break;
		case -2:
			job.note("更新Dice失败!dll文件未下载到指定位置", 0b1);
			break;
		case 0:
		default:
			job.note("更新Dice!" + job.strVar["ver"] + "版成功√", 1);
		}
	}
	else {
		char** path = new char* ();
		_get_pgmptr(path);
		string strAppPath(*path);
		delete path;
		strAppPath = strAppPath.substr(0, strAppPath.find_last_of("\\")) + "\\app\\com.w4123.dice.cpk";
		string strURL("https://shiki.stringempty.xyz/DiceVer/" + job.strVar["ver"] + "?" + to_string(job.fromTime));
		switch (Cloud::DownloadFile(strURL.c_str(), strAppPath.c_str())) {
		case -1:
			job.echo("更新失败:" + strURL);
			break;
		case -2:
			job.note("更新Dice失败!文件未找到:" + strAppPath, 0b10);
			break;
		case 0:
			job.note("更新Dice!" + job.strVar["ver"] + "版成功√\n可用.system reload 重启应用更新", 1);
		}
	}
}

void dice_api_update(DiceJob& job) {
	string strURL("http://benzenpenxil.xyz/Oliva-Archives/");
	char** path = new char* ();
	_get_pgmptr(path);
	string strAppPath(*path);
	string strApiSaveLoc;
	strApiSaveLoc = DiceDir + "\\update\\DiceUpdateArchives.json";
	if (job.strVar["ver"] == "list" || job.strVar["ver"] == "")
	{
		job.note("开始刷新更新源", 1);
		switch (Cloud::DownloadFile(strURL.c_str(), strApiSaveLoc.c_str())) {
		case -1:
			job.echo("更新源获取失败:" + strURL);
			break;
		case -2:
			job.note("更新源加载失败!更新源缓存未找到:" + strApiSaveLoc, 0b10);
			break;
		case 0:
			nlohmann::json j_api = freadJson(strApiSaveLoc);
			if (j_api != nlohmann::json())
			{
				try
				{
					vector<string> public_list = UTF8toGBK(j_api["public"].get<vector<string>>());
					string strNameTmp = UTF8toGBK(j_api["name"].get<string>());
					string strCommentTmp = UTF8toGBK(j_api["comment"].get<string>());
					string strPublicTmp;
					strPublicTmp += "\n已指向:" + strNameTmp;
					strPublicTmp += "\n可选的更新指令有:";
					for (auto it : public_list)
					{
						strPublicTmp += "\n[.cloud update " + it + "]";
					}
					job.echo("已成功刷新更新源:" + strPublicTmp);
					job.echo("来自该更新源的公告:\n" + strCommentTmp);
					job.note("已成功将更新源指向:" + strNameTmp, 1);
				}
				catch (...)
				{
					job.note("更新源缓存加载异常!", 1);
				}
			}
			else
			{
				job.note("更新源缓存异常!", 1);
			}
			break;
		}
	}
	else
	{
		nlohmann::json j_api = freadJson(strApiSaveLoc);
		string strNameTmp = "";
		string strCommentTmp = "";
		string strVersionTypeTmp = "";
		string strVersionTmp = "";
		string strDownloadTmp = "";
		string strDownloadTmp_dll = "";
		string strDownloadTmp_json = "";
		if (j_api != nlohmann::json())
		{
			try
			{
				vector<string> public_list = UTF8toGBK(j_api["public"].get<vector<string>>());
				if (count(public_list.begin(), public_list.end(), job.strVar["ver"]) > 0)
				{
					strNameTmp = UTF8toGBK(j_api["data"][job.strVar["ver"]]["name"].get<string>());
					strCommentTmp = UTF8toGBK(j_api["data"][job.strVar["ver"]]["comment"].get<string>());
					strVersionTypeTmp = UTF8toGBK(j_api["data"][job.strVar["ver"]]["versiontype"].get<string>());
					strVersionTmp = UTF8toGBK(j_api["data"][job.strVar["ver"]]["version"].get<string>());
					strDownloadTmp = UTF8toGBK(j_api["data"][job.strVar["ver"]]["download"].get<string>());
					strDownloadTmp_dll = UTF8toGBK(j_api["data"][job.strVar["ver"]]["download_dll"].get<string>());
					strDownloadTmp_json = UTF8toGBK(j_api["data"][job.strVar["ver"]]["download_json"].get<string>());
					//job.echo("即将更新:[" + strNameTmp + "]\n分支说明:\n" + strCommentTmp + "\n分支版本:" + strVersionTmp + "\n更新dll:" + strDownloadTmp + "\n更新json:" + strDownloadTmp_json);
				}
				else
				{
					job.echo(job.strVar["ver"] + "分支:\n不存在!");
					return;
				}
			}
			catch (...)
			{
				job.note("更新源缓存加载异常!", 1);
				return;
			}
		}
		else
		{
			job.note("更新源缓存异常!", 1);
			return;
		}

		string strVerInfo;

		if (strVersionTypeTmp == "url") {
			string strApiVersionSaveLoc = DiceDir + "\\update\\DiceUpdateVersion_" + job.strVar["ver"] + ".json";
			switch (Cloud::DownloadFile(strVersionTmp.c_str(), strApiVersionSaveLoc.c_str())) {
			case -1:
				job.echo("分支版本获取失败:" + strURL);
				return;
				break;
			case -2:
				job.note("分支版本加载失败!分支版本缓存未找到:" + strApiVersionSaveLoc, 0b10);
				return;
				break;
			case 0:
				break;
			}

			ifstream fin(strApiVersionSaveLoc);
			if (!fin) {
				job.echo("更新源缓存为空!");
				return;
			}
			else {
				try
				{
					fin >> strVerInfo;
				}
				catch (...)
				{
					job.echo("更新源缓存加载异常!");
					return;
				}
			}
		}
		else if(strVersionTypeTmp == "raw") {
			strVerInfo = strVersionTmp;
		}
		else {
			strVerInfo = strVersionTmp;
		}

		job.echo("即将更新:[" + strNameTmp + "]\n版本号:" + strVerInfo + "\n分支说明:\n" + strCommentTmp);

		mkDir(DiceDir + "/update/DiceNew_" + job.strVar["ver"]);
		string strApiDllSaveLoc = DiceDir + "\\update\\DiceNew_" + job.strVar["ver"] + "\\com.w4123.dice.dll";
		string strApiJsonSaveLoc = DiceDir + "\\update\\DiceNew_" + job.strVar["ver"] + "\\com.w4123.dice.json";
		string urlDll(strDownloadTmp_dll);
		string urlJson(strDownloadTmp_json);
		switch (Cloud::DownloadFile(urlDll.c_str(), strApiDllSaveLoc.c_str())) {
		case -1:
			job.echo("更新失败:" + urlDll);
			return;
			break;
		case -2:
			job.note("更新Dice失败!dll文件未下载到指定位置", 0b1);
			return;
			break;
		case 0:
		default:
			switch (Cloud::DownloadFile(urlJson.c_str(), strApiJsonSaveLoc.c_str())) {
			case -1:
				job.echo("更新失败:" + urlJson);
				return;
				break;
			case -2:
				job.note("更新Dice失败!json文件未下载到指定位置", 0b1);
				return;
				break;
			case 0:
			default:
				break;
			}
		}

		if (frame == QQFrame::Mirai) {
			mkDir(dirExe + "plugins/MiraiNative/pluginsnew");
			mkDir(dirExe + "data/MiraiNative/pluginsnew");
			char pathDll[] = "plugins/MiraiNative/pluginsnew/com.w4123.dice.dll";
			char pathJson[] = "plugins/MiraiNative/pluginsnew/com.w4123.dice.json";
			char pathDll_new[] = "data/MiraiNative/pluginsnew/com.w4123.dice.dll";
			char pathJson_new[] = "data/MiraiNative/pluginsnew/com.w4123.dice.json";
			if (cp_file(strApiDllSaveLoc, pathDll)) {}
			else {
				job.note("更新Dice失败!dll文件整合时失败，但新版本已缓存。", 0b1);
				return;
			}
			if (cp_file(strApiJsonSaveLoc, pathJson)) {}
			else {
				job.note("更新Dice失败!json文件整合时失败，但新版本已缓存。", 0b1);
				return;
			}
			if (cp_file(strApiDllSaveLoc, pathDll_new)) {}
			else {
				job.note("更新Dice失败!dll文件整合时失败，但新版本已缓存。", 0b1);
				return;
			}
			if (cp_file(strApiJsonSaveLoc, pathJson_new)) {}
			else {
				job.note("更新Dice失败!json文件整合时失败，但新版本已缓存。", 0b1);
				return;
			}
			job.note("更新Dice!" + job.strVar["ver"] + "分支成功√", 1);
		}
		else if (frame == QQFrame::XianQu) {
			mkDir(dirExe + "CQPlugins/");
			char pathDll[] = "CQPlugins/com.w4123.dice.dll";
			char pathJson[] = "CQPlugins/com.w4123.dice.json";
			string urlDll(strDownloadTmp_dll);
			if (cp_file(strApiDllSaveLoc, pathDll)) {}
			else {
				job.note("更新Dice失败!dll文件整合时失败，但新版本已缓存。", 0b1);
				return;
			}
			if (cp_file(strApiJsonSaveLoc, pathJson)) {}
			else {
				job.note("更新Dice失败!json文件整合时失败，但新版本已缓存。", 0b1);
				return;
			}
			job.note("更新Dice!" + job.strVar["ver"] + "分支成功√", 1);
		}
		else {
			job.note("尝试进行未知服务平台中的更新，暂不予提供自动更新服务，但新版本已缓存。", 1);
			return;
			
			char** path = new char* ();
			_get_pgmptr(path);
			string strAppPath(*path);
			delete path;
			strAppPath = strAppPath.substr(0, strAppPath.find_last_of("\\")) + "\\app\\com.w4123.dice.cpk";
			string strURL(strDownloadTmp);
			switch (Cloud::DownloadFile(strURL.c_str(), strAppPath.c_str())) {
			case -1:
				job.echo("更新失败:" + strURL);
				break;
			case -2:
				job.note("更新Dice失败!文件未找到:" + strAppPath, 0b10);
				break;
			case 0:
				job.note("更新Dice!" + job.strVar["ver"] + "分支成功√\n可用.system reload 重启应用更新", 1);
			}
		}
	}
}

int cp_file(string sourcename, string destname)
{
	char buffer[256];
	try {
		ifstream in(sourcename, ios_base::in | ios_base::binary);
		ofstream out(destname, ios_base::out | ios_base::binary);
		if (!in || !out) {
			return FALSE;
		}
		while (!in.eof())
		{
			in.read(buffer, 256);
			auto n = in.gcount();
			out.write(buffer, n);
		}
		in.close();
		out.close();
	}
	catch (...) {
		return FALSE;
	}
	return TRUE;
}


void dice_cnmods_api(DiceJob& job) {
	char** path = new char* ();
	_get_pgmptr(path);
	string strAppPath(*path);
	string strApiSaveLoc, strApiGetLoc;
	if ((job.strVar["mode"] == "search" && job.strVar["name"] != "") || (job.strVar["mode"] == "luck" && job.strVar["name"] == ""))
	{
		string strURL("https://www.cnmods.net/index/moduleListPage.do?title=" + UrlEncode(GBKtoUTF8(job.strVar["name"])) + "&page=" + GBKtoUTF8(job.strVar["page"]));
		strApiSaveLoc = DiceDir + "\\cnmods\\cnmods_search_" + to_string(job.fromQQ) + ".json";
		switch (Cloud::DownloadFile(strURL.c_str(), strApiSaveLoc.c_str())) {
		case -1:
			job.echo("魔都模组访问失败");
			break;
		case -2:
			job.echo("魔都模组缓存故障");
			break;
		case 0:
			nlohmann::json j_api = freadJson(strApiSaveLoc);
			if (j_api != nlohmann::json())
			{
				try
				{
					string strPublicTmp;
					if (job.strVar["name"] == "")
					{
						strPublicTmp = "魔都推荐:";
						job.strVar["name"] = "魔都推荐";
					}
					else
					{
						strPublicTmp = "[" + job.strVar["name"] + "]的魔都模组搜索结果:";
					}
					int intCountThisPage = 0;
					for (auto it : j_api["data"]["list"])
					{
						intCountThisPage += 1;
						strPublicTmp += "\n["+ to_string(intCountThisPage) + "]" + UTF8toGBK(it["title"].get<string>());
					}
					strPublicTmp += "\n---[" + job.strVar["page"] + "/" + to_string(j_api["data"]["totalPages"].get<long long>()) + "]---";
					if (intCountThisPage > 0)
					{
						job.echo(strPublicTmp);
					}
					else
					{
						job.echo("没有关于[" + job.strVar["name"] + "]的魔都模组搜索结果");
					}
				}
				catch (...)
				{
					job.echo("魔都模组数据解析错误");
				}
			}
			else
			{
				job.echo("魔都模组数据解析失败");
			}
			break;
		}
	}
	else if (job.strVar["mode"] == "get")
	{
		strApiSaveLoc = DiceDir + "\\cnmods\\cnmods_search_" + to_string(job.fromQQ) + ".json";
		nlohmann::json j_api = freadJson(strApiSaveLoc);
		if (j_api != nlohmann::json())
		{
			try
			{
				string strPublicTmp;
				int intCountThisPage = 0;
				for (auto it : j_api["data"]["list"])
				{
					intCountThisPage += 1;
					if (to_string(intCountThisPage) == job.strVar["page"])
					{
						switch (console["CnmodsMode"])
						{
							default:
							case 1:
							{
								if (frame == QQFrame::Mirai)
								{
									strPublicTmp += "以下是搜索结果:\n\n";
								}
								strPublicTmp += "[CQ:share,url=https://www.cnmods.net/#/moduleDetail/index?keyId=";
								strPublicTmp += to_string(it["keyId"].get<long long>());
								strPublicTmp += ",title=";
								strPublicTmp += UTF8toGBK(it["title"].get<string>()) + " - 魔都模组";
								strPublicTmp += ",content=";
								strPublicTmp += UTF8toGBK(it["opinion"].get<string>());
								strPublicTmp += ",image=https://www.cnmods.net/modu.ico]";
								break;
							}
							case 2:
							{
								strPublicTmp += "以下是搜索结果:\n";
								strPublicTmp += "\n";
								strPublicTmp += UTF8toGBK(it["title"].get<string>());
								strPublicTmp += "\n作者: ";
								strPublicTmp += UTF8toGBK(it["article"].get<string>());
								strPublicTmp += "\n发布日期: ";
								strPublicTmp += UTF8toGBK(it["releaseDate"].get<string>());
								strPublicTmp += "\n规则书: ";
								strPublicTmp += it["moduleType"].get<string>() == "" ? "未指定" : UTF8toGBK(it["moduleType"].get<string>());
								strPublicTmp += it["moduleVersion"].get<string>() == "" ? "" : "-" + UTF8toGBK(it["moduleVersion"].get<string>());
								strPublicTmp += "\n难度: ";
								strPublicTmp += UTF8toGBK(it["freeLevel"].get<string>());
								strPublicTmp += "\n类型: ";
								strPublicTmp += UTF8toGBK(it["structure"].get<string>());
								strPublicTmp += "\n时代: ";
								strPublicTmp += UTF8toGBK(it["moduleAge"].get<string>());
								strPublicTmp += "\n地区: ";
								strPublicTmp += UTF8toGBK(it["occurrencePlace"].get<string>());
								strPublicTmp += "\n时长: ";
								strPublicTmp += to_string(it["minDuration"].get<long long>());
								strPublicTmp += "-";
								strPublicTmp += to_string(it["maxDuration"].get<long long>());
								strPublicTmp += "\n人数: ";
								strPublicTmp += to_string(it["minAmount"].get<long long>());
								strPublicTmp += "-";
								strPublicTmp += to_string(it["maxAmount"].get<long long>());
								strPublicTmp += "\n原创性: ";
								strPublicTmp += it["original"].get<bool>() == true ? "原创作品": "翻译作品";
								strPublicTmp += "\n简介: ";
								strPublicTmp += UTF8toGBK(it["opinion"].get<string>());
								strPublicTmp += "\n魔都页面: \nhttps://www.cnmods.net/#/moduleDetail/index?keyId=";
								strPublicTmp += to_string(it["keyId"].get<long long>());
							}
						}
					}
				}
				if (intCountThisPage > 0 && strPublicTmp.length() > 0)
				{
					job.echo(strPublicTmp);
				}
				else
				{
					job.echo("不存在的项目");
				}
			}
			catch (...)
			{
				job.echo("魔都模组数据解析错误");
			}
		}
		else
		{
			job.echo("魔都模组数据解析失败。\n或许你应当先试试查找模组。");
		}
	}
	else if (job.strVar["mode"] == "roll")
	{
		string strURL("https://www.cnmods.net/index/moduleListPage.do");
		strApiSaveLoc = DiceDir + "\\cnmods\\cnmods_roll_" + to_string(job.fromQQ) + ".json";
		bool flagApiOn = FALSE;
		long long intRollPage = 1;
		long long intTotalPages = 0;
		long long intTotalElements = 0;
		switch (Cloud::DownloadFile(strURL.c_str(), strApiSaveLoc.c_str())) {
		case -1:
			job.echo("魔都模组访问失败");
			break;
		case -2:
			job.echo("魔都模组缓存故障");
			break;
		case 0:
			nlohmann::json j_api = freadJson(strApiSaveLoc);
			if (j_api != nlohmann::json())
			{
				try
				{
					intTotalPages = j_api["data"]["totalPages"].get<long long>();
					intTotalElements = j_api["data"]["totalElements"].get<long long>();
					if (intTotalPages > 0 && intTotalElements > 0)
					{
						RD rdRollPage("1D" + to_string(intTotalPages));
						rdRollPage.Roll();
						intRollPage = rdRollPage.intTotal;
						flagApiOn = TRUE;
					}
				}
				catch (...)
				{
					job.echo("魔都模组数据解析错误");
				}
			}
			else
			{
				job.echo("魔都模组数据解析失败");
			}
			break;
		}
		if (flagApiOn)
		{
			string strURL2("https://www.cnmods.net/index/moduleListPage.do?page=" + to_string(intRollPage));
			int intRollCount = 1;
			switch (Cloud::DownloadFile(strURL2.c_str(), strApiSaveLoc.c_str())) {
			case -1:
				job.echo("魔都模组访问失败");
				break;
			case -2:
				job.echo("魔都模组缓存故障");
				break;
			case 0:
				nlohmann::json j_api = freadJson(strApiSaveLoc);
				if (j_api != nlohmann::json())
				{
					try
					{
						int intCountThisPage = 0;
						string strPublicTmp;
						for (auto it : j_api["data"]["list"])
						{
							intCountThisPage += 1;
						}
						if (intCountThisPage > 0)
						{
							RD rdRollCount("1D" + to_string(intCountThisPage));
							rdRollCount.Roll();
							intRollCount = rdRollCount.intTotal;
							intCountThisPage = 0;
							for (auto it : j_api["data"]["list"])
							{
								intCountThisPage += 1;
								if (to_string(intCountThisPage) == to_string(intRollCount))
								{
									switch (console["CnmodsMode"])
									{
										default:
										case 1:
										{
											if (frame == QQFrame::Mirai)
											{
												strPublicTmp += "以下是抽取结果:\n\n";
											}
											strPublicTmp += "[CQ:share,url=https://www.cnmods.net/#/moduleDetail/index?keyId=";
											strPublicTmp += to_string(it["keyId"].get<long long>());
											strPublicTmp += ",title=";
											strPublicTmp += UTF8toGBK(it["title"].get<string>()) + " - 魔都模组";
											strPublicTmp += ",content=";
											strPublicTmp += UTF8toGBK(it["opinion"].get<string>());
											strPublicTmp += ",image=https://www.cnmods.net/modu.ico]";
											break;
										}
										case 2:
										{
											strPublicTmp += "以下是抽取结果:\n";
											strPublicTmp += "\n";
											strPublicTmp += UTF8toGBK(it["title"].get<string>());
											strPublicTmp += "\n作者: ";
											strPublicTmp += UTF8toGBK(it["article"].get<string>());
											strPublicTmp += "\n发布日期: ";
											strPublicTmp += UTF8toGBK(it["releaseDate"].get<string>());
											strPublicTmp += "\n规则书: ";
											strPublicTmp += it["moduleType"].get<string>() == "" ? "未指定" : UTF8toGBK(it["moduleType"].get<string>());
											strPublicTmp += it["moduleVersion"].get<string>() == "" ? "" : "-" + UTF8toGBK(it["moduleVersion"].get<string>());
											strPublicTmp += "\n难度: ";
											strPublicTmp += UTF8toGBK(it["freeLevel"].get<string>());
											strPublicTmp += "\n类型: ";
											strPublicTmp += UTF8toGBK(it["structure"].get<string>());
											strPublicTmp += "\n时代: ";
											strPublicTmp += UTF8toGBK(it["moduleAge"].get<string>());
											strPublicTmp += "\n地区: ";
											strPublicTmp += UTF8toGBK(it["occurrencePlace"].get<string>());
											strPublicTmp += "\n时长: ";
											strPublicTmp += to_string(it["minDuration"].get<long long>());
											strPublicTmp += "-";
											strPublicTmp += to_string(it["maxDuration"].get<long long>());
											strPublicTmp += "\n人数: ";
											strPublicTmp += to_string(it["minAmount"].get<long long>());
											strPublicTmp += "-";
											strPublicTmp += to_string(it["maxAmount"].get<long long>());
											strPublicTmp += "\n原创性: ";
											strPublicTmp += it["original"].get<bool>() == true ? "原创作品" : "翻译作品";
											strPublicTmp += "\n简介: ";
											strPublicTmp += UTF8toGBK(it["opinion"].get<string>());
											strPublicTmp += "\n魔都页面: \nhttps://www.cnmods.net/#/moduleDetail/index?keyId=";
											strPublicTmp += to_string(it["keyId"].get<long long>());
										}
									}
								}
							}
							job.echo(strPublicTmp);
						}
						else
						{
							job.echo("魔都模组抽取失败");
						}
					}
					catch (...)
					{
						job.echo("魔都模组数据解析错误");
					}
				}
				else
				{
					job.echo("魔都模组数据解析失败");
				}
				break;
			}
		}
		else
		{
			job.echo("魔都模组数据接口未开放");
		}
	}
	else
	{
		job.echo("魔都模组模块:\n[.cnmods roll]在线抽取一个模组\n[.cnmods luck ([页码])]查看魔都推荐\n[.cnmods search [名称] ([页码])]在线查找模组\n[.cnmods get [编号]]获取对应模组");
	}
}

//获取云不良记录
void dice_cloudblack(DiceJob& job) {
	job.echo("开始获取云端记录"); 
	string strURL("https://shiki.stringempty.xyz/blacklist/checked.json?" + to_string(job.fromTime));
	switch (Cloud::DownloadFile(strURL.c_str(), (DiceDir + "/conf/CloudBlackList.json").c_str())) {
	case -1:
		job.echo("同步云不良记录同步失败:" + strURL);
		break;
	case -2:
		job.echo("同步云不良记录同步失败!文件未找到");
		break;
	case 0:
		job.note("同步云不良记录成功，" + getMsg("self") + "开始读取", 1);
		blacklist->loadJson(DiceDir + "/conf/CloudBlackList.json", true);
	}
	if (console["CloudBlackShare"])
		sch.add_job_for(24 * 60 * 60, "cloudblack");
}

void log_put(DiceJob& job) {
	job["ret"] = put_s3_object("dicelogger",
							   job.strVar["log_file"],
							   job.strVar["log_path"],
							   "ap-southeast-1");
	if (job["ret"] == "SUCCESS") {
		job.echo(getMsg("strLogUpSuccess", job.strVar));
	}
	else if (job.cntExec++ > 1) {
		job.echo(getMsg("strLogUpFailureEnd",job.strVar));
	}
	else {
		job["retry"] = to_string(job.cntExec);
		job.echo(getMsg("strLogUpFailure", job.strVar));
		sch.add_job_for(2 * 60, job);
	}
}

string print_master() {
	if (!console.master())return "（无主）";
	return printQQ(console.master());
}

string list_deck() {
	return listKey(CardDeck::mPublicDeck);
}
string list_extern_deck() {
	return listKey(CardDeck::mExternPublicDeck);
}
