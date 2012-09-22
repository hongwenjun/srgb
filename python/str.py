#!/usr/bin/python
#coding=utf-8

str = 'abcdef'

print str[ -1]      # 表示序列的最后一个元素
print str[ : -1]    # 返回除了最后一个项目外包含所有项目的序列切片

print str[ : : 2]   # 即 aceg,从 头每次2步
print str[ : : -1]  # 整个区间，-1表示到倒着的区间
print str[3: : -1]  # 从 索引3 开始

#    f
#    abcde
#    ace
#    fedcba
#    dcba

