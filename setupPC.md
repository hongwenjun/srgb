junction 文件夹安装程序
> C:\Ramdisk\junction.exe  "C:\Program Files\Corel\Graphics9" R:\corel\Graphics9
> C:\Ramdisk\junction.exe  "C:\Program Files\Adobe\Photoshop 6.0" "R:\adobe\Photoshop 6.0"

cache:  # google浏览器缓存  设定到R:\Cache
> rd /s /q "%USERPROFILE%\Local Settings\Application Data\Google\Chrome\User Data"
> junction -d "%USERPROFILE%\Local Settings\Application Data\Google\Chrome\User Data"
> junction "%USERPROFILE%\Local Settings\Application Data\Google\Chrome\User Data"  R:\Cache

save:   # 备份cache文件夹
> rd /s/q R:\Cache\Default\Cache
> del /f cache.7z
> 7z a cache.7z R:\Cache -r

XP 自动登陆
control userpasswords2



CD C:\CodeBlocks
junction  build D:\codeblocks\build
junction  tool D:\codeblocks\tool
junction  sdk D:\codeblocks\sdk
junction  chm D:\codeblocks\chm