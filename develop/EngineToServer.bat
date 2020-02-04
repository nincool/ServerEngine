SET EnginePath=%~dp0
SET ServerPath=%~dp0..\..\Server\code\

::-------------------------------------
:: 设置过滤文件
::-------------------------------------
echo PBConvert.hpp >> list_of_exclusions.txt
echo NFMessageDefine_d.lib >> list_of_exclusions.txt
echo NFMessageDefine_d.dll >> list_of_exclusions.txt
echo NFMessageDefine_d.pdb >> list_of_exclusions.txt
echo NFMessageDefine.lib >> list_of_exclusions.txt
echo NFMessageDefine.dll >> list_of_exclusions.txt
echo NFMessageDefine.pdb >> list_of_exclusions.txt


::-------------------------------------
:: 拷贝接口文件
::-------------------------------------
copy %EnginePath%NFComm\NFPluginModule\NFIKernelModule.h %ServerPath%NFComm\NFPluginModule\
copy %EnginePath%NFComm\NFPluginModule\IGameKernel.h %ServerPath%NFComm\NFPluginModule\
copy %EnginePath%NFComm\NFPluginModule\NFIPlugin.h %ServerPath%NFComm\NFPluginModule\
copy %EnginePath%NFComm\NFPluginModule\NFIPluginManager.h %ServerPath%NFComm\NFPluginModule\
copy %EnginePath%NFComm\NFPluginModule\NFPlatform.h %ServerPath%NFComm\NFPluginModule\
copy %EnginePath%NFComm\NFPluginModule\NFGUID.h %ServerPath%NFComm\NFPluginModule\
copy %EnginePath%NFComm\NFPluginModule\KConstDefine.h %ServerPath%NFComm\NFPluginModule\
copy %EnginePath%NFComm\NFPluginModule\NFILogModule.h %ServerPath%NFComm\NFPluginModule\
copy %EnginePath%NFComm\NFPluginModule\NFIModule.h %ServerPath%NFComm\NFPluginModule\
copy %EnginePath%NFComm\NFPluginModule\IPubKernel.h %ServerPath%NFComm\NFPluginModule\
copy %EnginePath%NFComm\NFPluginModule\IRoomKernel.h %ServerPath%NFComm\NFPluginModule\
copy %EnginePath%NFComm\NFPluginModule\IPlatformKernel.h %ServerPath%NFComm\NFPluginModule\


::-------------------------------------
:: 拷贝核心文件
::-------------------------------------
xcopy %EnginePath%NFComm\NFCore\*.hpp %ServerPath%NFComm\NFCore\ /E /I /K /Y /EXCLUDE:list_of_exclusions.txt
copy %EnginePath%NFComm\NFCore\NFIObject.h %ServerPath%NFComm\NFCore\
copy %EnginePath%NFComm\NFCore\NFIProperty.h %ServerPath%NFComm\NFCore\
copy %EnginePath%NFComm\NFCore\NFIRecord.h %ServerPath%NFComm\NFCore\
copy %EnginePath%NFComm\NFCore\NFIPropertyManager.h %ServerPath%NFComm\NFCore\
copy %EnginePath%NFComm\NFCore\NFIRecordManager.h %ServerPath%NFComm\NFCore\
xcopy %EnginePath%NFComm\NFPublic\*.* %ServerPath%NFComm\NFPublic /E /I /K /Y /EXCLUDE:list_of_exclusions.txt
xcopy %EnginePath%NFComm\NFUtils\NFTool.hpp %ServerPath%NFComm\NFUtils /E /I /K /Y /EXCLUDE:list_of_exclusions.txt
xcopy %EnginePath%NFComm\NFUtils\QLOG.h %ServerPath%NFComm\NFUtils /E /I /K /Y /EXCLUDE:list_of_exclusions.txt

::-------------------------------------
:: 拷贝编译文件
::-------------------------------------
xcopy %EnginePath%_Out\Debug\*.dll %ServerPath%\_Out\Debug\ /E /I /K /Y /EXCLUDE:list_of_exclusions.txt
xcopy %EnginePath%_Out\Debug\*.pdb %ServerPath%\_Out\Debug\ /E /I /K /Y /EXCLUDE:list_of_exclusions.txt
xcopy %EnginePath%_Out\Debug\*.exe %ServerPath%\_Out\Debug\ /E /I /K /Y /EXCLUDE:list_of_exclusions.txt
xcopy %EnginePath%_Out\Debug\*.lib %ServerPath%\_Out\Debug\ /E /I /K /Y /EXCLUDE:list_of_exclusions.txt


xcopy %EnginePath%_Out\Release\*.dll %ServerPath%\_Out\Release\ /E /I /K /Y /EXCLUDE:list_of_exclusions.txt
xcopy %EnginePath%_Out\Release\*.pdb %ServerPath%\_Out\Release\ /E /I /K /Y /EXCLUDE:list_of_exclusions.txt
xcopy %EnginePath%_Out\Release\*.exe %ServerPath%\_Out\Release\ /E /I /K /Y /EXCLUDE:list_of_exclusions.txt
xcopy %EnginePath%_Out\Release\*.lib %ServerPath%\_Out\Release\ /E /I /K /Y /EXCLUDE:list_of_exclusions.txt

::-------------------------------------
:: 删除过滤文件
::-------------------------------------
del list_of_exclusions.txt

pause