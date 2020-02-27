import time
import json
import socket
import requests
import operator as op

from xml.dom.minidom import parse
import xml.dom.minidom

import pymysql
import uuid

db = pymysql.connect("localhost", "root", "raspberry", "speedtest")
cursor = db.cursor()
batch_id = uuid.uuid4()

server_list = requests.get('http://c.speedtest.net/speedtest-servers.php')
with open('/tmp/speedtest-servers.php', 'wb') as f:
    f.write(server_list.content)

doc = xml.dom.minidom.parse('/tmp/speedtest-servers.php')
root = doc.documentElement
techs = root.getElementsByTagName('server')
for tech in techs:
    cc = tech.getAttribute('cc')
    country = tech.getAttribute('country')
    if op.eq(cc, 'CN') and op.eq(country, 'China'):
        ip = socket.gethostbyname(tech.getAttribute('host')[:-5])

        cursor.execute("SELECT * FROM geoip WHERE ip = '%s'" % ip)
        data = cursor.fetchone()
        if data == None:
            ipip = requests.get('http://freeapi.ipip.net/' + ip)
            r = json.loads(ipip.text)
            cursor.execute("""INSERT INTO geoip(ip,country,province,city,isp)
            VALUES ('%s','%s', '%s', '%s', '%s')""" % (ip, r[0], r[1], r[2], r[4]))
            db.commit()
            location = r[0] + r[1] + r[2] + r[4]
        else:
            location = data[2] + data[3] + data[4] + data[5]

        try:
            try:
                c_start = time.time()
                r = requests.post('http://' + tech.getAttribute('host') +
                                  '/speedtest/upload.php', timeout=(10, 10), files={"file": ('filename', open('test.bin', 'rb'))})

                c_end = time.time()
                c_diff = c_end - c_start
                up_speed = round((1048576 / c_diff), 2)
                up_status = 'HTTP 200'
            except requests.exceptions.ConnectTimeout:
                up_status = 'ConnectTimeout'
            except requests.exceptions.ChunkedEncodingError:
                up_status = 'ChunkedEncodingError'
            except requests.exceptions.ConnectionError:
                up_status = 'ConnectionError'
            except requests.exceptions.ReadTimeout:
                up_status = 'ReadTimeout'
            except socket.timeout:
                up_status = 'ContentTimeout'

            f = open('/tmp/speedtest.tmp', 'wb')

            c_start = time.time()
            # 一秒不能建立连接和一秒都下载不了首字节的,就会发生超时.
            r = requests.get('http://' + tech.getAttribute('host') +
                             '/speedtest/random4000x4000.jpg', timeout=(1, 1), stream=True)
            total_length = r.headers.get('content-length')
            dl = 0
            for chunk in r.iter_content(1024):
                dl += len(chunk)
                f.write(chunk)

            if r.status_code == 200:
                c_end = time.time()
                c_diff = c_end - c_start
                dl_speed = round((dl / c_diff), 2)

                if op.eq(up_status, 'HTTP 200'):
                    cursor.execute("""INSERT INTO result(ip,
                    download, upload, error,batch_id)
                    VALUES ('%s', %d, %d, '%s','%s')""" % (ip, int(dl_speed), int(up_speed), 'HTTP 200 / ' + up_status, batch_id))

                    db.commit()

                    print(location + ' 下载速度:' + str(round(dl_speed / 1048576, 2)) +
                          'MB/s 上传速度' + str(round(up_speed / 1048576, 2)) + 'MB/s')
                else:
                    cursor.execute("""INSERT INTO result(ip,
                    download, upload, error,batch_id)
                    VALUES ('%s', %d, %d, '%s','%s')""" % (ip, int(dl_speed), int(0.0), 'HTTP 200 / ' + up_status, batch_id))

                    db.commit()

                    print(location + ' 下载速度:' + str(round(dl_speed / 1048576, 2)) +
                          'MB/s 上传阻断!')
            else:
                cursor.execute("""INSERT INTO result(ip,
                download, upload, error,batch_id)
                VALUES ('%s', %d, %d, '%s','%s')""" % (ip, 0, 0, 'HTTP ' + str(r.status_code), batch_id))

                db.commit()

            f.close()

        except requests.exceptions.ConnectTimeout:
            cursor.execute("""INSERT INTO result(ip,
            download, upload, error,batch_id)
            VALUES ('%s', %d, %d, '%s','%s')""" % (ip, 0, 0, 'ConnectTimeout / ' + up_status, batch_id))

            db.commit()
        except requests.exceptions.ChunkedEncodingError:
            cursor.execute("""INSERT INTO result(ip,
            download, upload, error,batch_id)
            VALUES ('%s', %d, %d, '%s','%s')""" % (ip, 0, 0, 'ChunkedEncodingError / ' + up_status, batch_id))

            db.commit()
        except requests.exceptions.ConnectionError:
            cursor.execute("""INSERT INTO result(ip,
            download, upload, error,batch_id)
            VALUES ('%s', %d, %d, '%s','%s')""" % (ip, 0, 0, 'ConnectionError / ' + up_status, batch_id))

            db.commit()
        except requests.exceptions.ReadTimeout:
            cursor.execute("""INSERT INTO result(ip,
            download, upload, error,batch_id)
            VALUES ('%s', %d, %d, '%s','%s')""" % (ip, 0, 0, 'ReadTimeout / ' + up_status, batch_id))

            db.commit()
        except socket.timeout:
            cursor.execute("""INSERT INTO result(ip,
            download, upload, error,batch_id)
            VALUES ('%s', %d, %d, '%s','%s')""" % (ip, 0, 0, 'ContentTimeout / ' + up_status, batch_id))

            db.commit()
