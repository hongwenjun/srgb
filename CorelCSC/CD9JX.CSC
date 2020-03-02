REM 创建 2007年8月28日 By  蘭雅sRGB    更新于2020年03月02日
GLOBAL BITMAP_LOGO$ , Help_File$ , BJ_File$ ,BITMAP_CD3D$, URLText$

DECLARE SUB ShellExecute LIB "shell32" (BYVAL hWnd&,BYVAL Operation&,BYVAL File$,BYVAL Parameters&,BYVAL Directory&,BYVAL Show&) ALIAS "ShellExecuteA"
DECLARE SUB BJWJ              ' 快速导入个性标记文件
DECLARE SUB JX4J              ' 把角线放置到页面四角(请先选择角线)
DECLARE SUB ZX4B              ' 把中线放置到页边中间(请先选择中线)
DECLARE SUB WZQX              ' 文字曲线
DECLARE SUB Xgs2              ' 调用外部插件
DECLARE SUB OPENBJ            ' 隐藏功能：编辑标记文件
DECLARE SUB sRGB3D            ' 3D展示盒变形插件
DECLARE SUB SetPageSize       ' 拼版物件居中设定页面大小
DECLARE SUB COLORMARK         ' 导入颜色套准标记文件
DECLARE SUB CMYKMARK          ' 放置 CMYK 四色标记
DECLARE SUB Colorstrip_Place3B    ' 把色条放置到页面三边

DECLARE FUNCTION CheckDrawDoc&()    '  检查 CorelDRAW 文档 ，是否选择物件 

' 设置 CorelDRAW 软件版本 9=9，如果 X4=14
#define DRAWOBJECT "CorelDRAW.Automation.9"

' 找到当前目录程序
CurDir$ = GetCurrFolder()
IF MID(CurDir$, LEN(CurDir$), 1) = "\" THEN
    ' Make sure CurDir does not end with a backslash, since we will add one.
    CurDir$ = LEFT(CurDir$, LEN(CurDir$) - 1)
END IF

BITMAP_LOGO$ = CurDir$+"\CD9JX.bmp"
BITMAP_CD3D$ = CurDir$+"\CD3D.bmp"
Help_File$ = CurDir$+"\CD9JX.txt"
Help_File$ = CurDir$+"\CD9JX.txt"
URLText$ = "sRGB.vicp.net"

XZ=6   ' 对话框选择功能，默认 6 =  COLORMARK

BEGIN DIALOG Dialog1 257, 143, " sRGB.vicp.net  CorelDRAW9精品工具箱 By 蘭雅sRGB"
	OPTIONGROUP XZ
		OPTIONBUTTON  12, 5, 150, 13, "快速导入个性标记文件"
		OPTIONBUTTON  12, 20, 150, 13, "把角线放置到页面四角(请先选择角线)"
		OPTIONBUTTON  12, 35, 154, 13, "把中线放置到页边中间(请先选择中线)"
		OPTIONBUTTON  11, 50, 150, 13, "把所有文本转曲线(不需要解散群组)"
		OPTIONBUTTON  12, 65, 150, 13, "调用外部插件: 自动拼版5.0画角线"
		OPTIONBUTTON  12, 80, 153, 13, "CorelDRAW9 3D展示盒变形插件"
		OPTIONBUTTON  12, 95, 153, 13, "自动完成页面和色标套准标记"
	PUSHBUTTON  20, 115, 80, 22, "开始执行所选命令"
	PUSHBUTTON  112, 115, 60, 22, "关闭"
	IMAGE  179, 8, 67, 67, BITMAP_LOGO$
	PUSHBUTTON  187, 115, 60, 22, "帮助说明"
	TEXTBOX  173, 95, 77, 13, URLText$
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
        case 5
            CALL sRGB3D
        case 6
            CALL COLORMARK
        case else
        '没有选择操作
    END SELECT
END IF

IF j=5 THEN '选择帮助
   IF XZ=2 THEN 
      CALL OPENBJ
   END IF
    ShellExecute 0,0,"http://sRGB.vicp.net/",0,0,0
    ShellExecute 0,0,Help_File$,0,0,0
END IF

SUB BJWJ     ' 快速导入个性标记文件
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
        '缩放 .ZoomToSelection
        .Ungroup
    END WITHOBJECT
END SUB

