import mysql.connector

mysql_connection = mysql.connector.connect(
    user = 'root', 
    password = 'baiyue.one',
     host='192.168.1.133'
)
print(mysql_connection)