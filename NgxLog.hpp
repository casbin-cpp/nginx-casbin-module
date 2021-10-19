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

#ifndef _NGX_LOG_HPP
#define _NGX_LOG_HPP

template<ngx_uint_t level = NGX_LOG_DEBUG>
class NgxLog final : public NgxWrapper<ngx_log_t> {
    public:
        typedef NgxWrapper<ngx_log_t> super_type;
        typedef NgxLog this_type;
        // directly construct 
        NgxLog(ngx_log_t* l): super_type(l) {}
        // use others' (ngx_log_t) to construt
        template<typename T>
        NgxLog(T* x): super_type(x->log) {}
        // use http requestion
        NgxLog(ngx_http_request_t* r): NgxLog(r->connection) {}

        ~NgxLog() = default;

    public:
        // log without errno
        template<typename ... Args>
        void print(const char* fmt, const Args& ... args) const {
            ngx_log_error(level, get(), 0, fmt, args...);
        }
        // log with errno 
        template<typename ... Args>
        void print(ngx_err_t err, const Args& ... args) const {
            ngx_log_error(level, get(), err, args...);
        }
    
};

typedef NgxLog<NGX_LOG_DEBUG> NgxLogDebug;
typedef NgxLog<NGX_LOG_INFO> NgxLogInfo;
typedef NgxLog<NGX_LOG_ERR> NgxLogError;
typedef NgxLog<NGX_LOG_WARN> NgxLogWarn;

#endif