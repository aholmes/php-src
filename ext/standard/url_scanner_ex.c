/* Generated by re2c 0.9.2 on Sat Mar 13 15:56:45 2004 */
#line 1 "/usr/src/php5/ext/standard/url_scanner_ex.re"
/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2004 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.0 of the PHP license,       |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_0.txt.                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author: Sascha Schumann <sascha@schumann.cx>                         |
  +----------------------------------------------------------------------+
*/

#include "php.h"

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef HAVE_LIMITS_H
#include <limits.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "php_ini.h"
#include "php_globals.h"
#define STATE_TAG SOME_OTHER_STATE_TAG
#include "basic_functions.h"
#include "url.h"
#undef STATE_TAG

#define url_scanner url_scanner_ex

#include "php_smart_str.h"

static PHP_INI_MH(OnUpdateTags)
{
	url_adapt_state_ex_t *ctx;
	char *key;
	char *lasts;
	char *tmp;
	
	ctx = &BG(url_adapt_state_ex);
	
	tmp = estrndup(new_value, new_value_length);
	
	if (ctx->tags)
		zend_hash_destroy(ctx->tags);
	else
		ctx->tags = malloc(sizeof(HashTable));
	
	zend_hash_init(ctx->tags, 0, NULL, NULL, 1);
	
	for (key = php_strtok_r(tmp, ",", &lasts);
			key;
			key = php_strtok_r(NULL, ",", &lasts)) {
		char *val;

		val = strchr(key, '=');
		if (val) {
			char *q;
			int keylen;
			
			*val++ = '\0';
			for (q = key; *q; q++)
				*q = tolower(*q);
			keylen = q - key;
			/* key is stored withOUT NUL
			   val is stored WITH    NUL */
			zend_hash_add(ctx->tags, key, keylen, val, strlen(val)+1, NULL);
		}
	}

	efree(tmp);

	return SUCCESS;
}

PHP_INI_BEGIN()
	STD_PHP_INI_ENTRY("url_rewriter.tags", "a=href,area=href,frame=src,form=,fieldset=", PHP_INI_ALL, OnUpdateTags, url_adapt_state_ex, php_basic_globals, basic_globals)
PHP_INI_END()

#line 95 "/usr/src/php5/ext/standard/url_scanner_ex.re"


#define YYFILL(n) goto done
#define YYCTYPE unsigned char
#define YYCURSOR p
#define YYLIMIT q
#define YYMARKER r
	
static inline void append_modified_url(smart_str *url, smart_str *dest, smart_str *url_app, const char *separator)
{
	register const char *p, *q;
	const char *bash = NULL;
	const char *sep = "?";
	
	q = (p = url->c) + url->len;

scan:

#line 7 "re2c-output.c"
{
	YYCTYPE yych;
	unsigned int yyaccept;
	static unsigned char yybm[] = {
	128, 128, 128, 128, 128, 128, 128, 128, 
	128, 128, 128, 128, 128, 128, 128, 128, 
	128, 128, 128, 128, 128, 128, 128, 128, 
	128, 128, 128, 128, 128, 128, 128, 128, 
	128, 128, 128,   0, 128, 128, 128, 128, 
	128, 128, 128, 128, 128, 128, 128, 128, 
	128, 128, 128, 128, 128, 128, 128, 128, 
	128, 128,   0, 128, 128, 128, 128,   0, 
	128, 128, 128, 128, 128, 128, 128, 128, 
	128, 128, 128, 128, 128, 128, 128, 128, 
	128, 128, 128, 128, 128, 128, 128, 128, 
	128, 128, 128, 128, 128, 128, 128, 128, 
	128, 128, 128, 128, 128, 128, 128, 128, 
	128, 128, 128, 128, 128, 128, 128, 128, 
	128, 128, 128, 128, 128, 128, 128, 128, 
	128, 128, 128, 128, 128, 128, 128, 128, 
	128, 128, 128, 128, 128, 128, 128, 128, 
	128, 128, 128, 128, 128, 128, 128, 128, 
	128, 128, 128, 128, 128, 128, 128, 128, 
	128, 128, 128, 128, 128, 128, 128, 128, 
	128, 128, 128, 128, 128, 128, 128, 128, 
	128, 128, 128, 128, 128, 128, 128, 128, 
	128, 128, 128, 128, 128, 128, 128, 128, 
	128, 128, 128, 128, 128, 128, 128, 128, 
	128, 128, 128, 128, 128, 128, 128, 128, 
	128, 128, 128, 128, 128, 128, 128, 128, 
	128, 128, 128, 128, 128, 128, 128, 128, 
	128, 128, 128, 128, 128, 128, 128, 128, 
	128, 128, 128, 128, 128, 128, 128, 128, 
	128, 128, 128, 128, 128, 128, 128, 128, 
	128, 128, 128, 128, 128, 128, 128, 128, 
	128, 128, 128, 128, 128, 128, 128, 128, 
	};
	goto yy0;
yy1:	++YYCURSOR;
yy0:
	if(YYLIMIT == YYCURSOR) YYFILL(1);
	yych = *YYCURSOR;
	if(yybm[0+yych] & 128)	goto yy8;
	if(yych <= '9')	goto yy6;
	if(yych >= ';')	goto yy4;
	goto yy2;
yy2:	yych = *++YYCURSOR;
	goto yy3;
yy3:
#line 113 "/usr/src/php5/ext/standard/url_scanner_ex.re"
{ smart_str_append(dest, url); return; }
#line 59 "re2c-output.c"
yy4:	yych = *++YYCURSOR;
	goto yy5;
yy5:
#line 114 "/usr/src/php5/ext/standard/url_scanner_ex.re"
{ sep = separator; goto scan; }
#line 65 "re2c-output.c"
yy6:	yych = *++YYCURSOR;
	goto yy7;
yy7:
#line 115 "/usr/src/php5/ext/standard/url_scanner_ex.re"
{ bash = p - 1; goto done; }
#line 71 "re2c-output.c"
yy8:	++YYCURSOR;
	if(YYLIMIT == YYCURSOR) YYFILL(1);
	yych = *YYCURSOR;
	goto yy9;
yy9:	if(yybm[0+yych] & 128)	goto yy8;
	goto yy10;
yy10:
#line 116 "/usr/src/php5/ext/standard/url_scanner_ex.re"
{ goto scan; }
#line 81 "re2c-output.c"
}
#line 117 "/usr/src/php5/ext/standard/url_scanner_ex.re"

done:
	
	/* Don't modify URLs of the format "#mark" */
	if (bash && bash - url->c == 0) {
		smart_str_append(dest, url);
		return;
	}

	if (bash)
		smart_str_appendl(dest, url->c, bash - url->c);
	else
		smart_str_append(dest, url);

	smart_str_appends(dest, sep);
	smart_str_append(dest, url_app);

	if (bash)
		smart_str_appendl(dest, bash, q - bash);
}


