### debian 安装 Nginx + PHP-FPM
```
apt install nginx php-fpm

```

### 修改 Nginx 的配置文件
```
 # Add index.php to the list if you are using PHP
index index.html index.htm index.nginx-debian.html index.php;
```

```
# pass PHP scripts to FastCGI server
    #
    location ~ \.php$ {
            include snippets/fastcgi-php.conf;
    #
    #       # With php-fpm (or other unix sockets):
            fastcgi_pass unix:/var/run/php/php7.0-fpm.sock;
    #       # With php-cgi (or other tcp sockets):
    #       fastcgi_pass 127.0.0.1:9000;
    }
```

### 修改完毕，重启 Nginx 服务

```
systemctl restart nginx

```

### 向 Nginx 的网页存放位置（默认是/var/www/html）新建一个index.php

```
<?php echo phpinfo(); ?>

```

