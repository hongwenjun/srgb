REM ���� 2007��8��28�� By �m����  ������2012��9��12��
GLOBAL BITMAP_LOGO$ , Help_File$ , BJ_File$ ,BITMAP_CD3D$, URLText$

DECLARE SUB ShellExecute LIB "shell32" (BYVAL hWnd&,BYVAL Operation&,BYVAL File$,BYVAL Parameters&,BYVAL Directory&,BYVAL Show&) ALIAS "ShellExecuteA"
DECLARE SUB BJWJ
DECLARE SUB JX4J
DECLARE SUB ZX4B
DECLARE SUB WZQX
DECLARE SUB Xgs2
DECLARE SUB OPENBJ
DECLARE SUB sRGB3D
DECLARE SUB SetPageSize
DECLARE SUB COLORMARK
DECLARE SUB CMYKMARK

DECLARE FUNCTION CheckDrawDoc&()

#define DRAWOBJECT "CorelDRAW.Automation.14"

' �ҵ���ǰĿ¼����
CurDir$ = GetCurrFolder()
IF MID(CurDir$, LEN(CurDir$), 1) = "\" THEN
	' Make sure CurDir does not end with a backslash, since we
	' will add one.
	CurDir$ = LEFT(CurDir$, LEN(CurDir$) - 1)
ENDIF

BITMAP_LOGO$ = CurDir$+"\CDX4JX.bmp"
BITMAP_CD3D$ = CurDir$+"\CD3D.bmp"
Help_File$ = CurDir$+"\CDX4JX.txt"
Help_File$ = CurDir$+"\CDX4JX.txt"
URLText$ = "sRGB.GoogleCode.com"

XZ=6   ' �Ի���ѡ����


BEGIN DIALOG Dialog1 257, 143, "  sRGB.GoogleCode.com CorelDRAW9��Ʒ������ By �m����"
	OPTIONGROUP XZ
		OPTIONBUTTON  12, 5, 150, 13, "���ٵ�����Ա���ļ�"
		OPTIONBUTTON  12, 20, 150, 13, "�ѽ��߷��õ�ҳ���Ľ�(����ѡ�����)"
		OPTIONBUTTON  12, 35, 154, 13, "�����߷��õ�ҳ���м�(����ѡ������)"
		OPTIONBUTTON  11, 50, 150, 13, "�������ı�ת����(����Ҫ��ɢȺ��)"
		OPTIONBUTTON  12, 65, 150, 13, "�����ⲿ���: ά˼�Զ�ƴ��7.0"
		OPTIONBUTTON  12, 80, 153, 13, "CorelDRAW9 3Dչʾ�б��β��"
		OPTIONBUTTON  12, 95, 153, 13, "�Զ����ҳ���ɫ����׼���"		
	PUSHBUTTON  20, 115, 80, 22, "��ʼִ����ѡ����"
	PUSHBUTTON  112, 115, 60, 22, "�ر�"
	IMAGE  179, 8, 67, 67, BITMAP_LOGO$
	PUSHBUTTON  187, 115, 60, 22, "����˵��"
	TEXTBOX  173, 95, 77, 13, URLText$
END DIALOG



j=dialog(dialog1)
IF j=3 THEN 'ѡ��ȷ��
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
		'û��ѡ�����
	END SELECT
END IF

IF j=5 THEN 'ѡ�����
   IF XZ=2 THEN 
      CALL OPENBJ
   END IF
ShellExecute 0,0,"https://code.google.com/p/srgb/",0,0,0
ShellExecute 0,0,Help_File$,0,0,0
END IF

SUB BJWJ ' "���ٵ�����Ա���ļ�"
WITHOBJECT DRAWOBJECT
CurDir$ = GetCurrFolder()
IF MID(CurDir$, LEN(CurDir$), 1) = "\" THEN
	CurDir$ = LEFT(CurDir$, LEN(CurDir$) - 1)
