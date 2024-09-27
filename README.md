# nginx-cache-age-module

This modules is copied of [[free-nginx 1.27.](https://freenginx.org/)] repo.

```nginx
server {
    proxy_cache my_zone;
    add_header X-Cache-Status $upstream_cache_status always;
    add_header X-Cache-Age $age always;
    proxy_cache_valid 200 10s;
    proxy_ignore_headers Cache-Control Expires;
    proxy_cache_revalidate on;
    location / {
        proxy_pass http://origin;
    }
}
```

```bash
curl -I $URL/1.jpg
X-Cache-Status: HIT
X-Cache-Age: 9

curl -I $URL/1.jpg
HTTP/1.1 200 OK
X-Cache-Status: REVALIDATED

curl -I $URL/1.jpg
X-Cache-Status: HIT
X-Cache-Age: 1
```

After REVALIDATED, age is set to 0.