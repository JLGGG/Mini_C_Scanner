/***************************************************************
*      scanner routine for Mini C language                    *
*                                   2003. 3. 10               *
***************************************************************/

#include <stdio.h>
#include<stdlib.h>
#include <string.h>
#include <ctype.h>
#include <memory.h>

#include "Scanner.h"
#define TRUE 1
#define FALSE 0

extern FILE *sourceFile;                       // miniC source program


int superLetter(char ch);
int superLetterOrDigit(char ch);
int getNumber(char firstCharacter);
double getFnumber();
int hexValue(char ch);
void lexicalError(int n);


char *tokenName[] = {
	"!",        "!=",      "%",       "%=",     "%ident",   "%number",
	/* 0          1           2         3          4          5        */
	"&&",       "(",       ")",       "*",      "*=",       "+",
	/* 6          7           8         9         10         11        */
	"++",       "+=",      ",",       "-",      "--",	    "-=",
	/* 12         13         14        15         16         17        */
	"/",        "/=",      ";",       "<",      "<=",       "=",
	/* 18         19         20        21         22         23        */
	"==",       ">",       ">=",      "[",      "]",        "eof",
	/* 24         25         26        27         28         29        */
	//   ...........    word symbols ................................. //
	/* 30         31         32        33         34         35        */
	"const",    "else",     "if",      "int",     "return",  "void",
	/* 36         37         38        39          40                    */
	"while",    "{",        "||",       "}",      "for",
	/* 41         42         43        44          45           46       */
	"switch",   "case",     "goto",    "break",   "continue",   "double",
	/* 47       48        49        50         51        52               */
	":",    "\\a",    "\\b",   "\\f",   "\\n",    "\\r",
	/*53     54     55     56    57    58*/
	"\\t",   "\\v",    "\\\\",    "\\\'",    "\\\"",    "\\?",
	/* 59   60*/
	"\\e",  "\""
};

char *keyword[NO_KEYWORD] = {
	"const",  "else",    "if",    "int",    "return",  "void",    "while",
	"for", "switch", "case", "goto", "break", "continue", "double"
};

enum tsymbol tnum[NO_KEYWORD] = {
	tconst,    telse,     tif,     tint,     treturn,   tvoid,     twhile,
	tfor, tswitch, tcase, tgoto, tbreak, tcontinue, tdouble
};

struct tokenType scanner()
{
	struct tokenType token;
	int i, j=0, k=0, index;
	char ch, id[ID_LENGTH];
	char commentArr[100];
	char fBool = FALSE;

	token.number = tnull;
	memset(commentArr, 0, sizeof(commentArr));

