#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "rdparser.h"


#ifndef ALL

// 分析单文件
int main(int argc, char**argv){
	// fflush(stdout);
	// setvbuf(stdout, NULL, _IONBF, 0);
	// freopen("./my.log", "w", stdout); //打印到my.log文件

	++argv; --argc;
	if(argc < 1){
		printf("请输入要分析的文件\n");
		exit(1);
	}

	ana_file = fopen(argv[0], "r");

	if(ana_file == NULL){
		printf("要分析的文件不存在\n");
		exit(1);
	}

	past r = Recursive_Parse();
	ShowAst(r, 0);
	FreeAst(r);

	fclose(ana_file);
	return 0;
}

#else

// 遍历test_cases文件夹
// gcc rdparser.c man_lex.c -o rdparser
// ./rdparser /home/用户名/WorkSpace/Compile_Lab/test_cases/
int main(int argc, char* argv[]){

    DIR *dir;
    struct dirent *sourcedir;
    char *sourcefile, *copyfile;
    char sourcepath[80], objectpath[80], temppath[80];  // temppath 是strcat的参数, strcat 会改变传入的第一个参数， 如果直接传 sourcepath 第二次的参数就是错的
    char flow;

//     fflush(stdout);
//     setvbuf(stdout, NULL, _IONBF, 0);
//     freopen("./my.log", "w", stdout); //打印到my.log文件

//     /* 检查输入参数 */
//     for(int i = 1; i < argc; i++)
//         printf("\nmain函数的参数%d为%s\n",i,argv[i]);

    /* 得到源路径 */
    strcpy((char*)&sourcepath, argv[1]);

    /* 得到文件夹下的文件名 */
    dir = opendir(argv[1]);

    /* 遍历文件夹 */
    while (sourcedir = readdir(dir)){
        if (sourcedir->d_name[0] != '.'){

            /* 得到当前要分析的文件 */
            copyfile = sourcedir->d_name;
            printf("%s\n", copyfile);

            /* 得到源路径加文件 */
            strcpy((char*)&temppath, (char*)&sourcepath);
            sourcefile = strcat(temppath, copyfile);
            // printf("\nsource file:%s\n", sourcefile);

            /* 得到源文件文件指针 */
            ana_file = fopen(sourcefile, "r");
            if(ana_file == NULL){
                printf("\n源文件%s不存在, 程序退出 \n", sourcefile);
                exit(0);
            }

	    past r = Recursive_Parse();
	    ShowAst(r, 0);
	    FreeAst(r);

            /* 关闭文件 */
            fclose(ana_file);
        }
    }

    /* 关闭文件夹 */
    closedir(dir);

    return 0;
}

#endif
