import mysql.connector

class DBManager:
    def __init__(self, user, password, host,database):
        self.mysql_connection = mysql.connector.connect(user = user , password = password, host = host, database =database )
        self.cursor = self.mysql_connection.cursor()

    def insert_student(self, student):
        sql ="INSERT INTO Students (StudentID, Name, City) VALUES (%s, %s, %s)"
        self.cursor.execute(sql, student) 
        self.mysql_connection.commit()

    def show_data(self, table_name):
        self.cursor.execute(f"SELECT * FROM {table_name}")
        values = self.cursor.fetchall()
        for value in values:
            print(value)

db_manger = DBManager('root', 'baiyue.one', '192.168.1.133', 'PythonDB')
#     user = 'root', password = 'baiyue.one', host='192.168.1.133', database ='PythonDB' )

# Insert new students
students = [
    (6, '杰克', '伦敦'),
    (7, '阿三', '印度'),
    (8, '一平', '京都')
]
for student in students:
    db_manger.insert_student(student)

# Check students
db_manger.show_data("Students")