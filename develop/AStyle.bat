:: ===================
:: -A1 	ansi ������
:: -s4 	4�ո�����
:: -C  	�ո��滻tabs
:: -S  	switch-case����
:: -w  	Ԥ��������
:: -m0 	����min-0
:: -M80 ����max -80
:: -j	if for while �Ӵ�����
:: -H	if for while ����ӿո�
:: -p	������������ӿո�
:: -U	ȥ�������ڲ��ո�
:: -D	���ű�Ե��ӿո�
:: -Y	����ע�Ͷ���
:: -k2 	ָ�����÷��ŷ��м䣻k1 ���� type; k3 ���� name
:: -W2 	1,2,3ͬ��
:: -n	�����������ʱ�ļ�
:: ע�⣬���������ִ�Сд!
:: ===================


set ASTYLE_PARAM=-A1 -s4 -c -S -w -m0 -M80 -j -U -H -p -Y -k1 -n
set ASTYLE_DIR=../../server/Tools/AStyle/bin/

cd %ASTYLE_DIR%

::a file
::set SRC_FILE=D:/Server/a.cpp
::AStyle %ASTYLE_PARAM% %SRC_FILE%

::a dir
set SRC_DIR=%~dp0/NFComm
AStyle %ASTYLE_PARAM% -r %SRC_DIR%/*.h %SRC_DIR%/*.cpp %SRC_DIR%/*.hpp

set SRC_DIR=%~dp0/NFServer
AStyle %ASTYLE_PARAM% -r %SRC_DIR%/*.h %SRC_DIR%/*.cpp %SRC_DIR%/*.hpp

pause