/***************************************************************
*      scanner routine for Mini C language                    *
*                                   2003. 3. 10               *
***************************************************************/

#pragma once


#define NO_KEYWORD 14
#define ID_LENGTH 12

struct tokenType {
	int number;
	double fNumber;
	union {
		char id[ID_LENGTH];
		int num;
	} value;
};


enum tsymbol {
	tnull = -1,
	tnot, tnotequ, tremainder, tremAssign, tident, tnumber,
	/* 0          1            2         3            4          5     */
	tand, tlparen, trparen, tmul, tmulAssign, tplus,
	/* 6          7            8         9           10         11     */
	tinc, taddAssign, tcomma, tminus, tdec, tsubAssign,
	/* 12         13          14        15           16         17     */
	tdiv, tdivAssign, tsemicolon, tless, tlesse, tassign,
	/* 18         19          20        21           22         23     */
	tequal, tgreat, tgreate, tlbracket, trbracket, teof,
	/* 24         25          26        27           28         29     */
	//   ...........    word symbols ................................. //
	/* 30         31          32        33           34         35     */
	tconst, telse, tif, tint, treturn, tvoid,
	/* 36         37          38        39        40                     */
	twhile, tlbrace, tor, trbrace, tfor,
	/*41          42          43        44        45      46              */
	tswitch, tcase, tgoto, tbreak, tcontinue, tdouble,
	//---------------------escape sequence, :, " --------------------------
	/* 47       48        49        50         51        52               */                                                        
	 tdelimiter, tescapeA, tescapeB, tescapeF, tescapeN, tescapeR,
	/* 53     54     55     56    57    58                               */
	tescapeT, tescapeV, tescapeBack, tescapeSingle, tescapeDouble, tescapeQuestion,
	/* 59    60    61*/
	tescapeE, tdoublequote, tFnumber
};


struct tokenType scanner();
void printToken(struct tokenType token);
