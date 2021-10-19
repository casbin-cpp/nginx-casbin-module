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

#ifndef _NGINX_CASBIN_HANDLER_HPP
#define _NGINX_CASBIN_HANDLER_HPP

#include <iostream>
#include <assert.h>
#include "NginxCasbinConf.hpp"

class NgxCasbinHandler final {
    public:
        using this_type = NgxCasbinHandler;
        using ADOPTER_TYPE = NgxCasbinConf::ADOPTER_TYPE;

        static ngx_int_t init(ngx_conf_t* cf) {
            
            NgxHttpCoreModule::instance().handler(cf, handler, NGX_HTTP_ACCESS_PHASE);

            return NGX_OK;
        }
        
        static ngx_int_t handler(ngx_http_request_t* r) {
#ifdef CASBIN_MODULE_TEST
            test_orign_tokens();

            // test_request(r);

            // test_array(r);

            // test_command(r);

            // test_str();

            // test_clock();

            // test_date();

            // test_log(r);
#endif
            // If target in forbid list -> return NGX_HTTP_FORBIDDEN 403
            // If not in enable list Close Connetion -> return NGX_OK
            // If access continue other modules -> return NGX_DECLINED

            extern ngx_module_t ngx_http_casbin_module;
            auto model = NgxModule<NgxCasbinConf>(ngx_http_casbin_module);
            auto mcf = model.conf().loc(r);
            auto mreq = NgxRequest(r);
            auto model_path = NgxString(mcf->model_path).strcpy();
            auto policy_path = NgxString(mcf->policy_path).strcpy();

            if(mcf->enabled == (long int)1) {

                // TODO 
                // 1. build enforcer by different model and policy
                // 2. find key in url by enforcer model
                // 3. get ans from enforcer 
                // 4. return alias status code
                casbin::Enforcer e;
                
                switch (mcf->adopter_type)
                {
                    case ADOPTER_TYPE::FILE_ADOPTER:
                        NgxLogDebug(r).print("Use File adopter for casbin, model path: %V, policy path %V", 
                                            &mcf->model_path, &mcf->policy_path);
                        e = casbin::Enforcer(model_path, policy_path);
                        break;
                    
                    default:
                        NgxLogDebug(r).print("Use Unknow adopter for casbin, model path: %V, policy path %V", 
                                            &mcf->model_path, &mcf->policy_path);
                        break;
                }

                auto&& origin_tokens = get_origin_tokens(e);
                auto&& req = get_url_request(origin_tokens, mreq);

                return get_result(e, req, r);

                // return test_basic_model(e, r);
            
            } else {

                NgxLogDebug(r).print("Casbin off");
            }

            return NGX_DECLINED;
        }

    public:
        static ngx_int_t get_result(casbin::Enforcer& e, casbin::DataVector& req, ngx_http_request_t* r) {

            if(!e.Enforce(req)) {
                NgxLogDebug(r).print("%V Can't be permitted to access", &(r->unparsed_uri));

                return NGX_HTTP_FORBIDDEN;

            } else {
                NgxLogDebug(r).print("%V Can be permitted  access", &(r->unparsed_uri));

                return NGX_DECLINED;
            }
        }

        static std::vector<std::string> get_origin_tokens(casbin::Enforcer& e) {

            auto model = e.GetModel();

            std::vector<std::string> origin_tokens;

            auto tokens = model->m["r"].assertion_map["r"]->tokens;

            std::vector<std::string> basic_tokens;

            for(auto token: tokens) {
                auto idx = token.find('_');
                origin_tokens.push_back(token.substr(idx + 1));
            }

            return origin_tokens;
        }

        static casbin::DataVector get_url_request(std::vector<std::string>& origin_tokens, NgxRequest& mreq) {

            casbin::DataVector req;

            for(auto& origin_token: origin_tokens) {
                auto req_ele = mreq.arg(origin_token.c_str());
                req.push_back(NgxString(req_ele).strcpy());
            }

            return req;
        }

#ifdef CASBIN_MODULE_TEST
        // input file must be basic policy
        static void test_orign_tokens() {
            
            auto e = casbin::Enforcer("../config/basic_model.conf", "../config/basic_policy.csv");
            std::vector<std::string_view> target_tokens = {"sub", "obj", "act"};

            auto tokens = e.GetModel()->m["r"].assertion_map["r"]->tokens;

            std::vector<std::string> basic_tokens;

            for(auto token: tokens) {
                auto idx = token.find('_');
                basic_tokens.push_back(token.substr(idx + 1));
            }

            assert(basic_tokens.size() == tokens.size());

            for(std::size_t i = 0; i < basic_tokens.size(); i++ ) {
                assert(target_tokens[i].compare(basic_tokens[i]) == 0);
            }

            std::cout << "Test tokens Pass !" << std::endl;

        }

