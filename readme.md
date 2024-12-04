ZJUT的操作系统课设的一部分  
就是实现一个Linux下面的一个简单的shell  
也就是CS162的hw-shell  

## 编译

```sh
make
```

## 运行

```sh
./shell
```

## 文件说明
 
- `shell.c`
    - shell的main
- `tokenizer.*`
    - 自带的按空格分词的库
- `shell_cmd.*`
    - 定义shell的内置指令  
- `shell_program.*`
    - 定义shell执行外部程序行为  
- `shell_signal.*`
    - 定义shell的信号处理行为  
- `loop.c`
    - 用来测试后台运行之类的需要长时间执行的任务
- `print_stdin.c`
    - 用来测试管道传递的输入的任务