	do {
		while (isspace(ch = fgetc(sourceFile)));	// state 1: skip blanks
		if (superLetter(ch) && ch != '.') { // identifier or keyword
			i = 0;
			do {
				if (i < ID_LENGTH) id[i++] = ch;
				ch = fgetc(sourceFile);
			} while (superLetterOrDigit(ch));
			if (i >= ID_LENGTH) lexicalError(1);
			id[i] = '\0';
			ungetc(ch, sourceFile);  //  retract
									 // find the identifier in the keyword table
			for (index = 0; index < NO_KEYWORD; index++)
				if (!strcmp(id, keyword[index])) break;
			if (index < NO_KEYWORD)    // found, keyword exit
				token.number = tnum[index];
			else {                     // not found, identifier exit
				token.number = tident;
				strcpy_s(token.value.id, id);
			}
		}  // end of identifier or keyword
		else if (isdigit(ch)) {  // number or floating number
			int i;

			//소수인지 확인하기 위한 for문
			//숫자에 '.' 이 나오면 소수, 소수인 경우 fBool 변수는 TRUE로 초기화
			//정수는 fBool 변수 FALSE로 초기화
			for (i = 10; i > 0; i--)
			{
				if (ch == '.')
				{
					fBool = TRUE;
					break;
				}
				else if (ch == ';')
					break;

				ch = fgetc(sourceFile);
				k++;
			}
			k++;
		
			//소수 판단을 위해서 이동한 파일 포인터를 원상태로 복원
			fseek(sourceFile, -k, 1);

			if (fBool == FALSE) //정수
			{
				token.number = tnumber;
				token.value.num = getNumber(ch);
			}
			else //소수
			{
				token.number = tFnumber;
				token.fNumber = getFnumber();
			}
			
		}
		else switch (ch) {  // special character
		case '/': // 주석 처리 부분
			ch = fgetc(sourceFile);
			if (ch == '*') {		
				ch = fgetc(sourceFile);
				if (ch == '*')
				{
					ch = fgetc(sourceFile);
					do {//문서화 주석
						while (ch != '*')
						{
							commentArr[j++] = ch;
							ch = fgetc(sourceFile);
						}
						commentArr[j] = '\0';
						ch = fgetc(sourceFile);
					} while (ch != '/');
					printf("\n문서화 주석 : %s\n", commentArr); //문서화 주석 출력
				}
				else {
					do {
						while (ch != '*') ch = fgetc(sourceFile);
						ch = fgetc(sourceFile);
					} while (ch != '/');
				}
			}
			else if (ch == '/')		// line comment
				while (fgetc(sourceFile) != '\n');
			else if (ch == '=')  token.number = tdivAssign;
			else {
				token.number = tdiv;
				ungetc(ch, sourceFile); // retract
			}
			break;
		case '!':
			ch = fgetc(sourceFile);
			if (ch == '=')  token.number = tnotequ;
			else {
				token.number = tnot;
				ungetc(ch, sourceFile); // retract
			}
			break;
		case '%':
			ch = fgetc(sourceFile);
			if (ch == '=') {
				token.number = tremAssign;
			}
			else {
				token.number = tremainder;
				ungetc(ch, sourceFile);
			}
			break;
		case '&':
			ch = fgetc(sourceFile);
			if (ch == '&')  token.number = tand;
			else {
				lexicalError(2);
				ungetc(ch, sourceFile);  // retract
			}
			break;
		case '*':
			ch = fgetc(sourceFile);
			if (ch == '=')  token.number = tmulAssign;
			else {
				token.number = tmul;
				ungetc(ch, sourceFile);  // retract
			}
			break;
		case '+':
			ch = fgetc(sourceFile);
			if (ch == '+')  token.number = tinc;
			else if (ch == '=') token.number = taddAssign;
			else {
				token.number = tplus;
				ungetc(ch, sourceFile);  // retract
			}
			break;
		case '-':
			ch = fgetc(sourceFile);
			if (ch == '-')  token.number = tdec;
			else if (ch == '=') token.number = tsubAssign;
			else {
				token.number = tminus;
				ungetc(ch, sourceFile);  // retract
			}
			break;
		case '<':
			ch = fgetc(sourceFile);
			if (ch == '=') token.number = tlesse;
			else {
				token.number = tless;
				ungetc(ch, sourceFile);  // retract
			}
			break;
		case '=':
			ch = fgetc(sourceFile);
			if (ch == '=')  token.number = tequal;
			else {
				token.number = tassign;
				ungetc(ch, sourceFile);  // retract
			}
			break;
		case '>':
			ch = fgetc(sourceFile);
			if (ch == '=') token.number = tgreate;
			else {
				token.number = tgreat;
				ungetc(ch, sourceFile);  // retract
			}
			break;
		case '|':
			ch = fgetc(sourceFile);
			if (ch == '|')  token.number = tor;
			else {
				lexicalError(3);
				ungetc(ch, sourceFile);  // retract
			}
			break;
		case'\\': //이스케이프 시퀀스 인식
			ch = fgetc(sourceFile);
			if (ch == 'a') token.number = tescapeA;
			else if (ch == 'b') token.number = tescapeB;
			else if (ch == 'f') token.number = tescapeF;
			else if (ch == 'n') token.number = tescapeN;
			else if (ch == 'r') token.number = tescapeR;
			else if (ch == 't') token.number = tescapeT;
			else if (ch == 'v') token.number = tescapeV;
			else if (ch == '\\') token.number = tescapeBack;
			else if (ch == '\'')token.number = tescapeSingle;
			else if (ch == '\"')token.number = tescapeDouble;
			else if (ch == '\?')token.number = tescapeQuestion;
			else if (ch == 'e') token.number = tescapeE;
			break;
		case '(': token.number = tlparen;         break;
		case ')': token.number = trparen;         break;
		case ',': token.number = tcomma;          break;
		case ';': token.number = tsemicolon;      break;
		case ':': token.number = tdelimiter;      break; // : 구분자 추가
		case '\"': token.number = tdoublequote;   break; // : double quote 추가
		case '[': token.number = tlbracket;       break;
		case ']': token.number = trbracket;       break;
		case '{': token.number = tlbrace;         break;
		case '}': token.number = trbrace;         break;
		case EOF: token.number = teof;            break;
		default: {
			printf("Current character : %c", ch);
			lexicalError(4);
			break;
		}

		} // switch end
	} while (token.number == tnull);

