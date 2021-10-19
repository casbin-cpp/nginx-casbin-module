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

#ifndef _NGX_POOL_HPP
#define _NGX_POOL_HPP

#include "NgxWrapper.hpp"

class NgxPool final : public NgxWrapper<ngx_pool_t> {
    public:
        typedef NgxWrapper<ngx_pool_t> super_type;
        typedef NgxPool this_type;
        NgxPool(ngx_pool_t* p): super_type(p) {}
        ~NgxPool() = default;

        template<typename T, bool no_exception = false>
        T* palloc() const {
            auto p = ngx_pcalloc(get(), sizeof(T));

            if(!p) {
                if(no_exception) {
                    return nullptr;
                }
                // TODO: malloc error, break down
            }
            // must construct this class here.
            return new (p) T();
        }

        template<typename T>
        T* alloc() const {
            return palloc<T, false>();
        }
        template<typename T>
        ngx_array_t* array(ngx_uint_t n = 1) const
        {
            auto p = ngx_array_create(get(), n, sizeof(T));
            // UNDO: error 
            // NgxException::require(p);

            return p;
        }
};

#endif