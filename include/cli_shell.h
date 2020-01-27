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
 * @brief CLI �ͻ��˵��ýӿ�
 *
 */
class CLIShell
{
public:
    /**
     * @brief ���캯��
     * @param cfgPath ���������ļ�·��
     *
     */
    CLIShell( char *cfgPath );
    ~CLIShell();

    /**
     * @brief �����¼�ѭ��
     */
    void Run();


protected:
    // ���ӷ�����
    int ConnectTo(char *name);

    // ��ʾLOGO
    void ShowLogo();

    // ������
    bool CheckCmd(char *prompt, char *temp, CliClient *pCmd);

    // ��������
    bool HandleCommand(char *temp,char *prompt);


private:
    CliConfig *m_Config;        // ����������
};

#endif // _CLI_TOOL_H
