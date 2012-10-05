// client.cpp : Defines the entry point for the console application.
//

#include <winsock2.h>
#pragma comment(lib,"ws2_32")
#include <stdio.h>
#include <iostream.h>
#define PORT 5000


typedef struct 
{
		int solutionId;  
}JUDGE_DATA;

SOCKET sClient;

int initSocket(int port,char *ip){
	WSADATA wsaData;
    WORD sockVersion = MAKEWORD(2, 2);
	//����winsock��
	if(WSAStartup(sockVersion, &wsaData) != 0)
		return 0;
	// �����׽���
    sClient = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(sClient == INVALID_SOCKET)
	{
		printf("socket error\n");
		return 0;
	}	
	// ��sockaddr_in�ṹ��װ��������˵�ַ��Ϣ
	sockaddr_in servAddr;
	servAddr.sin_family = AF_INET;
	servAddr.sin_port = htons(port);// htons���� ���������޷��Ŷ�������ת��������	                                
	servAddr.sin_addr.S_un.S_addr =inet_addr(ip);//��������ip
	
	//���ӷ�������
	if(connect(sClient,(sockaddr*)&servAddr,sizeof(servAddr))==SOCKET_ERROR)
	{
		printf("connect error\n");
		closesocket(sClient);
		return 0;
	}
	return 1;
}

int main(int argc, char* argv[])
{

	SetErrorMode(SEM_NOGPFAULTERRORBOX ); 
	char INI_filename[256]="D:\\OJ\\GDOJ\\data.ini";
	char IP[30]="127.0.0.1";
	int languageId=1;
	JUDGE_DATA j;

	if(argc>0){
		j.solutionId=atoi(argv[1]);  
	}else{
		return 0;
	}
	if(argc>1){
		languageId=atoi(argv[2]); 
	}
	if(argc>2){
		strcpy(INI_filename,argv[3]);
	}
	
//	j.solutionId=45;  
//	int languageId=1; 

	int port;
	char languageName[100]={0};
	char keyname[110]={0};


	sprintf(keyname,"Language%d",languageId);
	GetPrivateProfileString("Language",keyname,"",languageName,sizeof(languageName),INI_filename);
	GetPrivateProfileString("JudgeIP",languageName,"127.0.0.1",IP,sizeof(IP),INI_filename);

	port=GetPrivateProfileInt("Tool","Port",PORT,INI_filename);
	



	initSocket(port,IP);

	send(sClient,(const char*)&j,sizeof(j),0);
	
	closesocket(sClient);
	WSACleanup();
	Sleep(1000);
	return 0;
}
