# Python 学习笔记2
# Lambda是使用lambda关键字定义, 拉姆达匿名函数语法: lambda parameters : expression

# 一个简单lambda匿名函数，它将参数的值增加三倍并返回这个新值
triple = lambda num : num * 3 # 将lambda 分配给变量
print (triple(10)) # 使用一个参数调用lambda 

# 一个简单的lambda，它将三个字符串的第一个字符连接在一起
concat_strings = lambda a, b, c : a[0] + b[0] + c[0]
print (concat_strings("World", "Wide", "Web"))

# Lambda不能具有多行表达式,lambda非常适合短的单行功能
my_lambda = lambda num : "High" if num > 50 else "Low"
print(my_lambda(60))

# 使用Lambda表达式
def calculator(operation, n1, n2):
    return operation(n1, n2)  # 使用 'operation' 参数作为函数
# 10 和 20 是参数
result = calculator(lambda n1, n2: n1 * n2, 10, 20)
# lambda将他们相乘
print (result)
print (calculator(lambda n1, n2: n1 + n2, 10, 20))


# map()和filter() 会返回一个新列表，而不更改原始列表，使用list()函数将该对象转换为列表
num_list = [0, 1, 2, 3, 4, 5]
double_list = map (lambda n : n * 2, num_list)  # 使用map()双倍的现有列表的值
print (list(double_list))

# 让我们写一个filter()函数来过滤所有大于的元素10：
numList = [30, 2, -15, 17, 9, 100]
greater_than_10 = list(filter(lambda n : n > 10, numList))
print (greater_than_10)

# 递归函数实现阶乘算法
def factorial(n):
    # Base case  # 对于阶乘0和1永远1
    if n == 0 or n == 1:
        return 1
    if n < 0:
        return -1 
    # Recursive call  # 递归调用
    return n * factorial(n - 1)
print (factorial(5))

# 循环是一种控制结构，用于执行一组指令特定次数。
# 可以在Python中使用两种类型的循环: for 或者 while
"""
for iterator in sequencd:
    1.迭代器的名称
    2.要遍历的序列
    3.要执行的一组操作

# 内置range()函数可用于创建整数序列
# range(start, end, step) """

for i in range(1,11): # A序列从1到10
    if i % 2 == 0:
        print (i, " is even")
    else:
        print (i, " is odd")

# A sequence from 1 to 10 with a step of 3 
for i in range(1,11,3): 
    print (i)

# 列表或字符串可以通过其索引进行迭代，使用for循环将列表中的每个值加倍
float_list = [2.5, 16.42, 10.77, 8.3, 34.21]
print (float_list)
for i in range(0, len(float_list)): # Iterator goes traverses to the last index of the list
    float_list[i] = float_list[i] * 2
print (float_list)

# 通过迭代器遍历列表/字符串的元素,检查有多少个元素大于10：
float_list = [2.5, 16.42, 10.77, 8.3, 34.21]
count_greater = 0
for num in float_list: # Iterator goes traverses to the last index of the list
  if num > 10:
    count_greater += 1
print (count_greater)

# 嵌套使用for循环, 假设我们要打印两个元素的总和等于某个数字n=50
n = 50
num_list = [10, 4, 23, 6, 18, 27, 47]
for n1 in num_list:
    for n2 in num_list: # Now we have two iterators
        if(n1 + n2 == n):
            print(n1, n2)

# 我们需要在循环结束之前退出循环,使用 break 关键字打破for或者while循环 
n = 50
num_list = [10, 4, 23, 6, 18, 27, 47]
result = ()
found = False # This bool will become true once a pair is found
for n1 in num_list:
    for n2 in num_list:
        if(n1 + n2 == n):
            result = (n1, n2)
            found = True # Set found to True
            break # Break inner loop if a pair is found
    if found == True:
        break # Break outer loop if a pair is found
print (result)

"""    continue 关键字
    使用continue关键字时，将跳过该特定迭代的其余部分。
    循环继续进行下一次迭代。我们可以说它不会中断循环，
    但是会跳过当前迭代中的所有代码并移至下一个代码。
"""
num_list = list(range(0, 10))
for num in num_list:
    if num == 3 or num == 6 or num == 8:
        continue
    print (num)

# while 循环使用: 找出n值超出1000之前的最大幂
n = 2 # n可以是任何数字
power = 0
val = n
while val < 1000:
    power += 1
    val *= n
print (power)

# 斐波那契数列 <Fibonacci> 0 1 1 2 3 5 8 13 ... 每个数字都是它前面两个数字的和。前两个数字是0和1
def fib(n):
  # The first and second values will always be fixedb # 前两个数字是0和1始终固定
  first = 0 
  second = 1
  if n < 1:
    return -1
  if n == 1:
    return first
  if n == 2:
    return second
    
  count = 3 # Starting from 3 because we already know the first two values
  while count <= n:
    fib_n = first + second
    first = second
    second = fib_n
    count += 1 # Increment count in each iteration
  return fib_n

n = 7
print (fib(n))

# 斐波那契数列 函数返回<Fibonacci>数列表 [0, 1, 1, 2, 3, 5, 8, 13, 21, 34]
def fib_list(n):
   # The first and second values will always be fixedb # 前两个数字是0和1始终固定
  first = 0 
  second = 1
  if n < 1: return [-1]
  if n == 1: return [0]
  if n == 2: return [0,1]
  ret = [0,1] 
  count = 3 # Starting from 3 because we already know the first two values
  while count <= n:
    fib_n = first + second
    first = second
    second = fib_n
    count += 1 # Increment count in each iteration
    ret.append(fib_n)
  return ret
n = 10
print (fib_list(n))