#ifndef _CLI_TOOL_H
#define _CLI_TOOL_H


#include <string>
#include <list>



class CliConfig;

struct _ModuleInfo;
typedef struct _ModuleInfo ModuleInfo;
struct _CliCommand;
typedef struct _CliCommand CliCommand;
struct _CliClient;
typedef struct _CliClient CliClient;


/**
 * @brief CLI 客户端调用接口
 *
 */
class CLIShell
{
public:
    /**
     * @brief 构造函数
     * @param cfgPath 命令配置文件路径
     *
     */
    CLIShell( char *cfgPath );
    ~CLIShell();

    /**
     * @brief 启动事件循环
     */
    void Run();


protected:
    // 连接服务器
    int ConnectTo(char *name);

    // 显示LOGO
    void ShowLogo();

    // 命令检测
    bool CheckCmd(char *prompt, char *temp, CliClient *pCmd);

    // 处理命令
    bool HandleCommand(char *temp,char *prompt);


private:
    CliConfig *m_Config;        // 命令配置类
};

#endif // _CLI_TOOL_H
