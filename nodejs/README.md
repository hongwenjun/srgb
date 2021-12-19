## 关于 Node.js®   官方网站 https://nodejs.org/zh-cn/
- 作为一个异步事件驱动的 JavaScript 运行时，Node.js 被设计用来构建可扩展的网络应用。
- 在下面的 “Hello World” 示例中，可以并发处理许多连接，每一个连接都会触发一个回调，而当没有可做的事情时，Node.js 就会进入休眠状态。

```
const http = require('http');

const hostname = '127.0.0.1';
const port = 3000;

const server = http.createServer((req, res) => {
  res.statusCode = 200;
  res.setHeader('Content-Type', 'text/plain');
  res.end('Hello World');
});

server.listen(port, hostname, () => {
  console.log(`Server running at http://${hostname}:${port}/`);
});
```
- 这与当今比较常见的采用操作系统线程的并发模型形成了鲜明对比。基于线程的网络效率相对较低且更难以使用。此外，由于没有锁，Node.js 的用户不用担心进程死锁的问题。Node.js 中几乎没有函数直接执行 I/O 操作（除非你使用 Node.js 标准库中的同步函数版本），其进程从不会被阻塞，因此用 Node.js 来开发可扩展系统是非常合理的。

- 如果你对上面的描述有一些不理解地方，这里有一篇专门关于阻塞对比非阻塞的文章供你参考。
- 
### 阻塞对比非阻塞一览  https://nodejs.org/zh-cn/docs/guides/blocking-vs-non-blocking/  

### Node.js 入门教程中文 非官方 http://nodejs.cn/learn
- Node.js 是一个开源和跨平台的 JavaScript 运行时环境。 它几乎是任何类型项目的流行工具！
- Node.js 在浏览器之外运行 V8 JavaScript 引擎（Google Chrome 的内核）。 这使得 Node.js 的性能非常好。
