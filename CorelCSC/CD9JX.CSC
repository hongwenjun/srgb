REM 创建 2007年8月28日 By 蘭公子
DECLARE SUB ShellExecute LIB "shell32" (BYVAL hWnd&,BYVAL Operation&,BYVAL File$,BYVAL Parameters&,BYVAL Directory&,BYVAL Show&) ALIAS "ShellExecuteA"
DECLARE SUB BJWJ
DECLARE SUB JX4J
DECLARE SUB ZX4B
DECLARE SUB WZQX
DECLARE SUB Xgs2
DECLARE SUB OPENBJ

#define DRAWOBJECT "CorelDRAW.Automation.9"

' 找到当前目录程序
CurDir$ = GetCurrFolder()
IF MID(CurDir$, LEN(CurDir$), 1) = "\" THEN
	' Make sure CurDir does not end with a backslash, since we
	' will add one.
	CurDir$ = LEFT(CurDir$, LEN(CurDir$) - 1)
ENDIF

BITMAP_LOGO$ = CurDir$+"\CD9JX.bmp"
Help_File$ = CurDir$+"\CD9JX.txt"

XZ=0
BEGIN DIALOG Dialog1 257, 117, "CorelDRAW9个性标记工具 By 蘭公子"
	OPTIONGROUP XZ
		OPTIONBUTTON  12, 5, 150, 13, "快速导入个性标记文件"
		OPTIONBUTTON  12, 20, 150, 13, "把角线放置到页面四角(请先选择角线)"
		OPTIONBUTTON  12, 35, 154, 13, "把中线放置到页边中间(请先选择中线)"
		OPTIONBUTTON  11, 50, 150, 13, "把所有文本转曲线(不需要解散群组)"
		OPTIONBUTTON  12, 65, 150, 13, "调用外部插件: 维思自动拼版7.0"
	PUSHBUTTON  20, 90, 80, 22, "开始执行所选命令"
	PUSHBUTTON  112, 90, 60, 22, "关闭"
	IMAGE  179, 8, 67, 67, BITMAP_LOGO$
	PUSHBUTTON  186, 90, 60, 22, "帮助说明"
END DIALOG

j=dialog(dialog1)
IF j=3 THEN '选择确定
 	SELECT CASE XZ

		case 0
		CALL  BJWJ
		case 1
		CALL  JX4J
		case 2
		CALL  ZX4B
		case 3
		CALL  WZQX
		case 4
		CALL  Xgs2
		case else
		'没有选择操作
	END SELECT
END IF

IF j=5 THEN '选择帮助
   IF XZ=2 THEN 
      CALL OPENBJ
   END IF
ShellExecute 0,0,"http://srgb.net.cn/",0,0,0
ShellExecute 0,0,Help_File$,0,0,0
END IF

SUB BJWJ ' "快速导入个性标记文件"
WITHOBJECT DRAWOBJECT
CurDir$ = GetCurrFolder()
IF MID(CurDir$, LEN(CurDir$), 1) = "\" THEN
	CurDir$ = LEFT(CurDir$, LEN(CurDir$) - 1)
ENDIF
BJ_File$ = CurDir$+"\CD9JX.cdr"
.GetPageSize PXSize&, PYSize& '得到页面大小
.FileImport BJ_File$, 1796, FALSE  '导入文件
.AlignToCenterOfPage 3, 1
.MoveObject 0, -(PYSize&/2+300000) '页面左下(3cm)定位
REM .ZoomToSelection
.Ungroup
END WITHOBJECT
END SUB

SUB JX4J ' "把角线放置到页面四角(请先选择角线)"
WITHOBJECT DRAWOBJECT 
Id&=.GetObjectID(0,TRUE,TRUE)
.SelectObjectOfCDRStaticID Id&
.GetSize XSize&, YSize& '得到物件大小
.GetPageSize PXSize&, PYSize& '得到页面大小
.SelectObjectOfCDRStaticID Id&
.AlignToCenterOfPage 2, 1 '左上对齐到页面中心
.MoveObject -PXSize&/2,  PYSize&/2 '左上移
.DuplicateObject 0, 0
.RotateObject -90000000, FALSE, 0, 0
.AlignToCenterOfPage 1, 1
.MoveObject  PXSize&/2,  PYSize&/2 '右上移
.DuplicateObject 0, 0
.RotateObject -180000000, FALSE, 0, 0
.AlignToCenterOfPage 2, 2
.MoveObject -PXSize&/2, -PYSize&/2 '左下移
.DuplicateObject 0, 0
.RotateObject 90000000, FALSE, 0, 0
.AlignToCenterOfPage 1, 2
.MoveObject  PXSize&/2, -PYSize&/2 '右下移
END WITHOBJECT
END SUB


SUB ZX4B ' "把中线放置到页边中间(请先选择中线)"
WITHOBJECT DRAWOBJECT 
Id&=.GetObjectID(0,TRUE,TRUE)
.SelectObjectOfCDRStaticID Id&
.GetSize XSize&, YSize& '得到物件大小
.GetPageSize PXSize&, PYSize& '得到页面大小
.SelectObjectOfCDRStaticID Id&
.AlignToCenterOfPage 3, 1 '上对齐到页面中心
.MoveObject 0,  PYSize&/2 '上移

.DuplicateObject 0, 0
.RotateObject 180000000, FALSE, 0, 0
.AlignToCenterOfPage 3, 2
.MoveObject  0,  -PYSize&/2 '下移

.DuplicateObject 0, 0
.RotateObject -90000000, FALSE, 0, 0
.AlignToCenterOfPage 2, 3
.MoveObject -PXSize&/2, 0 '左移

.DuplicateObject 0, 0
.RotateObject 180000000, FALSE, 0, 0
.AlignToCenterOfPage 1, 3
.MoveObject  PXSize&/2, 0 '右移
END WITHOBJECT
END SUB

SUB WZQX '文字曲线
WITHOBJECT DRAWOBJECT 
.SELECTOBJECTOFTYPE 4
.ConvertToCurves 
END WITHOBJECT
END SUB

SUB Xgs2 '调用外部插件
ShellExecute 0,0,"D:\Program Files\Corel\Graphics9\Draw\Scripts\Scripts\Xgs2.csb",0,0,0
END SUB

SUB OPENBJ ' "隐藏功能：编辑标记文件" 
WITHOBJECT DRAWOBJECT

CurDir$ = GetCurrFolder()
IF MID(CurDir$, LEN(CurDir$), 1) = "\" THEN
	CurDir$ = LEFT(CurDir$, LEN(CurDir$) - 1)
ENDIF
BJ_File$ = CurDir$+"\CD9JX.cdr"
.FileOpen BJ_File$
END WITHOBJECT
END SUB
