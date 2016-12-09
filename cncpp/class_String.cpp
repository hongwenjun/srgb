//    C++笔试题 String类的实现 三大复制控制函数
//
//    这个在面试或笔试的时候常问到或考到。
//
//    已知类String的原型为：
class String
{
public:
     String(const char *str = NULL);// 普通构造函数
     String(const String &other);    // 拷贝构造函数
     ~ String(void);    // 析构函数
     String & operate =(const String &other);// 赋值函数
private:
     char *m_data;// 用于保存字符串
};
//请编写String的上述4个函数。


//普通构造函数
String::String(const char *str)
{
        if(str==NULL)
        {
                m_data = new char[1]; // 得分点：对空字符串自动申请存放结束标志'\0'的//加分点：对m_data加NULL 判断
                *m_data = '\0';
        }
        else
        {
         int length = strlen(str);
         m_data = new char[length+1]; // 若能加 NULL 判断则更好
         strcpy(m_data, str);
        }
}
// String的析构函数
String::~String(void)
{
        delete [] m_data; // 或delete m_data;
}
//拷贝构造函数
String::String(const String &other) 　　　// 得分点：输入参数为const型
{
        int length = strlen(other.m_data);
        m_data = new char[length+1]; 　　　　//加分点：对m_data加NULL 判断
        strcpy(m_data, other.m_data);
}
//赋值函数
String & String::operate =(const String &other) // 得分点：输入参数为const型
{
        if(this == &other)                  　　//得分点：检查自赋值
                return *this;
        delete [] m_data;            　　　　//得分点：释放原有的内存资源
        int length = strlen( other.m_data );
        m_data = new char[length+1]; 　//加分点：对m_data加NULL 判断
        strcpy( m_data, other.m_data );
        return *this;     　　　　　　　　//得分点：返回本对象的引用

}

//    剖析：
//
//    能够准确无误地编写出String类的构造函数、拷贝构造函数、赋值函数和析构函数的面试者至少已经具备了C++基本功的60%以上！
//    在这个类中包括了指针类成员变量m_data，当类中包括指针类成员变量时，一定要重载其拷贝构造函数、赋值函数和析构函数，
//    这既是对C++程序员的基本要求，也是《Effective　C++》中特别强调的条款。
//    仔细学习这个类，特别注意加注释的得分点和加分点的意义，这样就具备了60%以上的C++基本功！
