import ipdb, ipaddress
from flask import Flask, request, jsonify

db = ipdb.BaseStation("qqwry.ipdb")
app = Flask(__name__)

@app.route("/ip/")
@app.route("/ip/<ipaddr>")
def show_ip(ipaddr=None):
    # ip 地址为空获得浏览器客户端IP
    if ipaddr is None:
        ip = request.remote_addr
        try:
            _ip = request.headers["X-Real-IP"]
            if _ip is not None:
                ip = _ip
        except Exception as e:
            print(e)
    else:
        ip = ipaddr

    # ip地址 从纯真IP数据库 搜索城市定位
    try:
        ipaddress.ip_address(ip).is_global
        city = db.find(ip, "CN")
        ipaddr = ip + " @" + city[0] + city[1] + city[2] + city[3] + "\n"
    except Exception as e:
        print(e)

    return ipaddr

# export FLASK_ENV=development   # 调试模式: 修改代码不用重启服务
# flask run --host=0.0.0.0       # 监听所有公开的 IP
