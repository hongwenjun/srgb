import  ipdb, ipaddress
from flask import Flask
# export FLASK_ENV=development
# flask run --host=0.0.0.0

db = ipdb.BaseStation("qqwry.ipdb")
app = Flask(__name__)

@app.route("/")
def hello_world():
    return "<p>Hello, World!</p>"

@app.route('/ip/<ipaddr>')
def show_ip_profile(ipaddr): 
    ip = ipaddr
    try:
        ipaddress.ip_address(ip).is_global
        # print(ip, db.find(ip, "CN"))
        city = db.find(ip, "CN")
        ipaddr =  ip + " @" + city[0] + city[1] + city[2] + "\n"
    except:
        pass
    return ipaddr