#undef YYFILL
#undef YYCTYPE
#undef YYCURSOR
#undef YYLIMIT
#undef YYMARKER

static inline void tag_arg(url_adapt_state_ex_t *ctx, char quotes, char type TSRMLS_DC)
{
	char f = 0;

	if (strncasecmp(ctx->arg.c, ctx->lookup_data, ctx->arg.len) == 0)
		f = 1;

	if (quotes)
		smart_str_appendc(&ctx->result, type);
	if (f) {
		append_modified_url(&ctx->val, &ctx->result, &ctx->url_app, PG(arg_separator).output);
	} else {
		smart_str_append(&ctx->result, &ctx->val);
	}
	if (quotes)
		smart_str_appendc(&ctx->result, type);
}

enum {
	STATE_PLAIN = 0,
	STATE_TAG,
	STATE_NEXT_ARG,
	STATE_ARG,
	STATE_BEFORE_VAL,
	STATE_VAL
};

#define YYFILL(n) goto stop
#define YYCTYPE unsigned char
#define YYCURSOR xp
#define YYLIMIT end
#define YYMARKER q
#define STATE ctx->state

#define STD_PARA url_adapt_state_ex_t *ctx, char *start, char *YYCURSOR TSRMLS_DC
#define STD_ARGS ctx, start, xp TSRMLS_CC

#if SCANNER_DEBUG
#define scdebug(x) printf x
#else
#define scdebug(x)
#endif

static inline void passthru(STD_PARA) 
{
	scdebug(("appending %d chars, starting with %c\n", YYCURSOR-start, *start));
	smart_str_appendl(&ctx->result, start, YYCURSOR - start);
}

/*
 * This function appends a hidden input field after a <form> or
 * <fieldset>.  The latter is important for XHTML.
 */

static void handle_form(STD_PARA) 
{
	int doit = 0;

	if (ctx->form_app.len > 0) {
		switch (ctx->tag.len) {

#define RECOGNIZE(x) do { 	\
	case sizeof(x)-1: \
		if (strncasecmp(ctx->tag.c, x, sizeof(x)-1) == 0) \
			doit = 1; \
		break; \
} while (0)
		
			RECOGNIZE("form");
			RECOGNIZE("fieldset");
		}

		if (doit)
			smart_str_append(&ctx->result, &ctx->form_app);
	}
}



/*
 *  HANDLE_TAG copies the HTML Tag and checks whether we 
 *  have that tag in our table. If we might modify it,
 *  we continue to scan the tag, otherwise we simply copy the complete
 *  HTML stuff to the result buffer.
 */

static inline void handle_tag(STD_PARA) 
{
	int ok = 0;
	int i;

	ctx->tag.len = 0;
	smart_str_appendl(&ctx->tag, start, YYCURSOR - start);
	for (i = 0; i < ctx->tag.len; i++)
		ctx->tag.c[i] = tolower((int)(unsigned char)ctx->tag.c[i]);
	if (zend_hash_find(ctx->tags, ctx->tag.c, ctx->tag.len, (void **) &ctx->lookup_data) == SUCCESS)
		ok = 1;
	STATE = ok ? STATE_NEXT_ARG : STATE_PLAIN;
}

static inline void handle_arg(STD_PARA) 
{
	ctx->arg.len = 0;
	smart_str_appendl(&ctx->arg, start, YYCURSOR - start);
}

static inline void handle_val(STD_PARA, char quotes, char type) 
{
	smart_str_setl(&ctx->val, start + quotes, YYCURSOR - start - quotes * 2);
	tag_arg(ctx, quotes, type TSRMLS_CC);
}

