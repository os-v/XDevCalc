//
//  @file Config.cpp
//  @author Sergii Oryshchenko <sergii.orishchenko@gmail.com>
//  @see https://github.com/os-v/XDevCalc/
//
//  Created on 26.05.22.
//  Copyright 2022 Sergii Oryshchenko. All rights reserved.
//

#include "Config.h"
#include "DevCalc.h"

#define CFG_MAIN			"main"
#define CFG_PRINTSTD		"PrintStd"
#define CFG_PRINTCMD		"PrintCmd"
#define CFG_PRINTOPT		"PrintOpt"
#define CFG_PRECISION		"Precision"
#define CFG_NUMERICTYPE		"NumericType"

std::string GAppName;

void PrintMainHelp()
{
	printf("%s [-h][-d][-s][-c][-rNUM][-p[d|o|h|b]] expression\r\n", GAppName.c_str());
	printf("\t-h - current help\r\n");
	printf("\t-f - show functions help\r\n");
	printf("\t-d - update defaults\r\n");
	printf("\t-r - precision NUM bits\r\n");
	printf("\t-s - print output to stdout\r\n");
	printf("\t-c - print output to command line\r\n");
	printf("\t-p - print d-dec,o-oct,h-hex,b-bin\r\n");
	printf("\t-t - numebers type, f-float, d-by definition (float defined with . like 1.0)\r\n");
	printf("version: %s\r\n", PRODUCT_SVERSION);
	printf("please visit %s for updates\r\n", PRODUCT_URL);
}

void PrintFuncHelp()
{

	printf(
		"\tSupported functions:\r\n"
		"\t\tabs(a), round(a), min(a,b), max(a,b), range(x,a,b), sign(a),\r\n"
		"\t\tPI(), deg(a), rad(a), sin(a), asin(a), cos(a), acos(a), tan(a),\r\n"
		"\t\tatan(a), sinh(a), asinh(a), cosh(a), acosh(a), tanh(a), atanh(a),\r\n"
		"\t\tE(), log(a), log10(a), exp(a), pow(a,b), sqr(a), sqrt(a),\r\n"
		"\t\trand(), randInt(min, max), charToInt(c), int(a), uint(a)\r\n"
		"\tSupported operators:\r\n"
		"\t\tarithmetic +, -, *, /, %%\r\n"
		"\t\tbitwise: ~, &, |, ^, <<, >>\r\n"
		"\t\tlogical: !, ||, &&\r\n"
		"\t\trelational: <, >, <=, >=, ==, !=\r\n"
		"\t\tconditional: condition ? expression_if_true : expression_if_false\r\n"
	);

}

bool InitConfigArgs(int argc, char** argv, ARGS& pArgs)
{

	const char* lpAppName = strrchr(argv[0], SYM_PATH);
	GAppName = lpAppName ? lpAppName : argv[0];

	CConfigFileInfo pConfig;
	pConfig.Load(GetConfigPath().c_str());

	pArgs.Expr = "";
	pArgs.Help = pArgs.FuncHelp = pArgs.Defaults = false;
	pArgs.PrintStd = pConfig.GetBool(CFG_MAIN, CFG_PRINTSTD, true);
	pArgs.PrintCmd = pConfig.GetBool(CFG_MAIN, CFG_PRINTCMD, false);
	pArgs.PrintOpt = pConfig.GetInt(CFG_MAIN, CFG_PRINTOPT, EPrintAll);
	pArgs.Precision = pConfig.GetInt(CFG_MAIN, CFG_PRECISION, 1024);
	pArgs.NumericType = pConfig.GetInt(CFG_MAIN, CFG_NUMERICTYPE, ENumericTypeFloat);

	for (int iArg = 1; iArg < argc; iArg++)
	{
		const char* lpArg = argv[iArg];
		if (!strcmp(lpArg, "-h"))
			pArgs.Help = true;
		else if (!strcmp(lpArg, "-f"))
			pArgs.FuncHelp = true;
		else if (!strcmp(lpArg, "-d"))
			pArgs.Defaults = true;
		else if (!strncmp(lpArg, "-r", 2))
			pArgs.Precision = atoi(lpArg + 2);
		else if (!strcmp(lpArg, "-s"))
			pArgs.PrintStd = true;
		else if (!strcmp(lpArg, "-c"))
			pArgs.PrintCmd = true;
		else if (!strcmp(lpArg, "-t") || !strcmp(lpArg, "-td"))
			pArgs.NumericType = ENumericTypeDefault;
		else if (!strcmp(lpArg, "-tf"))
			pArgs.NumericType = ENumericTypeFloat;
		else if (!strncmp(lpArg, "-p", 2))
		{
			for (const char* lpParam = lpArg + 2; *lpParam; lpParam++)
			{
				if (*lpParam == 'd')
					pArgs.PrintOpt |= EPrintDec;
				else if (*lpParam == 'o')
					pArgs.PrintOpt |= EPrintOct;
				else if (*lpParam == 'h')
					pArgs.PrintOpt |= EPrintHex;
				else if (*lpParam == 'b')
					pArgs.PrintOpt |= EPrintBin;
			}
		}
		else
			pArgs.Expr = argv[iArg];
	}

	if (pArgs.FuncHelp)
	{
		PrintFuncHelp();
		return false;
	}

	if (pArgs.Help || pArgs.Expr.empty())
	{
		PrintMainHelp();
		return false;
	}

	if (pArgs.Defaults)
	{
		pConfig.SetBool(CFG_MAIN, CFG_PRINTSTD, pArgs.PrintStd);
		pConfig.SetBool(CFG_MAIN, CFG_PRINTCMD, pArgs.PrintCmd);
		pConfig.SetInt(CFG_MAIN, CFG_PRINTOPT, pArgs.PrintOpt);
		pConfig.SetInt(CFG_MAIN, CFG_PRECISION, pArgs.Precision);
		pConfig.SetInt(CFG_MAIN, CFG_NUMERICTYPE, pArgs.NumericType);
		pConfig.Save();
	}

	return true;
}

std::string GetConfigPath()
{

#ifdef WIN32
	const char* lpAppData = getenv("APPDATA");
#else
	const char* lpAppData = getenv("HOME");
#endif

	if (!lpAppData)
		return "";

	std::string sAppData = lpAppData;
	if (sAppData.length() && sAppData[sAppData.length() - 1] != SYM_PATH)
		sAppData += SYM_PATH;

	sAppData += GAppName;

#ifdef WIN32
	size_t iExtPos = sAppData.rfind(".");
	if (iExtPos != (size_t)-1)
		sAppData = sAppData.substr(0, iExtPos);
#endif

	sAppData += ".conf";

	return sAppData;
}