ENDIF
	BJ_File$ = CurDir$+"\CDX4JX.cdr"
	.GetPageSize PXSize&, PYSize& '�õ�ҳ���С
	.FileImport BJ_File$, 1796, FALSE  '�����ļ�
	.AlignToCenterOfPage 3, 1
	.MoveObject 0, -(PYSize&/2+300000) 'ҳ������(3cm)��λ

	.Ungroup
END WITHOBJECT
END SUB

SUB JX4J ' "�ѽ��߷��õ�ҳ���Ľ�(����ѡ�����)"
WITHOBJECT DRAWOBJECT 
	Id&=.GetObjectID(0,TRUE,TRUE)
	.SelectObjectOfCDRStaticID Id&
	.GetSize XSize&, YSize& '�õ������С
	.GetPageSize PXSize&, PYSize& '�õ�ҳ���С
	
	.SelectObjectOfCDRStaticID Id&
	.AlignToCenterOfPage 2, 1 '���϶��뵽ҳ������
	.MoveObject -PXSize&/2,  PYSize&/2 '������
	.OrderToBack ' �Ƶ�����
	
	.DuplicateObject 0, 0
	.RotateObject -90000000, FALSE, 0, 0
	.AlignToCenterOfPage 1, 1
	.MoveObject  PXSize&/2,  PYSize&/2 '������
	.OrderToBack ' �Ƶ�����
	
	.DuplicateObject 0, 0
	.RotateObject -180000000, FALSE, 0, 0
	.AlignToCenterOfPage 2, 2
	.MoveObject -PXSize&/2, -PYSize&/2 '������
	.OrderToBack ' �Ƶ�����
	
	.DuplicateObject 0, 0
	.RotateObject 90000000, FALSE, 0, 0
	.AlignToCenterOfPage 1, 2
	.MoveObject  PXSize&/2, -PYSize&/2 '������
	.OrderToBack ' �Ƶ�����
	
END WITHOBJECT
END SUB


SUB ZX4B ' "�����߷��õ�ҳ���м�(����ѡ������)"
WITHOBJECT DRAWOBJECT 
	Id&=.GetObjectID(0,TRUE,TRUE)
	.SelectObjectOfCDRStaticID Id&
	.GetSize XSize&, YSize& '�õ������С
	.GetPageSize PXSize&, PYSize& '�õ�ҳ���С
	.SelectObjectOfCDRStaticID Id&
	.AlignToCenterOfPage 3, 1 '�϶��뵽ҳ������
	.MoveObject 0,  PYSize&/2 '����
	
	.DuplicateObject 0, 0
	.RotateObject 180000000, FALSE, 0, 0
	.AlignToCenterOfPage 3, 2
	.MoveObject  0,  -PYSize&/2 '����
	
	.DuplicateObject 0, 0
	.RotateObject -90000000, FALSE, 0, 0
	.AlignToCenterOfPage 2, 3
	.MoveObject -PXSize&/2, 0 '����
	
	.DuplicateObject 0, 0
	.RotateObject 180000000, FALSE, 0, 0
	.AlignToCenterOfPage 1, 3
	.MoveObject  PXSize&/2, 0 '����
END WITHOBJECT
END SUB

SUB WZQX '��������
WITHOBJECT DRAWOBJECT 
	.SELECTOBJECTOFTYPE 4
	.ConvertToCurves 
END WITHOBJECT
END SUB

SUB Xgs2 '�����ⲿ���
	CurDir$ = GetCurrFolder()
	IF MID(CurDir$, LEN(CurDir$), 1) = "\" THEN
		CurDir$ = LEFT(CurDir$, LEN(CurDir$) - 1)
	ENDIF
		Xgs_File$ = CurDir$+"\Xgs2.csb"

	ShellExecute 0,0, Xgs_File$ ,0,0,0
END SUB

SUB OPENBJ ' "���ع��ܣ��༭����ļ�" 
WITHOBJECT DRAWOBJECT