static inline void xx_mainloop(url_adapt_state_ex_t *ctx, const char *newdata, size_t newlen TSRMLS_DC)
{
	char *end, *q;
	char *xp;
	char *start;
	int rest;

	smart_str_appendl(&ctx->buf, newdata, newlen);
	
	YYCURSOR = ctx->buf.c;
	YYLIMIT = ctx->buf.c + ctx->buf.len;

	switch (STATE) {
		case STATE_PLAIN: goto state_plain;
		case STATE_TAG: goto state_tag;
		case STATE_NEXT_ARG: goto state_next_arg;
		case STATE_ARG: goto state_arg;
		case STATE_BEFORE_VAL: goto state_before_val;
		case STATE_VAL: goto state_val;
	}
	

state_plain_begin:
	STATE = STATE_PLAIN;
	
state_plain:
	start = YYCURSOR;

#line 85 "re2c-output.c"
{
	YYCTYPE yych;
	unsigned int yyaccept;
	static unsigned char yybm[] = {
	128, 128, 128, 128, 128, 128, 128, 128, 
	128, 128, 128, 128, 128, 128, 128, 128, 
	128, 128, 128, 128, 128, 128, 128, 128, 
	128, 128, 128, 128, 128, 128, 128, 128, 
	128, 128, 128, 128, 128, 128, 128, 128, 
	128, 128, 128, 128, 128, 128, 128, 128, 
	128, 128, 128, 128, 128, 128, 128, 128, 
	128, 128, 128, 128,   0, 128, 128, 128, 
	128, 128, 128, 128, 128, 128, 128, 128, 
	128, 128, 128, 128, 128, 128, 128, 128, 
	128, 128, 128, 128, 128, 128, 128, 128, 
	128, 128, 128, 128, 128, 128, 128, 128, 
	128, 128, 128, 128, 128, 128, 128, 128, 
	128, 128, 128, 128, 128, 128, 128, 128, 
	128, 128, 128, 128, 128, 128, 128, 128, 
	128, 128, 128, 128, 128, 128, 128, 128, 
	128, 128, 128, 128, 128, 128, 128, 128, 
	128, 128, 128, 128, 128, 128, 128, 128, 
	128, 128, 128, 128, 128, 128, 128, 128, 
	128, 128, 128, 128, 128, 128, 128, 128, 
	128, 128, 128, 128, 128, 128, 128, 128, 
	128, 128, 128, 128, 128, 128, 128, 128, 
	128, 128, 128, 128, 128, 128, 128, 128, 
	128, 128, 128, 128, 128, 128, 128, 128, 
	128, 128, 128, 128, 128, 128, 128, 128, 
	128, 128, 128, 128, 128, 128, 128, 128, 
	128, 128, 128, 128, 128, 128, 128, 128, 
	128, 128, 128, 128, 128, 128, 128, 128, 
	128, 128, 128, 128, 128, 128, 128, 128, 
	128, 128, 128, 128, 128, 128, 128, 128, 
	128, 128, 128, 128, 128, 128, 128, 128, 
	128, 128, 128, 128, 128, 128, 128, 128, 
	};
	goto yy11;
yy12:	++YYCURSOR;
yy11:
	if(YYLIMIT == YYCURSOR) YYFILL(1);
	yych = *YYCURSOR;
	if(yybm[0+yych] & 128)	goto yy15;
	goto yy13;
yy13:	yych = *++YYCURSOR;
	goto yy14;
yy14:
#line 285 "/usr/src/php5/ext/standard/url_scanner_ex.re"
{ passthru(STD_ARGS); STATE = STATE_TAG; goto state_tag; }
#line 135 "re2c-output.c"
yy15:	++YYCURSOR;
	if(YYLIMIT == YYCURSOR) YYFILL(1);
	yych = *YYCURSOR;
	goto yy16;
yy16:	if(yybm[0+yych] & 128)	goto yy15;
	goto yy17;
yy17:
#line 286 "/usr/src/php5/ext/standard/url_scanner_ex.re"
{ passthru(STD_ARGS); goto state_plain; }
#line 145 "re2c-output.c"
}
#line 287 "/usr/src/php5/ext/standard/url_scanner_ex.re"


state_tag:	
	start = YYCURSOR;

#line 149 "re2c-output.c"
{
	YYCTYPE yych;
	unsigned int yyaccept;
	static unsigned char yybm[] = {
	  0,   0,   0,   0,   0,   0,   0,   0, 
	  0,   0,   0,   0,   0,   0,   0,   0, 
	  0,   0,   0,   0,   0,   0,   0,   0, 
	  0,   0,   0,   0,   0,   0,   0,   0, 
	  0,   0,   0,   0,   0,   0,   0,   0, 
	  0,   0,   0,   0,   0,   0,   0,   0, 
	  0,   0,   0,   0,   0,   0,   0,   0, 
	  0,   0,   0,   0,   0,   0,   0,   0, 
	  0, 128, 128, 128, 128, 128, 128, 128, 
	128, 128, 128, 128, 128, 128, 128, 128, 
	128, 128, 128, 128, 128, 128, 128, 128, 
	128, 128, 128,   0,   0,   0,   0,   0, 
	  0, 128, 128, 128, 128, 128, 128, 128, 
	128, 128, 128, 128, 128, 128, 128, 128, 
	128, 128, 128, 128, 128, 128, 128, 128, 
	128, 128, 128,   0,   0,   0,   0,   0, 
	  0,   0,   0,   0,   0,   0,   0,   0, 
	  0,   0,   0,   0,   0,   0,   0,   0, 
	  0,   0,   0,   0,   0,   0,   0,   0, 
	  0,   0,   0,   0,   0,   0,   0,   0, 
	  0,   0,   0,   0,   0,   0,   0,   0, 
	  0,   0,   0,   0,   0,   0,   0,   0, 
	  0,   0,   0,   0,   0,   0,   0,   0, 
	  0,   0,   0,   0,   0,   0,   0,   0, 
	  0,   0,   0,   0,   0,   0,   0,   0, 
	  0,   0,   0,   0,   0,   0,   0,   0, 
	  0,   0,   0,   0,   0,   0,   0,   0, 
	  0,   0,   0,   0,   0,   0,   0,   0, 
	  0,   0,   0,   0,   0,   0,   0,   0, 
	  0,   0,   0,   0,   0,   0,   0,   0, 
	  0,   0,   0,   0,   0,   0,   0,   0, 
	  0,   0,   0,   0,   0,   0,   0,   0, 
	};
	goto yy18;
yy19:	++YYCURSOR;
yy18:
	if((YYLIMIT - YYCURSOR) < 2) YYFILL(2);
	yych = *YYCURSOR;
	if(yych <= '@')	goto yy22;
	if(yych <= 'Z')	goto yy20;
	if(yych <= '`')	goto yy22;
	if(yych >= '{')	goto yy22;
	goto yy20;
yy20:	yych = *++YYCURSOR;
	goto yy25;
yy21:
#line 292 "/usr/src/php5/ext/standard/url_scanner_ex.re"
{ handle_tag(STD_ARGS); /* Sets STATE */; passthru(STD_ARGS); if (STATE == STATE_PLAIN) goto state_plain; else goto state_next_arg; }
#line 202 "re2c-output.c"
yy22:	yych = *++YYCURSOR;
	goto yy23;
yy23:
#line 293 "/usr/src/php5/ext/standard/url_scanner_ex.re"
{ passthru(STD_ARGS); goto state_plain_begin; }
#line 208 "re2c-output.c"
yy24:	++YYCURSOR;
	if(YYLIMIT == YYCURSOR) YYFILL(1);
	yych = *YYCURSOR;
	goto yy25;
yy25:	if(yybm[0+yych] & 128)	goto yy24;
	goto yy21;
}
#line 294 "/usr/src/php5/ext/standard/url_scanner_ex.re"


state_next_arg_begin:
	STATE = STATE_NEXT_ARG;
	
state_next_arg:
	start = YYCURSOR;

#line 218 "re2c-output.c"
{
	YYCTYPE yych;
	unsigned int yyaccept;
	static unsigned char yybm[] = {
	  0,   0,   0,   0,   0,   0,   0,   0, 
	  0, 128, 128, 128,   0,   0,   0,   0, 
	  0,   0,   0,   0,   0,   0,   0,   0, 
	  0,   0,   0,   0,   0,   0,   0,   0, 
	128,   0,   0,   0,   0,   0,   0,   0, 
	  0,   0,   0,   0,   0,   0,   0,   0, 
	  0,   0,   0,   0,   0,   0,   0,   0, 
	  0,   0,   0,   0,   0,   0,   0,   0, 
	  0,   0,   0,   0,   0,   0,   0,   0, 
	  0,   0,   0,   0,   0,   0,   0,   0, 
	  0,   0,   0,   0,   0,   0,   0,   0, 
	  0,   0,   0,   0,   0,   0,   0,   0, 
	  0,   0,   0,   0,   0,   0,   0,   0, 
	  0,   0,   0,   0,   0,   0,   0,   0, 
	  0,   0,   0,   0,   0,   0,   0,   0, 
	  0,   0,   0,   0,   0,   0,   0,   0, 
	  0,   0,   0,   0,   0,   0,   0,   0, 
	  0,   0,   0,   0,   0,   0,   0,   0, 
	  0,   0,   0,   0,   0,   0,   0,   0, 
	  0,   0,   0,   0,   0,   0,   0,   0, 
	  0,   0,   0,   0,   0,   0,   0,   0, 
	  0,   0,   0,   0,   0,   0,   0,   0, 
	  0,   0,   0,   0,   0,   0,   0,   0, 
	  0,   0,   0,   0,   0,   0,   0,   0, 
	  0,   0,   0,   0,   0,   0,   0,   0, 
	  0,   0,   0,   0,   0,   0,   0,   0, 
	  0,   0,   0,   0,   0,   0,   0,   0, 
	  0,   0,   0,   0,   0,   0,   0,   0, 
	  0,   0,   0,   0,   0,   0,   0,   0, 
	  0,   0,   0,   0,   0,   0,   0,   0, 
	  0,   0,   0,   0,   0,   0,   0,   0, 
	  0,   0,   0,   0,   0,   0,   0,   0, 
	};
	goto yy26;
yy27:	++YYCURSOR;
yy26:
	if((YYLIMIT - YYCURSOR) < 2) YYFILL(2);
	yych = *YYCURSOR;
	if(yych <= '='){
		if(yych <= '\v'){
			if(yych <= '\b')	goto yy34;
			goto yy30;
		} else {
			if(yych == ' ')	goto yy30;
			goto yy34;
		}
	} else {
		if(yych <= 'Z'){
			if(yych <= '>')	goto yy28;
			if(yych <= '@')	goto yy34;
			goto yy32;
		} else {
			if(yych <= '`')	goto yy34;
			if(yych <= 'z')	goto yy32;
			goto yy34;
		}
	}
yy28:	yych = *++YYCURSOR;
	goto yy29;
yy29:
#line 302 "/usr/src/php5/ext/standard/url_scanner_ex.re"
{ passthru(STD_ARGS); handle_form(STD_ARGS); goto state_plain_begin; }
#line 285 "re2c-output.c"
yy30:	yych = *++YYCURSOR;
	goto yy37;
yy31:
#line 303 "/usr/src/php5/ext/standard/url_scanner_ex.re"
{ passthru(STD_ARGS); goto state_next_arg; }
#line 291 "re2c-output.c"
yy32:	yych = *++YYCURSOR;
	goto yy33;
yy33:
#line 304 "/usr/src/php5/ext/standard/url_scanner_ex.re"
{ --YYCURSOR; STATE = STATE_ARG; goto state_arg; }
#line 297 "re2c-output.c"
yy34:	yych = *++YYCURSOR;
	goto yy35;
yy35:
#line 305 "/usr/src/php5/ext/standard/url_scanner_ex.re"
{ passthru(STD_ARGS); goto state_plain_begin; }
#line 303 "re2c-output.c"
yy36:	++YYCURSOR;
	if(YYLIMIT == YYCURSOR) YYFILL(1);
	yych = *YYCURSOR;
	goto yy37;
yy37:	if(yybm[0+yych] & 128)	goto yy36;
	goto yy31;
}
#line 306 "/usr/src/php5/ext/standard/url_scanner_ex.re"


state_arg:
	start = YYCURSOR;

#line 313 "re2c-output.c"
{
	YYCTYPE yych;
	unsigned int yyaccept;
	static unsigned char yybm[] = {
	  0,   0,   0,   0,   0,   0,   0,   0, 
	  0,   0,   0,   0,   0,   0,   0,   0, 
	  0,   0,   0,   0,   0,   0,   0,   0, 
	  0,   0,   0,   0,   0,   0,   0,   0, 
	  0,   0,   0,   0,   0,   0,   0,   0, 
	  0,   0,   0,   0,   0, 128,   0,   0, 
	  0,   0,   0,   0,   0,   0,   0,   0, 
	  0,   0,   0,   0,   0,   0,   0,   0, 
	  0, 128, 128, 128, 128, 128, 128, 128, 
	128, 128, 128, 128, 128, 128, 128, 128, 
	128, 128, 128, 128, 128, 128, 128, 128, 
	128, 128, 128,   0,   0,   0,   0,   0, 
	  0, 128, 128, 128, 128, 128, 128, 128, 
	128, 128, 128, 128, 128, 128, 128, 128, 
	128, 128, 128, 128, 128, 128, 128, 128, 
	128, 128, 128,   0,   0,   0,   0,   0, 
	  0,   0,   0,   0,   0,   0,   0,   0, 
	  0,   0,   0,   0,   0,   0,   0,   0, 
	  0,   0,   0,   0,   0,   0,   0,   0, 
	  0,   0,   0,   0,   0,   0,   0,   0, 
	  0,   0,   0,   0,   0,   0,   0,   0, 
	  0,   0,   0,   0,   0,   0,   0,   0, 
	  0,   0,   0,   0,   0,   0,   0,   0, 
	  0,   0,   0,   0,   0,   0,   0,   0, 
	  0,   0,   0,   0,   0,   0,   0,   0, 
	  0,   0,   0,   0,   0,   0,   0,   0, 
	  0,   0,   0,   0,   0,   0,   0,   0, 
	  0,   0,   0,   0,   0,   0,   0,   0, 
	  0,   0,   0,   0,   0,   0,   0,   0, 
	  0,   0,   0,   0,   0,   0,   0,   0, 
	  0,   0,   0,   0,   0,   0,   0,   0, 
	  0,   0,   0,   0,   0,   0,   0,   0, 
	};
	goto yy38;
yy39:	++YYCURSOR;
yy38:
	if((YYLIMIT - YYCURSOR) < 2) YYFILL(2);
	yych = *YYCURSOR;
	if(yych <= '@')	goto yy42;
	if(yych <= 'Z')	goto yy40;
	if(yych <= '`')	goto yy42;
	if(yych >= '{')	goto yy42;
	goto yy40;
yy40:	yych = *++YYCURSOR;
	goto yy45;
yy41:
#line 311 "/usr/src/php5/ext/standard/url_scanner_ex.re"
{ passthru(STD_ARGS); handle_arg(STD_ARGS); STATE = STATE_BEFORE_VAL; goto state_before_val; }
#line 366 "re2c-output.c"
yy42:	yych = *++YYCURSOR;
	goto yy43;
yy43:
#line 312 "/usr/src/php5/ext/standard/url_scanner_ex.re"
{ passthru(STD_ARGS); STATE = STATE_NEXT_ARG; goto state_next_arg; }
#line 372 "re2c-output.c"
yy44:	++YYCURSOR;
	if(YYLIMIT == YYCURSOR) YYFILL(1);
	yych = *YYCURSOR;
	goto yy45;
yy45:	if(yybm[0+yych] & 128)	goto yy44;
	goto yy41;
}
#line 313 "/usr/src/php5/ext/standard/url_scanner_ex.re"


state_before_val:
	start = YYCURSOR;

#line 382 "re2c-output.c"
{
	YYCTYPE yych;
	unsigned int yyaccept;
	static unsigned char yybm[] = {
	  0,   0,   0,   0,   0,   0,   0,   0, 
	  0,   0,   0,   0,   0,   0,   0,   0, 
	  0,   0,   0,   0,   0,   0,   0,   0, 
	  0,   0,   0,   0,   0,   0,   0,   0, 
	128,   0,   0,   0,   0,   0,   0,   0, 
	  0,   0,   0,   0,   0,   0,   0,   0, 
	  0,   0,   0,   0,   0,   0,   0,   0, 
	  0,   0,   0,   0,   0,   0,   0,   0, 
	  0,   0,   0,   0,   0,   0,   0,   0, 
	  0,   0,   0,   0,   0,   0,   0,   0, 
	  0,   0,   0,   0,   0,   0,   0,   0, 
	  0,   0,   0,   0,   0,   0,   0,   0, 
	  0,   0,   0,   0,   0,   0,   0,   0, 
	  0,   0,   0,   0,   0,   0,   0,   0, 
	  0,   0,   0,   0,   0,   0,   0,   0, 
	  0,   0,   0,   0,   0,   0,   0,   0, 
	  0,   0,   0,   0,   0,   0,   0,   0, 
	  0,   0,   0,   0,   0,   0,   0,   0, 
	  0,   0,   0,   0,   0,   0,   0,   0, 
	  0,   0,   0,   0,   0,   0,   0,   0, 
	  0,   0,   0,   0,   0,   0,   0,   0, 
	  0,   0,   0,   0,   0,   0,   0,   0, 
	  0,   0,   0,   0,   0,   0,   0,   0, 
	  0,   0,   0,   0,   0,   0,   0,   0, 
	  0,   0,   0,   0,   0,   0,   0,   0, 
	  0,   0,   0,   0,   0,   0,   0,   0, 
	  0,   0,   0,   0,   0,   0,   0,   0, 
	  0,   0,   0,   0,   0,   0,   0,   0, 
	  0,   0,   0,   0,   0,   0,   0,   0, 
	  0,   0,   0,   0,   0,   0,   0,   0, 
	  0,   0,   0,   0,   0,   0,   0,   0, 
	  0,   0,   0,   0,   0,   0,   0,   0, 
	};
	goto yy46;
yy47:	++YYCURSOR;
yy46:
	if((YYLIMIT - YYCURSOR) < 2) YYFILL(2);
	yych = *YYCURSOR;
	if(yych == ' ')	goto yy48;
	if(yych == '=')	goto yy50;
	goto yy52;
yy48:	yyaccept = 0;
	yych = *(YYMARKER = ++YYCURSOR);
	if(yych == ' ')	goto yy55;
	if(yych == '=')	goto yy53;
	goto yy49;
yy49:
#line 319 "/usr/src/php5/ext/standard/url_scanner_ex.re"
{ --YYCURSOR; goto state_next_arg_begin; }
#line 436 "re2c-output.c"
yy50:	yych = *++YYCURSOR;
	goto yy54;
yy51:
#line 318 "/usr/src/php5/ext/standard/url_scanner_ex.re"
{ passthru(STD_ARGS); STATE = STATE_VAL; goto state_val; }
#line 442 "re2c-output.c"
yy52:	yych = *++YYCURSOR;
	goto yy49;
yy53:	++YYCURSOR;
	if(YYLIMIT == YYCURSOR) YYFILL(1);
	yych = *YYCURSOR;
	goto yy54;
yy54:	if(yybm[0+yych] & 128)	goto yy53;
	goto yy51;
yy55:	++YYCURSOR;
	if(YYLIMIT == YYCURSOR) YYFILL(1);
	yych = *YYCURSOR;
	goto yy56;
yy56:	if(yych == ' ')	goto yy55;
	if(yych == '=')	goto yy53;
	goto yy57;
yy57:	YYCURSOR = YYMARKER;
	switch(yyaccept){
	case 0:	goto yy49;
	}
}
#line 320 "/usr/src/php5/ext/standard/url_scanner_ex.re"



state_val:
	start = YYCURSOR;

#line 465 "re2c-output.c"
{
	YYCTYPE yych;
	unsigned int yyaccept;
	static unsigned char yybm[] = {
	248, 248, 248, 248, 248, 248, 248, 248, 
	248, 160, 160, 248, 248, 248, 248, 248, 
	248, 248, 248, 248, 248, 248, 248, 248, 
	248, 248, 248, 248, 248, 248, 248, 248, 
	160, 248,  56, 248, 248, 248, 248, 200, 
	248, 248, 248, 248, 248, 248, 248, 248, 
	248, 248, 248, 248, 248, 248, 248, 248, 
	248, 248, 248, 248, 248, 248,   0, 248, 
	248, 248, 248, 248, 248, 248, 248, 248, 
	248, 248, 248, 248, 248, 248, 248, 248, 
	248, 248, 248, 248, 248, 248, 248, 248, 
	248, 248, 248, 248, 248, 248, 248, 248, 
	248, 248, 248, 248, 248, 248, 248, 248, 
	248, 248, 248, 248, 248, 248, 248, 248, 
	248, 248, 248, 248, 248, 248, 248, 248, 
	248, 248, 248, 248, 248, 248, 248, 248, 
	248, 248, 248, 248, 248, 248, 248, 248, 
	248, 248, 248, 248, 248, 248, 248, 248, 
	248, 248, 248, 248, 248, 248, 248, 248, 
	248, 248, 248, 248, 248, 248, 248, 248, 
	248, 248, 248, 248, 248, 248, 248, 248, 
	248, 248, 248, 248, 248, 248, 248, 248, 
	248, 248, 248, 248, 248, 248, 248, 248, 
	248, 248, 248, 248, 248, 248, 248, 248, 
	248, 248, 248, 248, 248, 248, 248, 248, 
	248, 248, 248, 248, 248, 248, 248, 248, 
	248, 248, 248, 248, 248, 248, 248, 248, 
	248, 248, 248, 248, 248, 248, 248, 248, 
	248, 248, 248, 248, 248, 248, 248, 248, 
	248, 248, 248, 248, 248, 248, 248, 248, 
	248, 248, 248, 248, 248, 248, 248, 248, 
	248, 248, 248, 248, 248, 248, 248, 248, 
	};
	goto yy58;
yy59:	++YYCURSOR;
yy58:
	if((YYLIMIT - YYCURSOR) < 3) YYFILL(3);
	yych = *YYCURSOR;
	if(yych <= '!'){
		if(yych <= '\n'){
			if(yych <= '\b')	goto yy63;
			goto yy64;
		} else {
			if(yych == ' ')	goto yy64;
			goto yy63;
		}
	} else {
		if(yych <= '\''){
			if(yych <= '"')	goto yy60;
			if(yych <= '&')	goto yy63;
			goto yy62;
		} else {
			if(yych == '>')	goto yy64;
			goto yy63;
		}
	}
yy60:	yyaccept = 0;
	yych = *(YYMARKER = ++YYCURSOR);
	goto yy77;
yy61:
#line 328 "/usr/src/php5/ext/standard/url_scanner_ex.re"
{ handle_val(STD_ARGS, 0, '\0'); goto state_next_arg_begin; }
#line 532 "re2c-output.c"
yy62:	yyaccept = 0;
	yych = *(YYMARKER = ++YYCURSOR);
	goto yy69;
yy63:	yych = *++YYCURSOR;
	goto yy67;
yy64:	yych = *++YYCURSOR;
	goto yy65;
yy65:
#line 329 "/usr/src/php5/ext/standard/url_scanner_ex.re"
{ passthru(STD_ARGS); goto state_next_arg_begin; }
#line 543 "re2c-output.c"
yy66:	++YYCURSOR;
	if(YYLIMIT == YYCURSOR) YYFILL(1);
	yych = *YYCURSOR;
	goto yy67;
yy67:	if(yybm[0+yych] & 8)	goto yy66;
	goto yy61;
yy68:	yyaccept = 0;
	YYMARKER = ++YYCURSOR;
	if(YYLIMIT == YYCURSOR) YYFILL(1);
	yych = *YYCURSOR;
	goto yy69;
yy69:	if(yybm[0+yych] & 16)	goto yy68;
	if(yych <= '&')	goto yy72;
	if(yych >= '(')	goto yy61;
	goto yy70;
yy70:	yych = *++YYCURSOR;
	if(yybm[0+yych] & 8)	goto yy66;
	goto yy71;
yy71:
#line 327 "/usr/src/php5/ext/standard/url_scanner_ex.re"
{ handle_val(STD_ARGS, 1, '\''); goto state_next_arg_begin; }
#line 565 "re2c-output.c"
yy72:	++YYCURSOR;
	if(YYLIMIT == YYCURSOR) YYFILL(1);
	yych = *YYCURSOR;
	goto yy73;
yy73:	if(yybm[0+yych] & 32)	goto yy72;
	if(yych <= '=')	goto yy75;
	goto yy74;
yy74:	YYCURSOR = YYMARKER;
	switch(yyaccept){
	case 0:	goto yy61;
	}
yy75:	yych = *++YYCURSOR;
	goto yy71;
yy76:	yyaccept = 0;
	YYMARKER = ++YYCURSOR;
	if(YYLIMIT == YYCURSOR) YYFILL(1);
	yych = *YYCURSOR;
	goto yy77;
yy77:	if(yybm[0+yych] & 64)	goto yy76;
	if(yych <= '!')	goto yy80;
	if(yych >= '#')	goto yy61;
	goto yy78;
yy78:	yych = *++YYCURSOR;
	if(yybm[0+yych] & 8)	goto yy66;
	goto yy79;
yy79:
#line 326 "/usr/src/php5/ext/standard/url_scanner_ex.re"
{ handle_val(STD_ARGS, 1, '"'); goto state_next_arg_begin; }
#line 594 "re2c-output.c"
yy80:	++YYCURSOR;
	if(YYLIMIT == YYCURSOR) YYFILL(1);
	yych = *YYCURSOR;
	goto yy81;
yy81:	if(yybm[0+yych] & 128)	goto yy80;
	if(yych >= '>')	goto yy74;
	goto yy82;
yy82:	yych = *++YYCURSOR;
	goto yy79;
}
#line 330 "/usr/src/php5/ext/standard/url_scanner_ex.re"


stop:
	rest = YYLIMIT - start;
	scdebug(("stopped in state %d at pos %d (%d:%c) %d\n", STATE, YYCURSOR - ctx->buf.c, *YYCURSOR, *YYCURSOR, rest));
	/* XXX: Crash avoidance. Need to work with reporter to figure out what goes wrong */	
	if (rest < 0) rest = 0;
	
	if (rest) memmove(ctx->buf.c, start, rest);
	ctx->buf.len = rest;
}

