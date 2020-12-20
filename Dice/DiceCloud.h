/*
 * 骰娘网络
 * Copyright (C) 2019 String.Empty
 * 该部分代码存在大量前任开发者初学阶段的大量贸然试验痕迹，后续开发者不应过分依赖该部分不安全代码
 * Copyright (C) 2019-2020 lunzhiPenxil仑质
 */
#pragma once

class FromMsg;

namespace Cloud
{
	void update();
	int checkWarning(const char* warning);
	int DownloadFile(const char* url, const char* downloadPath);
	int checkUpdate(FromMsg* msg);
}
