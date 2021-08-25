/*
 *  The scanner definition for COOL.
 */

/*
 *  Stuff enclosed in %{ %} in the first section is copied verbatim to the
 *  output, so headers and global definitions are placed here to be visible
 * to the code in the file.  Don't remove anything that was here initially
 */
%{
#include <cool-parse.h>
#include <stringtab.h>
#include <utilities.h>

/* The compiler assumes these identifiers. */
#define yylval cool_yylval
#define yylex  cool_yylex

/* Max size of string constants */
#define MAX_STR_CONST 1025
#define YY_NO_UNPUT   /* keep g++ happy */

extern FILE *fin; /* we read from this file */

/* define YY_INPUT so we read from the FILE fin:
 * This change makes it possible to use this scanner in
 * the Cool compiler.
 */
#undef YY_INPUT
#define YY_INPUT(buf,result,max_size) \
	if ( (result = fread( (char*)buf, sizeof(char), max_size, fin)) < 0) \
		YY_FATAL_ERROR( "read() in flex scanner failed");

char string_buf[MAX_STR_CONST]; /* to assemble string constants */
char *string_buf_ptr;

extern int curr_lineno;
extern int verbose_flag;

extern YYSTYPE cool_yylval;

/*
 *  Add Your own definitions here
 */
int commentha=0;
%}

%x string
%x comment
%x escape

/*
 * Define names for regular expressions here.
 */

LETTER      [a-zA-Z_]
DIGIT       [0-9]
NEWLINE     (\r\n|\n)+
WHITESPACE  [ \f\r\t\v]+
DASHCOMMENT --.*\n
TYPEID      [A-Z]({DIGIT}|{LETTER})*
OBJECTID    [a-z]({DIGIT}|{LETTER})*
INT_CONST   {DIGIT}+

CLASS       (?i:class)
ELSE        (?i:else)
FI          (?i:fi)
IF          (?i:if)
IN          (?i:in)
INHERITS    (?i:inherits)
LET         (?i:let)
LOOP        (?i:loop)
POOL        (?i:pool)
THEN        (?i:then)
WHILE       (?i:while)
CASE        (?i:case)
ESAC        (?i:esac)
NEW         (?i:new)
ISVOID      (?i:isvoid)
OF          (?i:of)
NOT         (?i:not)
TRUE        (t)(?i:rue)
FALSE       (f)(?i:alse)

%%

 /*
  *  Nested comments
  */

"*)" {
	yylval.error_msg ="Unmatched *)";
	return ERROR;
}

"(*" {
	BEGIN(comment);
	commentha++;
}
<comment>"(*" {
	commentha++;
}
<comment>\n { 
	curr_lineno++;
}
<comment>. {}
<comment>"*)" {
	commentha--;
	if(commentha==0)
		BEGIN(INITIAL);
	else if(commentha<0){
		yylval.error_msg = "Unmatched *)";
		commentha=0;
		BEGIN(INITIAL);
		return ERROR;
	}
}
<comment><<EOF>> {
	BEGIN(INITIAL);
	if(commentha>0){
		commentha=0;
		yylval.error_msg ="EOF in comment";
		return ERROR;
	}
}
{DASHCOMMENT} {
	curr_lineno++;
}


 /*
  *  The multiple-character operators.
  */
"=>"        { return DARROW;}
"=<"        { return LE;}
"<-"        { return ASSIGN;}
"<"         { return '<';}
"@"         { return '@';}
"~"         { return '~';}
"="         { return '=';}
"."         { return '.';}
"-"         { return '-';}
","         { return ',';}
"+"         { return '+';}
"*"         { return '*';}
"/"         { return '/';}
"}"         { return '}';}
"{"         { return '{';}
"("         { return '(';}
")"         { return ')';}
":"         { return ':';}
";"         { return ';';}
{CLASS}     { return CLASS;}
{ELSE}      { return ELSE;}
{FI}        { return FI;}
{IF}        { return IF;}
{IN}        { return IN;}
{INHERITS}  { return INHERITS;}    
{LET}       { return LET;} 
{LOOP}      { return LOOP;}    
{POOL}      { return POOL;}
{THEN}      { return THEN;}
{WHILE}     { return WHILE;}
{CASE}      { return CASE;}
{ESAC}      { return ESAC;}
{NEW}       { return NEW;}
{OF}        { return OF;}
{NOT}       { return NOT;}

{OBJECTID}|(self) { 
    yylval.symbol = idtable.add_string(yytext); 
    return OBJECTID; 
}
{TYPEID} { 
    yylval.symbol = idtable.add_string(yytext); 
    return TYPEID; 
}
{INT_CONST} {
	yylval.symbol = inttable.add_string(yytext);
	return INT_CONST;
}
\n { 
	curr_lineno++; 
}
{WHITESPACE} {}

 /*
  * Keywords are case-insensitive except for the values true and false,
  * which must begin with a lower-case letter.
  */
{FALSE} {
    yylval.boolean = false;
    return BOOL_CONST;
}
{TRUE} {
    yylval.boolean = true;
    return BOOL_CONST;
}

 /*
  *  String constants (C syntax)
  *  Escape sequence \c is accepted for all characters c. Except for 
  *  \n \t \b \f, the result is c.
  *
  */
"\"" {
	BEGIN(string);
	string_buf_ptr = string_buf;
}
<string>"\"" {
	if (string_buf_ptr - string_buf > MAX_STR_CONST-1) {
		*string_buf = '\0';
		yylval.error_msg = "String constant too long";
		BEGIN(escape);
		return ERROR;
	}
	BEGIN(INITIAL);
	*string_buf_ptr = '\0';
	yylval.symbol = stringtable.add_string(string_buf);
	return STR_CONST;
}
<string><<EOF>> {
	yylval.error_msg = "EOF in string constant";
	BEGIN(INITIAL);
	return ERROR;
}
<string>\n {
	*string_buf = '\0';
	curr_lineno++; 
	yylval.error_msg = "Unterminated string constant";
	BEGIN(INITIAL);
	return ERROR;
}
<string>\0 {
	BEGIN(escape);
	*string_buf = '\0';
	yylval.error_msg = "String contains null character";
	return ERROR;
}
<string>"\\n"		*string_buf_ptr++ = '\n';
<string>"\\t"		*string_buf_ptr++ = '\t';
<string>"\\b"		*string_buf_ptr++ = '\b';
<string>"\\f"		*string_buf_ptr++ = '\f';
<string>"\\"[^ntbf]	*string_buf_ptr++ = yytext[1];
<string>.		*string_buf_ptr++ = *yytext;

<escape>[\n|"]	BEGIN(INITIAL);
<escape>[^\n|"]

. {
	yylval.error_msg = yytext;
	return ERROR;
}

%%
