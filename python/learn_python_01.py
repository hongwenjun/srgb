# Python 学习笔记
# if-elif-else 语句来检查交通信号灯状态
light = "Red"
if light == "Green":
  print ("Go")
elif light == "Yellow":
  print ("Caution")
elif light == "Red":
  print ("Stop")
else:
  print ("Incorrect light signal")

# def function_name (parameters):  关键字def用来定义函数
def minimum (first, second):
    if (first < second):
        return first
    else:
        return second
result = minimum (99, 68)
print (result)

# python 内置函数
random_string = "This is a string"
# 使用in关键字查找子字符串
print("string" in random_string)

# 查找子字符串的另一种find()方法
print(random_string.find("is")) # First instance of 'is' occurs at index 2
print(random_string.find("is", 9, 13)) # No instance of 'is' in this range

# replace()方法可用于将字符串的一部分替换为另一字符串
a_string = "Welcome to Educative!"
new_string = a_string.replace("Welcome to", "Greetings from")
print(a_string)
print(new_string)

# 可以使用upper()和lower()方法轻松更改字符串的字母大小写
print ("UpperCase".upper())
print ("LowerCase".lower())

# 要将数据转换为整数，可以使用 int()
print (int("12") * 10) # String to integer  数字字符转整数
print (int (20.5)) # Float to integer  浮点数转整数
print (int(False)) # Bool to integer  布尔值转换为整数

# print (int("Hello")) # This wouldn't work!

# ord() 此函数可用于将字符转换为其Unicode值：
print (ord('a'))
print (ord('0'))

# float() 函数将数据转换为浮点数：
print (float(24))
print (float('24.5'))
print (float(True))

# str() 要将数据转换为字符串：
print (str(12) + '.345')
print (str(False))
print (str(12.345) + ' is a string')

# str() 函数接收数据并为我们提供相应的布尔值。
print (bool(10))
print (bool(0.0))
print (bool("Hello"))

