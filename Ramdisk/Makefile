# 双击 gmake.exe 执行解压，或者把gmake.exe 添加到开机启动

all:    # 解压目录到 Ramdisk盘 R:

	@7z x R_dir.zip -oR:\   -y > R:\Makefile.log
	@7z x cache.7z -oR:\    -y    >> R:\Makefile.log
#	gmake chrome
#	gmake qq
#	gmake adobe
	gmake soft
#	gmake CORELDRAW9


	
adobe:  # adobe 软件配置
	@echo Adobe AICS3 IDCS3 PS6 软件 解压到Ramdisk盘 ...........
	@7z x PS6.7z -oR:\adobe\   -y >> R:\Makefile.log
	@echo .............Adobe PS6  .....................OK	
	@7z x AICS3.7z -oR:\adobe\   -y >> R:\Makefile.log
	@echo .............Adobe AI CS3.....................OK	
#	@7z x IDCS3.7z -oR:\adobe\   -y >> R:\Makefile.log
#	@echo .............Adobe ID CS3.....................OK

chrome: # google浏览器
	@echo Chrome 解压到Ramdisk盘 ...........
#	@7z x chrome17.7z -oR:\   -y >> R:\Makefile.log
	@7z x cache.7z -oR:\    -y    >> R:\Makefile.log
	@echo ..................................OK
qq:     
	@echo QQ 解压到Ramdisk盘 ...............
	@7z x QQ.7z -oR:\   -y >> R:\Makefile.log
	@echo ................................OK

help:  
	@echo 使用命令： gmake soft [cache] [save] [help]
	@echo -------------------------------------------------------  
	@echo 双击 gmake.exe 执行解压，或者把gmake.exe 添加到开机启动

cache:  # google浏览器缓存  设定到R:\Cache
	rd /s /q "%USERPROFILE%\Local Settings\Application Data\Google\Chrome\User Data\Default\Cache"
	mklink /d "%USERPROFILE%\Local Settings\Application Data\Google\Chrome\User Data\Default\Cache"  R:\Cache

save:   # 备份cache文件夹
	del /f cache.7z
	7z a cache.7z R:\Cache -r
	
AICS5:  # AI CS5 软件配置
	@echo Adobe AICS5 软件 解压到Ramdisk盘 ...........
	@7z x AICS5.7z -oR:\adobe\   -y >> R:\Makefile.log
	@echo .............Adobe AI CS5.....................OK

IDCS3:  # ID CS3 软件配置
	@echo Adobe IDCS3 软件 解压到Ramdisk盘 ...........
	@7z x IDCS3.7z -oR:\adobe\   -y >> R:\Makefile.log
	@7z x InDesignCS2.7z -oR:\adobe\   -y >> R:\Makefile.log
	@echo .............Adobe ID CS3.....................OK

CORELDRAW9:  # CorelDRAW 9 软件配置
	@echo CorelDRAW 9  软件 解压到Ramdisk盘 ...........
	@7z x Graphics9.7z -oR:\corel\   -y >> R:\Makefile.log
	@echo .............CorelDRAW 9.....................OK


	
soft:
	@7z x soft.7z -oR:\soft\   -y > R:\Makefile.log
	@taskkill /f /im QQProtect.exe



