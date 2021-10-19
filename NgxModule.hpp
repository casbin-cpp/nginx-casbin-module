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

#ifndef _NGX_MODULE_HPP
#define _NGX_MODULE_HPP

#include "NgxPool.hpp"

/**
 * @name: NgxModuleConfig
 * @msg: Get module config
 * @param {T3:loacl config, T2: server config, T1: main config} 
 */
template< typename T3 = void, typename T2 = void, typename T1 = void>
class NgxModuleConfig final {
    public:
        using ngx_session_type = ngx_http_request_t;
        using ngx_conf_ctx_type = ngx_http_conf_ctx_t;

        NgxModuleConfig(ngx_uint_t idx): m_idx(idx) {}
        // use defalut 
        ~NgxModuleConfig() = default;      
        // get model index
        ngx_uint_t index() const {
            return m_idx;
        }
        // trans data type by template
        template<typename T, typename U>
        T* get(U conf) const { 
            return reinterpret_cast<T*>(conf[index()]);
        }
    
    private: 
        ngx_uint_t m_idx = 0;

        // trans (void* ) ctx to http_conf
        ngx_http_conf_ctx_t* ctx(ngx_conf_t* cf) const {
            return reinterpret_cast<ngx_http_conf_ctx_t*>(cf->ctx);
        }
    public:
        // from config
        // main http level
        T1* main(ngx_conf_t* cf) const {
            return get<T1>(ctx(cf)->main_conf);
        }
        // main srv level
        T2* srv(ngx_conf_t* cf) const {
            return get<T3>(ctx(cf)->srv_conf);
        }
        // main local level
        T3* loc(ngx_conf_t* cf) const {
            return get<T3>(ctx(cf)->loc_conf);
        }
    public:
        // from http_request 
        // main http level
        T1* main(ngx_http_request_t* r) const {
            return get<T1>(r->main_conf);
        }
        // main srv level
        T2* srv(ngx_http_request_t* r) const {
            return get<T3>(r->srv_conf);
        }
        // main local level
        T3* loc(ngx_http_request_t* r) const {
            return get<T3>(r->loc_conf);
        }
};

class NgxModuleCtx final {
    public:
        NgxModuleCtx(ngx_uint_t idx): m_idx(idx) {}
        ~NgxModuleCtx() = default;
    public:
        ngx_uint_t index() const {
            return m_idx;
        }

        using raw_pointer = void*;
        // return reference of void* so we can decide pointer point to where if we want.
        raw_pointer& ctx(ngx_http_request_t* r) const {
            return r->ctx[index()];
        }

        bool empty(ngx_http_request_t* r) const {
            return !ctx(r);
        }

        void clear(ngx_http_request_t* r) const {
            // if alloc memory, may cause memory leakage
            ctx(r) = nullptr;
        }

        // if not exit then create 
        template<typename T>
        T& data(ngx_http_request_t* r) const {
            if(empty(r) ) {
                auto p = NgxPool(r->pool);
                ctx(r) = p.alloc<T>();
            }

            return *reinterpret_cast<T*>(ctx(r));
        }

    private:
        ngx_uint_t m_idx = 0;
};

template<typename T3 = void, typename T2 = void, typename T1 = void>
class NgxModule {
    public:
        typedef NgxModuleConfig<T3, T2, T1> config_type;
        typedef NgxModuleCtx ctx_type;
        // genete NgxModuleConfig by pass ctx_idx;
        NgxModule(ngx_module_t& m): m_config(m.ctx_index), m_ctx(m.ctx_index){}
        ~NgxModule() = default;
        // get ngxmodule config
        const config_type& conf() const {
            return m_config;
        }
        // get ngxmodule context
        const ctx_type& ctx() const {
            return m_ctx;
        }

        template<typename T>
        T& data(ngx_http_request_t* r) const {
            // why need template 
            return ctx().template data<T>(r);
        }
    private:
        config_type m_config;
        ctx_type    m_ctx;
};

class NgxHttpCoreModule final: public NgxModule<ngx_http_core_loc_conf_t, 
                                                ngx_http_core_srv_conf_t,
                                                ngx_http_core_main_conf_t> {
    public:
        using super_type = NgxModule<ngx_http_core_loc_conf_t, 
                                     ngx_http_core_srv_conf_t, 
                                     ngx_http_core_main_conf_t>;
        using this_type = NgxHttpCoreModule;
    public:
        NgxHttpCoreModule(): super_type(ngx_http_core_module) {} 
        ~NgxHttpCoreModule() = default;
    public:
        static NgxHttpCoreModule& instance() {
            static NgxHttpCoreModule m;
            return m;
        }

        // http handle call back
        template<typename F>
        void handler(ngx_conf_t* cf, F f, ngx_http_phases p) const {
            auto c = conf().main(cf);

            ngx_http_handler_pt* h = (ngx_http_handler_pt* )ngx_array_push(&(c->phases[p].handlers));

            *h = f;
        }
        // http location call back
        template<typename F>
        void handler(ngx_conf_t* cf, F f) const {
            conf().loc(cf)->handler = f;
        }
};

// Undo: why do this
#define NGX_MOD_INSTANCE(T, mod, ... )      \
    struct T {                              \
        typedef NgxModule<__VA_ARGS__> mod_type;   \
        typedef mod_type::config_type config_type;  \
        static mod_type& instance()         \
        {                                   \
            extern ngx_module_t mod;        \
            static mod_type m(mod);         \
            return m;                       \
        }                                   \
        static const config_type& conf() \
        {   return instance().conf();}             \
    };

#define NGX_MOD_CTX_INSTANCE(T, mod, c, ... )     \
    struct T {                                      \
        typedef NgxModule<__VA_ARGS__> mod_type;    \
        typedef mod_type::config_type config_type;  \
        typedef NgxModuleCtx ctx_type;              \
        typedef c ctx_data_type;                       \
        static mod_type& instance()         \
        {                                   \
            extern ngx_module_t mod;        \
            static mod_type m(mod);         \
            return m;                       \
        }                                   \
        static const config_type& conf() \
        {   return instance().conf();}             \
        static const ctx_type& ctx()                \
        {   return instance().ctx();}       \
        static ctx_data_type& data(mod_type::ngx_session_type* r)                 \
        {   return instance().template data<ctx_data_type>(r);}             \
    };
#endif