#ifndef _CLI_SERVER_H_
#define _CLI_SERVER_H_


#include "cli_public.h"



class CliConfig;


/**
 *@brief CLI服务端实现，采用UNIX DOMAIN方式
 *
 */
class CLIServer
{
public:
    /**
     * @brief 构造函数
     * @param name 服务名称，必须唯一
     * @param cfgPath 命令配置文件路径
     *
     */
    CLIServer( char *name,char *cfgPath );
    ~CLIServer();

    /**
     * @brief 注册回调函数
     * @param commandID 命令ID
     * @param proc 命令处理函数
     *
     * @return 处理结果
     */
    bool RegisterFunction(int commandID, CliCommandProc proc );

    /**
     * @brief 注册多个回调函数
     * @param commandTable 命令列表
     * @param size 回调函数数目
     *
     */
    void RegisterFunction( CommandPair *commandTable,int size );

    /**
     * @brief 启动事件循环
     *
     */
    void Run();


protected:
    // 建立套接字
    int CreateSocket( char *name );
    bool HandleCmd( CliClient *pClient, int clientfd );

    // 处理连接
    static void *HandleConnect(void *arg);
    static void *HandleRequest(void *arg);


private:
    int m_sockfd;           // 套接字
    char m_path[50];        // IPC路径
    CliConfig *m_Config;    // 命令配置
};

#endif // _CLI_SERVER_H_
