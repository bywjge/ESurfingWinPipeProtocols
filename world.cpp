#include "windows.h"
#include <iostream>
using namespace std;

#define BUF_SIZE 4096
// 定义管道名 , 如果是跨网络通信 , 则在圆点处指定服务器端程序所在的主机名
// 这里先把 EsurfingSvr.exe 服务开起来
#define EXAMP_PIPE   L"\\\\.\\pipe\\ESurfingClientPipe"

int main()
{
    HANDLE hPipe = NULL;
    char  szBuffer[BUF_SIZE] = { 0 };
    DWORD dwReturn = 0;

    // 判断是否有可以利用的命名管道  
    if (!WaitNamedPipe(EXAMP_PIPE, NMPWAIT_USE_DEFAULT_WAIT))
    {
        cout << "No Read Pipe Accessible" << endl;
        return 0;
    }

    // 打开可用的命名管道 , 并与服务器端进程进行通信  
    hPipe = CreateFile(EXAMP_PIPE, GENERIC_READ | GENERIC_WRITE,
        0,
        NULL, OPEN_EXISTING, 0, NULL);

    if (hPipe == INVALID_HANDLE_VALUE)
    {
        cout << "Open Read Pipe Error" << endl;
        return 0;
    }

    DWORD  cbRead, cbToWrite, cbWritten, dwMode;
    BOOL   fSuccess = FALSE;



    const wchar_t* wcL = L"<M><OpS>Portal</OpS><OpT>Portal</OpT><OpC>ClientStart</OpC><P></P></M>";
    const wchar_t* wcL2 = L"<M><OpS>Portal</OpS><OpT>Portal</OpT><OpC>CheckEnv</OpC><P></P></M>";
    const wchar_t* wcL3 = L"<M><OpS>Portal</OpS><OpT>Portal</OpT><OpC>PortalConnect</OpC><P><user>账号</user><password>密码</password></P></M>";
   // const wchar_t* wcL = L"<M><OpS>Portal</OpS><OpT>Portal</OpT><OpC>CheckEnv</OpC><P></P></M>";

    //DIS_REASON_STARTUP: "5",        //5：客户端启动自动下线（上次退出未正常下线）；
    const wchar_t* Discon = L"<M><OpS>Portal</OpS><OpT>Portal</OpT><OpC>PortalDisConnect</OpC><P><src>5</src></P></M>";

    /*
var CLIENT_START ='ClientStart'  //客户端启动
var CLIENT_START_COMPLETE='ClientStartComplete' //服务启动完成
var SERVER_STATUS_RESP ='ServerStatusResp' //服务状态    ESERVERSTATUS
var CHECK_ENV ='CheckEnv' //检测网络环境
var CHECK_ENV_RESP ='CheckEnvResp' //返回网络环境状态
var GET_NETINFO_RESP ='GetNetInfoResp' //返回网络信息
var PORTAL_CONNECT ='PortalConnect' //请求连接
var PORTAL_CONNECT_RESP ='PortalConnectResp' //连接请求返回
var PORTAL_DISCONNECT ='PortalDisConnect' //请求断开连接
var PORTAL_DISCONNECT_RESP ='PortalDisconnectResp' //断开连接返回
var APP_ERR ='AppErr'  //程序出错
var CHECK_WIFI_RESP ='CheckWifiResp' //Wifi共享软件检测结果

var NET_STATE_MESSAGE_RESP ='NetStateMessageResp' //拨号消息
var NET_STATE_MESSAGE_RESP_EX ='NetStateMessageRespEx' //拨号消息
var TICKET_RESP ='TicketResp'    //返回ticket

var SET_VALUE ='SetValue' //设置数据
var GET_VALUE ='GetValue' //获取数据
var FUN_CFG =	"FunCfg"  //功能配置

var LOG_TRACE ='Log' //日志跟踪
var LOG_CHECK_WIFI_POST ='LogCheckWifiPost' //检测到共享软件后错误上报

var TERMINAL_REQUEST ='GetTerminalData' //请求终端信息
var TERMINAL_RESP ='SetTerminalData' //终端信息返回
var TERMINAL_TERM_REQUEST ='TerminalTermSend' //上网终端界面请求其他设备下线
var TERMINAL_TERM_RESP ='TerminalTermReturn' //上网终端界面请求其他设备下线返回

var CHECK_SYSTEM_TIME ='CheckSystemTime' //系统时间检测

var DATA_SUBMIT = 'DataSubmit' //错误上报
var DISASTER_URL = 'DisasterUrl' //打开应急页面

var UPDATE_COMPLETE = 'UpdateComplete';
    
    */

    //通信方式为半双工 需要确认 完全收完了消息才能写


   // The pipe connected; change to message-read mode. 
   

    DWORD dwWritten;

        WriteFile(hPipe,
            wcL,
            2 * wcslen(wcL),
            &dwWritten,
            NULL);

        DWORD NumberOfBytesRead = 0;
        void * lpBuffer = operator new[](0x400000u);

        do {

            memset(lpBuffer, 0, 0x200000u);
            fSuccess = ReadFile(
                hPipe,    // pipe handle 
                lpBuffer,    // buffer to receive reply 
                0x200000u,  // size of buffer 
                &cbRead,  // number of bytes read 
                NULL);    // not overlapped 


            wcout << static_cast<wchar_t*>(lpBuffer) << endl;
        } while (!wcsstr(static_cast<wchar_t*>(lpBuffer), L"ClientStartComplete"));

        cout << "初始化完毕， IP 更换后需要在这里开始执行一次" << endl << endl << endl << endl;
        //修改Hook的地方直接改IP 好像更方便一点

            WriteFile(hPipe,
                wcL2,
                2 * wcslen(wcL2),
                &dwWritten,
                NULL);

            int i = 0;
            do {
                memset(lpBuffer, 0, 0x200000u);
                fSuccess = ReadFile(
                    hPipe,    // pipe handle 
                    lpBuffer,    // buffer to receive reply 
                    0x200000u,  // size of buffer 
                    &cbRead,  // number of bytes read 
                    NULL);    // not overlapped 


                wcout << static_cast<wchar_t*>(lpBuffer) << endl;

                if (wcsstr(static_cast<wchar_t*>(lpBuffer), L"<newState>2</newState>")) {
                    cout << "已经连网了, 正在退出...." << endl << endl << endl << endl;

                    WriteFile(hPipe,
                        Discon,
                        2 * wcslen(Discon),
                        &dwWritten,
                        NULL);
                    
                    // 这里好像退出不了 原因未知 只能重启服务然后挤掉线
                }
                else if (wcsstr(static_cast<wchar_t*>(lpBuffer), L"<ErrorCode>30000105</ErrorCode>")) {

                    //code.js '30000105': '重新获取用户标识'

                    WriteFile(hPipe,
                        wcL2,
                        2 * wcslen(wcL2),
                        &dwWritten,
                        NULL);

                }
            } while (!wcsstr(static_cast<wchar_t*>(lpBuffer), L"TicketResp"));



          
            cout << "登录中" << endl << endl << endl << endl;
            WriteFile(hPipe,
                wcL3,
                2 * wcslen(wcL3),
                &dwWritten,
                NULL);

            //check globalDefine.js   ET_PORTAL -> 51 
            //<oldState>1</oldState><newState>2</newState> -> ES_CONNECTED

            i = 0;
            do {
                memset(lpBuffer, 0, 0x200000u);
                fSuccess = ReadFile(
                    hPipe,    // pipe handle 
                    lpBuffer,    // buffer to receive reply 
                    0x200000u,  // size of buffer 
                    &cbRead,  // number of bytes read 
                    NULL);    // not overlapped 


                wcout << static_cast<wchar_t*>(lpBuffer) << endl;

                if(wcsstr(static_cast<wchar_t*>(lpBuffer), L"<newState>2</newState>"))
                    cout << "登录成功!" << endl << endl << endl << endl;
            } while (true);

            //没登录成功可以重新开一次试试 (Ticket过期)

        if (!fSuccess)
        {
            cout << "ReadFile from pipe failed. GLE=%d\n";
            return -1;
        }

        cout << "退出!" << endl << endl << endl << endl;
        CloseHandle(hPipe);

    return 0;
}
