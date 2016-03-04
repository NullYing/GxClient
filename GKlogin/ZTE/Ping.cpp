
#include "stdafx.h"
#include "ping.h"

#include <windows.h>
#include <time.h>
#include <ws2tcpip.h> //IP_TTL  


void CPing::Ping(UINT nRetries, LPCSTR pstrHost, LPCSTR pstrHost2, HWND hWnd, bool *Pingstopsign, char *m_ip)
{
	m_hWnd = hWnd;
	ASSERT(IsWindow(hWnd));

	int nRet;
	LPHOSTENT lpHost, lpHost2;
	u_char    cTTL;
	CString str;

	WSADATA wsadata;
	if (WSAStartup(MAKEWORD(2, 2), &wsadata) != 0)
	{
		//printf("初始化socket dll失败\n");
		WSAError("初始化socket dll失败");
		return ;
	}
	//设置原始套接字
	SOCKET sock = WSASocket(AF_INET, SOCK_RAW, IPPROTO_ICMP, NULL, 0, 0);
	if (INVALID_SOCKET == sock)
	{
		//printf("创建socket失败\n");
		WSAError("创建socket失败");
		WSACleanup();
		return ;
	}
	int ttl = 255;
	//设置TTL为64
	if (setsockopt(sock, IPPROTO_IP, IP_TTL, (const char*)&ttl, sizeof(ttl)) == SOCKET_ERROR)
	{
		//printf("设置TTL错误!\n");
		WSAError("设置TTL错误!");
		WSACleanup();
		return ;
	}

	//获得主机信息
	
	lpHost = gethostbyname(pstrHost);
	if (lpHost == NULL)
	{
		//printf("主机未找到: %s", pstrHost);
		str.Format("主机未找到: %s", pstrHost);
		if (nRetries == 2){
			::PostMessage(m_hWnd, WM_MSG_STATUS, 10, (LPARAM)AllocBuffer(str));
			::PostMessage(m_hWnd, WM_MSG_PING_END, PING_2_END, 0);
		}
		else{
			::PostMessage(m_hWnd, WM_MSG_STATUS, 0, (LPARAM)AllocBuffer(str));
			::PostMessage(m_hWnd, WM_MSG_PING_END, PING_1_END, 0);
		}
		return;
	}
	//
	SOCKADDR_IN Local_addr;
	memset(&Local_addr, 0, sizeof(Local_addr));
	Local_addr.sin_family = AF_INET;
	Local_addr.sin_addr.s_addr = *((u_long FAR *) (m_ip));
	Local_addr.sin_port = htons(6000);
	bind(sock, (SOCKADDR*)&Local_addr, sizeof(SOCKADDR));
	//目标地址
	SOCKADDR_IN Dest_addr;
	memset(&Dest_addr, 0, sizeof(Dest_addr));
	Dest_addr.sin_family = AF_INET;
	Dest_addr.sin_addr.s_addr = *((u_long FAR *) (lpHost->h_addr));
	Dest_addr.sin_port = 3177;//0;	

	SOCKADDR_IN Dest_addr2;
	if (pstrHost2!=NULL){
		lpHost2 = gethostbyname(pstrHost2);
		if (lpHost2 == NULL)
		{
			//printf("主机2未找到: %s", pstrHost2);
			str.Format("主机2未找到: %s", pstrHost2);
			::PostMessage(m_hWnd, WM_MSG_STATUS, PING_2_Status, (LPARAM)AllocBuffer(str));
			::PostMessage(m_hWnd, WM_MSG_PING_END, PING_2_END, 0);
			::PostMessage(m_hWnd, WM_MSG_STATUS, PING_1_Status, (LPARAM)AllocBuffer(str));
			::PostMessage(m_hWnd, WM_MSG_PING_END, PING_1_END, 0);
			return;
		}
		//目标地址2
		memset(&Dest_addr2, 0, sizeof(Dest_addr2));
		Dest_addr2.sin_family = AF_INET;
		Dest_addr2.sin_addr.s_addr = *((u_long FAR *) (lpHost2->h_addr));
		Dest_addr2.sin_port = 3178;//0;
		//发送ICMP请求报文
	}
	LARGE_INTEGER  beg, end;
	LARGE_INTEGER  beg2, end2;
	double dqFreq;                /*计时器频率*/
	LARGE_INTEGER f;            /*计时器频率*/
	QueryPerformanceFrequency(&f);
	dqFreq = (double)f.QuadPart;

	double dur; 
	double dur2;
	ECHOREQUEST echoReq;
	ECHOREPLY echoReply;
	ECHOREQUEST echoReq2;
	ECHOREPLY echoReply2;
	while (true)
	{
		SendEchoRequest(sock, &echoReq,&Dest_addr, &beg);
		
		nRet = WaitForEchoReply(sock);
		if (nRet == SOCKET_ERROR)
		{
			WSAError("select()");
			break;
		}
		if (nRet==0)
		{
			printf("Ping %s 超时\n", inet_ntoa(Dest_addr.sin_addr));
			str.Format("Ping超时");
			::PostMessage(m_hWnd, WM_MSG_STATUS, PING_1_Status, (LPARAM)AllocBuffer(str));
		}
		//接受ICMP回应报文并分析
		else
		{
			RecvEchoReply(sock, &echoReply, &Dest_addr, &end, &cTTL);
			if (echoReply.icmpHdr.type == 0)
			{
				if (echoReply.icmpHdr.id == echoReq.icmpHdr.id)
				{
					dur = ((end.QuadPart - beg.QuadPart) / dqFreq);
					//printf("Ping %s,%.0lf ms,echoReq1.icmpHdr.id=%d\n", inet_ntoa(Dest_addr.sin_addr), dur * 1000, echoReq.icmpHdr.id);
					str.Format("网络延迟：%.0lfms", dur * 1000);
					::PostMessage(m_hWnd, WM_MSG_STATUS, PING_1_Status, (LPARAM)AllocBuffer(str));
					//break;
				}
			}
			else
			{
				//printf("Ping失败\n");
				str.Format("Ping失败");
				::PostMessage(m_hWnd, WM_MSG_STATUS, PING_1_Status, (LPARAM)AllocBuffer(str));
				//break;
			}
		}
		Sleep(2000);
		if (*Pingstopsign)
			break;
		if (pstrHost2){
			SendEchoRequest(sock, &echoReq2, &Dest_addr2, &beg2);
			nRet = WaitForEchoReply(sock);
			if (nRet == SOCKET_ERROR)
			{
				WSAError("select()");
				break;
			}
			if (nRet == 0)
			{
				//printf("Ping %s 超时\n", inet_ntoa(Dest_addr.sin_addr));
				str.Format("Ping内网超时");
				::PostMessage(m_hWnd, WM_MSG_STATUS, PING_2_Status, (LPARAM)AllocBuffer(str));
			}
			else
			{
				RecvEchoReply(sock, &echoReply2, &Dest_addr2, &end2, &cTTL);
				if (echoReply2.icmpHdr.type == 0){
					if (echoReply2.icmpHdr.id == echoReq2.icmpHdr.id&&pstrHost2)
					{
						dur2 = ((end2.QuadPart - beg2.QuadPart) / dqFreq);
						//printf("Ping2 %s,%.0lf ms,echoReq2.icmpHdr.id=%d\n", inet_ntoa(Dest_addr2.sin_addr), dur2 * 1000, echoReq2.icmpHdr.id);
						str.Format("内网延迟：%.0lfms", dur2 * 1000);
						::PostMessage(m_hWnd, WM_MSG_STATUS, PING_2_Status, (LPARAM)AllocBuffer(str));
					}
				}
				else
				{
					//printf("Ping内网失败\n");
					str.Format("Ping内网失败");
					::PostMessage(m_hWnd, WM_MSG_STATUS, PING_2_Status, (LPARAM)AllocBuffer(str));
					//break;
				}
			}
		}
		if (*Pingstopsign)
			break;
		Sleep(3000);
	}
	//printf("Ping结束");
	::PostMessage(m_hWnd, WM_MSG_PING_END, PING_1_END, 1);
	::PostMessage(m_hWnd, WM_MSG_PING_END, PING_2_END, 1);
	shutdown(sock, SD_BOTH);
	nRet=closesocket(sock);
	if (nRet == SOCKET_ERROR)
		WSAError("closesocket()");
	WSACleanup();
}

