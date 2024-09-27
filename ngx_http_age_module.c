#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>
#include <time.h>
#include <stdio.h>

static ngx_int_t ngx_http_age_variable(ngx_http_request_t *r, ngx_http_variable_value_t *v, uintptr_t data);
static ngx_int_t ngx_http_age_add_variable(ngx_conf_t *cf);

static ngx_command_t ngx_http_age_commands[] = {
    ngx_null_command
};

static ngx_http_module_t ngx_http_age_module_ctx = {
    ngx_http_age_add_variable,    /* preconfiguration */
    NULL,                          /* postconfiguration */
    NULL,                          /* create main configuration */
    NULL,                          /* init main configuration */
    NULL,                          /* create server configuration */
    NULL,                          /* merge server configuration */
    NULL,                          /* create location configuration */
    NULL,                          /* merge location configuration */
};

ngx_module_t ngx_http_age_module = {
    NGX_MODULE_V1,
    &ngx_http_age_module_ctx,      /* module context */
    ngx_http_age_commands,         /* module directives */
    NGX_HTTP_MODULE,               /* module type */
    NULL,                          /* init master */
    NULL,                          /* init module */
    NULL,                          /* init process */
    NULL,                          /* init thread */
    NULL,                          /* exit thread */
    NULL,                          /* exit process */
    NULL,                          /* exit master */
    NGX_MODULE_V1_PADDING
};

static ngx_http_variable_t ngx_age_vars[] = {
    {ngx_string("age"),NULL,ngx_http_age_variable,0,NGX_HTTP_VAR_NOCACHEABLE,0},
    { ngx_null_string, NULL, NULL, 0, 0, 0 } 
};

static ngx_int_t
ngx_http_age_add_variable(ngx_conf_t *cf)
{
    ngx_http_variable_t  *var, *v;

    for(v= ngx_age_vars;v->name.len;v++){
        var = ngx_http_add_variable(cf, &v->name, v->flags);
        if (var == NULL) {
            return NGX_ERROR;
        }
        
        var->get_handler = ngx_http_age_variable;
        var->data = v->data;
    }



    return NGX_OK;
}


static ngx_int_t
ngx_http_age_variable(ngx_http_request_t *r, ngx_http_variable_value_t *v, uintptr_t data)
{
   u_char  *p;
    time_t   now, age;

    if (r->upstream == NULL) {
        v->not_found = 1;
        return NGX_OK;
    }

    if (!r->cached
        || r->cache == NULL
        || r->upstream->cache_status == NGX_HTTP_CACHE_REVALIDATED)
    {
        v->not_found = 1;
        return NGX_OK;
    }

    p = ngx_pnalloc(r->pool, NGX_TIME_T_LEN);
    if (p == NULL) {
        return NGX_ERROR;
    }

    now = ngx_time();
    age = now - r->cache->date;

    if (r->cache->date > now) {
        age = 0;
    }


    v->len = ngx_sprintf(p, "%T", age) - p;
    v->valid = 1;
    v->no_cacheable = 0;
    v->not_found = 0;
    v->data = p;

    return NGX_OK;
}