char *php_url_scanner_adapt_single_url(const char *url, size_t urllen, const char *name, const char *value, size_t *newlen TSRMLS_DC)
{
	smart_str surl = {0};
	smart_str buf = {0};
	smart_str url_app = {0};

	smart_str_setl(&surl, url, urllen);

	smart_str_appends(&url_app, name);
	smart_str_appendc(&url_app, '=');
	smart_str_appends(&url_app, value);

	append_modified_url(&surl, &buf, &url_app, PG(arg_separator).output);

	smart_str_0(&buf);
	if (newlen) *newlen = buf.len;

	smart_str_free(&url_app);

	return buf.c;
}


static char *url_adapt_ext(const char *src, size_t srclen, size_t *newlen, zend_bool do_flush TSRMLS_DC)
{
	url_adapt_state_ex_t *ctx;
	char *retval;

	ctx = &BG(url_adapt_state_ex);

	xx_mainloop(ctx, src, srclen TSRMLS_CC);

	*newlen = ctx->result.len;
	if (!ctx->result.c) {
		smart_str_appendl(&ctx->result, "", 0);
	}
	smart_str_0(&ctx->result);
	if (do_flush) {
		smart_str_appendl(&ctx->result, ctx->buf.c, ctx->buf.len);
		*newlen += ctx->buf.len;
		smart_str_free(&ctx->buf);
	}
	retval = ctx->result.c;
	ctx->result.c = NULL;
	ctx->result.len = 0;
	return retval;
}

