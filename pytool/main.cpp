#include "pytool.h"
#include <sstream>
int main(int argc, char* argv[])
{
    map<wchar_t, wstring> pyTable;
    vector<PINYIN> textTable;

    setlocale(LC_ALL, "");
    wstring inputText = L"汉语拼音中标声调位置的規則如下：\n如果有a，则标在a上。如果没有a，但有o或e，则标在这两个字母上。这两个字母不会同时出现。\n如果也没有o和e，则一定有i、u或ü。如果i和u同时出现，则标在第二个韵母上。这是特别针对ui和iu而言的（这两个音的实际读音应该是uei和iou）。如果i和u不同时出现，则标在出现的那个韵母上。\n声调一律标原调，不标变调。但是在语音教学时可以根据需要按变调标写。\n";

    if (InitPinyindb(pyTable) == false)   // 初始化拼音表 pyTable
        return -1;

    const char* inFile = argv[1];
    wifstream wfin(inFile);  // gbk 编码的 inputText 文件
    if (argc > 1) {
        wstringstream iws;
        iws << wfin.rdbuf();
        inputText = iws.str();
    } else {
        AppHelp(); // 使用帮助
        return -8;
    }

    const char* outFile = "InDesign标记文件测试.txt";
    if (argc > 2)
        outFile = argv[2];
    wofstream wfout(outFile); // 保存结果文件

//  初始化文章文本  textTable
    for (auto it = inputText.begin(); it != inputText.end(); it++) {
        PINYIN tmp = { *it , wstring(L"") , 0 };
        textTable.push_back(tmp);
    }

// 给文章文本加拼音
    for (auto it = textTable.begin() ; it != textTable.end(); it++) {
        auto py_iter = pyTable.find(it->hz);
        if (py_iter != pyTable.end())
            it->pyyb = py_iter ->second ;
    }

//  生成InDesign标记文本
//    <CGB18030-WIN>
//    <vsn:4><fset:InDesign-Roman><pstyle:><cr:1><crstr:p>拼<cr:><crstr:><cr:1><crstr:p>拼<cr:><crstr:>不加拼音<cr:1><crstr:p>拼<cr:><crstr:>
    wstringstream oss;
    oss << L"<CGB18030-WIN>\n<vsn:4><fset:InDesign-Roman><pstyle:>";
    for (auto it = textTable.begin() ; it != textTable.end(); it++) {
        if (it->pyyb.length()) {
            oss << L"<cr:1><crstr:" << it->pyyb << L">"   // 拼音
                << it->hz << L"<cr:><crstr:>" ;        // 汉字
            // 测试文章文本加拼音
            wcout << it->hz << "(" << it->pyyb << ") ";
        } else
            oss << it->hz ;  // 没有拼音
    }
    wfout << oss.str();

    wfin.close();
    wfout.close();
    return 0;
}
// 测试文章文本加拼音
//    for (auto it = textTable.begin() ; it != textTable.end(); it++)
//        wcout << it->hz << " " << it->pyyb << " ";

// 测试拼音表
//    for (auto it = pyTable.begin() ; it != pyTable.end(); it++)
//        wcout << (*it).first << " => " << (*it).second << endl;

