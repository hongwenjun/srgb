#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/*     Output the given string as a quoted according to C or TCL quoting rules.
 *     输入字符串，输出成C或者TCL转移字符的(带双引号)字符串.
 */
static void output_c_string(FILE* out, const char* z)
{
    unsigned int c;
    fputc('"', out);
    while ((c = *(z++)) != 0) {
        if (c == '\\') {
            fputc(c, out);
            fputc(c, out);
        } else if (c == '\t') {
            fputc('\\', out);
            fputc('t', out);
        } else if (c == '\n') {
            fputc('\\', out);
            fputc('n', out);
        } else if (c == '\r') {
            fputc('\\', out);
            fputc('r', out);
        } else if (!isprint(c)) {
            fprintf(out, "\\%03o", c & 0xff);
        } else {
            fputc(c, out);
        }
    }
    fputc('"', out);
}


/*     Output the given string with characters that are special to HTML escaped.
 *     输入字符串，输出成带HTML转移字符的字符串.
 */

static void output_html_string(FILE* out, const char* z)
{
    int i;
    while (*z) {
        for (i = 0;   z[i]
                && z[i] != '<'
                && z[i] != '&'
                && z[i] != '>'
                && z[i] != '\"'
                && z[i] != '\'';
                i++) {}
        if (i > 0) {
            fprintf(out, "%.*s", i, z);
        }
        if (z[i] == '<') {
            fprintf(out, "&lt;");
        } else if (z[i] == '&') {
            fprintf(out, "&amp;");
        } else if (z[i] == '>') {
            fprintf(out, "&gt;");
        } else if (z[i] == '\"') {
            fprintf(out, "&quot;");
        } else if (z[i] == '\'') {
            fprintf(out, "&#39;");
        } else {
            break;
        }
        z += i + 1;
    }
}

/*
** Output the given string as a hex-encoded blob (eg. X'1234' )
*/
static void output_hex_blob(FILE* out, const void* pBlob, int nBlob)
{
    int i;
    char* zBlob = (char*)pBlob;
    fprintf(out, "X'");
    for (i = 0; i < nBlob; i++) { fprintf(out, "%02x", zBlob[i] & 0xff); }
    fprintf(out, "'");
}

/*
** Return a pathname which is the user's home directory.  A
** 0 return indicates an error of some kind.
*/

static char* find_home_dir(void)
{
    static char* home_dir = NULL;
    if (home_dir) return home_dir;

#if !defined(_WIN32) && !defined(WIN32) && !defined(_WIN32_WCE) && !defined(__RTP__) && !defined(_WRS_KERNEL)
    {
        struct passwd* pwent;
        uid_t uid = getuid();
        if ((pwent = getpwuid(uid)) != NULL) {
            home_dir = pwent->pw_dir;
        }
    }
#endif

#if defined(_WIN32_WCE)
    /* Windows CE (arm-wince-mingw32ce-gcc) does not provide getenv()
     */
    home_dir = "/";
#else

#if defined(_WIN32) || defined(WIN32)
    if (!home_dir) {
        home_dir = getenv("USERPROFILE");
    }
#endif

    if (!home_dir) {
        home_dir = getenv("HOME");
    }


#endif /* !_WIN32_WCE */

    if (home_dir) {
        int n = strlen(home_dir) + 1;
        char* z = (char * )malloc(n);
        if (z) memcpy(z, home_dir, n);
        home_dir = z;
    }

    return home_dir;
}


int main()
{
    char str[100];
    printf("请输入一行代码!\n");
    fgets(str, 100, stdin);

    fprintf(stdout , "\n%s\n\n" , str);
    output_html_string(stdout , str);
    printf("\n\n");
    output_c_string(stdout , str);
    printf("\n\n");
    output_hex_blob(stdout , str , 100);

    printf("\n\n%s", find_home_dir());

    return 0;
}

/* window 环境变量
C:\Documents and Settings\Administrator> set
ALLUSERSPROFILE=C:\Documents and Settings\All Users
APPDATA=C:\Documents and Settings\Administrator\Application Data
CLIENTNAME=Console
CommonProgramFiles=C:\Program Files\Common Files
COMPUTERNAME=DEEPIN-2E8C6099
ComSpec=C:\WINDOWS\system32\cmd.exe
FP_NO_HOST_CHECK=NO
HOMEDRIVE=C:
HOMEPATH=\Documents and Settings\Administrator
LOGONSERVER=\\DEEPIN-2E8C6099
NUMBER_OF_PROCESSORS=1
OS=Windows_NT
Path=C:\WINDOWS\system32;C:\WINDOWS;C:\WINDOWS\System32\Wbem;C:\Python27
PATHEXT=.COM;.EXE;.BAT;.CMD;.VBS;.VBE;.JS;.JSE;.WSF;.WSH
PROCESSOR_ARCHITECTURE=x86
PROCESSOR_IDENTIFIER=x86 Family 15 Model 44 Stepping 2, AuthenticAMD
PROCESSOR_LEVEL=15
PROCESSOR_REVISION=2c02
ProgramFiles=C:\Program Files
PROMPT=$P$G
SESSIONNAME=Console
SystemDrive=C:
SystemRoot=C:\WINDOWS
TEMP=R:\TEMP
TMP=R:\TEMP
USERDOMAIN=DEEPIN-2E8C6099
USERNAME=Administrator
USERPROFILE=C:\Documents and Settings\Administrator
windir=C:\WINDOWS
*/