        static ngx_int_t test_basic_model(casbin::Enforcer& e, ngx_http_request_t* r) {

            auto mreq = NgxRequest(r);
            auto substr = mreq.arg("sub");
            auto objstr = mreq.arg("obj");
            auto actstr = mreq.arg("act");
            auto sub = NgxString(substr).strcpy();
            auto obj = NgxString(objstr).strcpy();
            auto act = NgxString(actstr).strcpy();

            if(!e.Enforce({sub, obj, act})) {
                NgxLogDebug(r).print("%V can't be permitted to %V access %V",
                                    &substr, &objstr, &actstr);

                return NGX_HTTP_FORBIDDEN;

            } else {
                NgxLogDebug(r).print("%V can be permitted to %V access %V",
                                    &substr, &objstr, &actstr);

                return NGX_DECLINED;
            }
            
        }
        static void test_command(ngx_http_request_t* r) {
            
            extern ngx_module_t ngx_http_casbin_module;
            NgxCasbinConf* conf =  reinterpret_cast<NgxCasbinConf*>(r->loc_conf[ngx_http_casbin_module.ctx_index]);
            if(NgxValue::isvalid(conf->enabled)) {
                std::cout << "casbin is not set " << std::endl;
            }else {
                if(conf->enabled) {
                    std::cout << "casbin is on" << std::endl;
                } else {
                    std::cout << "casbin is off" << std::endl;
                }
            }

        }
        static void test_str() {

            char data_[10] = "123";
            ngx_str_t test;
            test.data = (u_char* )data_;
            test.len = 3;

            auto s = NgxString(test).str();
            std::cout << s << std::endl; // 123
            data_[0] = '2';
            std::cout << s << std::endl; // 223

            auto scpy = NgxString(test).strcpy();
            scpy[0] = '3';
            std::cout << scpy << std::endl; // 323

            std::cout << s << std::endl;    // 223
        }

        static void test_clock() {
            auto t = NgxClock();

            for(int i = 0; i < 1e7; i++) {
                ;
            }
            std::cout << "Pass "<< t.elapsed() << "s" << std::endl;
        }

        static void test_date() {
            std::cout << NgxDatetime::today() << std::endl;

            std::cout << NgxDatetime::http() << std::endl;
        }

        static void test_log(ngx_http_request_t* r) {
            NgxLogInfo(r).print("info print");

            NgxLogError(r).print("error print");

            NgxLogDebug(r).print("debug print");
        }

        static void test_request(ngx_http_request_t* r) {
            auto Myr = NgxRequest(r);

            NgxLogDebug(r).print("method: %V", &Myr->method_name);
            NgxLogDebug(r).print("uri: %V", &Myr->uri);
            NgxLogDebug(r).print("uri args: %V", &Myr->args);
            // get object from uri

            if(!NgxString(Myr->exten).empty() ) {
                auto uri = NgxString(Myr->uri).str();
                std::string_view obj = nullptr;
                for(int i = uri.size() - 1; i >= 0; i --) {
                    if(uri[i] == '/') {
                        obj = uri.substr(i + 1);
                        break;
                    }
                }
                std::cout << "obj:" << obj << std::endl;
            } else {

                std::cout << "if not have object: " << Myr->exten << std::endl;
                
                auto uri = NgxString(Myr->uri).str();
                std::string_view obj = nullptr;

                if(uri.size() == 1) {
                    obj = uri;
                } else {
                    for(int i = uri.size() - 1; i >= 0; i --) {
                        if(uri[i] == '/' && i != (int)uri.size() - 1) {
                            obj = uri.substr(i + 1);
                            break;
                        }
                    }

                }

                std::cout << obj << std::endl;
            }


            std::cout << Myr.arg("usr") << std::endl;
            ngx_str_t usr = ngx_string("usr");
            std::cout << Myr.arg(usr) << std::endl;
            // NgxLogDebug(r).print("arg: %V", &(Myr.arg(Myr->method_name)));
        }

        static void test_array (ngx_http_request_t* r) {
            auto arr = NgxArray<int>(NgxPool(r->pool));

            arr.push(1);
            arr.push(2);
            arr.push(3);

            for(int i = 0; i < (int)arr.size(); i++ ) {
                std::cout << arr[i] << ' ';
            }

            std::cout << std::endl;
        }
#endif
};


#endif