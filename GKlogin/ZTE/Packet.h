// Packet.h: interface for the CPacket class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PACKET_H__AE293802_9E7F_4666_8323_49ECE7F9A8D0__INCLUDED_)
#define AFX_PACKET_H__AE293802_9E7F_4666_8323_49ECE7F9A8D0__INCLUDED_

#pragma once

/* default snap length (maximum bytes per packet to capture) */
#define SNAP_LEN 1518

/* ethernet headers are always exactly 14 bytes [1] */
#define SIZE_ETHERNET 14

#define	OFFSET_EAPOL    0x0E			/*  */
#define OFFSET_EAP      0x12
#define ETHER_ADDR_LEN 6

#include <stdint.h>
#include <GKloginDlg.h>
#include "GKlogin.h"
#include "md5.h"
#include "rc4.h"


class CPacket  
{
public:
	CPacket();
	virtual ~CPacket();

	enum EAPType {
		EAPOL_START =1,
		EAPOL_LOGOFF,
		EAP_REQUEST_IDENTITY,
		EAP_RESPONSE_IDENTITY,
		EAP_REQUEST_IDENTITY_KEEP_ALIVE,
		EAP_RESPONSE_IDENTITY_KEEP_ALIVE,
		EAP_REQUETS_MD5_CHALLENGE,
		EAP_RESPONSE_MD5_CHALLENGE,
		EAP_SUCCESS,
		EAP_FAILURE,
		EAP_KEEP_ALIVE,
		RUIJIE_EAPOL_MSG,
		EAP_ERROR
	};
	enum STATE {
		READY,
		STARTED,
		ID_AUTHED,
		ONLINE,
		STATUS_ERROR
	};
	enum STATE  state;                      /* program state */

	int get_packet(u_char *, const struct pcap_pkthdr *, const u_char *);
	char Servermsg[300];//服务器信息
private:

	CGKloginDlg *ZTE;
	/* #####   TYPE DEFINITIONS   ######################### */
	/*-----------------------------------------------------------------------------
	*  报文缓冲区，由init_frame函数初始化。
	*-----------------------------------------------------------------------------*/
	uint8_t eapol_start[1000];            /* EAPOL START报文 */
	uint8_t eapol_logoff[1000];           /* EAPOL LogOff报文 */
	uint8_t eap_response_ident[1000]; /* EAP RESPON/IDENTITY报文 */
	uint8_t eap_response_md5ch[1000]; /* EAP RESPON/MD5 报文 */
	uint8_t eap_life_keeping[1000];
	/* #####   GLOBLE VAR DEFINITIONS   #########################
	*-----------------------------------------------------------------------------
	*  报文相关信息变量，由init_info 、init_device函数初始化。
	*-----------------------------------------------------------------------------*/
	int         username_length;
	int         password_length;
	/* #####   GLOBLE VAR DEFINITIONS   ###################
	*-----------------------------------------------------------------------------
	*  用户信息的赋值变量，由init_argument函数初始化
	*-----------------------------------------------------------------------------*/
	char        *username = NULL;
	char        *password = NULL;
	uint8_t     local_mac[ETHER_ADDR_LEN]; /* MAC地址 */


	/* #####   GLOBLE VAR DEFINITIONS   ######################### */
	/*-----------------------------------------------------------------------------
	*  程序的主控制变量
	*-----------------------------------------------------------------------------*/
	
	pcap_t      *handle;			        /* packet capture handle */
	uint8_t  muticast_mac[6];
	/* Star认证服务器多播地址 */
	//  {0x01, 0xd0, 0xf8, 0x00, 0x00, 0x03};
	//{0x00, 0x1a, 0xa9, 0x19, 0x23, 0xd4};
	struct ether_header{
		u_char ether_dhost[ETHER_ADDR_LEN];
		u_char ether_shost[ETHER_ADDR_LEN];
		u_short ether_type;
	};

	struct eap_header {
		u_char eapol_v;//0
		u_char eapol_t;//1
		u_short eapol_length;//2
		u_char eap_t;//4
		u_char eap_id;//5
		u_short eap_length;//6
		u_char eap_op;//8
		u_char eap_v_length;//9
		u_char eap_md5_challenge[16];//10
	};

	enum EAPOL_TYPE{
		EAPOL_EAP = 0,
		EAPOL_KEY = 3
	};



	void
		action_by_eap_type(enum EAPType pType,
		const eap_header *eap_head,
		const pcap_pkthdr *packetinfo,
		const uint8_t *packet);



	//应答函数
	void
		action_eapol_success(const  eap_header *eap_head,
		const  pcap_pkthdr *packetinfo,
		const uint8_t *packet);

	void
		action_eapol_failre(const  eap_header *eap_head,
		const  pcap_pkthdr *packetinfo,
		const uint8_t *packet);

	void
		action_eap_req_idnty(const  eap_header *eap_head,
		const  pcap_pkthdr *packetinfo,
		const uint8_t *packet);

	void
		action_eap_req_md5_chg(const  eap_header *eap_head,
		const  pcap_pkthdr *packetinfo,
		const uint8_t *packet);
	void
		print_server_info(const uint8_t *packet);

	void
		print_notification_msg(const uint8_t *packet);



	void
		keep_alive(const  eap_header *eap_head,
		const  pcap_pkthdr *packetinfo,
		const uint8_t *packet);

	static uint32_t
		get_ruijie_success_key(const uint8_t *success_packet);
	u_char*
		get_md5_digest(const char* str, size_t len);
	void
		fill_password_md5(uint8_t attach_key[], uint8_t eap_id);

	//回呼函数
	
	int	get_eap_type(const struct eap_header *eap_header);

public:
	void init_frames();
	void initialize(char *username_r, char *password_r, const int username_length_r, const int password_length_r, pcap_t *adapterHandle, uint8_t *MacAdd, void *para);
	//回呼函数
	int send_eap_packet(enum EAPType send_type);
	int         exit_flag = 0;
}; 
#endif // !defined(AFX_PACKET_H__AE293802_9E7F_4666_8323_49ECE7F9A8D0__INCLUDED_)
