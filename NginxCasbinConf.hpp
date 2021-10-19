/*
* Copyright 2021 The casbin Authors. All Rights Reserved.
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*    http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/

#ifndef _NGINX_CASBIN_CONF_HPP
#define _NGINX_CASBIN_CONF_HPP

#include "NginxCpp.hpp"
#include <casbin/casbin.h>

class NgxCasbinConf
{
public:
    using this_type = NgxCasbinConf;

    NgxCasbinConf() = default;
    ~NgxCasbinConf() = default;

    enum class ADOPTER_TYPE { 
        FILE_ADOPTER,
        MYSQL_ADOPTER,
        UNKNOW_TYPE,
    };

    // Must init the variable
    ngx_flag_t enabled =  (ngx_flag_t)NgxUnsetValue::get();
    ngx_str_t adopter = ngx_null_string;
    ngx_str_t model_path = ngx_null_string;
    ngx_str_t policy_path = ngx_null_string;
    ADOPTER_TYPE adopter_type = ADOPTER_TYPE::UNKNOW_TYPE;

    static void* create(ngx_conf_t* cf)
    {
        auto pool = NgxPool(cf->pool);
        // pass template type
        auto ret = pool.alloc<this_type>();

        return ret;
    }

    // static char* merge(ngx_conf_t *cf, void *parent, void *child)
    // {
    //     boost::ignore_unused(cf);

    //     auto prev = reinterpret_cast<this_type*>(parent);
    //     auto conf = reinterpret_cast<this_type*>(child);

    //     NgxValue::merge(conf->enabled, prev->enabled, 0);

    //     return NGX_CONF_OK;
    // }

    // static ADOPTER_TYPE get_adopter_type (ngx_str_t& adopter_str) {
    //     auto str = NgxString(adopter_str).str();

    //     // std::transform(str.begin(), str.end(), str.begin(), std::tolower);
    //     // TODO: upper char
    //     if(str == "file") {
    //         return ADOPTER_TYPE::FILE_ADOPTER;
    //     } else {
    //         return ADOPTER_TYPE::UNKNOW_TYPE;
    //     }
    // }
};

// NGX_MOD_INSTANCE(NgxCasbinModule, ngx_http_casbin_module, NgxCasbinConf)

#endif