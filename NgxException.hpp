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

#ifndef _NGX_EXCEPTION_HPP
#define _NGX_EXCEPTION_HPP

class NgxException final: public std::exception {
    public:
        NgxException(ngx_int_t x, std::string msg):
            m_code(x), m_msg(msg) {}
        NgxException(ngx_int_t x = NGX_ERROR):
            NgxException(x, "") {}
        NgxException(std::string msg): 
            NgxException(NGX_ERROR, msg) {}
        ~NgxException() = default;
    public:
        static void raise(ngx_int_t rc = NGX_ERROR, std::string msg = "") {
            throw NgxException(rc, msg);
        }

        static void require(bool cond, ngx_int_t e = NGX_ERROR, std::string msg = "") {
            if (!cond) {
                raise(e, msg);
            }
        }

        template<typename T>
        static void require(T* t, ngx_int_t e = NGX_ERROR, std::string msg = "") {
            require(t != nullptr, e, msg);
        }
    private:
        ngx_int_t m_code = NGX_ERROR;
        std::string m_msg;
};

#endif