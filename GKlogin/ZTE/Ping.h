//
// Ping.h
//

#pragma pack(1)

#define ICMP_ECHOREPLY	0
#define ICMP_ECHOREQ	8
#define REQ_DATASIZE 32		// Echo 请求数据的大小
#define PING_1_END 1		// Ping结束消息标志
#define PING_2_END 2		// Ping结束消息标志
#define PING_1_Status 1		// Ping消息标志
#define PING_2_Status 2		// Ping消息标志
// IP Header -- RFC 791
typedef struct tagIPHDR
{
	u_char  VIHL;			// Version and IHL
	u_char	TOS;			// Type Of Service
	short	TotLen;			// Total Length
	short	ID;				// Identification
	short	FlagOff;		// Flags and Fragment Offset
	u_char	TTL;			// Time To Live
	u_char	Protocol;		// Protocol
	u_short	Checksum;		// Checksum
	struct	in_addr iaSrc;	// Internet Address - Source
	struct	in_addr iaDst;	// Internet Address - Destination
}IPHDR, *PIPHDR;


// ICMP Header - RFC 792
typedef struct tagICMPHDR
{
	u_char	Type;			// Type
	u_char	Code;			// Code
	u_short	Checksum;		// Checksum
	u_short	ID;				// Identification
	u_short	Seq;			// Sequence
	char	Data;			// Data
}ICMPHDR, *PICMPHDR;


//ICMP首部结构体
typedef struct tagICMPHEADER
{
	unsigned char type;//类型
	unsigned char code;//代码
	unsigned short checknum;//检验和
	unsigned short id;//标识符
	unsigned short seq_num;//序列号
	unsigned char data[REQ_DATASIZE];
}ICMPHEADER;
//IP首部结构体
typedef struct _ip_hdr
{
	unsigned char ihl : 4;   //首部长度
	unsigned char version : 4; //版本 
	unsigned char tos;   //服务类型
	unsigned short tot_len; //总长度
	unsigned short id;    //标志
	unsigned short frag_off; //分片偏移
	unsigned char ttl;   //生存时间
	unsigned char protocol; //协议
	unsigned short chk_sum; //检验和
	struct in_addr srcaddr; //源IP地址
	struct in_addr dstaddr; //目的IP地址
}IPHEADER;

// ICMP Echo Request
typedef struct tagECHOREQUEST
{
	ICMPHEADER icmpHdr;
}ECHOREQUEST, *PECHOREQUEST;

// ICMP Echo Reply
typedef struct tagECHOREPLY
{
	IPHDR	ipHdr;
	ICMPHEADER icmpHdr;
	char    cFiller[256];
}ECHOREPLY, *PECHOREPLY;


class CPing
{
public:
	HWND m_hWnd;	//窗口句柄
	void Ping(UINT nRetries, LPCSTR pstrHost, LPCSTR pstrHost2, HWND hWnd, bool *Pingstopsign,char *m_ip);

protected:
	unsigned short checksum(unsigned short *buffer, int size);
	int  WaitForEchoReply(SOCKET s);
	//ICMP回应的请求和回答函数
	int		SendEchoRequest(SOCKET, ECHOREQUEST*, LPSOCKADDR_IN, LARGE_INTEGER  *);
	DWORD RecvEchoReply(SOCKET, ECHOREPLY *, LPSOCKADDR_IN, LARGE_INTEGER  *, u_char *);
	void WSAError(LPCSTR pstrFrom);
};

#pragma pack()

