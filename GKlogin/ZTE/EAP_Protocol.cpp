#include "stdafx.h"
#include "Packet.h"
#include "md5.h"
/*
* ===  FUNCTION  ======================================================================
*         Name:  fill_password_md5
*  Description:  给RESPONSE_MD5_Challenge报文填充相应的MD5值。
*  只会在接受到REQUEST_MD5_Challenge报文之后才进行，因为需要
*  其中的Key
* =====================================================================================
*/
/*
void print_hex(const uint8_t *array, int count)
{
	int i;
	for (i = 0; i < count; i++){
		if (!(i % 16))
			fprintf(stderr, "\n");
		fprintf(stderr, "%02x ", array[i]);
	}
	fprintf(stderr, "\n");
}
*/
void CPacket::fill_password_md5(uint8_t eap_md5_challenge[], uint8_t eap_id)
{
		//print_hex(eap_md5_challenge,16);
		char *psw_key;
		u_char *md5;
		char text[] = "zte142052";
		psw_key = (char*)malloc(1 + password_length + 9 + 16);
		psw_key[0] = eap_id;
		memcpy(psw_key + 1, password, password_length);
		memcpy(psw_key + 1 + password_length, text, 9);
		memcpy(psw_key + 1 + password_length + 9, eap_md5_challenge, 16);
		md5 = get_md5_digest(psw_key, 1 + password_length + 9 + 16);
		memcpy(eap_response_md5ch + 14 + 10, md5, 16);
		//print_hex((uint8_t *)eap_response_md5ch + 14 + 10, 16);
		free(psw_key);
}
/*
* ===  FUNCTION  ======================================================================
*         Name:  get_md5_digest
*  Description:  calcuate for md5 digest
* =====================================================================================
*/
u_char * CPacket::get_md5_digest(const char* str, size_t len)
{
	static u_char digest[16];
	MD5_CTX state;
	MD5Init(&state);
	MD5Update(&state, (unsigned char *)str, len);
	MD5Final(digest, &state);
	return (u_char *)digest;
}

void CPacket::action_eapol_success(const struct eap_header *eap_head, const struct pcap_pkthdr *packetinfo, const uint8_t *packet)
{
	//    extern pthread_t    live_keeper_id;
	state = ONLINE;
	ZTE->Log(ZTE->I_INFO, "EAP_SUCCESS\n");
	//    print_server_info (packet);
	/* Set alarm to send keep alive packet */
	//    alarm(30);
}

void CPacket::action_eapol_failre(const struct eap_header *eap_head, const struct pcap_pkthdr *packetinfo, const uint8_t *packet)
{
	//    extern pthread_t    exit_waiter_id;

	state = STATUS_ERROR;

	//print_server_info(packet);
	char  msg[300];
	if (strlen(Servermsg) < 3)
		print_notification_msg(packet);
	strcpy(msg, Servermsg);
	*Servermsg = '\0';
	if (strlen(msg) != 0 && (strstr(msg, "http") == 0) && (strstr(msg, "错误码") != 0))
	{
		ZTE->Log(ZTE->I_ERR, "服务器认证失败,%s", msg);
	}
	else if ((strlen(msg) != 0) && (strstr(msg, "http") == 0) && (strstr(msg, "Radius server not responding") != 0))
		ZTE->Log(ZTE->I_ERR, "校园网故障，请拨打天翼营业厅电话：86211959 进行报障\r\n服务器信息：%s", msg);
	else if (strlen(msg) != 0)
	{
		ZTE->Log(ZTE->I_ERR, "认证失败，服务器信息：%s");
	}
	else
	{
		ZTE->Log(ZTE->I_ERR, "认证失败,服务器无返回信息！");
	}
	ZTE->FaultMsg(msg);
	
	if (exit_flag) {
		ZTE->Log(ZTE->I_INFO, "&&Info: Session Ended.\n");
		pcap_breakloop(handle);
	}
	//else{
	//	exit_flag = 1;
	//	Sleep(1*1000);
	//}
}

void CPacket::action_eap_req_idnty(const struct eap_header *eap_head, const struct pcap_pkthdr *packetinfo, const uint8_t *packet)
{
	if (state == STARTED){
		ZTE->Log(ZTE->I_INFO, "REQUEST EAP-Identity\n");
	}
	//if (exit_flag)
		//return;
	eap_response_ident[0x13] = eap_head->eap_id;
	send_eap_packet(EAP_RESPONSE_IDENTITY);
}

