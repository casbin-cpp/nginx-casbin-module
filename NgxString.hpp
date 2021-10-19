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

#ifndef _NGX_STRING_HPP
#define _NGX_STRING_HPP

#include <string_view>
#include "NgxWrapper.hpp"

class NgxString final : public NgxWrapper<ngx_str_t> {
    public:
        typedef NgxWrapper<ngx_str_t> super_type;
        typedef NgxString this_type;

        // typedef std::string_ref string_ref_type;

        NgxString(ngx_str_t& str) : super_type(str) {}
        NgxString(const ngx_str_t& str) : 
            super_type(const_cast<ngx_str_t&>(str)) {}

        // disable temporarya object : don't know why
        NgxString(ngx_str_t&& str) = delete;
        ~NgxString() = default;

    public:
        const char* data() const {
            return reinterpret_cast<const char*> (get()->data);
        }

        std::size_t size() const {
            return get()->len;
        }

        bool empty() const {
            return !get()->data || !get()->len;
        }

        // read only
        std::string_view str() const {
            return std::string_view(data(), size());
        }
        // copy a new string and can write
        std::string strcpy() const {
            return std::string(str());   
        }

        void tolower() {
            for(int i = 0; i < (int)size(); i++ ) {
                get()->data[i] = ngx_tolower(get()->data[i]);
            }
        }
        // Case sensity
        bool isSameCase (ngx_str_t&& str) {
            return this->str().compare(NgxString(str).str()) == 0;
        }

        bool isSameCase (ngx_str_t& str) {
            return this->str().compare(NgxString(str).str()) == 0;
        }

        // turn to lower then compare
        bool isSame (ngx_str_t&& str) {
            tolower();

            return isSameCase(str);
        }

        // why write like this? how to use
        // template<typename ... Args>
        // void printf(const Args& ... args) const {
        //     auto p = ngx_snprintf(get()->data, get()->len, args ...);

        //     get()->len = static_cast<std::size_t>(p - get()->data);
        // }
};

// workaround for some compilers
inline namespace ostream_for_ngx_str_t{

template<typename T>
T& operator<<(T& o, const ngx_str_t& s)
{
    o.write(reinterpret_cast<const char*>(s.data), s.len);
    return o;
}

template<typename T>
T& operator<<(T& o, const volatile ngx_str_t& s)
{
    o.write(reinterpret_cast<const char*>(s.data), s.len);
    return o;
}

}
#endif