#include <unistd.h>
#include "cli_config.h"
#include "tinyxml2.h"
#include <math.h>


using namespace tinyxml2;


CliConfig::CliConfig()
{
    memset( (void*)m_MInfo,0,sizeof(m_MInfo) );
}


CliConfig::~CliConfig()
{
    CliCommand *pCmd = NULL;
    SubCmd *pSub = NULL;

    CliIter it;

    // Module
    for( int i=0; (i<MAX_MODULE_SIZE) && m_MInfo[i]; i++ )
    {
        if( m_MInfo[i]->fd > 0 )
            close( m_MInfo[i]->fd );

        delete m_MInfo[i];
        m_MInfo[i] = NULL;
    }

    // Cmd
    for( it = m_CliCmds.begin(); it!=m_CliCmds.end(); )
    {
        pCmd = it->second;
        it++;

        // SubCmd
        pSub = pCmd->subcmd;
        while( pSub )
        {
            SubCmd *pTmpSub = pSub;
            pSub = pSub->next;

            delete pTmpSub;
            pTmpSub = NULL;
        }

        delete pCmd;
        pCmd = NULL;
    }

    m_CliCmds.clear();
}


CliConfig *CliConfig::Inst()
{
    static CliConfig cfg;
    return &cfg;
}


// 加载配置
bool CliConfig::LoadConfig( char *path )
{
    XMLDocument doc;
    if( doc.LoadFile( path ) != XML_SUCCESS )
    {
        printf( "Fail to open %s",path );
        return false;
    }

    XMLElement *root = doc.RootElement();
    XMLElement *module = root->FirstChildElement("Module");

    ModuleInfo *pModule = NULL;
    CliCommand *pCommand = NULL;
    SubCmd *pSubCmd = NULL,*pLastSub = NULL;
    int j = 0;

    // 遍历
    while( module )
    {
        // module
        pModule = new ModuleInfo();
        memset( (void*)pModule,0,sizeof(ModuleInfo) );

        strcpy( pModule->name,module->Attribute("name") );
        module->QueryIntAttribute( "external",&pModule->external );
        pModule->fd = -1;

        // 模块列表
        m_MInfo[j++] = pModule;

        // cmd
        XMLElement *cmd = module->FirstChildElement();
        while( cmd )
        {
            pCommand = new CliCommand();
            memset( (void*)pCommand,0,sizeof(CliCommand) );

            pCommand->module = pModule;
            sprintf( pCommand->name,"%s",cmd->Attribute("name") );
            cmd->QueryIntAttribute("id",&pCommand->id );
            cmd->QueryIntAttribute("arity",&pCommand->arity );
            pCommand->proc = NULL;

            pSubCmd = NULL;
            pLastSub = NULL;

            // subcmd
            XMLElement *subcmd = cmd->FirstChildElement();
            while( subcmd )
            {
                pSubCmd = new SubCmd();
                memset( (void*)pSubCmd,0,sizeof(SubCmd) );

                pSubCmd->next = pLastSub;
                subcmd->QueryIntAttribute("arity",&pSubCmd->arity );
                strcpy( pSubCmd->name,subcmd->Attribute("name") );

                // Property
                XMLElement *property = subcmd->FirstChildElement();
                while( property )
                {
                    Property &props = pSubCmd->props[ pSubCmd->plen++ ];
                    strcpy( props.property, property->Attribute("value") );
                    subcmd->QueryIntAttribute( "arity",&props.arity );         // 修正值

                    property = property->NextSiblingElement();
                }

                pLastSub = pSubCmd;
                subcmd = subcmd->NextSiblingElement();
            }

            // 缓存
            pCommand->subcmd = pSubCmd;
            m_CliCmds.insert( make_pair(pCommand->id,pCommand) );

            // next
            cmd = cmd->NextSiblingElement();
        }//while(cmd;)

        module = module->NextSiblingElement();
    }//while(module;)

    return true;
}


