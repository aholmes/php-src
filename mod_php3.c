/*
   +----------------------------------------------------------------------+
   | PHP HTML Embedded Scripting Language Version 3.0                     |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997,1998 PHP Development Team (See Credits file)      |
   +----------------------------------------------------------------------+
   | This program is free software; you can redistribute it and/or modify |
   | it under the terms of one of the following licenses:                 |
   |                                                                      |
   |  A) the GNU General Public License as published by the Free Software |
   |     Foundation; either version 2 of the License, or (at your option) |
   |     any later version.                                               |
   |                                                                      |
   |  B) the PHP License as published by the PHP Development Team and     |
   |     included in the distribution in the file: LICENSE                |
   |                                                                      |
   | This program is distributed in the hope that it will be useful,      |
   | but WITHOUT ANY WARRANTY; without even the implied warranty of       |
   | MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        |
   | GNU General Public License for more details.                         |
   |                                                                      |
   | You should have received a copy of both licenses referred to here.   |
   | If you did not, or have any questions about PHP licensing, please    |
   | contact core@php.net.                                                |
   +----------------------------------------------------------------------+
   | Authors: Rasmus Lerdorf <rasmus@lerdorf.on.ca>                       |
   | (with helpful hints from Dean Gaudet <dgaudet@arctic.org>            |
   | PHP 4.0 patches by Zeev Suraski <zeev@zend.com>                      |
   +----------------------------------------------------------------------+
 */
/* $Id$ */

#include "httpd.h"
#include "http_config.h"
#if MODULE_MAGIC_NUMBER > 19980712
#include "ap_compat.h"
#else
#if MODULE_MAGIC_NUMBER > 19980324
#include "compat.h"
#endif
#endif
#include "http_core.h"
#include "http_main.h"
#include "http_protocol.h"
#include "http_request.h"
#include "http_log.h"

#include "php.h"
#include "php_ini.h"
#include "SAPI.h"
#include "main.h"

#include "util_script.h"

#include "php_version.h"
#include "mod_php3.h"
#if HAVE_MOD_DAV
# include "mod_dav.h"
#endif

PHPAPI int apache_php3_module_main(request_rec *r, int fd, int display_source_mode SLS_DC);

/* ### these should be defined in mod_php3.h or somewhere else */
#define USE_PATH 1
#define IGNORE_URL 2

module MODULE_VAR_EXPORT php3_module;

int saved_umask;


#if WIN32|WINNT
/* popenf isn't working on Windows, use open instead*/
# ifdef popenf
#  undef popenf
# endif
# define popenf(p,n,f,m) open((n),(f),(m))
# ifdef pclosef
#  undef pclosef
# endif
# define pclosef(p,f) close(f)
#else
# define php3i_popenf(p,n,f,m) popenf((p),(n),(f),(m))
#endif

php_apache_info_struct php_apache_info;		/* active config */



void php3_save_umask()
{
	saved_umask = umask(077);
	umask(saved_umask);
}


static int zend_apache_ub_write(const char *str, uint str_length)
{
	SLS_FETCH();
	
	if (SG(server_context)) {
		return rwrite(str, str_length, (request_rec *) SG(server_context));
	} else {
		return fwrite(str, 1, str_length, stdout);
	}
}


int sapi_apache_read_post(char *buffer, uint count_bytes SLS_DC)
{
	uint total_read_bytes=0, read_bytes;
	request_rec *r = (request_rec *) SG(server_context);
	void (*handler)(int);

	handler = signal(SIGPIPE, SIG_IGN);
	while (total_read_bytes<count_bytes) {
		hard_timeout("Read POST information", r); /* start timeout timer */
		read_bytes = get_client_block(r, buffer+total_read_bytes, count_bytes-total_read_bytes);
		reset_timeout(r);
		if (read_bytes<=0) {
			break;
		}
		total_read_bytes += read_bytes;
	}
	signal(SIGPIPE, handler);	
	return total_read_bytes;
}


char *sapi_apache_read_cookies(SLS_D)
{
	return (char *) table_get(((request_rec *) SG(server_context))->subprocess_env, "HTTP_COOKIE");
}


int sapi_apache_header_handler(sapi_header_struct *sapi_header, sapi_headers_struct *sapi_headers SLS_DC)
{
	char *header_name, *header_content, *p;
	request_rec *r = (request_rec *) SG(server_context);

	header_name = sapi_header->header;

	header_content = p = strchr(header_name, ':');
	if (!p) {
		return 0;
	}

	*p = 0;
	do {
		header_content++;
	} while (*header_content==' ');

	if (!strcasecmp(header_name, "Content-Type")) {
		r->content_type = pstrdup(r->pool, header_content);
	} else {
		table_set(r->headers_out, header_name, header_content);
	}

	*p = ':';  /* a well behaved header handler shouldn't change its original arguments */

	efree(sapi_header->header);
	
	return 0;  /* don't use the default SAPI mechanism, Apache duplicates this functionality */
}


