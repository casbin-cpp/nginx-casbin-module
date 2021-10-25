<!--
 * @Descripttion: 
 * @version: 
 * @Author: Stonex
 * @Date: 2021-10-15 22:12:10
 * @LastEditTime: 2021-10-18 14:53:43
-->
# Nginx-casbin-module

### Compile

+ Install casbin lib in [casbin-cpp](https://github.com/casbin/casbin-cpp).

+ Then you will get the `libcasbin.a` and `casbin` head file in your path, check the path that your compiler can find it. For example.

  ![image-20211015170159781](https://picture-table.oss-cn-beijing.aliyuncs.com/img/image-20211015170159781.png)

+ Then you can download [nginx source code](http://nginx.org/en/download.html), I used `ngix-release-1.19 `.
+ `tar -zxvf nginx-release-1.19.0.tar.gz && cd nginx-release-1.19.0 `.
+ Nignx is written by c, but `libcasbin` is a cpp lib. So we must change the compile shell to ensure the cpp environment. You can ref this [repository](https://github.com/chronolaw/ngx_cpp_dev#usage), but you must use cpp std17.
+ After your change you can compile this module with `casbin` by `./config --add-module=module_path`.
+ Finally `make`  you will get `nginx` with `casbin` model.

#### Dynamic modules
+ If you want generate dynamic modules, you can generate makefile by `./conifg --add-dynamic-module=module_path`.
+ And you will get `ngx_http_casbin_module.so` in `nignx/objs/`.
+ Then you can choose whether load this dynamic module by `load_module ./objs/ngx_http_casbin_module.so;`.

### Feature

+ This module is base in `NGX_HTTP_ACCESS_PHASE` phase.
+ Only support `get` method.
+ Only support `location` area.
+ You can use by pass `request policy` in url.
+ Only support `file adopter` current.

### Usage

#### nginx-conf-Command

+ `casbin [on/off]`:Open or close cabin module.
+ `casbin [adopter] [model_path] [policy_path]`.

``` yaml
master_process off;
daemon off;

#user  nobody;
worker_processes  1;

error_log stderr debug;

# load dynamic library
# load_module ./objs/ngx_http_casbin_module.so;

events {
    worker_connections  1024;
}

http {
    # include       mime.types;
    default_type  application/octet-stream;

    sendfile        on;

    keepalive_timeout  60;

    #main server
    server {
        listen       9999;
        server_name  localhost;

        set $var1 "xyz-abc";

        location /test {
            casbin on;
            casbin_adopter File ../config/rbac_with_domains_model.conf ../config/rbac_with_domains_policy.csv;
            # return 200 "in test\n";
        }
    }
}
```

#### basic model

+ url1:`http://ip:port/test/?sub=alice&obj=data1&act=read`->Allow policy
+ url2:`http://up:port/test/?sub=alice&obj=data1&act=write`->Deny policy

![pic](https://picture-table.oss-cn-beijing.aliyuncs.com/img/pic.png)

