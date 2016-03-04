#include "stdafx.h"
#include "Packet.h"

CPacket::CPacket()
{

}

CPacket::~CPacket()
{

}
/*
* ===  FUNCTION  ======================================================================
*         Name:  init_frames
*  Description:  初始化发送帧的数据缓冲区
* =====================================================================================
*/
void CPacket::init_frames()
{
	/*****  EAPOL Header  *******/
	uint8_t eapol_eth_header[SIZE_ETHERNET];
	struct ether_header *eth = (struct ether_header *)eapol_eth_header;
	memcpy(eth->ether_dhost, muticast_mac, ETHER_ADDR_LEN);
	memcpy(eth->ether_shost, local_mac, ETHER_ADDR_LEN);
	eth->ether_type = htons(0x888e);

	/**** EAPol START ****/
	uint8_t start_data[4] = { 0x01, 0x01, 0x00, 0x00 };
	memset(eapol_start, 0, 1000);
	memcpy(eapol_start, eapol_eth_header, SIZE_ETHERNET);
	memcpy(eapol_start + OFFSET_EAPOL, start_data, sizeof(start_data));

	/****EAPol LOGOFF ****/
	uint8_t logoff_data[4] = { 0x01, 0x02, 0x00, 0x00 };
	memset(eapol_logoff, 0, 1000);
	memcpy(eapol_logoff, eapol_eth_header, SIZE_ETHERNET);
	memcpy(eapol_logoff + OFFSET_EAPOL, logoff_data, sizeof(logoff_data));

	/* EAP RESPONSE IDENTITY */
	uint8_t eap_resp_iden_head[9] = { 0x01, 0x00,
		0x00, 0x00, /* eapol_length */
		0x02, 0x00,
		0x00, 0x00, /* eap_length */
		0x01 };
	eap_resp_iden_head[3] = 5 + (uint8_t)username_length;
	eap_resp_iden_head[7] = 5 + (uint8_t)username_length;
	memset(eap_response_ident, 0, 1000);
	memcpy(eap_response_ident, eapol_eth_header, SIZE_ETHERNET);
	memcpy(eap_response_ident + SIZE_ETHERNET, eap_resp_iden_head, 9);
	memcpy(eap_response_ident + SIZE_ETHERNET + 9, username, username_length);


	/** EAP RESPONSE MD5 Challenge **/
	uint8_t eap_resp_md5_head[10] = { 0x01, 0x00,
		0x00, 0x00,/* eapol_length */
		0x02, 0x36,
		0x00, 0x00,/* eap-length */
		0x04, 0x10 };
	eap_resp_md5_head[3] = 22 + (uint8_t)username_length;
	eap_resp_md5_head[7] = 22 + (uint8_t)username_length;
	memset(eap_response_md5ch, 0, 1000);
	memcpy(eap_response_md5ch, eapol_eth_header, SIZE_ETHERNET);
	memcpy(eap_response_md5ch + SIZE_ETHERNET, eap_resp_md5_head, 10);
	// 剩余16位在收到REQ/MD5报文后由fill_password_md5填充 
	memcpy(eap_response_md5ch + SIZE_ETHERNET + 10 + 16, username, username_length);

	/* LIFE KEEP PACKET */
	uint8_t eapol_keep_alive[52] = {
		0x01, //version
		0x03, //type
		0x00, 0x30, //length
		0x01,//decript type
		0x00, 0x04,//key length
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	//Replay Counter: 8字节
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	//16字节的Key IV 前8字节
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	//16字节的Key IV 后8字节
		0x00,				//index	
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	//16字节的Key Signature 前8字节
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	//16字节的Key Signature 后8字节
		0x00, 0x00, 0x00, 0x00							//4字节的Key
	};
	memset(eap_life_keeping, 0, 1000);
	memcpy(eap_life_keeping, eapol_eth_header, SIZE_ETHERNET);
	memcpy(eap_life_keeping + SIZE_ETHERNET, eapol_keep_alive, 52);
}
void CPacket::initialize(char *username_r, char *password_r, const int username_length_r, const int password_length_r, pcap_t *adapterHandle, uint8_t *MacAdd, void *para)
{
	handle = adapterHandle;
	muticast_mac[0] = { 0x01 };
	muticast_mac[1] = { 0x80 };
	muticast_mac[2] = { 0xC2 };
	muticast_mac[3] = { 0x00 };
	muticast_mac[4] = { 0x00 };
	muticast_mac[5] = { 0x03 };
	/* Star认证服务器多播地址 */
	//  {0x01, 0xd0, 0xf8, 0x00, 0x00, 0x03};
	//{0x00, 0x1a, 0xa9, 0x19, 0x23, 0xd4};
	memset(local_mac, 0, ETHER_ADDR_LEN);
	memcpy(local_mac, MacAdd, ETHER_ADDR_LEN);
	username = username_r; username_length = strlen(username);
	password = password_r; password_length = strlen(password);
	ZTE = (CGKloginDlg*)para;
	exit_flag = 0;
}