int php_url_scanner_ex_activate(TSRMLS_D)
{
	url_adapt_state_ex_t *ctx;
	
	ctx = &BG(url_adapt_state_ex);

	memset(ctx, 0, ((size_t) &((url_adapt_state_ex_t *)0)->tags));

	return SUCCESS;
}

int php_url_scanner_ex_deactivate(TSRMLS_D)
{
	url_adapt_state_ex_t *ctx;
	
	ctx = &BG(url_adapt_state_ex);

	smart_str_free(&ctx->result);
	smart_str_free(&ctx->buf);
	smart_str_free(&ctx->tag);
	smart_str_free(&ctx->arg);

	return SUCCESS;
}

static void php_url_scanner_output_handler(char *output, uint output_len, char **handled_output, uint *handled_output_len, int mode TSRMLS_DC)
{
	size_t len;

    if (BG(url_adapt_state_ex).url_app.len != 0) {
        *handled_output = url_adapt_ext(output, output_len, &len, (zend_bool) (mode & (PHP_OUTPUT_HANDLER_END|PHP_OUTPUT_HANDLER_CONT) ? 1 : 0) TSRMLS_CC);
		if (sizeof(uint) < sizeof(size_t)) {
			if (len > UINT_MAX)
				len = UINT_MAX;
		}
		*handled_output_len = len;
    } else {
        *handled_output = NULL;
    }
}

