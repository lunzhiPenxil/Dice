# Dice!
QQ Dice Robot For TRPG Based on CoolQ/Mirai/CQXQ

[![License](https://img.shields.io/github/license/lunzhiPenxil/Dice.svg)](http://www.gnu.org/licenses)
[![Build status](https://ci.appveyor.com/api/projects/status/k1kxb3dpnd2ng88m/branch/Oliva?svg=true)](https://ci.appveyor.com/project/lunzhiPenxil/olivadice/branch/Oliva)
[![Downloads](https://img.shields.io/github/downloads/lunzhiPenxil/dice/total.svg)](https://github.com/lunzhiPenxil/Dice/releases)
[![GitHub contributors](https://img.shields.io/github/contributors/lunzhiPenxil/dice.svg)](https://github.com/lunzhiPenxil/Dice/graphs/contributors)
[![GitHub last commit](https://img.shields.io/github/last-commit/lunzhiPenxil/dice.svg)](https://github.com/lunzhiPenxil/Dice/commits)

## 简介

Dice!是一款基于酷Q的QQ跑团掷骰机器人，而这是由仑质维护的Oliva分支。

主页:    
<http://kokona.tech/>   
<https://oliva.dicer.wiki/>   

Latest Stable Release: [![GitHub release](https://img.shields.io/github/release/lunzhiPenxil/dice.svg)](https://github.com/w4123/lunzhiPenxil/releases) [![GitHub Release Date](https://img.shields.io/github/release-date/lunzhiPenxil/dice.svg)](https://github.com/lunzhiPenxil/Dice/releases)

Latest Release: [![GitHub release](https://img.shields.io/github/release-pre/lunzhiPenxil/dice.svg)](https://github.com/lunzhiPenxil/Dice/releases) [![GitHub Release Date](https://img.shields.io/github/release-date-pre/lunzhiPenxil/dice.svg)](https://github.com/lunzhiPenxil/Dice/releases)

## 开发者

贡献者:w4123溯洄 Shiki jh123111 緋色月下、スイカを食う 仑质

感谢:Flandre Cirno 回転 他是王家乐。白いとう 哞哞哞哞哞哞哞哞哞哞哞哞 丸子 黯星 一盏大师 初音py2001 Coxxs orzFly 等(排名不分先后)(如有缺漏请务必联系溯洄:QQ1840686745) 

## 编译须知

从GitHub克隆源码时请不要直接从master分支克隆, 因为所有的更改都会提交到此分支, 很有可能包含最新的测试性更改, 未经过测试无法保证稳定 请选择Tag中最新的Release进行下载

请使用最新版Visual Studio 2019或以上版本 (或其独立编译器)进行编译, 项目主文件为Dice.sln, 编译时务必使用Win32模式否则无法编译成功

新增: 现在可以用GCC/Clang编译, 只测试了几个版本, 编译出现问题请反馈, 下面列出编译选项, 正在写cmake

- GCC(9.0.0+): ` g++ -shared -static -std=c++17 -O2 -o com.w4123.dice.dll -Wl,--kill-at -I CQSDK\ -I Dice\ CQSDKCPP\*.cpp Dice\*.cpp Dice\CQP.lib -pthread -lWinInet -luser32 `
- Clang(4+)+MSVC(VS2019+): ` clang-cl --target=i686-pc-windows-msvc /MT /O2 /EHsc /std:c++17 /D "UNICODE" /LD /link "user32.lib" /o com.w4123.dice.dll /I CQSDK\ /I Dice\ CQSDKCPP\*.cpp Dice\*.cpp Dice\CQP.lib -Wno-invalid-source-encoding  `
- Clang(4+)+GCC(9.0.0+): ` clang++ --target=i686-pc-windows-gnu -m32 -shared -static -o com.w4123.dice.dll -Xclang -flto-visibility-public-std -Wl,--kill-at -std=c++17 -O2 -I CQSDK\ -I Dice\ CQSDKCPP\*.cpp Dice\*.cpp Dice\CQP.lib -lWinInet -luser32 -pthread -Wno-invalid-source-encoding  `

编译后会得到com.w4123.dice.dll文件, 请勿更改此文件的名称! 请从Releases中下载对应的json文件(或自己编写), 放至酷Q app文件夹下, 并开启开发模式, 在应用管理中合成cpk文件即可正常使用

## Issue提交

提交Issue前请务必确认没有其他人提交过相同的Issue, 善用搜索功能 提交Bug时最好附有截图以及问题复现方法, 同时也欢迎新功能建议

## License

Dice! QQ Dice Robot for TRPG

Copyright (C) 2018-2020 w4123溯洄 Shiki 仑质

This program is free software: you can redistribute it and/or modify it under the terms
of the GNU Affero General Public License as published by the Free Software Foundation,
either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU Affero General Public License for more details.

You should have received a copy of the GNU Affero General Public License along with this
program. If not, see <http://www.gnu.org/licenses/>.

