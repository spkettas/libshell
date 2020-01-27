#ifndef _CLI_CONFIG_H_
#define _CLI_CONFIG_H_


#include "cli_public.h"




/**
 * @brief 配置类
 *
 */
class CliConfig
{
protected:
    CliConfig();
    ~CliConfig();

public:
    // 单例模式
    static CliConfig *Inst();

    // 加载配置
    bool LoadConfig(char *path);

    // 检测命令合法性
    CMDError CheckCommand( CliClient *pCmd );

    // 获取模块
    bool GetModule( char *name, ModuleInfo **pModule );

    // 获取命令描述
    CliCommand *GetCommand(ModuleInfo *pModule, char *cmd);
    CliCommand *GetCommand( int commandID );

    // 获取回调函数
    CliCommandProc *GetProc(unsigned int commandID);

    // 注册回调函数
    bool RegisterCallback(unsigned int commandID, CliCommandProc proc );

    // 获取当前模块
    void SetCurrentModule( ModuleInfo *pInfo );
    ModuleInfo *GetCurrentModule();

    // 补全命令
    char *CmdGenerator( const char *text, int state );


private:
    // 检测参数数目
    bool CheckNumber( int cliNum,int srcNum );


private:
    typedef hash_map<int,CliCommand*>::iterator CliIter;
    hash_map<int,CliCommand*> m_CliCmds;    // 命令列表

#define MAX_MODULE_SIZE 20
    ModuleInfo *m_MInfo[MAX_MODULE_SIZE];  // 支持最大进程列表
    ModuleInfo *m_CurModule;               // 当前模块
};

#endif // _CLI_CONFIG_H_
