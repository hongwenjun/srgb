#include "atpch.h"
#include <string.h>
using namespace std;

const char* version_info = "sRGB.googlecode.com   sRGB色彩联盟, 给你的工作增光添彩!\n\n"
                           "Adobe AI EPS INDD 缩略图收集工具  版本代号：给力就能把思想转化成软件!\n"
                           "(C) 版权所有 2013.06 Hongwenjun (蘭公子)\n";

const char* srgb_zfb = "请捐赠sRGB色彩联盟开源项目，给力就能把思想转化成软件!\n"
                       "支付宝讨饭: http://me.alipay.com/srgb \n";

void helpinfo(const char* exefile)
{
    printf("%s\nUsage: %s  <Adobe文件.ai>  [SaveJpgFile]\n", version_info, exefile);
}

int main(int argc , char* argv[])
{

    const char* adobe_filename = "AICS5.ai";
    const char* savejpeg_filename = "AICS5.jpeg";

    char app_path[MAX_PATH] = {0};
    GetAppDir(app_path);

    char tmp_filename[MAX_PATH] = {0};
    char line_filename[MAX_PATH] = {0};


    if (argc >= 2) {
        adobe_filename = argv [1];
        strcpy(tmp_filename , GetFileBaseName(adobe_filename));
        strcat(tmp_filename , ".Thumb.jpg");
        savejpeg_filename = tmp_filename;
    }

    if (argc >= 3)
        strcpy(tmp_filename , argv [2]);

    // 没有参数，调用交互操作
    if (argc == 1) {
        printf("%s\n", version_info);

        printf("全盘搜索模式开启: 请输入搜索信息关键字， 会搜索相关的adobe文件文档(AI EPS Indd)格式.\n"
               "示例1:  2013  画册 \t"
               "示例2:  VIP客户 设计\t"
               "示例3:  C:\\印刷厂 \n\n"
               "输入您的关键字: ");



        string es_cmd;
        getline(cin , es_cmd);

        char es_exe_file[MAX_PATH] = {0};
        strcpy(es_exe_file, app_path);
        strcat(es_exe_file, "\\es.exe ");



        system("rd /s /q .\\Thumbs");
        system("md .\\Thumbs");

        es_cmd = string(es_exe_file) + " " + es_cmd + " > filelist.txt";

        int es_err = system(es_cmd.c_str());
        if (es_err == 0) {
            string filename_line;
            string filename_out;

            ifstream infile("filelist.txt");
            while (getline(infile, filename_line)) {
                adobe_filename = filename_line.c_str();
                strcpy(tmp_filename , GetFileBaseName(adobe_filename));
                filename_out = string("./Thumbs/") + tmp_filename + ".Thumb.jpg";
                savejpeg_filename = filename_out.c_str();

                bool ret = AdobeThumbnail(adobe_filename , savejpeg_filename);
                if (ret)
                    putchar('.');
            }
        } else {
            printf("\a全盘搜索模式需要调用软件: Everything\n\n请看使用说明 第5行.\n\n");
            system("pause");
            return 1;
        }

        printf("%s\n%s\n", version_info, srgb_zfb);
        system("pause");
        string start_Thumbs_path = string("%SystemRoot%\\explorer.exe ") + app_path + string("\\Thumbs");
        system(start_Thumbs_path.c_str());

        return 88;
    }


    bool ret = AdobeThumbnail(adobe_filename , savejpeg_filename);   // 执行

    if (ret)
        printf("请看 %s ----> %s\n" , adobe_filename , savejpeg_filename);
    else
        helpinfo(argv[0]);

    return 0;
}