CMDError CliConfig::CheckCommand( CliClient *pCmd )
{
    // 据模块找到命令
    CliCommand *pCommand = GetCommand( m_CurModule,pCmd->argv[0] );
    if( pCommand == NULL )
        return UNKNOWN_ERR;

    // 更新ID
    pCmd->id = pCommand->id;

    // Cmd
    if( !CheckNumber(pCmd->argc,pCommand->arity) )
        return COUNT_ERR;

    // Subcmd
    SubCmd *pSub = pCommand->subcmd;
    while( pSub )
    {
        if( !strcmp(pSub->name,pCmd->argv[1]) )
        {
            if( !CheckNumber(pCmd->argc,pSub->arity) )
                return COUNT_ERR;

            // Property
            for( int i = 0; i < pSub->plen; i++ )
            {
                if( !strcmp( pSub->props[i].property,pCmd->argv[2]) )
                {
                    if( !CheckNumber(pCmd->argc,pSub->props[i].arity) )
                    {
                        return COUNT_ERR;
                    }
                }
            }

            break;
        }//if(sub)

        pSub = pSub->next;
    }//while(pSub)

    return CLI_OK;
}


// 获取命令
bool CliConfig::GetModule( char *name,ModuleInfo **pModule )
{
    ModuleInfo *module = NULL;

    for( int i=0; (i < MAX_MODULE_SIZE) && m_MInfo[i] ; i++ )
    {
        module = m_MInfo[i];

        if( !strcmp(name,module->name) )
        {
            *pModule = module;
            return true;
        }
    }

    return false;
}


// 获取命令
CliCommand *CliConfig::GetCommand( ModuleInfo *pModule,char *cmd )
{
    if( pModule == NULL )
        return NULL;

    CliIter it = m_CliCmds.begin();
    CliCommand *pCommand = NULL;

    for( ; it != m_CliCmds.end(); it++ )
    {
        pCommand = it->second;

        // CMD
        if( (pCommand->module == pModule)
                && !strcmp(pCommand->name,cmd) )
        {
            return pCommand;
        }
    }

    return NULL;
}


// 获取命令
CliCommand *CliConfig::GetCommand( int commandID )
{
    CliCommand *pCommand = NULL;

    CliIter it = m_CliCmds.find( commandID );
    if( it != m_CliCmds.end() )
    {
        pCommand = it->second;
    }

    return pCommand;
}


// 获取回调函数
CliCommandProc *CliConfig::GetProc( unsigned int commandID )
{
    CliCommand *pCommand = GetCommand( commandID );
    if( pCommand )
        return pCommand->proc;

    return NULL;
}


bool CliConfig::RegisterCallback( unsigned int commandID,CliCommandProc proc )
{
    CliCommand *pCommand = GetCommand( commandID );
    if( pCommand )
    {
        pCommand->proc = proc;
        return true;
    }

    return false;
}

bool CliConfig::CheckNumber( int cliNum,int srcNum )
{
    if( srcNum < 0 )
    {
        srcNum = abs(srcNum);

        if( cliNum < srcNum )
            return false;
    }
    else if( cliNum != srcNum )
    {
        return false;
    }

    return true;
}


void CliConfig::SetCurrentModule( ModuleInfo *pInfo )
{
    this->m_CurModule = pInfo;
}


ModuleInfo *CliConfig::GetCurrentModule()
{
    return m_CurModule;
}


char *CliConfig::CmdGenerator( const char *text,int state )
{
    static CliIter it;
    static int len;

    // reset
    if( !state )
    {
        it = m_CliCmds.begin();
        len = strlen( text );
    }

    // &&&
    ModuleInfo *pModule = NULL;
    GetModule( "system",&pModule );

    CliCommand *pCmd = NULL;

    while( it != m_CliCmds.end() )
    {
        pCmd = it->second;
        it++;

        // CMD
        if( (pCmd->module == pModule)
                && !strncmp(pCmd->name,text,len) )
        {
            int nSize = strlen( pCmd->name );
            char *mystr = new char[ nSize + 1 ];

            strcpy( mystr,pCmd->name );
            mystr[ nSize ] = '\0';

            return mystr;
        }
    }//for(;)

    return NULL;
}
