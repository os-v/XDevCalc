//
//  @file DevCalc.cpp
//  @author Sergii Oryshchenko <sergii.orishchenko@gmail.com>
//  @see https://github.com/os-v/XDevCalc/
//
//  Created on 14.03.22.
//  Copyright 2022 Sergii Oryshchenko. All rights reserved.
//

#include <TinyJS/TinyJS.h>
#include <TinyJS/TinyJS_Functions.h>
#include <TinyJS/TinyJS_MathFunctions.h>
#include <ConfigFileInfo.h>
#include "DevCalc.h"

#ifdef WIN32
	#define SYM_PATH		'\\'
	#include <Windows.h>
#else
	#define SYM_PATH		'/'
#endif

#define CFG_MAIN		"main"
#define CFG_PRINTSTD		"PrintStd"
#define CFG_PRINTCMD		"PrintCmd"
#define CFG_PRINTOPT		"PrintOpt"
#define CFG_PRECISION		"Precision"

std::string GAppName;
gmp_randclass GRandClass(gmp_randinit_default);

enum { EPrintAll = 0x00, EPrintDec = 0x01, EPrintOct = 0x02, EPrintHex = 0x04, EPrintBin = 0x08 };

typedef struct ARGS {
	std::string Expr;
	bool Help, Defaults, PrintStd, PrintCmd;
	uint32_t PrintOpt, Precision;
} *PARGS;

bool ParseArgs(int argc, char** argv, ARGS& pArgs, CConfigFileInfo& pConfig);
std::string ToString(const mpf_class& pValue);
std::string ToString(const mpz_class& pValue, int nBase, const mpz_class& pNegative);
mpz_class ToAligned(const mpz_class& pValue, uint32_t nBytesSize);
std::string ToUpperCase(std::string sValue);
std::string GetConfigPath();

int main(int argc, char **argv)
{

	const char* lpAppName = strrchr(argv[0], SYM_PATH);
	GAppName = lpAppName ? lpAppName : argv[0];

	CConfigFileInfo pConfig;
	pConfig.Load(GetConfigPath().c_str());

#ifdef WIN32
	GRandClass.seed(GetTickCount());
#else
	timespec pTimeSpec;
	clock_gettime(CLOCK_REALTIME, &pTimeSpec);
	GRandClass.seed(pTimeSpec.tv_nsec / 1000000);
#endif

	CTinyJS pTinyJS;
	registerFunctions(&pTinyJS);
	registerMathFunctions(&pTinyJS);

	ARGS pArgs;
	if (!ParseArgs(argc, argv, pArgs, pConfig))
		return -1;

	if (pArgs.Precision)
		mpf_set_default_prec(pArgs.Precision);

	try
	{
		CScriptVarLink pResult = pTinyJS.evaluateComplex(pArgs.Expr);
		std::string sResult = "";
		if (pResult.var->isInt() || pResult.var->isDouble())
		{
			mpz_class pIntValue = pResult.var->getInt();
			mpz_class pNegValue = pIntValue < 0 ? ToAligned(pIntValue, 0) : 0;
			if (pArgs.PrintOpt & EPrintDec || pArgs.PrintOpt == EPrintAll)
				sResult += std::string(sResult.empty() ? "" : " ") + "dec=" + ToString(pResult.var->getDouble());
			if (pArgs.PrintOpt & EPrintOct || pArgs.PrintOpt == EPrintAll)
				sResult += std::string(sResult.empty() ? "" : " ") + "oct=" + ToString(pIntValue, 8, pNegValue);
			if (pArgs.PrintOpt & EPrintHex || pArgs.PrintOpt == EPrintAll)
				sResult += std::string(sResult.empty() ? "" : " ") + "hex=" + ToString(pIntValue, 16, pNegValue);
			if (pArgs.PrintOpt & EPrintBin || pArgs.PrintOpt == EPrintAll)
				sResult += std::string(sResult.empty() ? "" : " ") + "bin=" + ToString(pIntValue, 2, pNegValue);
		}
		else
			sResult = "str='" + pResult.var->getString() + "'";
		if(pArgs.PrintStd)
			printf("%s\r\n", sResult.c_str());
	}
	catch (CScriptException* e)
	{
		printf("ERROR: %s\r\n", e->text.c_str());
	}
	catch (std::exception e)
	{
		printf("INVALID ARGUMENT\r\n");
	}
	catch (...)
	{
		printf("INVALID EXPRESSION\r\n");
	}

	return 0;
}

