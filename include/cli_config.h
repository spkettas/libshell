#ifndef _CLI_CONFIG_H_
#define _CLI_CONFIG_H_


#include "cli_public.h"




/**
 * @brief ������
 *
 */
class CliConfig
{
protected:
    CliConfig();
    ~CliConfig();

public:
    // ����ģʽ
    static CliConfig *Inst();

    // ��������
    bool LoadConfig(char *path);

    // �������Ϸ���
    CMDError CheckCommand( CliClient *pCmd );

    // ��ȡģ��
    bool GetModule( char *name, ModuleInfo **pModule );

    // ��ȡ��������
    CliCommand *GetCommand(ModuleInfo *pModule, char *cmd);
    CliCommand *GetCommand( int commandID );

    // ��ȡ�ص�����
    CliCommandProc *GetProc(unsigned int commandID);

    // ע��ص�����
    bool RegisterCallback(unsigned int commandID, CliCommandProc proc );

    // ��ȡ��ǰģ��
    void SetCurrentModule( ModuleInfo *pInfo );
    ModuleInfo *GetCurrentModule();

    // ��ȫ����
    char *CmdGenerator( const char *text, int state );


private:
    // ��������Ŀ
    bool CheckNumber( int cliNum,int srcNum );


private:
    typedef hash_map<int,CliCommand*>::iterator CliIter;
    hash_map<int,CliCommand*> m_CliCmds;    // �����б�

#define MAX_MODULE_SIZE 20
    ModuleInfo *m_MInfo[MAX_MODULE_SIZE];  // ֧���������б�
    ModuleInfo *m_CurModule;               // ��ǰģ��
};

#endif // _CLI_CONFIG_H_
