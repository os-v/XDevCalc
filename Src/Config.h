//
//  @file Config.h
//  @author Sergii Oryshchenko <sergii.orishchenko@gmail.com>
//  @see https://github.com/os-v/XDevCalc/
//
//  Created on 26.05.22.
//  Copyright 2022 Sergii Oryshchenko. All rights reserved.
//

#ifndef _CONFIG_H_
#define _CONFIG_H_

#include <ConfigFileInfo.h>

#ifdef WIN32
	#define SYM_PATH		'\\'
	#include <Windows.h>
#else
	#define SYM_PATH		'/'
#endif

enum { EPrintAll = 0x00, EPrintDec = 0x01, EPrintOct = 0x02, EPrintHex = 0x04, EPrintBin = 0x08 };
enum { ENumericTypeDefault = 0, ENumericTypeFloat };

typedef struct ARGS {
	std::string Expr;
	bool Help;
	bool FuncHelp;
	bool Defaults;
	bool PrintStd;
	bool PrintCmd;
	uint32_t PrintOpt;
	uint32_t Precision;
	uint32_t NumericType;
} *PARGS;

void PrintMainHelp();
void PrintFuncHelp();
bool InitConfigArgs(int argc, char** argv, ARGS& pArgs);
std::string GetConfigPath();

#endif