bool ParseArgs(int argc, char** argv, ARGS& pArgs, CConfigFileInfo& pConfig)
{

	pArgs.Expr = "";
	pArgs.Help = pArgs.Defaults = false;
	pArgs.PrintStd = pConfig.GetBool(CFG_MAIN, CFG_PRINTSTD, true);
	pArgs.PrintCmd = pConfig.GetBool(CFG_MAIN, CFG_PRINTCMD, false);
	pArgs.PrintOpt = pConfig.GetInt(CFG_MAIN, CFG_PRINTOPT, EPrintAll);
	pArgs.Precision = pConfig.GetInt(CFG_MAIN, CFG_PRECISION, 1024);

	for (int iArg = 1; iArg < argc; iArg++)
	{
		const char* lpArg = argv[iArg];
		if (!strcmp(lpArg, "-h"))
			pArgs.Help = true;
		else if (!strcmp(lpArg, "-d"))
			pArgs.Defaults = true;
		else if (!strncmp(lpArg, "-r", 2))
			pArgs.Precision = atoi(lpArg + 2);
		else if (!strcmp(lpArg, "-s"))
			pArgs.PrintStd = true;
		else if (!strcmp(lpArg, "-c"))
			pArgs.PrintCmd = true;
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

	if (pArgs.Help || pArgs.Expr.empty())
	{
		printf("%s [-h][-d][-s][-c][-rNUM][-p[d|o|h|b]] expression\r\n", GAppName.c_str());
		printf("\t-h - current help\r\n");
		printf("\t-d - update defaults\r\n");
		printf("\t-r - precision NUM bits\r\n");
		printf("\t-s - print output to stdout\r\n");
		printf("\t-c - print output to command line\r\n");
		printf("\t-p - print d-dec,o-oct,h-hex,b-bin\r\n");
		printf("please visit %s for updates\r\n", PRODUCT_URL);
		return false;
	}

	if (pArgs.Defaults)
	{
		pConfig.SetBool(CFG_MAIN, CFG_PRINTSTD, pArgs.PrintStd);
		pConfig.SetBool(CFG_MAIN, CFG_PRINTCMD, pArgs.PrintCmd);
		pConfig.SetInt(CFG_MAIN, CFG_PRINTOPT, pArgs.PrintOpt);
		pConfig.SetInt(CFG_MAIN, CFG_PRECISION, pArgs.Precision);
		pConfig.Save();
	}

	return true;
}

std::string ToString(const mpz_class& pValue, int nBase, const mpz_class& pNegative)
{

	std::string sValue = pNegative > 0 ? pNegative.get_str(nBase) : pValue.get_str(nBase);
	if (nBase == 16)
		return ToUpperCase(sValue);

	return sValue;
}

std::string ToString(const mpf_class& pValue)
{

	mp_exp_t expo;
	std::string sResult = pValue.get_str(expo);

	if (sResult.length() && sResult[0] == '-')
		sResult = sResult.substr(1);

	if (expo <= 0)
	{
		for (; expo < 0; expo++, sResult = '0' + sResult);
		sResult = "0." + sResult;
	}
	else if (expo < (mp_exp_t)sResult.length())
		sResult.insert(sResult.begin() + expo, '.');
	else
		sResult.insert(sResult.end(), expo - sResult.length(), '0');

	if (pValue < 0)
		sResult = "-" + sResult;

	return sResult;
}

mpz_class ToAligned(const mpz_class& pValue, uint32_t nBytesSize)
{

	mpz_class pResult;

	bool fNegative = pValue < 0;
	if (fNegative)
	{
		size_t nLength = pValue.get_str(16).length() - 1, nDigits = 8;
		for (; nLength > nDigits; nDigits <<= 1);
		std::string sHex = "1" + std::string(nDigits, '0');
		if(mpz_class(sHex, 16) / 2 < -pValue)
			sHex = "1" + std::string(nDigits << 1, '0');
		pResult = mpz_class(sHex, 16) + pValue;
	}
	else
		pResult = pValue;

	if (nBytesSize)
	{
		mpz_class pMask(0xFF);
		for (; --nBytesSize; )
			pResult = (pResult << 8) | 0xFF;
		pResult &= pMask;
	}

	return pResult;
}

std::string ToUpperCase(std::string sValue)
{
	std::transform(sValue.begin(), sValue.end(), sValue.begin(), ::toupper);
	return sValue;
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

