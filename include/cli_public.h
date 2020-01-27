#ifndef _CLI_PUBLIC_H_
#define _CLI_PUBLIC_H_


#include <string>
#include <ext/hash_map>


using namespace std;
using namespace __gnu_cxx;


#define IPC_PATH            ("/tmp/%s.sock")    // IPC通信路径
#define MAX_SIZE            (10)                // 最大命令段数
#define MAX_KEY_SIZE        (20)                // 最大子属性数目
#define MAX_CMD_LEN         (100)               // 命令最大长度


/**
 * @brief CLI回调函数
 * @param c 命令参数
 * @return 返回处理结果(字符串)
 */
struct _CliClient;
typedef std::string CliCommandProc( _CliClient *c );   // 函数指针&回调函数


/**
 * 回调函数结构
 *
 */
typedef struct _CommandPair
{
    int id;                 // 命令ID
    CliCommandProc *proc;   // 命令回调函数
}CommandPair;


/**
 * 模块描述
 *
 */
typedef struct _ModuleInfo
{
    char name[MAX_CMD_LEN];  // 模块名称
    int external;      // 是否为外部进程
    int fd;             // socket
}ModuleInfo;


/**
 * 命令属性对
 */
typedef struct Property
{
    char property[MAX_CMD_LEN];  // 属性
    int arity;                   // 命令数目，负表至少N个参数
}Property;


/**
 * 子命令列表
 */
typedef struct _SubCmd
{
    Property props[MAX_KEY_SIZE];     // 参数列表
    int plen;                         // 子参数个数

    char name[MAX_CMD_LEN];      // 子命令名称
    int arity;                   // 命令数目
    _SubCmd *next;               // 链表结构
}SubCmd;


/**
 * CLI命令参数
 *
 */
typedef struct _CliCommand
{
    ModuleInfo *module; // 模块描述

    int id;             // 命令标识
    char name[MAX_CMD_LEN];    // 命令名称
    int arity;          // 参数数目

    CliCommandProc *proc;   // 命令回调函数
    SubCmd *subcmd;         // 子命令列表
}CliCommand;



/**
 * CLI网络参数
 *
 */
typedef struct _CliClient
{
    int id;                            // 命令ID
    char argv[MAX_SIZE][MAX_CMD_LEN];  // 参数列表
    unsigned int argc;             // 参数数目
}CliClient;


// 错误码
typedef enum _CMDError
{
    CLI_OK,
    EMPTY_ERR,
    UNKNOWN_ERR,
    COUNT_ERR,
    NETWORK_ERR
}CMDError;


// 错误提示
#define CMD_OK                  "OK\n"
#define ERR_MODULE_CMD          "ERR,unknown module '%s'\n"
#define ERR_FIND_CMD            "-bash: %s: command not found"
#define ERR_UNKNOWN_CMD         "ERR,unknown command '%s'\n"
#define ERR_COUNT_CMD           "ERR,wrong number of arguments for '%s'\n"
#define ERR_NETWORK             "Could not connect to %s\n"

#endif // _CLI_PUBLIC_H_
