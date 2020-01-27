#include <stdio.h>
#include <libgen.h>


/**
 * Shell 启动本地测试demo
 * @param argc
 * @param argv
 * @return
 */
int main(int argc, char** argv)
{
    if (argc < 2)
    {
        printf("Usage:%s policy\n", basename(argv[0]));
        return 1;
    }

    // 提示
    printf("check latest  version is V1.0.1  newer than local.\n"
           "Do you  want to stop parser process? [Y/N]");

    // 选择
    char option = getchar();
    if (option == 'y' || option == 'Y')
    {
        printf("download ...........100%\n"
               "download complete.\n"
               "check md5sum : fe01e4e9a589a2ffc33fb3b415b04d42\n"
               "now restart %s...\n",
                argv[1]);
    }

    return 0;
}
