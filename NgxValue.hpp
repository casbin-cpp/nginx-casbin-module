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

#ifndef _NGX_VALUE_HPP
#define _NGX_VALUE_HPP

class NgxUnsetValue final {
    public:
        // transform unset value
        template<typename T>
        operator T () const {
            return static_cast<T>(-1);
        }

        // transform unset pointer
        template<typename T>
        operator T* () const {
            return reinterpret_cast<T*>(-1);
        }
    public:
        static const NgxUnsetValue& get() {
            static NgxUnsetValue const v = {};
            return v;
        }
};


class NgxValue final {
    public:
        NgxValue() = default;
        ~NgxValue() = default;
    public:
        template<typename T>
        static bool isvalid(const T& v) {
            return v == static_cast<T>(NgxUnsetValue::get());
        }

        template<typename T, typename U>
        static void init(T& x, const U& v) {
            if (isvalid(x)) {
                x = v;
            }
        }

        // T = U ok ? U : V;
        template<typename T, typename U, typename V>
        static void merge(T& c, const U& p, const V& d) {
            if(isvalid(c)) {
                c = isvalid(p) ? d : p;
            }
        }

};

#endif