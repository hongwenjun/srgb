#!/usr/bin/python
#coding=utf-8
# Python 字符串操作

'''1.复制字符串 '''
#strcpy( sStr1, sStr2)
sStr1 = 'strcpy'
sStr2 = sStr1
sStr1 = 'strcpy2'
print sStr2

'''2.连接字符串'''
#strcat( sStr1, sStr2)
sStr1 = 'strcat'
sStr2 = 'append'
sStr1 += sStr2
print sStr1

'''3.查找字符'''
#strchr( sStr1, sStr2)
sStr1 = 'strchr'
sStr2 = 'r'
nPos = sStr1.index( sStr2)
print nPos

'''4.比较字符串'''
#strcmp( sStr1, sStr2)
sStr1 = 'strchr'
sStr2 = 'strch'
print cmp( sStr1, sStr2)

'''5.扫描字符串是否包含指定的字符'''
#strspn( sStr1, sStr2)  函数返回字符串str1中第一个不包含于字符串str2的字符的索引
sStr1 = '1234567812345678'
sStr2 = ''
#sStr1 and chars both in sStr1 and sStr2
print len( sStr1 and sStr2)

'''6.字符串长度'''
#strlen( sStr1)
sStr1 = 'strlen'
print len( sStr1)

'''7.将字符串中的小写字符转换为大写字符'''
#strlwr( sStr1)   strlwr
sStr1 = 'JCstrlwr'
sStr1 = sStr1. upper( )
print sStr1 , sStr1. lower( )

'''8.追加指定长度的字符串'''
#strncat( sStr1, sStr2, n)
sStr1 = '12345'
sStr2 = 'abcdef'
n = 3
sStr1 += sStr2[ 3: 3+n]
print sStr1

'''9.字符串指定长度比较'''
#strncmp( sStr1, sStr2, n)
sStr1 = '12345'
sStr2 = '123bc'
n = 3
print cmp( sStr1[ 0: n] , sStr2[ 0: n] )

'''10.复制指定长度的字符'''
#strncpy( sStr1, sStr2, n)
sStr1 = ''
sStr2 = '12345'
n = 3
sStr1 = sStr2[ 0: n]
print sStr1

'''11.字符串比较，不区分大小写'''
#stricmp( sStr1, sStr2)
sStr1 = 'abcefg'
sStr2 = 'ABCEFG'
print cmp( sStr1. upper( ) , sStr2. upper( ) )

'''12.将字符串前n个字符替换为指定的字符'''
#strnset( sStr1, ch, n)
sStr1 = '1234567890'
ch = 'r'
n = 3
print n * ch + sStr1[ 3 ]  , n * ch + sStr1[ 3: ]

'''13.扫描字符串'''
#strpbrk( sStr1, sStr2)   函数返回一个指针，它指向字符串str2中任意字符在字符串str1 首次出现的位置，如果不存在返回NULL
sStr1 = 'cekjgdklab'
sStr2 = 'gka'
nPos = - 1
for c in sStr1:
    if c in sStr2:
        nPos = sStr1. index( c)
        break
print nPos

'''14.翻转字符串'''
#strrev( sStr1)
sStr1 = 'abcdefg'
sStr1 = sStr1[ : : - 1]         # 整个区间，-1表示到倒着的区间
print sStr1

'''15.查找字符串'''
#strstr( sStr1, sStr2)
sStr1 = 'abcdefg'
sStr2 = 'cde'
print sStr1. find( sStr2)

'''16.分割字符串'''
#strtok( sStr1, sStr2)
sStr1 = 'ab,cde,fgh,ijk'
sStr2 = ','
pos = sStr1. index( sStr2)
print sStr1[ 0: pos ]
print sStr1[ pos + 1: ]
