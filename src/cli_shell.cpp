#include "cli_shell.h"
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <assert.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "cli_public.h"
#include "cli_config.h"
#include <dirent.h>
#include <sys/wait.h>



// 消息缓冲区
#define READ_INTERVAL       (20*1000)       // 20s
#define REV_BUFF_SIZE       (5*1024)        // 5k


static char* CmdGenerator(const char*, int);

static char** AutoCompletion(const char*, int, int);


/**
 * 注册自动补全函数
 */
char** AutoCompletion(const char* text, int start, int end)
{
    char** matches;
    matches = (char**) NULL;

    (void) start;
    (void) end;

    // 多级命令补全
    matches = rl_completion_matches(text, CmdGenerator);

    return (matches);
}


/**
 * 命令自动补全回调函数
 */
char* CmdGenerator(const char* text, int state)
{
    //printf("INPUT: %s\n", rl_line_buffer);
    return CliConfig::Inst()->CmdGenerator(text, state);
}


///
CLIShell::CLIShell(char* cfgPath)
        : m_Config(NULL)
{
    // 加载配置
    m_Config = CliConfig::Inst();
    m_Config->LoadConfig(cfgPath);

    // system
    ModuleInfo* pInfo = NULL;
    m_Config->GetModule("system", &pInfo);
    m_Config->SetCurrentModule(pInfo);

    // 注册自动完成函数
    rl_attempted_completion_function = AutoCompletion;
}


CLIShell::~CLIShell()
{

}


void CLIShell::ShowLogo()
{
    printf("\n\n");
    printf("    #       #  #    #   #####  #   #  #####  #      #       \n");
    printf("    ##     ##  #    #  #       #   #  #      #      #       \n");
    printf("    # #   # #   #  #   #       #   #  #      #      #       \n");
    printf("    # #   # #    #     ######  #####  #####  #      #       \n");
    printf("    #  # #  #    #          #  #   #  #      #      #       \n");
    printf("    #   #   #    #          #  #   #  #      #      #       \n");
    printf("    #   #   #    #     #####   #   #  #####  #####  #####   \n");
    printf("\n\n");
}


int CLIShell::ConnectTo(char* name)
{
    int sockfd = socket(PF_UNIX, SOCK_STREAM, 0);
    assert(sockfd > 0);

    char path[50] = {};
    sprintf(path, IPC_PATH, name);

    struct sockaddr_un server;
    server.sun_family = AF_UNIX;
    strcpy(server.sun_path, path);

    // connect
    int ret = connect(sockfd, (struct sockaddr*) &server, sizeof(server));
    if (ret < 0)
    {
        close(sockfd);
        return -1;
    }

    return sockfd;
}


/**
 * @brief 查找命令
 *
 */
int FindCommand(char* command)
{
    DIR* d;
    struct dirent* ptr;
    char temp[512];
    char* dir;

    // 获得环境变量
    char* path = getenv("PATH");

    /*分隔环境变量，且依次查找各个目录看命令是否存在*/
    sprintf(temp, ".:%s", path);
    dir = strtok(temp, ":");

    while (dir)
    {
        d = opendir(dir);
        while ((ptr = readdir(d)) != NULL)
        {
            if (strcmp(ptr->d_name, command) == 0)
            {
                closedir(d);
                return 1;
            }
        }

        closedir(d);
        dir = strtok(NULL, ":");
    }

    return 0;
}