CurDir$ = GetCurrFolder()
IF MID(CurDir$, LEN(CurDir$), 1) = "\" THEN
	CurDir$ = LEFT(CurDir$, LEN(CurDir$) - 1)
ENDIF
	BJ_File$ = CurDir$+"\CDX4JX.cdr"
	.FileOpen BJ_File$
END WITHOBJECT
END SUB


SUB COLORMARK ' ������ɫ��׼����ļ��� ��ʽ��MarkNameID&(5) ����
WITHOBJECT DRAWOBJECT

	IF CheckDrawDoc()=1 THEN  STOP
	
	.SuppressPainting TRUE  ' ��ֹ��Ļ����

	CALL SetPageSize    ' ��ƴ���� �趨ҳ���С

    CurDir$ = GetCurrFolder()
    IF MID(CurDir$, LEN(CurDir$), 1) = "\" THEN
        CurDir$ = LEFT(CurDir$, LEN(CurDir$) - 1)
    ENDIF
    BJ_File$ = CurDir$+"\ColorMark.cdr"
    .GetPageSize PXSize&, PYSize& '�õ�ҳ���С
    .FileImport BJ_File$, 1796, FALSE  '�����ļ�
    .AlignToCenterOfPage 3, 1
    .MoveObject 0, -(PYSize&/2+300000) 'ҳ������(3cm)��λ

    .Ungroup

	DIM MarkNameID&(5)   ' �洢��ɫ��׼���Id��
	.SelectAllObjects   ' ��ȫѡ���ͻ�����������ѡ����

	FOR ix = 1 TO 10 STEP 1	     ' �洢��ɫ��׼��� �� MarkNameID&(5) ������
		.SelectNextObject 0
		MarkName$ = .GetObjectData( "MarkName" )

		IF MarkName$ = "CenterLine" THEN
			MarkNameID&(1) = .GetObjectID(0,TRUE,TRUE)
			.ClearObjectData ( "MarkName" )

		ELSEIF 	MarkName$ = "TargetLine" THEN
			MarkNameID&(2) = .GetObjectID(0,TRUE,TRUE)
			.ClearObjectData ( "MarkName" )

		ELSEIF 	MarkName$ = "ColorStrip" THEN
			MarkNameID&(3) = .GetObjectID(0,TRUE,TRUE)
			.ClearObjectData ( "MarkName" )

		ELSEIF 	MarkName$ = "ColorMark" THEN
			MarkNameID&(4) = .GetObjectID(0,TRUE,TRUE)
			.ClearObjectData ( "MarkName" )

		ENDIF

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
                CALL  JX4J
            case 4
                CAll  CMYKMARK
            case else
            'û�в���
        END SELECT

	NEXT ix

	.ResumePainting 	' �ָ���Ļ����
END WITHOBJECT
END SUB


SUB CMYKMARK
WITHOBJECT DRAWOBJECT
	Id&=.GetObjectID(0,TRUE,TRUE)
    	.SelectObjectOfCDRStaticID Id&

    	.GetPageSize PXSize&, PYSize& '�õ�ҳ���С

	IF PXSize& > PYSize& THEN         ' ���ҳ���Ǻ�
		.AlignToCenterOfPage 3, 1
		.MoveObject 0, -(PYSize&/2 - 23000) 'ҳ������(5mm)��λ
		 ' �����޸��ļ���    .ZoomToSelection    
		.ZoomOut		
	ENDIF

	.Ungroup                  ' ��ɢȺ��
	.UnSelectAll
END WITHOBJECT
END SUB


SUB SetPageSize 'ƴ����������趨ҳ���С

REM  ƴ�湤��--������趨ҳ���С  ���� 2012��9��11�� By �m����

WITHOBJECT  DRAWOBJECT
    .Group                  ' Ⱥ��
    .Separate               ' �������
    .GetSize XSize&, YSize& '�õ������С

    ' ��ѡ�������
    Id&=.GetObjectID(0,TRUE,TRUE)
    .SelectObjectOfCDRStaticID Id&
    .AlignToCenterOfPage 3, 3 ' ���ж���
    .Ungroup                  ' ��ɢȺ��
    .UnSelectAll              ' ȡ��ѡ���������

    ' �趨ҳ���С Ϊ�����С
    XSize& = int(XSize& / 10000 + 0.5) * 10000   ' �ߴ� mmȡ��
    YSize& = int(YSize& / 10000 + 0.5) * 10000
    .SetCurrentPageSize  XSize&, YSize&      ' �趨ҳ���СΪ �����Сȡ��

    ' ��һ������        Top       Left      Bottom     Right   CornerRadius(Բ�ǰ뾶��ѡ�ĸ�����)
    .CreateRectangle YSize&/2, -XSize&/2, -YSize&/2, XSize&/2,   0   ' ��������ĵ� ��-��-��-�� �ĸ����� ������
    .ApplyOutline 1500, 1, 0, 0, 100, 0, 0, -1, -1, FALSE, 2, 0, FALSE   ' 0.15mm������

	.StoreColor 5020, 100, 100, 100, 100, 0, 0, 0, 0            ' ƴ��ɫ
	.SetOutlineColor                                            ' ����������ɫ
	.OrderToBack 										' �Ƶ�����

END WITHOBJECT

END SUB





SUB sRGB3D
CD3D=0
BEGIN DIALOG CD3D_Dialog 227, 131, "CorelDRAW9 3Dչʾ�б��β�� By �m����"
	OPTIONGROUP CD3D
		OPTIONBUTTON  12, 5, 120, 13, "�������(����ѡ���������)"
		OPTIONBUTTON  12, 20, 120, 13, "�������(����ѡ��������)"
		OPTIONBUTTON  11, 35, 120, 13, "���Ǳ���(����ѡ�񶥸����)"
	PUSHBUTTON  15, 101, 80, 22, "ִ����ѡ��������"
	PUSHBUTTON  106, 101, 60, 22, "�ر�"
	IMAGE  152, 8, 67, 67, BITMAP_LOGO$
	IMAGE  6, 50, 136, 47, BITMAP_CD3D$
	TEXTBOX  146, 80, 77, 13, URLText$
END DIALOG

J3D=dialog(CD3D_Dialog)

IF J3D=3 THEN 'ѡ��ȷ��
 	SELECT CASE CD3D

		case 0
		'�������
WITHOBJECT DRAWOBJECT
	.StretchObject 0.951, 1, 0.937, 1, FALSE, FALSE, 1
	.SkewObject 0, 7000000, 1
END WITHOBJECT
		case 1
'�������
WITHOBJECT DRAWOBJECT
	.StretchObject 0.468, 1, 0.937, 1, FALSE, FALSE, 3
	.SkewObject 0, -45000000, 3
END WITHOBJECT
		case 2
'���Ǳ���
WITHOBJECT DRAWOBJECT
	.StretchObject 0.951, 1, 0.525, 1, FALSE, FALSE, 1
	.SkewObject 41700000, 7000000, 1
END WITHOBJECT
		case else
		'û��ѡ�����
	END SELECT
END IF


END SUB




'============== ��� CorelDRAW �ĵ� ���Ƿ�ѡ����� =======================
FUNCTION CheckDrawDoc&()
ret&=0
WITHOBJECT DRAWOBJECT
	IF NOT .IsDocument() THEN 
		MessageBox "����ĵ��ٲ���","����ļ��ٲ���",0
		ret=1
	END IF
	IF ret=0 THEN
		IF NOT .IsSelection() THEN 
			MessageBox "��ǰ�ĵ�û�����ѡ����ѡ��ƴ�������ʹ�ù���!","��ѡ��ƴ�����",0
			ret=1
		END IF
	ENDIF
END WITHOBJECT
CheckDrawDoc=ret
END FUNCTION