int php_url_scanner_add_var(char *name, int name_len, char *value, int value_len, int urlencode TSRMLS_DC)
{
	char *encoded;
	int encoded_len;
	smart_str val;
	
	if (! BG(url_adapt_state_ex).active) {
		php_url_scanner_ex_activate(TSRMLS_C);
		php_ob_set_internal_handler(php_url_scanner_output_handler, 0, "URL-Rewriter", 1 TSRMLS_CC);
		BG(url_adapt_state_ex).active = 1;
	}


	if (BG(url_adapt_state_ex).url_app.len != 0) {
		smart_str_appends(&BG(url_adapt_state_ex).url_app, PG(arg_separator).output);
	}

	if (urlencode) {
		encoded = php_url_encode(value, value_len, &encoded_len);
		smart_str_setl(&val, encoded, encoded_len);
	} else {
		smart_str_setl(&val, value, value_len);
	}
	
	smart_str_appendl(&BG(url_adapt_state_ex).url_app, name, name_len);
	smart_str_appendc(&BG(url_adapt_state_ex).url_app, '=');
	smart_str_append(&BG(url_adapt_state_ex).url_app, &val);

	smart_str_appends(&BG(url_adapt_state_ex).form_app, "<input type=\"hidden\" name=\""); 
	smart_str_appendl(&BG(url_adapt_state_ex).form_app, name, name_len);
	smart_str_appends(&BG(url_adapt_state_ex).form_app, "\" value=\"");
	smart_str_append(&BG(url_adapt_state_ex).form_app, &val);
	smart_str_appends(&BG(url_adapt_state_ex).form_app, "\" />");

	if (urlencode)
		efree(encoded);

	return SUCCESS;
}

