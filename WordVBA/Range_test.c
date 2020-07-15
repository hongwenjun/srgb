#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main()
{
    FILE* file = fopen("村名.TXT", "r");

    int cnt = 1;
    char key[64] = "NULL";
    char line[1024];

    while (fgets(line, 1024, file) != NULL) {

        if (strcmp(line, key) == 0) {
            //  printf("%d\t%s", cnt, line);
        } else {

            strcpy(key, line);
            printf("E%d  %sL%d:", cnt - 1,   key, cnt);
            // L开始单元格 E 结束
        }

        cnt++;
    }

    fclose(file);
    return 0;
}

/*
Sub line_paste()


'  行列转换粘贴

    Range("E2:E7").Select
    Application.CutCopyMode = False
    Selection.Copy
    Range("F2").Select
    Selection.PasteSpecial Paste:=xlPasteValues, Operation:=xlNone, SkipBlanks:= _
        False, Transpose:=True

    Range("E8:E14").Select
    Application.CutCopyMode = False
    Selection.Copy
    Range("F8").Select
    Selection.PasteSpecial Paste:=xlPasteValues, Operation:=xlNone, SkipBlanks:= _
        False, Transpose:=True

End Sub


Sub line_paste2()


'  行列转换粘贴

    Dim cell As String
    cell = "E2:E7"

    Range(cell).Select
    Application.CutCopyMode = False
    Selection.Copy

    ' 选择区选一个单元格，偏移1列
    Range(cell).Resize(1).Offset(0, 1).Select
    Selection.PasteSpecial Paste:=xlPasteValues, Operation:=xlNone, SkipBlanks:= _
        False, Transpose:=True

End Sub



Sub range_test()
    Range("a1").Select
    Range("a1") = "Range单元格"

End Sub


*/
