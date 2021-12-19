console.log('Hello World')

function printf(...arg) {
    return console.log(...arg);
}

printf("你好世界！%s","增加参数")

printf("%s: %d + %d = %d", "公式", 88, 22, 88+22)
