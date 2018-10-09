###   git-bash 的 .bashrc 文件内容
```bash
# MINGW64(git_bash) 解决 ping 和 ipconfig 乱码
alias ping='_win_ping() { winpty ping $@ ; }; _win_ping'
alias ipconfig='_win_ipconfig() { winpty ipconfig $@ ; }; _win_ipconfig'


alias gcc='_win_gcc() { winpty gcc $@ ; }; _win_gcc'
alias g++='_win_g++() { winpty g++ $@ ; }; _win_g++'
alias cl='_win_cl() { winpty cl $@ ; }; _win_cl'

# 添加 youtube-dl 和 ffmpeg 可执行文件的路径，用 ：分割
PATH=$PATH:/c/soft/youtube-dl:/c/soft/MP4BOX_ffmpeg/bin:/c/CodeBlocks/MinGW64/bin
'''