	return token;
} // end of scanner

void lexicalError(int n)
{
	printf(" *** Lexical Error : ");
	switch (n) {
	case 1: printf("an identifier length must be less than 12.\n");
		break;
	case 2: printf("next character must be &\n");
		break;
	case 3: printf("next character must be |\n");
		break;
	case 4: printf("invalid character\n");
		break;
	}
}

int superLetter(char ch)
{
	if (isalpha(ch) || ch == '_') return 1;
	else return 0;
}

int superLetterOrDigit(char ch)
{
	if (isalnum(ch) || ch == '_') return 1;
	else return 0;
}

int getNumber(char firstCharacter)
{
	int num = 0;
	int value;
	char ch;

	if (firstCharacter == '0') {
		ch = fgetc(sourceFile);
		if ((ch == 'X') || (ch == 'x')) {		// hexa decimal
			while ((value = hexValue(ch = fgetc(sourceFile))) != -1)
				num = 16 * num + value;
		}
		else if ((ch >= '0') && (ch <= '7'))	// octal
			do {
				num = 8 * num + (int)(ch - '0');
				ch = fgetc(sourceFile);
			} while ((ch >= '0') && (ch <= '7'));
		else num = 0;						// zero
	}
	else {									// decimal
		ch = fgetc(sourceFile);
		do {
			num = 10 * num + (int)(ch - '0');
			ch = fgetc(sourceFile);
		} while (isdigit(ch));
	}
	ungetc(ch, sourceFile);  /*  retract  */
	return num;
}

double getFnumber()
{
	char ch;
	char tFnumArr[20];
	int i = 0;

	memset(tFnumArr, 0, sizeof(tFnumArr));

	ch = fgetc(sourceFile);
	while (1) //소수 인식 
	{
		if (ch == ';' || ch == ',')
		{
			ungetc(ch, sourceFile);
			break;
		}
		tFnumArr[i++] = ch;
		ch = fgetc(sourceFile);
	}
	
	return atof(tFnumArr);//문자열을 실수로 변환해서 반환
}

int hexValue(char ch)
{
	switch (ch) {
	case '0': case '1': case '2': case '3': case '4':
	case '5': case '6': case '7': case '8': case '9':
		return (ch - '0');
	case 'A': case 'B': case 'C': case 'D': case 'E': case 'F':
		return (ch - 'A' + 10);
	case 'a': case 'b': case 'c': case 'd': case 'e': case 'f':
		return (ch - 'a' + 10);
	default: return -1;
	}
}

void printToken(struct tokenType token)
{
	if (token.number == tident)
		printf("number: %d, value: %s\n", token.number, token.value.id);
	else if (token.number == tnumber)
		printf("number: %d, value: %d\n", token.number, token.value.num);
	else if (token.number == tFnumber)
		printf("number: %d, value: %f, value: %e\n", token.number, token.fNumber, token.fNumber);
	else
		printf("number: %d(%s)\n", token.number, tokenName[token.number]);

}