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

#ifndef _NGX_DATE_HPP
#define _NGX_DATE_HPP

class NgxClock final {
    public:
        NgxClock() = default;
        ~NgxClock() = default;
    public:
        static const ngx_time_t& now() {
            ngx_time_update();

            return *ngx_timeofday();
        }

        ngx_time_t delta() const {
            auto t = now();

            t.sec -= m_time.sec;
            t.msec -= m_time.msec;

            return t;
        }

        // return delta time by double 
        double elapsed() const {
            auto t = delta();

            return t.sec + t.msec * 1.0 / 1000;
        }

        void reset() {
            m_time = now();
        }
    private:
        ngx_time_t m_time = now();
};

class NgxDatetime final {
    public:
        NgxDatetime() = default;
        ~NgxDatetime() = default;
    public:
        static time_t since() {
            return ngx_time();
        }

        static ngx_str_t today() {
            ngx_tm_t tm;

            ngx_localtime(since(), &tm);

            static u_char buf[20] = {};

            auto p = ngx_snprintf(buf, 20, "%d-%02d-%02d", tm,
                    tm.tm_year, tm.tm_mon, tm.tm_mday);
            
            return ngx_str_t{
                static_cast<std::size_t>(p - buf), buf
            };
        }

        static ngx_str_t http(time_t t = since()) {
            static u_char buf[50] = {};
            auto p = ngx_http_time(buf, t);

            return ngx_str_t{static_cast<std::size_t>(p - buf), buf};
        }

        static time_t http(ngx_str_t& str) {
            return ngx_http_parse_time(str.data, str.len);
        }
};



#endif