bool CLIShell::CheckCmd(char* prompt, char* temp, CliClient* pCmd)
{
    if (!strcmp(temp, "") || pCmd == NULL)
    {
        return false;
    }

    bzero(pCmd, sizeof(CliClient));

    // 分割参数
    char* p = strtok(temp, " ");
    while (p)
    {
        if (pCmd->argc > MAX_SIZE)
        {
            break;
        }

        strcpy(pCmd->argv[pCmd->argc++], p);
        p = strtok(NULL, " ");
    }

    if (pCmd->argc == 0)
    {
        return false;
    }

    // 内部命令，切换上下文
    if (!strcasecmp(pCmd->argv[0], "use"))
    {
        ModuleInfo* pInfo = NULL;
        if (m_Config->GetModule(pCmd->argv[1], &pInfo))
        {
            m_Config->SetCurrentModule(pInfo);

            // 更改提示
            sprintf(prompt, "%s> ", pCmd->argv[1]);

            if (pInfo->fd < 0)
            {
                pInfo->fd = ConnectTo(pCmd->argv[1]);
                if (pInfo->fd < 0)
                {
                    printf(ERR_NETWORK, pCmd->argv[1]);
                    return false;
                }
            }

            printf(CMD_OK);
            return false;
        }

        printf(ERR_MODULE_CMD, pCmd->argv[1]);
        return false;
    }//if(use)

    ModuleInfo* pextern = NULL;

    // 外部命令
    if (m_Config->GetModule(pCmd->argv[0], &pextern)
            && pextern->external)
    {
        if (!FindCommand(pCmd->argv[0]))
        {
            printf(ERR_FIND_CMD, pCmd->argv[1]);
            return false;
        }

        pid_t pid = fork();
        if (pid > 0)
        {
            waitpid(pid, NULL, 0);  // 等待子进程退出
        }
        else if (pid == 0)
        {
            char* av[10] = {};      // 必须构造参数
            for (int i = 0; i < 10; i++)
            {
                if (i == pCmd->argc)
                {
                    break;
                }

                av[i] = pCmd->argv[i];
            }

            execvp(pCmd->argv[0], av);     // 执行子进程
        }

        return false;
    }//if(external)

    // 检测命令
    CMDError status = m_Config->CheckCommand(pCmd);
    switch (status)
    {
        case COUNT_ERR:
            printf(ERR_COUNT_CMD, pCmd->argv[0]);
            break;
        case UNKNOWN_ERR:
            printf(ERR_UNKNOWN_CMD, pCmd->argv[0]);
            break;
        default:
            break;
    }

    return status == CLI_OK;
}


// 命令处理
bool CLIShell::HandleCommand(char* temp, char* prompt)
{
    int repeat = 5 * 1000 * 1000 / READ_INTERVAL;     // 20s超时时间
    int len = 0;

    char pbuff[REV_BUFF_SIZE];
    CliClient pCmd;

    // 命令检测
    if (!CheckCmd(prompt, temp, &pCmd))
    {
        return false;
    }

    // 网络状况
    int fd = m_Config->GetCurrentModule()->fd;
    if (fd < 0)
    {
        printf(ERR_NETWORK, m_Config->GetCurrentModule()->name);
        return false;
    }

    // 请求
    len = write(fd, (void*) &pCmd, sizeof(CliClient));

    // 堵塞获取消息
    while (repeat--)
    {
        len = read(fd, pbuff, sizeof(pbuff));
        if (len == -1)
        {
            if ((errno == EAGAIN) || (errno == EINTR))
            {
                //continue
            }
            else
            {
                strcpy(pbuff, "Connection reset");
                break;
            }
        }
        else if (len == 0)
        {
            strcpy(pbuff, "Server close the connnection");
            break;
        }
        else
        {
            break;
        }

        usleep(READ_INTERVAL);
        fflush(stdout);
    }

    if (repeat <= 0)
    {
        strcpy(pbuff, "Read timeout");
    }

    pbuff[len] = '\0';
    printf("%s\n", pbuff);     // 已有换行
    fflush(stdout);

    return true;
}


void CLIShell::Run()
{
    char* temp = NULL;
    char prompt[50] = "cli> ";
    int done = 0;

    // Logo
    ShowLogo();

    // 事件循环
    while (!done)
    {
        temp = readline(prompt);

        if (!temp)
        {
            exit(1);
        }

        if (*temp)
        {
            add_history(temp);
        }

        // 内部命令
        if (!strcasecmp(temp, "quit")
                || !strcasecmp(temp, "exit"))
        {
            done = 1;
        }
        else if (!strcasecmp(temp, "clear"))
        {
            write(STDOUT_FILENO, "\x1b[H\x1b[2J", 7);
        }
        else
        {
            /// 处理命令
            HandleCommand(temp, prompt);
        }

        free(temp);
    }//while(!done)
}
