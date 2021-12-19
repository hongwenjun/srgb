const fs = require('fs')

// 在 Node.js 中读取文件最简单的方式是使用 fs.readFile() 方法 ,也可以使用同步的版本 fs.readFileSync()

// fs.readFile('./hello.js', 'utf8' , (err, data) => {
//   if (err) {
//     console.error(err)
//     return
//   }
//   console.log(data)
// })


data = fs.readFileSync('./hello.js', 'utf-8')
// console.log(data)


// 在 Node.js 中写入文件最简单的方式是使用 fs.writeFile() ,也可以使用同步的版本 fs.writeFileSync()
const content = '一些内容: fs.readFile() fs.readFileSync() fs.writeFile() fs.writeFileSync()  \n'

// fs.writeFile('./test.txt', content, err => {
//   if (err) {
//     console.error(err)
//     return
//   }
//   //文件写入成功。
// })

const data2 = fs.writeFileSync('./test.txt', content)

// 追加到文件末尾的便捷方法是 fs.appendFile() 及其对应的 fs.appendFileSync()
// fs.appendFile('./test.txt', content, err => {
//     if (err) {
//         console.error(err)
//         return
//     }
// //完成！
// })

const flag = fs.appendFileSync('./test.txt', data)