SUB JX4J   ' 把角线放置到页面四角(请先选择角线)
  WITHOBJECT DRAWOBJECT 
    Id&=.GetObjectID(0,TRUE,TRUE)
    .SelectObjectOfCDRStaticID Id&
    .GetSize XSize&, YSize& '得到物件大小
    .GetPageSize PXSize&, PYSize& '得到页面大小
    
    .SelectObjectOfCDRStaticID Id&
    .AlignToCenterOfPage 2, 1 '左上对齐到页面中心
    .MoveObject -PXSize&/2,  PYSize&/2 '左上移
    .OrderToBack ' 移到最下
    
    .DuplicateObject 0, 0
    .RotateObject -90000000, FALSE, 0, 0
    .AlignToCenterOfPage 1, 1
    .MoveObject  PXSize&/2,  PYSize&/2 '右上移
    .OrderToBack ' 移到最下
    
    .DuplicateObject 0, 0
    .RotateObject -180000000, FALSE, 0, 0
    .AlignToCenterOfPage 2, 2
    .MoveObject -PXSize&/2, -PYSize&/2 '左下移
    .OrderToBack ' 移到最下
    
    .DuplicateObject 0, 0
    .RotateObject 90000000, FALSE, 0, 0
    .AlignToCenterOfPage 1, 2
    .MoveObject  PXSize&/2, -PYSize&/2 '右下移
    .OrderToBack ' 移到最下
    
  END WITHOBJECT
END SUB

SUB Colorstrip_Place3B     ' 把色条放置到页面三边
  WITHOBJECT DRAWOBJECT 
    Id&=.GetObjectID(0,TRUE,TRUE)
    .SelectObjectOfCDRStaticID Id&
    .GetSize XSize&, YSize& '得到物件大小
    .GetPageSize PXSize&, PYSize& '得到页面大小

    IF PXSize& >= PYSize& THEN         ' 如果页面是横	
        .SelectObjectOfCDRStaticID Id&
        .AlignToCenterOfPage 2, 1 '左上对齐到页面中心
        .MoveObject -PXSize&/2+100000,  PYSize&/2 '左上移
        .OrderToBack ' 移到最下
        
        .DuplicateObject 0, 0
        .AlignToCenterOfPage 1, 1
        .MoveObject  PXSize&/2-100000,  PYSize&/2 '右上移
        .OrderToBack ' 移到最下
        
        .DuplicateObject 0, 0
        .RotateObject 90000000, FALSE, 0, 0
        .AlignToCenterOfPage 2, 2
        .MoveObject -PXSize&/2, -PYSize&/2+100000 '左下移
        .OrderToBack ' 移到最下
        
        .DuplicateObject 0, 0
        '  .RotateObject -180000000, FALSE, 0, 0
        .AlignToCenterOfPage 1, 2
        .MoveObject  PXSize&/2, -PYSize&/2+100000 '右下移
        .OrderToBack ' 移到最下
    ENDIF

    IF PXSize& < PYSize& THEN         ' 如果页面是竖	
        .SelectObjectOfCDRStaticID Id&
        .AlignToCenterOfPage 2, 1 '左上对齐到页面中心
        .MoveObject -PXSize&/2+100000,  PYSize&/2 '左上移
        .OrderToBack ' 移到最下
        
        .DuplicateObject 0, 0
        .AlignToCenterOfPage 2, 2
        .MoveObject -PXSize&/2+100000, -PYSize&/2 '左下移
        .OrderToBack 

        .DuplicateObject 0, 0
        .RotateObject -90000000, FALSE, 0, 0
        .AlignToCenterOfPage 1, 1
        .MoveObject  PXSize&/2,  PYSize&/2-100000 '右上移
        .OrderToBack 
        
        .DuplicateObject 0, 0
        .AlignToCenterOfPage 1, 2
        .MoveObject  PXSize&/2, -PYSize&/2+100000 '右下移
        .OrderToBack
    END IF

  END WITHOBJECT
END SUB

SUB ZX4B    ' 把中线放置到页边中间(请先选择中线)
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

SUB WZQX    ' 文字曲线
  WITHOBJECT DRAWOBJECT 
    .SELECTOBJECTOFTYPE 4
    .ConvertToCurves 
  END WITHOBJECT
END SUB

SUB Xgs2   ' 调用外部插件
    CurDir$ = GetCurrFolder()
    IF MID(CurDir$, LEN(CurDir$), 1) = "\" THEN
        CurDir$ = LEFT(CurDir$, LEN(CurDir$) - 1)
    END IF
        Xgs_File$ = CurDir$+"\Xgs5.csb"

    ShellExecute 0,0, Xgs_File$ ,0,0,0