void CPacket::action_eap_req_md5_chg(const struct eap_header *eap_head, const struct pcap_pkthdr *packetinfo, const uint8_t *packet)
{
	state = ID_AUTHED;

	ZTE->Log(ZTE->I_INFO, "REQUEST MD5-Challenge(PASSWORD)\n");
	fill_password_md5((uint8_t*)eap_head->eap_md5_challenge, eap_head->eap_id);
	eap_response_md5ch[0x13] = eap_head->eap_id;
	send_eap_packet(EAP_RESPONSE_MD5_CHALLENGE);
}

void CPacket::keep_alive(const struct eap_header *eap_head, const struct pcap_pkthdr *packetinfo, const uint8_t *packet)
{
	memcpy(eap_life_keeping + 21, packet + 21, 24);//Replay Counter  +  Key IV
	//此包的index值，直接从接收包复制过来
	eap_life_keeping[45] = packet[45];

	//////////////////////////////////////////////////////////////////////////
	//使用rc4算法生成Key，基于（Key IV + Key IV最后四个字节）==20字节
	uint8_t enckey[] = { 0x02, 0x02, 0x14, 0x00 };
	uint8_t wholekey[20];
	memcpy(wholekey, packet + 29, 16);
	memcpy(wholekey + 16, packet + 41, 4);
	int keylen = 4;
	u_char deckey[64] = { 0 };
	rc4_crypt(enckey, keylen, wholekey, 20);
	memcpy(eap_life_keeping + 62, enckey, 4);

	//////////////////////////////////////////////////////////////////////////
	//使用hmac_md5算法生成Key Signature，此用于包的校验	
	u_char encDat[64];
	memset(eap_life_keeping + 46, 0, 16); //before hmac_md5 just set the key signature to zeros.
	memcpy(encDat, eap_life_keeping + 14, 52);
	enckey[0] = eap_life_keeping[45];
	hmac_md5(encDat, 52, enckey, 1, deckey);
	memcpy(eap_life_keeping + 46, deckey, 16);

	//print_hex(eap_life_keeping, 66);

	send_eap_packet(EAP_RESPONSE_IDENTITY_KEEP_ALIVE);
}
/*
* ===  FUNCTION  ======================================================================
*         Name:  action_by_eap_type
*  Description:  根据eap报文的类型完成相关的应答
* =====================================================================================
*/
void CPacket::action_by_eap_type(enum EAPType pType, const struct eap_header *eap_head, const struct pcap_pkthdr *packetinfo, const uint8_t *packet) {
	switch (pType){
	case EAP_SUCCESS:
		action_eapol_success(eap_head, packetinfo, packet);
		break;
	case EAP_FAILURE:
		action_eapol_failre(eap_head, packetinfo, packet);
		break;
	case EAP_REQUEST_IDENTITY:
		action_eap_req_idnty(eap_head, packetinfo, packet);
		break;
	case EAP_REQUETS_MD5_CHALLENGE:
		action_eap_req_md5_chg(eap_head, packetinfo, packet);
		break;
	case RUIJIE_EAPOL_MSG:
		print_server_info(packet);
		break;
	case EAP_KEEP_ALIVE:
		keep_alive(eap_head, packetinfo, packet);
		break;
	default:
		return;
	};
}
/*
* ===  FUNCTION  ======================================================================
*         Name:  send_eap_packet
*  Description:  根据eap类型发送相应数据包
* =====================================================================================
*/
int CPacket::send_eap_packet(enum EAPType send_type)
{
	static uint8_t *frame_data;
	int             frame_length = 0;
	static int num = 0;
	switch (send_type){
	case EAPOL_START:
		state = STARTED;
		frame_data = eapol_start;
		frame_length = 18;
		ZTE->Log(ZTE->I_INFO, "SEND EAPOL-Start\n");
		break;
	case EAPOL_LOGOFF:
		state = READY;
		frame_data = eapol_logoff;
		frame_length = 18;
		ZTE->Log(ZTE->I_INFO, "SEND EAPOL-Logoff\n");
		break;
	case EAP_RESPONSE_IDENTITY:
		frame_data = eap_response_ident;
		frame_length = 23 + username_length;
		ZTE->Log(ZTE->I_INFO, "SEND EAP-Response/Identity\n");
		break;
	case EAP_RESPONSE_MD5_CHALLENGE:
		frame_data = eap_response_md5ch;
		frame_length = 40 + username_length;
		ZTE->Log(ZTE->I_INFO, "SEND EAP-Response/Md5-Challenge\n");
		break;
	case EAP_RESPONSE_IDENTITY_KEEP_ALIVE:
		frame_data = eap_life_keeping;
		frame_length = 66;
		if (num <= 5){
			ZTE->Log(ZTE->I_INFO, "SEND EAPOL_KEEP_ALIVE\n"); num++;
		}
		break;
	default:
		ZTE->Log(ZTE->I_INFO, "&&IMPORTANT: Wrong Send Request Type.%02x\n", send_type);
		return 3;
	}

	//    ZTE->Log(ZTE->I_INFO,"&&[debug][eap_response_ident]: frame_length:%d\n", 23 + username_length);
	//    print_hex(eap_response_ident, 23 + username_length);

	if (pcap_sendpacket(handle, frame_data, frame_length) != 0)
	{
		ZTE->Log(ZTE->I_INFO, "&&IMPORTANT: Error Sending the packet: %s\n",pcap_geterr(handle));
		return 0;
	}
	return 1;
}
/*
* ===  FUNCTION  ======================================================================
*         Name:  get_packet
*  Description:  pcap的回呼函数，当收到EAPOL报文时自动被调用
* =====================================================================================
*/
int CPacket::get_packet(u_char *args, const struct pcap_pkthdr *pcaket_header, const u_char *packet)
{
	/* declare pointers to packet headers */
	const struct ether_header *ethernet;  /* The ethernet header [1] */
	const struct eap_header *eap_header;
	enum EAPType p_type;

	ethernet = (struct ether_header*)(packet);
	eap_header = (struct eap_header *)(packet + SIZE_ETHERNET);

	p_type = (enum EAPType)get_eap_type(eap_header);
	if (p_type != EAP_ERROR)
		action_by_eap_type(p_type, eap_header, pcaket_header, packet);
	return p_type;
}
/*
* ===  FUNCTION  ======================================================================
*         Name:  get_eap_type
*  Description:  根据报文的动作位返回enum EAPType内定义的报文类型
* =====================================================================================
*/
int CPacket::get_eap_type(const struct eap_header *eap_header)
{
	if (eap_header->eapol_t == EAPOL_KEY) {
		return EAP_KEEP_ALIVE;
	}
	switch (eap_header->eap_t){
	case 0x00:
		if (eap_header->eap_op == 0x1c &&
			eap_header->eap_id == 0x00)
			return RUIJIE_EAPOL_MSG;
		break;
	case 0x01:
		if (eap_header->eap_op == 0x01)
			return EAP_REQUEST_IDENTITY;
		if (eap_header->eap_op == 0x02)
			return RUIJIE_EAPOL_MSG;
		if (eap_header->eap_op == 0x04)
			return EAP_REQUETS_MD5_CHALLENGE;
		break;
	case 0x03:
		return EAP_SUCCESS;
		break;
	case 0x04:
		return EAP_FAILURE;
	}
	ZTE->Log(ZTE->I_INFO, "&&IMPORTANT: Unknown Package : eap_t :    %02x\n"
		"                               eap_id:    %02x\n"
		"                               eap_op:    %02x\n",
		eap_header->eap_t, eap_header->eap_id,
		eap_header->eap_op);
	return EAP_ERROR;
}

void CPacket::print_notification_msg(const uint8_t *packet)
{
	char msg_length[2] = {0};
	char  msg[300] = { 0 };
	int length;
	/* 中兴的通知报文 */
	
	memcpy(msg_length,(packet + 23),1);
	length = (int)(msg_length[0]);

	if (length > 0)
	{
		memcpy(msg, (packet + 24), length);
		if (msg[9] == '\0')
			msg[9] = ':';
		*Servermsg = '\0';
		strcpy(Servermsg, msg);
		ZTE->Log(ZTE->I_MSG, ">>服务器信息: %s", msg);
	}
}

void CPacket::print_server_info(const uint8_t *packet)
{
	char msg_length[2] = { 0 };
	int length;
	memcpy(msg_length, (packet + 21), 1);
	length = (int)(msg_length[0]);
	//ZTE->Log(ZTE->I_MSG, ">>服务器信息长度1: %d\n", length);
	/* success和failure报文系统信息的固定位置 */
	if (length>0) {
		memcpy(Servermsg, (packet + 23), length);
		ZTE->Log(ZTE->I_MSG, ">>服务器信息: %s\n", Servermsg);
	}
}