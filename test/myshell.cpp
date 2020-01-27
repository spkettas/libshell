#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cli_shell.h"


/**
 * @brief 函数入口
 *
 */
int main( int argc,char **argv )
{
    CLIShell *pTool = new CLIShell("../config/cmd.xml");
    pTool->Run();

    return 0;
}