END SUB

SUB OPENBJ ' 隐藏功能：编辑标记文件" 
  WITHOBJECT DRAWOBJECT

    CurDir$ = GetCurrFolder()
    IF MID(CurDir$, LEN(CurDir$), 1) = "\" THEN
        CurDir$ = LEFT(CurDir$, LEN(CurDir$) - 1)
    END IF
    BJ_File$ = CurDir$+"\CD9JX.cdr"
    .FileOpen BJ_File$
  END WITHOBJECT
END SUB


SUB COLORMARK     ' 导入颜色套准标记文件， 格式化MarkNameID&(5) 数组
  WITHOBJECT DRAWOBJECT

    IF CheckDrawDoc()=1 THEN  STOP
    
    .SuppressPainting TRUE  ' 禁止屏幕更新

    CALL SetPageSize    ' 按拼版物件尺寸 设定页面大小

    CurDir$ = GetCurrFolder()
    IF MID(CurDir$, LEN(CurDir$), 1) = "\" THEN
        CurDir$ = LEFT(CurDir$, LEN(CurDir$) - 1)
    END IF
    BJ_File$ = CurDir$+"\ColorMark.cdr"
    .GetPageSize PXSize&, PYSize& '得到页面大小
    .FileImport BJ_File$, 1796, FALSE  '导入文件
    .AlignToCenterOfPage 3, 1
    .MoveObject 0, -(PYSize&/2+300000) '页面左下(3cm)定位
    '缩放 .ZoomToSelection
    .Ungroup

    DIM MarkNameID&(5)   ' 存储颜色套准标记Id号
    .SelectAllObjects    ' 先全选，就会从最后导入的物件选择了

    FOR ix = 1 TO 10 STEP 1	     ' 存储颜色套准标记 到 MarkNameID&(5) 数组中
        .SelectNextObject 0
        MarkName$ = .GetObjectData( "MarkName" )

        IF MarkName$ = "CenterLine" THEN
            MarkNameID&(1) = .GetObjectID(0,TRUE,TRUE)
            .ClearObjectData ( "MarkName" )

        ELSEIF MarkName$ = "TargetLine" THEN
            MarkNameID&(2) = .GetObjectID(0,TRUE,TRUE)
            .ClearObjectData ( "MarkName" )

        ELSEIF MarkName$ = "ColorStrip" THEN
            MarkNameID&(3) = .GetObjectID(0,TRUE,TRUE)
            .ClearObjectData ( "MarkName" )

        ELSEIF MarkName$ = "ColorMark" THEN
            MarkNameID&(4) = .GetObjectID(0,TRUE,TRUE)
            .ClearObjectData ( "MarkName" )
        END IF
    NEXT ix
    
    FOR ix = 1 TO 5 STEP 1
        Id& =MarkNameID&(ix)
        .SelectObjectOfCDRStaticID Id&

         SELECT CASE ix
            case 1
                CALL  ZX4B
            case 2
                CALL  JX4J
            case 3
                CALL  Colorstrip_Place3B
            case 4
                CAll  CMYKMARK
            case  else
            '没有操作
        END SELECT

    NEXT ix

    .ResumePainting 	' 恢复屏幕更新
  END WITHOBJECT
END SUB

SUB CMYKMARK     '  放置 CMYK 四色标记
 WITHOBJECT DRAWOBJECT
    Id&=.GetObjectID(0,TRUE,TRUE)
        .SelectObjectOfCDRStaticID Id&
        .GetPageSize PXSize&, PYSize& '得到页面大小

    IF PXSize& >= PYSize& THEN         ' 如果页面是横
        .AlignToCenterOfPage 3, 1
        .MoveObject 0, -(PYSize&/2 - 23000)  ' 上移2.3mm参数 为了移动到页面内
        '缩放 .ZoomToSelection     ' 方便修改文件名
        .ZoomOut		
    END IF

    IF PXSize& < PYSize& THEN         ' 如果页面是竖
        .RotateObject -90000000, FALSE, 0, 0
        .AlignToCenterOfPage 2, 3
        .MoveObject -PXSize&/2, 0    '左移
        .MoveObject 0,  20000     ' Y 坐标上移动 2mm
        .ZoomOut		
    END IF

    .Ungroup                  ' 解散群组
    .UnSelectAll
 END WITHOBJECT
