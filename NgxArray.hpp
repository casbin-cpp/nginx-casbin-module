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

#ifndef _NGX_ARRAY_HPP
#define _NGX_ARRAY_HPP

#include "NgxPool.hpp"
#include "NgxWrapper.hpp"

template<typename T>
class NgxArray final: public NgxWrapper<ngx_array_t> {
    public:
        typedef NgxWrapper<ngx_array_t> super_type;
        typedef NgxArray this_type;

        typedef T value_type;
    public:
        // alloc from memory pool
        NgxArray(const NgxPool& p, ngx_uint_t n = 10): 
            super_type(p.array<T>(n)) {

            }
        // wrap ngx_array_t*
        NgxArray(ngx_array_t* arr): super_type(arr) {

        }

        NgxArray(ngx_array_t& arr): super_type(arr) {

        }
        ~NgxArray() = default;

    public:
        ngx_uint_t size() const {
            return get()?get()->nelts:0;
        }

        T& operator[](ngx_uint_t i) const {
            
            NgxException::require(i >= 0 && i < size(), NGX_ERROR, "Ngx array out of range \n");

            return elts()[i];
        }

        bool empty() const {
            return size() == 0;
        }

        void clear() const {
            get()->nelts = 0;
        }

        T& prepare() const {
            // get last unused element
            auto tmp = ngx_array_push(get());

            // tmp isn't nullptr
            NgxException::require(tmp);

            // assert(tmp);

            return *reinterpret_cast<T*>(tmp);
        }

        void push(const T& x) const {
            prepare() = x;
        }

    private:
        T* elts() const {

            return reinterpret_cast<T*>(get()->elts);
        }
};

#endif