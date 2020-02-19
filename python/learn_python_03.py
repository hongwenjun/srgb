# Python 学习笔记3
"""  Python中的数据结构 
 #  序列类型 --- list, tuple, range  有三种基本序列类型：list, tuple 和 range 对象

List :  列表（list）是由多个值组成的序列。列表中的值称为 元素（element），有时也被称为 项（item）。
class list([iterable])          可以用多种方式构建列表：
1. 使用一对方括号来表示空列表: []          2. 使用方括号，其中的项以逗号分隔: [a], [a, b, c]
3. 使用列表推导式: [x for x in iterable]    4. 使用类型的构造器: list() 或 list(iterable)

Tuple :  元组（tuple）是不可变序列，通常用于储存异构数据的多项集
class tuple([iterable])         可以用多种方式构建元组：
1. 使用一对圆括号来表示空元组: ()          2. 使用一个后缀的逗号来表示单元组: a, 或 (a,)
3. 使用以逗号分隔的多个项: a, b, c or (a, b, c)        4. 使用内置的 tuple(): tuple() 或 tuple(iterable)
 
range 对象 :  range 类型表示不可变的数字序列，通常用于在 for 循环中循环指定的次数
class range(stop)   或者  class range(start, stop[, step])      range 构造器的参数必须为整数
"""

# 创建列表  ＃ 列表的内容用 [] 方括号括起来
jon_snow = ["Jon Snow", "Winterfell", 30]
print (jon_snow)
print (jon_snow[0])   # Indexing
print (len(jon_snow))   # Length

# 列表的优点在于我们不局限于一种数据。 # 列表是可变的，这进一步扩展了它们的功能：
jon_snow = ["Jon Snow", "Winterfell", 30]
print (jon_snow[2])
jon_snow[2] += 3
print (jon_snow[2])

#  使用range()  可以使用 list() 强制转换将范围进一步转换为列表。这是一个用于创建数字列表的范围的示例
num_seq = range(0, 10) # A sequence from 0 to 9
num_list = list(num_seq) # The list() method casts the sequence into a list
print (num_list)
num_seq = range(3, 20, 3) # A sequence from 3 to 19 with a step of 3
print (list(num_seq))

# List-Ception 嵌套列表  这是另一个列表中的列表示例：
world_cup_winners = [[2006, "Italy"], [2010, "Spain"], [2014, "Germany"], [2018, "France"]]
print (world_cup_winners)

# 合并列表   Python使合并列表变得非常容易。最简单的方法是+像对字符串一样使用运算符：
part_A = [1, 2, 3, 4, 5]
part_B = [6, 7, 8, 9, 10]
merged_list = part_A + part_B
print (merged_list)    #  结果  [1, 2, 3, 4, 5, 6, 7, 8, 9, 10]
# 或者可以使用 extend() 列表的属性将一个列表的元素添加到另一个列表的末尾
part_A.extend(part_B)
print (part_A)

# 添加元素  使用 append() 方法可用于在的末尾添加新元素list。遵循模板：a_list.append(newElement)
num_list = [] # Empty list
num_list.append(1)
num_list.append(2)
num_list.append(3)
print (num_list)     #  结果  [1, 2, 3]

# 要在列表中的特定索引处添加元素，我们可以使用 insert() 方法。遵循模板： aList.insert(index, newElement)
num_list = [1, 2, 3, 5, 6]
num_list.insert(3, 4)   # Inserting 4 at the 3rd index. 5 and 6 shifted ahead
print (num_list)        # 结果  [1, 2, 3, 4, 5, 6]

# 删除元素  使用 pop() 从列表中删除最后一个元素的操作。 示例 : pop() 删除元素，将弹出的元素存储在变量中
houses = ["Gryffindor", "Hufflepuff", "Ravenclaw", "Slytherin"]
last_house = houses.pop()
print (last_house)   # 获得  Slytherin
print (houses)        # 结果  ['Gryffindor', 'Hufflepuff', 'Ravenclaw']

# 需要从列表中删除特定值，可以使用 remove() 方法。 遵循模板： aList.remove(element_to_be_deleted)
houses = ["Gryffindor", "Hufflepuff", "Ravenclaw", "Slytherin"]
houses.remove("Ravenclaw")
print (houses)           #  结果   ['Gryffindor', 'Hufflepuff', 'Slytherin']
# For nested lists   # 对于嵌套列表方法
populations = [ ["Winterfell", 10000],  ["King's Landing", 50000], ["Iron Islands", 5000] ]
populations.remove(["King's Landing", 50000])
print (populations)

# 列表切片编号
num_list = [1, 2, 3, 4, 5, 6, 7, 8]
print (num_list[2:5])
print (num_list[0::2])

# 索引搜索   With列表非常易于通过索引访问值。但是知道元素值，可以使用 index() 方法获得索引值：
cities = ["London", "Paris", "Los Angeles", "Beirut"]
print (cities.index("Los Angeles")) # It is at the 2nd index

#  如果只想验证列表中元素的存在，则可以使用in运算符：
cities = ["London", "Paris", "Los Angeles", "Beirut"]
print ("London" in cities)          # True
print ("Moscow" not in cities)  # True

# 列表排序  使用该 sort() 方法以升序对列表进行排序。
num_list = [20, 40, 10, 50.4, 30, 100, 5]
num_list.sort()
print (num_list)

"""  #  Dictionary(字典) :   字典 dict  即映射
      字典包含了一个索引的集合，被称为 键（keys） ，和一个值(values)的集合。 一个键对应一个值。
这种一一对应的关联被称为 键值对（key-value pair) ， 有时也被称为 项（item）
     字典可以通过将以逗号分隔的 键: 值 对列表包含于花括号之内来创建，也可以通过 dict 构造器来创建。
"""
# 作为演示，以下示例返回的字典均等于 {"one": 1, "two": 2, "three": 3}:
a = dict(one=1, two=2, three=3)
b = {'one': 1, 'two': 2, 'three': 3}
c = dict(zip(['one', 'two', 'three'], [1, 2, 3]))
d = dict([('two', 2), ('one', 1), ('three', 3)])
e = dict({'three': 3, 'one': 1, 'two': 2})

"""
#  集合类型 --- set, frozenset :  目前有两种内置集合类型，set 和 frozenset。 
set 类型是可变的 --- 其内容可以使用 add() 和 remove() 这样的方法来改变。 
      由于是可变类型，它没有哈希值，且不能被用作字典的键或其他集合的元素。 
frozenset 类型是不可变并且为 hashable --- 其内容在被创建后不能再改变；因此它可以被用作字典的键或其他集合的元素。
"""
# 在Python中，可以使用管道运算符 | 或 union() 方法执行联合 产生并集。
set_A = {1, 2, 3, 4}
set_B = {'a', 'b', 'c', 'd'}
print (set_A | set_B)    #  结果相同  set_A.union(set_B)     set_B.union(set_A)

# 在Python中，可以使用 & 运算符或 intersection() 方法执行交集：
set_A = {1, 2, 3, 4}
set_B = {2, 8, 4, 16}
print (set_A & set_B)   #  结果相同  set_A.intersection(set_B)  set_B.intersection(set_A)

# 在Python中，可以使用 - 运算符或 difference() 方法来找到两组之间的差异。
print (set_A - set_B)   #  或方法  set_A.difference(set_B)
print (set_B - set_A)   #  或方法  set_B.difference(set_A)

