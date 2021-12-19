// Node.js 文件系统模块
const fs = require('fs')

fs.copyFileSync('test.txt', '2.txt')




// fs 模块提供了许多非常实用的函数来访问文件系统并与文件系统进行交互,包括：
// fs.access(): 检查文件是否存在，以及 Node.js 是否有权限访问。
// fs.appendFile(): 追加数据到文件。如果文件不存在，则创建文件。
// fs.chmod(): 更改文件（通过传入的文件名指定）的权限。相关方法：fs.lchmod()、fs.fchmod()。
// fs.chown(): 更改文件（通过传入的文件名指定）的所有者和群组。相关方法：fs.fchown()、fs.lchown()。
// fs.close(): 关闭文件描述符。
// fs.copyFile(): 拷贝文件。
// fs.createReadStream(): 创建可读的文件流。
// fs.createWriteStream(): 创建可写的文件流。
// fs.link(): 新建指向文件的硬链接。
// fs.mkdir(): 新建文件夹。
// fs.mkdtemp(): 创建临时目录。
// fs.open(): 设置文件模式。
// fs.readdir(): 读取目录的内容。
// fs.readFile(): 读取文件的内容。相关方法：fs.read()。
// fs.readlink(): 读取符号链接的值。
// fs.realpath(): 将相对的文件路径指针（.、..）解析为完整的路径。
// fs.rename(): 重命名文件或文件夹。
// fs.rmdir(): 删除文件夹。
// fs.stat(): 返回文件（通过传入的文件名指定）的状态。相关方法：fs.fstat()、fs.lstat()。
// fs.symlink(): 新建文件的符号链接。
// fs.truncate(): 将传递的文件名标识的文件截断为指定的长度。相关方法：fs.ftruncate()。
// fs.unlink(): 删除文件或符号链接。
// fs.unwatchFile(): 停止监视文件上的更改。
// fs.utimes(): 更改文件（通过传入的文件名指定）的时间戳。相关方法：fs.futimes()。
// fs.watchFile(): 开始监视文件上的更改。相关方法：fs.watch()。
// fs.writeFile(): 将数据写入文件。相关方法：fs.write()。
// 关于 fs 模块的特殊之处是，所有的方法默认情况下都是异步的，但是通过在前面加上 Sync 也可以同步地工作。