END SUB

'''  拼版工序--按物件设定页面大小  创建 2012年9月11日 By 蘭雅sRGB
SUB SetPageSize    '拼版物件居中设定页面大小
  WITHOBJECT  DRAWOBJECT
    .Group                  ' 群组
    .Separate               ' 分离物件
    .GetSize XSize&, YSize& '得到物件大小

    ' 所选物件居中
    Id&=.GetObjectID(0,TRUE,TRUE)
    .SelectObjectOfCDRStaticID Id&
    .AlignToCenterOfPage 3, 3 ' 居中对齐
    .Ungroup                  ' 解散群组
    .UnSelectAll              ' 取消选择所有物件

    ' 设定页面大小 为物件大小
    XSize& = int(XSize& / 10000 + 0.5) * 10000   ' 尺寸 mm取整
    YSize& = int(YSize& / 10000 + 0.5) * 10000
    .SetCurrentPageSize  XSize&, YSize&      ' 设定页面大小为 物件大小取整

    ' 画一个矩形        Top       Left      Bottom     Right   CornerRadius(圆角半径可选四个参数)
    .CreateRectangle YSize&/2, -XSize&/2, -YSize&/2, XSize&/2,   0   ' 相对于中心的 顶-左-下-右 四个坐标 画矩形
    .ApplyOutline 1500, 1, 0, 0, 100, 0, 0, -1, -1, FALSE, 2, 0, FALSE   ' 0.15mm轮廓线

    .StoreColor 5020, 100, 100, 100, 100, 0, 0, 0, 0            ' 拼版色
    .SetOutlineColor                                            ' 设置轮廓颜色
    .OrderToBack 										' 移到最下

  END WITHOBJECT
END SUB

SUB sRGB3D  ' 3D展示盒变形插件 
    CD3D=0
    BEGIN DIALOG CD3D_Dialog 227, 131, "CorelDRAW9 3D展示盒变形插件 By 蘭雅sRGB"
        OPTIONGROUP CD3D
            OPTIONBUTTON  12, 5, 120, 13, "正面变形(请先选择正面物件)"
            OPTIONBUTTON  12, 20, 120, 13, "侧面变形(请先选择侧面物件)"
            OPTIONBUTTON  11, 35, 120, 13, "顶盖变形(请先选择顶盖物件)"
        PUSHBUTTON  15, 101, 80, 22, "执行所选变形命令"
        PUSHBUTTON  106, 101, 60, 22, "关闭"
        IMAGE  152, 8, 67, 67, BITMAP_LOGO$
        IMAGE  6, 50, 136, 47, BITMAP_CD3D$
        TEXTBOX  146, 80, 77, 13, URLText$
    END DIALOG

    J3D=dialog(CD3D_Dialog)

    IF J3D=3 THEN '选择确定
        SELECT CASE CD3D
            case 0
            '正面变形
    WITHOBJECT DRAWOBJECT
        .StretchObject 0.951, 1, 0.937, 1, FALSE, FALSE, 1
        .SkewObject 0, 7000000, 1
    END WITHOBJECT
            case 1
    '侧面变形
    WITHOBJECT DRAWOBJECT
        .StretchObject 0.468, 1, 0.937, 1, FALSE, FALSE, 3
        .SkewObject 0, -45000000, 3
    END WITHOBJECT
            case 2
    '顶盖变形
    WITHOBJECT DRAWOBJECT
        .StretchObject 0.951, 1, 0.525, 1, FALSE, FALSE, 1
        .SkewObject 41700000, 7000000, 1
    END WITHOBJECT
            case else
            '没有选择操作
        END SELECT
    END IF
END SUB

'  检查 CorelDRAW 文档 ，是否选择物件 
FUNCTION CheckDrawDoc&()
    ret&=0
    WITHOBJECT DRAWOBJECT
        IF NOT .IsDocument() THEN 
            MessageBox "请打开文档再操作","请打开文件再操作",0
            ret=1
        END IF
        IF ret=0 THEN
            IF NOT .IsSelection() THEN 
                MessageBox "当前文档没有物件选择，请选择拼版物件再使用工具!","请选择拼版物件",0
                ret=1
            END IF
        END IF
    END WITHOBJECT
    CheckDrawDoc=ret
END FUNCTION
