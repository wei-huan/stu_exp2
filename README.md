# sysY-compiler
sysY语言的词法分析器和语法分析器，本项目语法分析器基于递归下降法，后续还会完成基于预测分析法的语法分析器，然后更进一步完成语义分析器，期待最终能实现在某平台运行的编译器



## 项目背景

电子科技大学软工学院2021年编译原理的实验内容，自己完成C语言子集sysY语言的词法分析器和语法分析器



## 安装
### 直接安装
```sh
git clone git@github.com:wei-huan/sysY-compiler.git
```



### docker安装

```dockerfile
```



## 使用
### 词法分析器的使用
```makefile
cd sysY_compiler
make run_man_lex ana_file=test_cases/99_register_realloc.c
```



### 语法分析器的使用

#### 不生成AST

##### 针对某个测试用例

```makefile
cd sysY_compiler
make run_check ana_file=test_cases/97_many_global_var.c
```



##### 针对全部测试用例(test_cases的全部文件)
```makefile
cd sysY_compiler
make run_check ALL=1
```



#### 生成AST

##### 针对某个测试用例

```makefile
cd sysY_compiler
make run_parser ana_file=test_cases/97_many_global_var.c
```



##### 针对全部测试用例(test_cases的全部文件)
```makefile
cd sysY_compiler
make run_parse ALL=1
```



## 主要项目负责人

[@weihuan](https://github.com/weihuan)



## 开源协议

[MIT © Richard McRichface.](./LICENSE)