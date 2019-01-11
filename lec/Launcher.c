// 后台程序工具启动器
#include <windows.h>

int main(int argc, char *argv[]) {
	WinExec("kcptun-clt.exe -r \"remote_ip:port\" -l \":port\" -mode fast2", SW_HIDE);
	return 0;
}
