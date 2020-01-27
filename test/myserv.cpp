#include <unistd.h>
#include <stdio.h>
#include "cli_server.h"
#include <string.h>


// 内存变量演示
static int mirrorVal = 5;


/**
 * @brief Show命令回调函数
 *
 */
std::string ShowFunction(CliClient* c)
{
    char buff[1024] = {};

    if ((c->argc == 4) && !strcmp(c->argv[1], "log"))
    {
        if (!strcmp(c->argv[2], "error"))
        {
            int size = atoi(c->argv[3]);      // show log error 10

            int j = 0;
            char strerr[] = "paser:ERR resource/manhua/pkg/aio-a04e6dc_8475978.js,www.hao123.com,441900\n";
            for (int i = 0; i < size; i++)
            {
                strcpy(buff + j, strerr);
                j += strlen(strerr);
            }
        }
        else if (!strcmp(c->argv[2], "info"))
        {
            int size = atoi(c->argv[3]);      // show log error 20

            int j = 0;
            char strerr[] = "paser:INFO resource/manhua/pkg/aio-a04e6dc_8475978.js,www.hao123.com,441900\n";
            for (int i = 0; i < size; i++)
            {
                strcpy(buff + j, strerr);
                j += strlen(strerr);
            }
        }
    }//if(4)
    if ((c->argc == 2) && !strcmp(c->argv[1], "mirror"))
    {
        sprintf(buff, "parser: mirror: %d", mirrorVal);       // show mirror
    }

    // 返回结果串
    string strMsg(buff);
    return strMsg;
}


/**
 * @brief Set命令回调函数
 *
 */
std::string SetFunction(CliClient* c)
{
    if ((c->argc == 3) && !strcmp(c->argv[1], "mirror"))
    {
        mirrorVal = atoi(c->argv[2]);                     // set mirror 5
    }

    std::string strMsg("OK");
    return strMsg;
}


/// 全局命令列表
CommandPair commandTable[] = {
        { 20001, ShowFunction },
        { 20002, SetFunction }
};


/**
 * @brief 函数入口
 *
 */
int main(int argc, char** argv)
{
    CLIServer* pServer = new CLIServer("myserv", "../config/cmd.xml");

    // 注册多个函数
    int size = sizeof(commandTable) / sizeof(commandTable[0]);
    pServer->RegisterFunction(commandTable, size);

    // 启动事件循环
    pServer->Run();

    while (1)
    {
        sleep(1);
    }

    return 0;
}


