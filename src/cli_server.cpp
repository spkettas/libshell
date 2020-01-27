#include "cli_server.h"
#include <unistd.h>
#include <stdio.h>
#include <stddef.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <errno.h>
#include <assert.h>
#include <string.h>
#include <strings.h>
#include "cli_config.h"



static int Clients[FD_SETSIZE] = {0};           // 客户端列表



CLIServer::CLIServer( char *name, char *cfgPath )
        :m_sockfd( -1 )
        ,m_Config( NULL )
{
    // 配置
    m_Config = CliConfig::Inst();
    m_Config->LoadConfig( cfgPath );

    bzero( m_path,sizeof(m_path) );
    sprintf( m_path,IPC_PATH,name );

    // 清除
    unlink( m_path );

    // 套接字
    CreateSocket( m_path );
}


CLIServer::~CLIServer()
{
    if( m_sockfd > 0 )
        close( m_sockfd );

    // remove
    unlink( m_path );
}


int CLIServer::CreateSocket( char *path )
{
    struct sockaddr_un addr;
    bzero( &addr, sizeof(struct sockaddr_un) );

    // socket
    m_sockfd = socket( AF_UNIX, SOCK_STREAM, 0 );
    assert( m_sockfd > 0 );

    addr.sun_family = AF_UNIX;
    strcpy( addr.sun_path,path );

    int len = offsetof(struct sockaddr_un,sun_path) + strlen(path);
    int ret = bind( m_sockfd, ( struct sockaddr* )&addr, len );
    assert( ret != -1 );

    // listen
    ret = listen( m_sockfd, 5 );
    assert( ret != -1 );
    return ret;
}


void *CLIServer::HandleConnect( void *arg )
{
    int s_s = *((int*)arg);

    struct sockaddr_in from;
    socklen_t len = sizeof(from);

    struct timeval timeout;
    timeout.tv_sec = 1;
    timeout.tv_usec = 0;

    fd_set listenset;

    while( 1 )
    {
        FD_ZERO( &listenset);
        FD_SET( s_s,&listenset );

        timeout.tv_sec = 1;     /// 每次都设置
        int ret = select( s_s+1,&listenset,NULL,NULL,&timeout );
        if( ret < 0 )
            break;

        if( FD_ISSET( s_s,&listenset ) )
        {
            int s_c = accept( s_s,(struct sockaddr*)&from, &len );

            for( int i=0; i<FD_SETSIZE; i++ )
            {
                if( Clients[i] == 0 )
                {
                    Clients[i] = s_c;
                    break;
                }
            }
        }
    }

    return (void*)NULL;
}


void *CLIServer::HandleRequest( void *arg )
{
    CLIServer *pServer = (CLIServer*)arg;

    struct timeval timeout;
    timeout.tv_sec = 1;
    timeout.tv_usec = 0;

    CliClient pCmd;

    int maxfd = -1;
    fd_set scanfd;

    int ret = 0;

    while( 1 )
    {
        FD_ZERO(&scanfd);

        for( int i=0; i<FD_SETSIZE; i++ )
        {
            if( Clients[i] != 0 )
            {
                FD_SET( Clients[i], &scanfd );	// 加入集合

                if( maxfd < Clients[i] )
                {
                    maxfd = Clients[i];         // 更新最大Fd
                }
            }
        }

        timeout.tv_sec = 1;
        ret = select( maxfd + 1, &scanfd, NULL, NULL, &timeout );
        if( ret < 0 )  // 0超时，-1错误
            break;

        for( int i = 0; i<FD_SETSIZE; i++ )
        {
            if( Clients[i] == 0 )
                continue;

            if( FD_ISSET(Clients[i],&scanfd) )
            {
                bzero( (void*)&pCmd, sizeof(CliClient) );
                ret = recv(Clients[i], &pCmd, sizeof(CliClient),0);
                if( ret <= 0 )
                {
                    close( Clients[i] );
                    Clients[i] = -1;
                }
                else
                {
                    /// 调用命令处理函数
                    pServer->HandleCmd( &pCmd, Clients[i] );
                }
            }
        }//for(;)
    }//while(1)

    return (void*)NULL;
}


bool CLIServer::HandleCmd( CliClient *pClient,int clientfd )
{
    CliCommandProc *proc = m_Config->GetProc(pClient->id);
    int len = 0;

    if( proc != NULL )
    {
        // 回调处理
        std::string strMsg = (*proc)(pClient);

        // 响应
        len = write( clientfd, strMsg.c_str(), strMsg.size() );
    }

    return len > 0;
}


// 注册回调函数
bool CLIServer::RegisterFunction( int commandID,CliCommandProc proc )
{
    return m_Config->RegisterCallback(commandID,proc);
}


void CLIServer::RegisterFunction( CommandPair *commandTable,int size )
{
    CommandPair *pCommand = NULL;

    for( int i=0 ; i<size; i++ )
    {
        pCommand = commandTable + i;

        // 逐个注册
        m_Config->RegisterCallback( pCommand->id,pCommand->proc );
    }
}


void CLIServer::Run()
{
    pthread_t threads[2];

    // connect
    pthread_create( &threads[0],NULL,HandleConnect,(void*)&m_sockfd );

    // request
    pthread_create( &threads[1],NULL,HandleRequest,this );

    // detach
    for( int i=0; i<2; i++)
    {
        pthread_detach(threads[i]);
    }
}