int php_url_scanner_reset_vars(TSRMLS_D)
{
	BG(url_adapt_state_ex).form_app.len = 0;
	BG(url_adapt_state_ex).url_app.len = 0;

	return FAILURE;
}

PHP_MINIT_FUNCTION(url_scanner)
{
	BG(url_adapt_state_ex).tags = NULL;

	BG(url_adapt_state_ex).form_app.c = BG(url_adapt_state_ex).url_app.c = 0;
	BG(url_adapt_state_ex).form_app.len = BG(url_adapt_state_ex).url_app.len = 0;

	REGISTER_INI_ENTRIES();
	return SUCCESS;
}

PHP_MSHUTDOWN_FUNCTION(url_scanner)
{
	UNREGISTER_INI_ENTRIES();
	zend_hash_destroy(BG(url_adapt_state_ex).tags);
	free(BG(url_adapt_state_ex).tags);

	return SUCCESS;
}

PHP_RINIT_FUNCTION(url_scanner)
{
	BG(url_adapt_state_ex).active = 0;
	
	return SUCCESS;
}

PHP_RSHUTDOWN_FUNCTION(url_scanner)
{
	if (BG(url_adapt_state_ex).active) {
		php_url_scanner_ex_deactivate(TSRMLS_C);
		BG(url_adapt_state_ex).active = 0;
	}

	smart_str_free(&BG(url_adapt_state_ex).form_app);
	smart_str_free(&BG(url_adapt_state_ex).url_app);

	return SUCCESS;
}
