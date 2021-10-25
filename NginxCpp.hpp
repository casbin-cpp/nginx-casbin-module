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

#ifndef _NGX_NGINX_HEADERS_HPP
#define _NGX_NGINX_HEADERS_HPP

#include <string>   // for new
#include <string_view>
#include <algorithm>
#include <chrono>
#include <thread>

#include <nginx.h>      // for NGINX_VER...

extern "C" {

#include <ngx_http.h>

// in ngx_http_core_module.h
//#include <ngx_thread_pool.h>

#include <ngx_md5.h>
#include <ngx_sha1.h>
}

#include "NgxModule.hpp"
#include "NgxValue.hpp"
#include "NgxPool.hpp"
#include "NgxWrapper.hpp"

#include "NgxString.hpp"
#include "NgxDate.hpp"
#include "NgxLog.hpp"
#include "NgxRequest.hpp"
#include "NgxException.hpp"

#include "NgxArray.hpp"

#endif  //_NGX_COMMON_HEADERS_HPP

