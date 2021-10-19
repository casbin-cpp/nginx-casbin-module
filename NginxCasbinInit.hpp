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

#ifndef _NGINX_CASBIN_INIT_HPP
#define _NGINX_CASBIN_INIT_HPP 

#include "NginxCasbinHandler.hpp"
#include "NginxCasbinConf.hpp"

class NgxCasbinInit final {
    public:
    
        using conf_type = NgxCasbinConf;
        using handler_type = NgxCasbinHandler;
        using this_type = NgxCasbinInit;

        static char *ngx_set_command_call_back (ngx_conf_t *cf, ngx_command_t *cmd, void *conf) {
            // TODO: whether can cached the casbin enforcer here. 
            auto ok = ngx_conf_set_flag_slot(cf, cmd, conf);

            if(ok != NGX_OK) {
                NgxLogError(cf).print("parse error");
            }

            // Test command parse
            // if(NgxValue::isvalid(my_cf->enabled)) {
            //     std::cout << "casbin is not set " << std::endl;
            // }else {
            //     if(my_cf->enabled) {
            //         std::cout << "casbin is on" << std::endl;
            //     } else {
            //         std::cout << "casbin is off" << std::endl;
            //     }
            // }

            return NGX_OK;            
        }

        static char* casbin_adopter_parser(ngx_conf_t* cf, ngx_command_t* cmd, void* conf) {
            conf_type* mcf = reinterpret_cast<conf_type*>(conf);
            
            auto arr = NgxArray<ngx_str_t>(cf->args);

            if(arr.size() < 4) {
                return (char*)NGX_ERROR;
            }

            mcf->adopter = arr[1];
            mcf->model_path = arr[2];
            mcf->policy_path = arr[3];

            auto str = NgxString(mcf->adopter);
            if(str.isSame(ngx_string("file"))) {
                mcf->adopter_type = conf_type::ADOPTER_TYPE::FILE_ADOPTER;
            } else {
                mcf->adopter_type = conf_type::ADOPTER_TYPE::UNKNOW_TYPE;
            }

            // Test command Parse cb
            // NgxHttpCoreModule::instance().handler(cf, casbin_adopter_command_cb);

            return NGX_OK;
        }

        static ngx_int_t casbin_adopter_command_cb(ngx_http_request_t* r) {

            extern ngx_module_t ngx_http_casbin_module;
            NgxCasbinConf* mcf =  reinterpret_cast<conf_type*>(r->loc_conf[ngx_http_casbin_module.ctx_index]);

            if(!NgxString(mcf->adopter).empty()) {
                NgxLogInfo(r).print("casbin adopter: %V", &(mcf->adopter));
            }

            if(!NgxString(mcf->model_path).empty()) {
                NgxLogInfo(r).print("casbin model path: %V", &(mcf->model_path));
            }

            if(!NgxString(mcf->policy_path).empty()) {
                NgxLogInfo(r).print("casbin policy path: %V", &(mcf->policy_path));
            }

            if(NgxValue::isvalid(mcf->enabled)) {
                    NgxLogInfo(r).print("casbin not set");
            }else {
                if(mcf->enabled) {
                    NgxLogInfo(r).print("casbin on");
                } else {
                    NgxLogInfo(r).print("casbin off");
                }
            }

            return NGX_DECLINED;
        }
    
        static ngx_command_t* cmds() {
            static ngx_command_t n[] = {
                {
                    ngx_string("casbin"),
                    NGX_HTTP_LOC_CONF | NGX_CONF_FLAG | NGX_CONF_TAKE1,
                    ngx_set_command_call_back,
                    NGX_HTTP_LOC_CONF_OFFSET,
                    offsetof(NgxCasbinConf, enabled),
                    NULL 
                },
                {
                    ngx_string("casbin_adopter"),
                    NGX_HTTP_SRV_CONF| NGX_HTTP_LOC_CONF | NGX_CONF_TAKE3,
                    casbin_adopter_parser,
                    NGX_HTTP_LOC_CONF_OFFSET,
                    offsetof(NgxCasbinConf, adopter),
                    NULL 
                },

                ngx_null_command
            };

            return n;
        }
        static ngx_http_module_t* ctx() {
            static ngx_http_module_t c = {
                nullptr,
                &handler_type::init,
                nullptr,
                nullptr,
                nullptr,
                nullptr,

                &conf_type::create,
                nullptr
            };

            return &c;
        }

        static const ngx_module_t& module() {
            static ngx_module_t m = {
                NGX_MODULE_V1,

                ctx(),
                cmds(),

                NGX_HTTP_MODULE,
                nullptr,
                nullptr,
                nullptr,
                nullptr,
                nullptr,
                nullptr,
                nullptr,
                NGX_MODULE_V1_PADDING
            };

            return m;
        } 
};

#endif