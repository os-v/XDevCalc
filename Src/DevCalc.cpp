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
#include <mputils.h>
#include "DevCalc.h"
#include "Config.h"
#include "Helper.h"

gmp_randclass GRandClass(gmp_randinit_default);

int main(int argc, char **argv)
{

#ifdef WIN32
	GRandClass.seed(GetTickCount());
#else
	timespec pTimeSpec;
	clock_gettime(CLOCK_REALTIME, &pTimeSpec);
	GRandClass.seed(pTimeSpec.tv_nsec / 1000000);
#endif

	ARGS pArgs;
	if (!InitConfigArgs(argc, argv, pArgs))
		return -1;

	if (pArgs.Precision)
		mpf_set_default_prec(pArgs.Precision);

	LEX_TYPES eTypes = pArgs.NumericType == ENumericTypeFloat ? LEX_FLOAT : LEX_EOF;

	CTinyJS pTinyJS(eTypes);
	registerFunctions(&pTinyJS);
	registerMathFunctions(&pTinyJS);

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