//发送ICMPECHO数据包请求
u_short CPing::checksum(u_short *addr, int len)
{
	register int nleft = len;
	register u_short *w = addr;
	register u_short answer;
	register int sum = 0;

	while (nleft > 1)  {
		sum += *w++;
		nleft -= 2;
	}

	if (nleft == 1) {
		u_short	u = 0;

		*(u_char *)(&u) = *(u_char *)w;
		sum += u;
	}

	sum = (sum >> 16) + (sum & 0xffff);
	sum += (sum >> 16);
	answer = ~sum;
	return (answer);
}
//发送ICMPECHO数据包请求
int CPing::SendEchoRequest(SOCKET s, ECHOREQUEST *echoReq, LPSOCKADDR_IN lpsaFrom, LARGE_INTEGER  *begtime)
{
	int nRet;
	srand((u_short)begtime->QuadPart);
	//构造回应请求
	echoReq->icmpHdr.type = ICMP_ECHOREQ;
	echoReq->icmpHdr.code = 0;
	echoReq->icmpHdr.checknum = 0;
	echoReq->icmpHdr.id = rand();
	echoReq->icmpHdr.seq_num = 256;

	int i;
	memcpy(echoReq->icmpHdr.data, "bbs.gxgk.cc", 10);
	for (nRet = 11, i = 0; nRet < REQ_DATASIZE; nRet++,i++)
	{
		echoReq->icmpHdr.data[nRet] = 'a' + i;
		if (echoReq->icmpHdr.data[nRet] == 'z')
			i = 0;
	}

	echoReq->icmpHdr.checknum = checksum((u_short *)&echoReq->icmpHdr, sizeof(ECHOREQUEST));

	//保存发送时间
	QueryPerformanceCounter(begtime);
	
	//发送请求
	nRet = sendto(s,
		(LPSTR)&echoReq->icmpHdr,
		sizeof(ECHOREQUEST),
		0,
		(LPSOCKADDR)lpsaFrom,
		sizeof(SOCKADDR_IN));

	if (nRet == SOCKET_ERROR)
		WSAError("sendto()");
	return (nRet);
}
//接收ICMPECHO数据包回应
DWORD CPing::RecvEchoReply(SOCKET s, ECHOREPLY *echoReply, LPSOCKADDR_IN lpsaFrom, LARGE_INTEGER  *time, u_char *pTTL)
{
	int nRet;
	int nAddrLen = sizeof(struct sockaddr_in);
	sockaddr RecvData;
	//接收请求回应
	nRet = recvfrom(s,
		(LPSTR)echoReply,
		sizeof(ECHOREPLY),
		0,
		&RecvData,
		&nAddrLen);

	//返回发送的时间
	QueryPerformanceCounter(time);

	//检查返回值
	if (nRet == SOCKET_ERROR)
		WSAError("recvfrom()");

	//返回发送的时间
	*pTTL = echoReply->ipHdr.TTL;
	return 0;
}

//等待回应
int CPing::WaitForEchoReply(SOCKET s)
{
	struct timeval Timeout;
	fd_set readfds;
	FD_ZERO(&readfds);
	readfds.fd_count = 1;
	readfds.fd_array[0] = s;
	Timeout.tv_sec = 2;
	Timeout.tv_usec = 0;

	return(select(1, &readfds, NULL, NULL, &Timeout));
}

//错误处理
void CPing::WSAError(LPCSTR lpMsg)
{
	CString strMsg;
	strMsg.Format("%s-WSAError: %ld", lpMsg, WSAGetLastError());
	//发送报错信息
	::PostMessage(m_hWnd, PING_1_Status, 0, (LPARAM)AllocBuffer(strMsg));
	::PostMessage(m_hWnd, PING_2_Status, 0, (LPARAM)AllocBuffer(strMsg));
}