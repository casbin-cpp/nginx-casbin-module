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

#ifndef _NGX_HTTP_REQUEST_HPP
#define _NGX_HTTP_REQUEST_HPP

#include "NgxWrapper.hpp"

class NgxRequest final: public NgxWrapper<ngx_http_request_t > {
    public:
        using SuperType = NgxWrapper<ngx_http_request_t>;
        using ThisType = NgxRequest;

        NgxRequest(ngx_http_request_t* r): SuperType(r) {}
        ~NgxRequest() = default;
    public:
        // judge method whether is x
        bool method(ngx_uint_t x) const {
            return get()->method & x;
        }
    public:
        ngx_str_t arg(const ngx_str_t& name) const {
            ngx_str_t value = ngx_null_string;

            ngx_http_arg(get(), name.data, name.len, &value);

            return value;
        }
        ngx_str_t arg(const char* str) const {
            ngx_str_t name;
            name.len = strlen(str);
            name.data = (u_char* )str;
            ngx_str_t value = ngx_null_string;

            ngx_http_arg(get(), name.data, name.len, &value);

            return value;
        }

};

#endif