int sapi_apache_send_headers(sapi_headers_struct *sapi_headers SLS_DC)
{
	((request_rec *) SG(server_context))->status = SG(sapi_headers).http_response_code;
	send_http_header((request_rec *) SG(server_context));
	return SAPI_HEADER_SENT_SUCCESSFULLY;
}


sapi_module_struct sapi_module = {
	"PHP Language",					/* name */
									
	php_module_startup,				/* startup */
	php_module_shutdown_wrapper,	/* shutdown */

	zend_apache_ub_write,			/* unbuffered write */

	php3_error,						/* error handler */

	sapi_apache_header_handler,		/* header handler */
	sapi_apache_send_headers,		/* send headers handler */
	NULL,							/* send header handler */

	sapi_apache_read_post,			/* read POST data */
	sapi_apache_read_cookies		/* read Cookies */
};


void php3_restore_umask()
{
	umask(saved_umask);
}


static void init_request_info(SLS_D)
{
	request_rec *r = ((request_rec *) SG(server_context));
	char *content_length = (char *) table_get(r->subprocess_env, "CONTENT_LENGTH");
	const char *authorization=NULL;
	char *tmp;

	SG(request_info).query_string = r->args;
	SG(request_info).path_translated = r->filename;
	SG(request_info).request_uri = r->uri;
	SG(request_info).request_method = r->method;
	SG(request_info).content_type = (char *) table_get(r->subprocess_env, "CONTENT_TYPE");
	SG(request_info).content_length = (content_length ? atoi(content_length) : 0);

	if (r->headers_in) {
		authorization = table_get(r->headers_in, "Authorization");
	}
	if (authorization
		&& !auth_type(r)
		&& !strcmp(getword(r->pool, &authorization, ' '), "Basic")) {
		tmp = uudecode(r->pool, authorization);
		SG(request_info).auth_user = getword_nulls_nc(r->pool, &tmp, ':');
		if (SG(request_info).auth_user) {
			SG(request_info).auth_user = estrdup(SG(request_info).auth_user);
		}
		SG(request_info).auth_password = tmp;
		if (SG(request_info).auth_password) {
			SG(request_info).auth_password = estrdup(SG(request_info).auth_password);
		}
	} else {
		SG(request_info).auth_user = NULL;
		SG(request_info).auth_password = NULL;
	}
}


int send_php3(request_rec *r, int display_source_mode, char *filename)
{
	int fd, retval;
	SLS_FETCH();

	/* We don't accept OPTIONS requests, but take everything else */
	if (r->method_number == M_OPTIONS) {
		r->allowed |= (1 << METHODS) - 1;
		return DECLINED;
	}

	/* Make sure file exists */
	if (filename == NULL && r->finfo.st_mode == 0) {
		return NOT_FOUND;
	}

	/* If PHP parser engine has been turned off with a "php3_engine off"
	 * directive, then decline to handle this request
	 */
	if (!php_apache_info.engine) {
		r->content_type = "text/html";
		r->allowed |= (1 << METHODS) - 1;
		return DECLINED;
	}
	if (filename == NULL) {
		filename = r->filename;
	}
	/* Open the file */
	if ((fd = popenf(r->pool, filename, O_RDONLY, 0)) == -1) {
		log_reason("file permissions deny server access", filename, r);
		return FORBIDDEN;
	}

	/* Apache 1.2 has a more complex mechanism for reading POST data */
#if MODULE_MAGIC_NUMBER > 19961007
	if ((retval = setup_client_block(r, REQUEST_CHUNKED_ERROR)))
		return retval;
#endif

	if (php_apache_info.last_modified) {
#if MODULE_MAGIC_NUMBER < 19970912
		if ((retval = set_last_modified(r, r->finfo.st_mtime))) {
			return retval;
		}
#else
		update_mtime (r, r->finfo.st_mtime);
		set_last_modified(r);
		set_etag(r);
#endif
	}
	/* Assume output will be HTML.  Individual scripts may change this 
	   further down the line */
	r->content_type = "text/html";

	/* Init timeout */
	hard_timeout("send", r);

	SG(server_context) = r;
	init_request_info(SLS_C);
	
	php3_save_umask();
	chdir_file(filename);
	add_common_vars(r);
	add_cgi_vars(r);
	apache_php3_module_main(r, fd, display_source_mode SLS_CC);

	/* Done, restore umask, turn off timeout, close file and return */
	php3_restore_umask();
	kill_timeout(r);
	pclosef(r->pool, fd);
	return OK;
}


int send_parsed_php3(request_rec * r)
{
	return send_php3(r, 0, NULL);
}


int send_parsed_php3_source(request_rec * r)
{
	return send_php3(r, 0, NULL);
}


/*
 * Create the per-directory config structure with defaults
 */
