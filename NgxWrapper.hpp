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

#ifndef _NGX_WRAPPER_HPP
#define _NGX_WRAPPER_HPP

#include <type_traits>

template<typename T>
class NgxWrapper {
    public:
        // define wrapperd type
        typedef typename std::remove_pointer<T>::type wrap_type;
        typedef wrap_type* wrap_pointer;
        typedef wrap_type& wrap_ref;
    private:
        wrap_pointer m_ptr = nullptr;

    protected:
        // pass pointer
        NgxWrapper(wrap_pointer p): m_ptr(p) {} 
        // pass reference
        NgxWrapper(wrap_ref r): m_ptr(&r) {}
        ~NgxWrapper() = default;

    public:
        wrap_pointer get() const {
            return m_ptr;
        }

        // don't know why
        // compare
        operator bool () const {
            return get();
        }
        // transform
        operator wrap_type () const {
            return get();
        }
        // usr -> get member
        wrap_pointer operator->() const {
            return get();
        } 

};

#endif