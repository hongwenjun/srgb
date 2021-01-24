# 安装mysql连接库  pip install mysql-connector-python
import mysql.connector

mysql_connection = mysql.connector.connect(
    user = 'root', password = 'baiyue.one', host='192.168.1.133', database ='PythonDB' )

cursor = mysql_connection.cursor()

sql ="INSERT INTO Students (StudentID, Name, City) VALUES (%s, %s, %s)"
students = [
    (1, '李梅', '北京'),
    (2, '张鹏', '上海'),
    (3, '江艺平', '杭州'),
    (4, '特朗普', '洛杉矶'),
    (5, '唐鹤德', '广州'),
]
for student in students:
    cursor.execute(sql, student)      # "INSERT INTO ..."" (1, '李梅', '北京')

mysql_connection.commit()