static void *php3_create_dir(pool * p, char *dummy)
{
	php_apache_info_struct *new;

	new = (php_apache_info_struct *) palloc(p, sizeof(php_apache_info_struct));
	memcpy(new, &php_apache_info, sizeof(php_apache_info_struct));

	return new;
}


#if MODULE_MAGIC_NUMBER > 19961007
#define CONST_PREFIX const
#else
#define CONST_PREFIX
#endif

CONST_PREFIX char *php_apache_value_handler(cmd_parms *cmd, php_apache_info_struct *conf, char *arg1, char *arg2)
{
	php_alter_ini_entry(arg1, strlen(arg1)+1, arg2, strlen(arg2)+1, PHP_INI_PERDIR);
	return NULL;
}


CONST_PREFIX char *php_apache_flag_handler(cmd_parms *cmd, php_apache_info_struct *conf, char *arg1, char *arg2)
{
	char bool_val[2];

	if (!strcmp(arg2, "On")) {
		bool_val[0] = '1';
	} else {
		bool_val[0] = '0';
	}
	bool_val[1] = 0;
	
	php_alter_ini_entry(arg1, strlen(arg1)+1, bool_val, 2, PHP_INI_PERDIR);
	return NULL;
}


int php3_xbithack_handler(request_rec * r)
{
	php_apache_info_struct *conf;

	conf = (php_apache_info_struct *) get_module_config(r->per_dir_config, &php3_module);
	if (!(r->finfo.st_mode & S_IXUSR)) {
		r->allowed |= (1 << METHODS) - 1;
		return DECLINED;
	}
	if (conf->xbithack == 0) {
		r->allowed |= (1 << METHODS) - 1;
		return DECLINED;
	}
	return send_parsed_php3(r);
}


void php3_init_handler(server_rec *s, pool *p)
{
	register_cleanup(p, NULL, php_module_shutdown, php_module_shutdown_for_exec);
	php_module_startup(&sapi_module);
#if MODULE_MAGIC_NUMBER >= 19980527
	ap_add_version_component("PHP/" PHP_VERSION);
#endif
}


#if HAVE_MOD_DAV

extern int phpdav_mkcol_test_handler(request_rec *r);
extern int phpdav_mkcol_create_handler(request_rec *r);

/* conf is being read twice (both here and in send_php3()) */
int send_parsed_php3_dav_script(request_rec *r)
{
	php_apache_info_struct *conf;

	conf = (php_apache_info_struct *) get_module_config(r->per_dir_config,
													&php3_module);
	return send_php3(r, 0, 0, conf->dav_script);
}

static int php3_type_checker(request_rec *r)
{
	php_apache_info_struct *conf;

	conf = (php_apache_info_struct *)get_module_config(r->per_dir_config,
												   &php3_module);

    /* If DAV support is enabled, use mod_dav's type checker. */
    if (conf->dav_script) {
		dav_api_set_request_handler(r, send_parsed_php3_dav_script);
		dav_api_set_mkcol_handlers(r, phpdav_mkcol_test_handler,
								   phpdav_mkcol_create_handler);
		/* leave the rest of the request to mod_dav */
		return dav_api_type_checker(r);
	}

    return DECLINED;
}

#else /* HAVE_MOD_DAV */

# define php3_type_checker NULL

#endif /* HAVE_MOD_DAV */


handler_rec php3_handlers[] =
{
	{"application/x-httpd-php3", send_parsed_php3},
	{"application/x-httpd-php3-source", send_parsed_php3_source},
	{"text/html", php3_xbithack_handler},
	{NULL}
};


command_rec php3_commands[] =
{
	{"php4_value",		php_apache_value_handler, NULL, OR_OPTIONS, TAKE2, "PHP Value Modifier"},
	{"php4_flag",			php_apache_flag_handler, NULL, OR_OPTIONS, TAKE2, "PHP Flag Modifier"},
	{NULL}
};



module MODULE_VAR_EXPORT php3_module =
{
	STANDARD_MODULE_STUFF,
	php3_init_handler,			/* initializer */
	php3_create_dir,			/* per-directory config creator */
	NULL,						/* dir merger */
	NULL,						/* per-server config creator */
	NULL, 						/* merge server config */
	php3_commands,				/* command table */
	php3_handlers,				/* handlers */
	NULL,						/* filename translation */
	NULL,						/* check_user_id */
	NULL,						/* check auth */
	NULL,						/* check access */
	php3_type_checker,			/* type_checker */
	NULL,						/* fixups */
	NULL						/* logger */
#if MODULE_MAGIC_NUMBER >= 19970103
	,NULL						/* header parser */
#endif
#if MODULE_MAGIC_NUMBER >= 19970719
	,NULL             			/* child_init */
#endif
#if MODULE_MAGIC_NUMBER >= 19970728
	,NULL						/* child_exit */
#endif
#if MODULE_MAGIC_NUMBER >= 19970902
	,NULL						/* post read-request */
#endif
};

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
