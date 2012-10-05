#define _MT   //多线程，加了他才能用beginthread
#define   _WIN32_WINNT     0x0500
#include <windows.h>
#include <process.h>
#include <iostream>
#include <conio.h>
#include <stdlib.h>
#include  <io.h>
#include <time.h>
#include  "psapi.h" 
#include "tlhelp32.h" 
#include <queue>
#include <string>
#include <sstream>

#include "define.h"
#include "util.h"
 //必须在mysql.h之前，因为里面用到socket  #include <winsock2.h> ,windows.h貌似已经包含
#include "include\mysql.h"
using namespace std;
//#pragma comment(lib, "libcmt.lib")

#pragma  comment(lib,"lib/psapi.lib")

#pragma comment(lib,"ws2_32")
#pragma comment(lib, "lib/libmysql.lib")
#define BUFSIZE 4096

const int MAX_NAME = 30;
const int MAX_CODE = 100000;
const int MAX_TITLE = 200;
const int MAX_CONTENT = 100000;
const int MAX_WAITTIME = 10000 ;


MYSQL *mysql;     //mysql连接
char query[1024]; //查询语句
const char INI_filename[]="GDOJ\\data.ini";
char Mysql_url[255];
char Mysql_username[255];
char Mysql_password[255];
char Mysql_table[255];
int  Mysql_port;  
char Mysql_Character[255];  //编码

int  nLanguageCount=0;
int isDeleteTemp=0;  
int isRestrictedFunction=0;
int  limitJudge=20;  //裁判队列最大等待数量
DWORD OutputLimit=10000; //最大输出
char workPath[MAX_PATH];  //临时工作目录
char dataPath[MAX_PATH];  //数据
char logPath[MAX_PATH]="log\\";  //log
char judgePath[MAX_PATH]; //judge.exe

int isTranscoding=0;   //针对VS的转码
int limitIndex=1;    //时间限制倍数
int nProcessLimit=1;
char GL_username[MAX_NAME];
int GL_solutionId;
int GL_problemId;
int GL_languageId;
int GL_verdictId;
int GL_contestId;
int GL_time;
int GL_memory;
int GL_reJudge;
int GL_testcase;
time_t GL_submitDate;
char GL_languageName[100]={0};
char GL_languageExt[10]={0};
char GL_languageExe[10]={0};
int GL_time_limit;
int GL_memory_limit;
int GL_spj;
char compileCmd_str[1024]={0};
char runCmd_str[1024]={0};

char sourcePath[1024]={0};
char exePath[1024]={0};

char inFileName[MAX_PATH];
char outFileName[MAX_PATH];
char DebugFile[MAX_PATH];
char ErrorFile[MAX_PATH];

clock_t startt,endt ; //每次run的时间点

STARTUPINFO si; 
PROCESS_INFORMATION G_pi = {0};

class CSolution
{
public:
	char username[MAX_NAME];
	int solutionId;
	int problemId;
	int languageId;
	int verdictId;
	int contestId;
	int time;
	int memory;
	int reJudge;
	time_t submitDate;
protected:
private:
};

/////////////////////////LOG
//#define LOG(level) Log(__FILE__, __LINE__, level).GetStream()


string GetLocalTimeAsString(const char* format) {
	time_t t = time(NULL);
	struct tm *p;
	p = localtime(&t);
	char buf[1024];
	strftime(buf, sizeof(buf), format, p);
	return buf;
}

void write_log(int level, const char *fmt, ...) {
	va_list ap;
	char buffer[4096];
	time_t  timep = time(NULL);
	int l;
	struct tm *p;
    p = localtime(&timep);
    p->tm_year = p->tm_year + 1900;
    p->tm_mon = p->tm_mon + 1;
	sprintf(buffer,"log/Judge-%04d-%02d-%02d.log",p->tm_year, p->tm_mon, p->tm_mday);

	FILE *fp = fopen(buffer, "a+");
	if (fp == NULL) {
		fprintf(stderr, "open logfile error!\n");
		return;
	}
	
	fprintf(fp, "%s:%04d-%02d-%02d %02d:%02d:%02d ",LEVEL_NAME[level],p->tm_year, p->tm_mon, p->tm_mday,p->tm_hour,p->tm_min,p->tm_sec);
	if (DEBUG)
		printf("%s:%04d-%02d-%02d %02d:%02d:%02d ",LEVEL_NAME[level],p->tm_year, p->tm_mon, p->tm_mday,p->tm_hour,p->tm_min,p->tm_sec);

	va_start(ap, fmt);
	l = vsprintf(buffer, fmt, ap);
	fprintf(fp, "%s\n", buffer);
	if (DEBUG)
		printf("%s\n", buffer);
	va_end(ap);
	fclose(fp);
}

///////////END LOG


////////////////////////////////////////////////////socket
#define PORT 5000
#define BUFFER 1024
#pragma comment(lib,"WSOCK32.lib")   //必须的
int port=PORT;

typedef struct 
{
	int solutionId;  
}JUDGE_DATA;

int GL_currentId;//当前裁判id
queue <JUDGE_DATA> Q;//全局队列

SOCKET sListen;

//#pragma comment(linker, "/subsystem:windows /ENTRY:mainCRTStartup") // 设置连接器选项

int initSocket()
{
	write_log(INFO,"Start initialization of Socket...");

	WSADATA wsaData;
    WORD sockVersion = MAKEWORD(2, 2);
	//加载winsock库
	if(WSAStartup(sockVersion, &wsaData) != 0)
		return 0;
	// 创建套节字
	sListen = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(sListen == INVALID_SOCKET)
	{
		write_log(SYSTEM_ERROR,"create socket error");
		return 0;
	}	
	// 在sockaddr_in结构中装入地址信息
	sockaddr_in sin;
	sin.sin_family = AF_INET;
	sin.sin_port = htons(port);	// htons函数 将主机的无符号短整形数转换成网络
	//字节顺序
	sin.sin_addr.S_un.S_addr = INADDR_ANY;


	int trybind=50;  //重试bind次数
	int ret=0;
	while((ret=bind(sListen,(LPSOCKADDR)&sin,sizeof(sin)))==SOCKET_ERROR&&trybind>0)
	{
		write_log(SYSTEM_ERROR,"bind failed:%d , it will try later...",WSAGetLastError());
		trybind--;
		Sleep(100);
	}
	if(ret<0) {
		write_log(SYSTEM_ERROR,"Bind failed...");
		return 0;
	}
	write_log(INFO,"Bind success...");

	//进入监听状态 
	int trylisten=50; //重试listen次数
	while((ret=listen(sListen,20))==SOCKET_ERROR&&trylisten) 
	{
		write_log(SYSTEM_ERROR,"listen failed:%d , it will try later..",WSAGetLastError());
		trylisten--;
		Sleep(100);
		return 0;
	}
	if(ret<0) {
		write_log(SYSTEM_ERROR,"Listen failed...");
		return 0;
	}
	write_log(INFO,"Listen success...");

	return 1;
}
//////////////////////////////////////////////////////////////end socket

int InitMySQL()   //初始化mysql，并设置字符集
{ 
	mysql=mysql_init((MYSQL*)0); 
	if(mysql!=0 && !mysql_real_connect(mysql,Mysql_url, Mysql_username, Mysql_password, Mysql_table,Mysql_port,NULL,CLIENT_MULTI_STATEMENTS )){
		write_log(ERROR,mysql_error(mysql));	
		return 0;			  
	} 	 
	strcpy(query,"SET CHARACTER SET gbk"); //设置编码 gbk
	
	int ret=mysql_real_query(mysql,query,(unsigned int)strlen(query));	  
	if(ret){			  
		write_log(ERROR,mysql_error(mysql));	
		return 0;
	}		
	return 1;
}


void InitMySqlConfig(){
	port=GetPrivateProfileInt("Tool","Port",PORT,INI_filename);
	isDeleteTemp=GetPrivateProfileInt("Tool","DeleteTemp",0,INI_filename);  
	limitJudge=GetPrivateProfileInt("Tool","LimitJudge",20,INI_filename);
	OutputLimit=GetPrivateProfileInt("Tool","OutputLimit",10000,INI_filename);
	isRestrictedFunction=GetPrivateProfileInt("Tool","isRestrictedFunction",0,INI_filename);
	GetPrivateProfileString("Tool","WorkingPath","",workPath,sizeof(workPath),INI_filename);
	GetPrivateProfileString("Tool","DataPath","",dataPath,sizeof(dataPath),INI_filename);
	GetPrivateProfileString("Tool","JudgePath","",judgePath,sizeof(judgePath),INI_filename);

	GetPrivateProfileString("MySQL","url","",Mysql_url,sizeof(Mysql_url),INI_filename);
	GetPrivateProfileString("MySQL","username","",Mysql_username,sizeof(Mysql_username),INI_filename);
	GetPrivateProfileString("MySQL","password","",Mysql_password,sizeof(Mysql_password),INI_filename);
	GetPrivateProfileString("MySQL","table","",Mysql_table,sizeof(Mysql_table),INI_filename);
	Mysql_port=GetPrivateProfileInt("MySQL","port",0,INI_filename);


//  	
	write_log(INFO,"socketPort:%d, DataPath:%s, TempPath:%s",port,dataPath,workPath);
 	//cout<<"MySQL:"<<Mysql_url<<" "<<Mysql_username<<" "<<Mysql_password<<" "<<Mysql_table<<" "<<Mysql_port<<endl;

}
//字符串替换所有old_value->new_value
string&  replace_all_distinct(string&   str,const   string&   old_value,const   string&   new_value)   
{   
	for(string::size_type   pos(0);   pos!=string::npos;   pos+=new_value.length())   {   
		if(   (pos=str.find(old_value,pos))!=string::npos   )   
			str.replace(pos,old_value.length(),new_value);   
		else   break;   
	}   
	return   str;   
}   

void InitPath() 
{
	if( (_access(workPath, 0 )) == -1 )   {
		CreateDirectory(workPath,NULL);
	}

	//GetPrivateProfileString(lpAppName,lpKeyName,lpDefault,lpReturnedString,nSize,lpFileName);
	//GetPrivateProfileInt(lpAppName,lpKeyName,nDefault,lpFileName);
	
	char keyname[100]={0};
	sprintf(keyname,"Language%d",GL_languageId);
	
	GetPrivateProfileString("Language",keyname,"",GL_languageName,100,INI_filename);
	
	
	//sprintf(keyname,"%s",GL_languageName);
	
	GetPrivateProfileString("LanguageExt",GL_languageName,"",GL_languageExt,10,INI_filename);
	
	GetPrivateProfileString("LanguageExe",GL_languageName,"",GL_languageExe,10,INI_filename);
	
	GetPrivateProfileString("CompileCmd",GL_languageName,"",compileCmd_str,1024,INI_filename);
	
	GetPrivateProfileString("RunCmd",GL_languageName,"",runCmd_str,1024,INI_filename);
	
	GetPrivateProfileString("SourcePath",GL_languageName,"",sourcePath,1024,INI_filename);
	
	GetPrivateProfileString("ExePath",GL_languageName,"",exePath,1024,INI_filename);
	
	isTranscoding=GetPrivateProfileInt("Transcoding",GL_languageName,0,INI_filename);
	
	limitIndex=GetPrivateProfileInt("TimeLimit",GL_languageName,1,INI_filename);
	
	nProcessLimit=GetPrivateProfileInt("ProcessLimit",GL_languageName,1,INI_filename);
	
	
	char buf[1024];
	sprintf(buf, "%d", GL_solutionId);
	string name = buf;
	string compile_string=compileCmd_str;
	replace_all_distinct(compile_string,"%PATH%",workPath);
	replace_all_distinct(compile_string,"%NAME%",name);
	replace_all_distinct(compile_string,"%EXT%",GL_languageExt);
	replace_all_distinct(compile_string,"%EXE%",GL_languageExe);
	strcpy(compileCmd_str,compile_string.c_str());      //编译命令行
	//	cout<<CompileCmd_str<<endl;
	
	string runcmd_string=runCmd_str;
	replace_all_distinct(runcmd_string,"%PATH%",workPath);
	replace_all_distinct(runcmd_string,"%NAME%",name);
	replace_all_distinct(runcmd_string,"%EXT%",GL_languageExt);
	replace_all_distinct(runcmd_string,"%EXE%",GL_languageExe);
	strcpy(runCmd_str,runcmd_string.c_str());			//运行命令行
	//	cout<<RunCmd_str<<endl;
	
	string sourcepath_string=sourcePath;
	replace_all_distinct(sourcepath_string,"%PATH%",workPath);
	replace_all_distinct(sourcepath_string,"%NAME%",name);
	replace_all_distinct(sourcepath_string,"%EXT%",GL_languageExt);
	strcpy(sourcePath,sourcepath_string.c_str());		//源程序路径
	//  cout<<SourcePath<<endl;
	
	string exepath_string=exePath;
	replace_all_distinct(exepath_string,"%PATH%",workPath);
	replace_all_distinct(exepath_string,"%NAME%",name);
	replace_all_distinct(exepath_string,"%EXE%",GL_languageExe);
	strcpy(exePath,exepath_string.c_str());				//可执行文件路径
	//	cout<<ExePath<<endl;
	
	sprintf(DebugFile,"%s%s.txt",workPath,name.c_str()); //debug文件路径
	sprintf(ErrorFile,"%s%s_re.txt",workPath,name.c_str()); //re文件路径
	
	//	cout<<DebugFile<<endl;
}

int SQL_getSolutionSource(){
	sprintf(query,"select source from solution_source where solution_id=%d",GL_solutionId);
	int ret=mysql_real_query(mysql,query,(unsigned int)strlen(query));	  
	if(ret){			  
		write_log(ERROR,mysql_error(mysql));
		return 0;
	}	
	MYSQL_RES *recordSet = mysql_store_result(mysql); 
	if (recordSet==NULL){
		write_log(ERROR,"SQL_getSolutionSource");
		return 0;
	}
	FILE *fp_source = fopen(sourcePath, "w");
	char code[MAX_CODE]={0};
	MYSQL_ROW row; 
	if(row=mysql_fetch_row(recordSet))  
	{	
		sprintf(code, "%s", row[0]);
	}else {
		write_log(ERROR,"SQL_getSolutionSource Error");
	}
	
	if(isTranscoding==1){
		int ii=0;
		//解决VS下字符问题
		while (code[ii]!='\0') {
			if (code[ii]=='\r') code[ii] = '\n';ii++;
		}
	}

	fprintf(fp_source, "%s", code);

	mysql_free_result(recordSet);
	fclose(fp_source);
	return 1;
}
int SQL_getSolutionInfo(){
	//problemId,languageId,submitDate,username

	sprintf(query,"select problem_id,contest_id,language,username,submit_date from solution where solution_id=%d",GL_solutionId);
	int ret=mysql_real_query(mysql,query,(unsigned int)strlen(query));	  
	if(ret){			 
		write_log(ERROR,mysql_error(mysql));
		//printf("Error SQL_getSolutionData...\n");
		return 0;
	}	
	MYSQL_RES *recordSet = mysql_store_result(mysql); 
	if (recordSet==NULL){
		write_log(ERROR,"Error SQL_getSolutionData");
		return 0;
	}
	
	//获取数据
	MYSQL_ROW row; //一个行数据的类型安全(type-safe)的表示
	if(row=mysql_fetch_row(recordSet))  //获取下一条记录
	{
		GL_problemId=atoi(row[0]);
		GL_contestId=atoi(row[1]);		
		GL_languageId=atoi(row[2]);
		strcpy(GL_username,row[3]);
		StringToTimeEX(row[4],GL_submitDate);
	
	}else {
		write_log(ERROR,"Error SQL_getSolutionData");
	}
	mysql_free_result(recordSet);//释放结果集
	return 1;
}

int SQL_getProblemInfo(){//time_limit,memory_limit,spj
	
	sprintf(query,"select time_limit,memory_limit,spj from problem where problem_id=%d",GL_problemId);

	int ret=mysql_real_query(mysql,query,(unsigned int)strlen(query));	  
	if(ret){			  
		write_log(ERROR,mysql_error(mysql));
		return 0;
	}	
	MYSQL_RES *recordSet = mysql_store_result(mysql); 
	if (recordSet==NULL){
		return 0;
	}
	
	//获取数据
	MYSQL_ROW row; //一个行数据的类型安全(type-safe)的表示
	if(row=mysql_fetch_row(recordSet))  //获取下一条记录
	{
		GL_time_limit=atoi(row[0]);
		GL_memory_limit=atoi(row[1]);		
		GL_spj=atoi(row[2]);
	}
	
	mysql_free_result(recordSet);//释放结果集
	return 1;
}

int SQL_getProblemInfo_contest(int contestId,int problemId,char *num){//num
	
	sprintf(query,"select num from contest_problem where contest_id=%d and problem_id=%d",contestId,problemId);
	
	int ret=mysql_real_query(mysql,query,(unsigned int)strlen(query));	  
	if(ret){			  
		write_log(ERROR,mysql_error(mysql));
		return 0;
	}	
	MYSQL_RES *recordSet = mysql_store_result(mysql); 
	if (recordSet==NULL){
		write_log(INFO,"SQL_getProblemInfo_contest ,No record");
		return 0;
	}
	
	//获取数据
	MYSQL_ROW row; //一个行数据的类型安全(type-safe)的表示
	if(row=mysql_fetch_row(recordSet))  //获取下一条记录
	{
		strcpy(num,row[0]);
	}
	
	mysql_free_result(recordSet);//释放结果集
	return 1;
}

int SQL_getContestInfo(int contestId,time_t &start_time,time_t &end_time){
	//start_time,end_time
	
	sprintf(query,"select start_time,end_time from contest where contest_id=%d",contestId);
	int ret=mysql_real_query(mysql,query,(unsigned int)strlen(query));	  
	if(ret){			  
		write_log(ERROR,mysql_error(mysql));
		return 0;
	}	
	MYSQL_RES *recordSet = mysql_store_result(mysql); 
	if (recordSet==NULL){
		return 0;
	}
	
	//获取数据
	MYSQL_ROW row; //一个行数据的类型安全(type-safe)的表示
	if(row=mysql_fetch_row(recordSet))  //获取下一条记录
	{
		StringToTimeEX(row[0],start_time);
		StringToTimeEX(row[1],end_time);
	}
	
	mysql_free_result(recordSet);//释放结果集
	return 1;
}
int SQL_getContestAttend(int contestId,char *username,char num,long &ac_time,int &wrongsubmits){
	//

	sprintf(query,"select %c_time,%c_wrongsubmits from attend where contest_id=%d and username='%s';",num,num,contestId,username);
	
	//cout<<query<<endl;
	int ret=mysql_real_query(mysql,query,(unsigned int)strlen(query));	  
	if(ret){			  
		write_log(ERROR,mysql_error(mysql));
		return 0;
	}	
	MYSQL_RES *recordSet = mysql_store_result(mysql); 
	if (recordSet==NULL){
		return 0;
	}

	MYSQL_ROW row;
	if(row=mysql_fetch_row(recordSet))  {
		ac_time=atol(row[0]);
		wrongsubmits=atoi(row[1]);
	}

	mysql_free_result(recordSet);
	return 1;
}
int SQL_countContestProblems(int contestId){
	sprintf(query,"select count(problem_id) from contest_problem where contest_id=%d",contestId);
	int ret=mysql_real_query(mysql,query,(unsigned int)strlen(query));	  
	if(ret){			  
		write_log(ERROR,mysql_error(mysql));
		return 0;
	}	
	MYSQL_RES *recordSet = mysql_store_result(mysql); 
	if (recordSet==NULL){
		return 0;
	}
	int nCount=0;
	MYSQL_ROW row; 
	if(row=mysql_fetch_row(recordSet))  
	{
		nCount=atoi(row[0]);
	}

	mysql_free_result(recordSet);
	return nCount;
}

int SQL_getFirstACTime_contest(int contestId,int problemId,char *username,time_t &ac_time,time_t start_time,time_t end_time){
	//
	string s_t,e_t;
	API_TimeToString(s_t,start_time);
	API_TimeToString(e_t,end_time);

	sprintf(query,"select submit_date from solution where contest_id=%d and problem_id=%d and username='%s'and verdict=%d and submit_date between '%s' and '%s' order by solution_id ASC limit 1;",contestId,problemId,username,V_AC,s_t.c_str(),e_t.c_str());
	int ret=mysql_real_query(mysql,query,(unsigned int)strlen(query));	  
	if(ret){			  
		write_log(ERROR,mysql_error(mysql));
		return 0;
	}	
	MYSQL_RES *recordSet = mysql_store_result(mysql); 
	if (recordSet==NULL){
	
		return 0;
	}

	MYSQL_ROW row;
	if(row=mysql_fetch_row(recordSet))  {
		
		StringToTimeEX(row[0],ac_time);
	}else {
		return 0;
	}
	mysql_free_result(recordSet);
	return 1;
}
long SQL_countProblemVerdict(int contestId,int problemId,int verdictId,char *username){

	sprintf(query,"select count(solution_id) from solution where contest_id=%d and problem_id=%d and verdict=%d and username='%s'",contestId,problemId,verdictId,username);
	int ret=mysql_real_query(mysql,query,(unsigned int)strlen(query));	  
	if(ret){			  
		write_log(ERROR,mysql_error(mysql));
		return 0;
	}	
	MYSQL_RES *recordSet = mysql_store_result(mysql); 
	if (recordSet==NULL){
		return 0;
	}
	
	long nCount=0;

	//获取数据
	MYSQL_ROW row; //一个行数据的类型安全(type-safe)的表示
	if(row=mysql_fetch_row(recordSet))  //获取下一条记录
	{
		nCount=atoi(row[0]);
	}
	mysql_free_result(recordSet);//释放结果集
	return nCount;
}


void SQL_updateSolution(int solutionId,int verdictId,int testCase,int time,int memory)
{
	sprintf(query,"update solution set verdict=%d,testcase=%d,time=%d,memory=%d where solution_id=%d;",verdictId,testCase,time,memory,solutionId);
	//	printf("%s\n",query);  
	if(mysql_real_query(mysql,query,(unsigned int)strlen(query))){ 			  
		write_log(ERROR,mysql_error(mysql));
	}
}

void SQL_updateProblem(int problemId)
{
	sprintf(query,"update problem set accepted=(SELECT count(*) FROM solution WHERE problem_id=%d and verdict=%d) where problem_id=%d;",problemId,V_AC,problemId);
//cout<<query;
	if(mysql_real_query(mysql,query,(unsigned int)strlen(query))){ 			  
		write_log(ERROR,mysql_error(mysql));			
	}
	sprintf(query,"update problem set submit=(SELECT count(*) FROM solution WHERE problem_id=%d)  where problem_id=%d;",problemId,problemId);
	if(mysql_real_query(mysql,query,(unsigned int)strlen(query))){ 			  
		write_log(ERROR,mysql_error(mysql));		
	}
	sprintf(query,"update problem set solved=(SELECT count(DISTINCT username) FROM solution WHERE problem_id=%d and verdict=%d) where problem_id=%d;",problemId,V_AC,problemId);
	if(mysql_real_query(mysql,query,(unsigned int)strlen(query))){ 			  
		write_log(ERROR,mysql_error(mysql));		
	}
	sprintf(query,"update problem set submit_user=(SELECT count(DISTINCT username) FROM solution WHERE problem_id=%d) where problem_id=%d;",problemId,problemId);
	if(mysql_real_query(mysql,query,(unsigned int)strlen(query))){ 			  
		write_log(ERROR,mysql_error(mysql));			
	}
}
void SQL_updateProblem_contest(int contestId,int problemId)
{
	sprintf(query,"update contest_problem set accepted=(SELECT count(*) FROM solution WHERE contest_id=%d and problem_id=%d and verdict=%d) where contest_id=%d and problem_id=%d;",contestId,V_AC,contestId,problemId);
//	cout<<query;
	if(mysql_real_query(mysql,query,(unsigned int)strlen(query))){ 			  
		write_log(ERROR,mysql_error(mysql));			
	}
	sprintf(query,"update contest_problem set submit=(SELECT count(*) FROM solution WHERE contest_id=%d and problem_id=%d)  where contest_id=%d and problem_id=%d;",contestId,problemId,contestId,problemId);
	if(mysql_real_query(mysql,query,(unsigned int)strlen(query))){ 			  
		write_log(ERROR,mysql_error(mysql));		
	}
	sprintf(query,"update contest_problem set solved=(SELECT count(DISTINCT username) FROM solution WHERE contest_id=%d and problem_id=%d and verdict=%d) where contest_id=%d and problem_id=%d;",contestId,problemId,V_AC,contestId,problemId);
	if(mysql_real_query(mysql,query,(unsigned int)strlen(query))){ 			  
		write_log(ERROR,mysql_error(mysql));		
	}
	sprintf(query,"update contest_problem set submit_user=(SELECT count(DISTINCT username) FROM solution WHERE contest_id=%d and problem_id=%d) where contest_id=%d and problem_id=%d;",contestId,problemId,contestId,problemId);
	if(mysql_real_query(mysql,query,(unsigned int)strlen(query))){ 			  
		write_log(ERROR,mysql_error(mysql));			
	}
}

int SQL_countACContestProblems(int contestId,char *username,time_t start_time,time_t end_time){

	string s_t,e_t;
	API_TimeToString(s_t,start_time);
	API_TimeToString(e_t,end_time);

	sprintf(query,"select count(distinct(problem_id)) from solution where  verdict=%d and contest_id=%d and username='%s' and submit_date between '%s' and '%s'",V_AC,contestId,username,s_t.c_str(),e_t.c_str());

	if(mysql_real_query(mysql,query,(unsigned int)strlen(query))){			  
		write_log(ERROR,mysql_error(mysql));
		return 0;
	}	
	MYSQL_RES *recordSet = mysql_store_result(mysql); 
	if (recordSet==NULL){
		return 0;
	}
	int nCount=0;
	MYSQL_ROW row; 
	if(row=mysql_fetch_row(recordSet))  
	{
		nCount=atoi(row[0]);
	}

	mysql_free_result(recordSet);
	return nCount;
}


int SQL_getContestScore(int contestId,char *username,time_t start_time,time_t end_time){

	if(SQL_countACContestProblems(contestId,username,start_time,end_time)==0){
		return 0;
	}
	string s_t,e_t;
	API_TimeToString(s_t,start_time);
	API_TimeToString(e_t,end_time);

	sprintf(query,"SELECT sum(point) from contest_problem where contest_id=%d and problem_id in (select distinct(problem_id) from solution where  verdict=%d and contest_id=%d and username='%s' and submit_date between '%s' and '%s')",contestId,V_AC,contestId,username,s_t.c_str(),e_t.c_str());

	if(mysql_real_query(mysql,query,(unsigned int)strlen(query))){			  
		write_log(ERROR,mysql_error(mysql));
		return 0;
	}	
	MYSQL_RES *recordSet = mysql_store_result(mysql); 
	if (recordSet==NULL){
		write_log(ERROR,"SQL_getContestScore recordSet ERROR");
	}
	int point=0;
	MYSQL_ROW row; 
	
	if(row=mysql_fetch_row(recordSet))  
	{		
		point=atoi(row[0]);
	}
	mysql_free_result(recordSet);
	return point;
}

void SQL_updateAttend_contest(int contestId,int verdictId,int problemId,char *num,char *username,time_t start_time,time_t end_time){
	
	//已经AC的不需要修改attend
	//update ac_time
	long AC_time=0;		
	time_t first_ac_t;
	if(SQL_getFirstACTime_contest(contestId,problemId,username,first_ac_t,start_time,end_time)){
		AC_time=getdiftime(first_ac_t,start_time);
	}else{
		AC_time=0;	
		first_ac_t = end_time;
	}
	sprintf(query,"update attend set %s_time=%ld where contest_id=%d and username='%s';",num,AC_time,contestId,username);
	//cout<<query<<endl;
	if(mysql_real_query(mysql,query,(unsigned int)strlen(query))){ 			  
		write_log(ERROR,mysql_error(mysql));			
	}

	long ac_nCount=SQL_countProblemVerdict(GL_contestId,problemId,V_AC,username);
	int score_ = SQL_getContestScore(contestId,username,start_time,end_time);
	string s_t,e_t,fAC_t;
	API_TimeToString(s_t,start_time);
	API_TimeToString(e_t,end_time);
	API_TimeToString(fAC_t,first_ac_t);

	//update score solved ,wrongsubmits
	sprintf(query,"update attend set solved=(SELECT count(DISTINCT problem_id) FROM solution WHERE contest_id=%d and username='%s' and verdict=%d and submit_date between '%s' and '%s'),%s_wrongsubmits=(SELECT count(solution_id) FROM solution WHERE contest_id=%d and problem_id=%d and username='%s' and verdict>%d and submit_date between '%s' and '%s'),score=%d  where contest_id=%d and username='%s';",contestId,username,V_AC,s_t.c_str(),e_t.c_str(),   num,contestId,problemId,username,V_AC,s_t.c_str(),fAC_t.c_str(),  score_,  contestId,username);

//	puts(query);	
	if(mysql_real_query(mysql,query,(unsigned int)strlen(query))){ 			  
			write_log(ERROR,mysql_error(mysql));		
	}

	//penalty
	int nCountProblems=SQL_countContestProblems(contestId);
	char index='A';
	long penalty=0;
	for(int i=0;i<nCountProblems;i++){
		long a_time_=0;
		int wrongsubmits_=0;
		SQL_getContestAttend(contestId,username,index+i,a_time_,wrongsubmits_);
		//cout<<a_time_<<" "<<wrongsubmits_<<endl;
		if(a_time_>0){
			penalty=penalty+a_time_+wrongsubmits_*60*20;
		}
	}
	sprintf(query,"update attend set penalty=%ld where contest_id=%d and username='%s';",penalty,contestId,username);
	if(mysql_real_query(mysql,query,(unsigned int)strlen(query))){ 			  
		write_log(ERROR,mysql_error(mysql));	
	}


}

void SQL_updateUser(char *username)
{
	sprintf(query,"update users set submit=(SELECT count(*) FROM solution WHERE username='%s') where username='%s';",username,username);
	if(mysql_real_query(mysql,query,(unsigned int)strlen(query))){ 			  
		write_log(ERROR,mysql_error(mysql));		
	}
	sprintf(query,"update users set solved=(SELECT count(DISTINCT problem_id) FROM solution WHERE username='%s' and verdict=%d) where username='%s';",username,V_AC,username);
	if(mysql_real_query(mysql,query,(unsigned int)strlen(query))){ 			  
		write_log(ERROR,mysql_error(mysql));		
	}
}


void SQL_updateCompileInfo(int solutionId)
{
	FILE *fp;
	char buffer[4096]={0};
	if ((fp = fopen (DebugFile, "r")) == NULL){
		write_log(ERROR,"DebugFile open error");
		return ;
	}
	//先删除
	sprintf(query,"delete from compile_info  where solution_id=%d;",solutionId);
	mysql_real_query(mysql,query,(unsigned int)strlen(query));	  
	
	if((fgets(buffer, 4095, fp))!= NULL){  //先插入 	
		sprintf(query,"insert into compile_info values(%d,\"%s\");",solutionId,buffer);
		int ret=mysql_real_query(mysql,query,(unsigned int)strlen(query));	  
		if(ret){			  
			write_log(ERROR,mysql_error(mysql));
			fclose(fp);
			return ;	
		}	
	}
	while ((fgets (buffer, 4095, fp))!= NULL){ //后连接
		buffer[strlen(buffer)];	
		sprintf(query,"update compile_info set error=CONCAT(error,\"%s\") where solution_id=%d;",buffer,solutionId);
		int ret=mysql_real_query(mysql,query,(unsigned int)strlen(query));	  
		if(ret){			  
			write_log(ERROR,mysql_error(mysql));
			fclose(fp);
			return ;	
		}	
 	}
	fclose(fp);
}
 
DWORD WINAPI CompileThread(LPVOID lp) //ac
{
	
	SQL_updateSolution(GL_solutionId,V_C,0,0,0); //V_C Compiling
	system(compileCmd_str);

	return 0;
}
//编译,是否应该防止编译器假死造成的卡死
int compile(){

	if(strcmp(compileCmd_str,"NULL")==0) return 1;

	HANDLE hThread_com;
	hThread_com=CreateThread(NULL,NULL,CompileThread,NULL,0,NULL);
	if(hThread_com==NULL) {
		write_log(ERROR,"Create CompileThread Error");
		CloseHandle(hThread_com);
	}
		
	DWORD status_ = WaitForSingleObject(hThread_com,30000);   //30S 编译时间,返回值大于零说明超时
	if(status_>0){
		write_log(WARNING,"Compile over time_limit");
	}
	//是否正常生成用户的可执行程序
	if( (_access(exePath, 0 )) != -1 )   {
		return 1;
	}
	else return 0;
}

//是否存在异常
BOOL RUN_existException(DWORD dw){
	switch(dw){
	case EXCEPTION_ACCESS_VIOLATION:
		return TRUE;
	case EXCEPTION_DATATYPE_MISALIGNMENT:   
		return TRUE;
	case EXCEPTION_BREAKPOINT:  
		return TRUE;
	case EXCEPTION_SINGLE_STEP:  
		return TRUE;
	case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:     
		return TRUE;
	case EXCEPTION_FLT_DENORMAL_OPERAND:  
		return TRUE;
	case EXCEPTION_FLT_DIVIDE_BY_ZERO:  
		return TRUE;
	case EXCEPTION_FLT_INEXACT_RESULT: 
		return TRUE;
	case EXCEPTION_FLT_INVALID_OPERATION: 
		return TRUE;
	case EXCEPTION_FLT_OVERFLOW: 
		return TRUE;
	case EXCEPTION_FLT_STACK_CHECK:  
		return TRUE;
	case EXCEPTION_FLT_UNDERFLOW:   
		return TRUE;
	case EXCEPTION_INT_DIVIDE_BY_ZERO:  
		return TRUE;
	case EXCEPTION_INT_OVERFLOW:   
		return TRUE;
	case EXCEPTION_PRIV_INSTRUCTION:    
		return TRUE;
	case EXCEPTION_IN_PAGE_ERROR:  
		return TRUE;
	case EXCEPTION_ILLEGAL_INSTRUCTION: 
		return TRUE;
	case EXCEPTION_NONCONTINUABLE_EXCEPTION: 
		return TRUE;
	case EXCEPTION_STACK_OVERFLOW:  
		return TRUE;
	case EXCEPTION_INVALID_DISPOSITION:
		return TRUE;
	case EXCEPTION_GUARD_PAGE:  
		return TRUE;
	case EXCEPTION_INVALID_HANDLE: 
		return TRUE;
	default:
		return FALSE;
	}          
}

HANDLE G_job=NULL;
HANDLE InputFile ;  //父进程输入文件句柄
HANDLE OutputFile;  //子进程标准输出句柄
DWORD dw_code;	//定义进程状态

HANDLE CreateSandBox(){
	HANDLE hjob =CreateJobObject(NULL,NULL);
	if(hjob!=NULL)
	{
		JOBOBJECT_BASIC_LIMIT_INFORMATION jobli;
		 memset(&jobli,0,sizeof(jobli));
		jobli.LimitFlags=JOB_OBJECT_LIMIT_PRIORITY_CLASS|JOB_OBJECT_LIMIT_ACTIVE_PROCESS;
		jobli.PriorityClass=IDLE_PRIORITY_CLASS;
		jobli.ActiveProcessLimit=nProcessLimit; //Limit of processes
	//	jobli.MinimumWorkingSetSize= 1;
	//	jobli.MaximumWorkingSetSize= 1024*GL_memory_limit;|JOB_OBJECT_LIMIT_WORKINGSET|JOB_OBJECT_LIMIT_PROCESS_TIME
	//	jobli.PerProcessUserTimeLimit.QuadPart=10000*(GL_time_limit+2000); 
		if(SetInformationJobObject(hjob,JobObjectBasicLimitInformation,&jobli,sizeof(jobli)))
		{
			JOBOBJECT_BASIC_UI_RESTRICTIONS jobuir;
			jobuir.UIRestrictionsClass=JOB_OBJECT_UILIMIT_NONE;
			jobuir.UIRestrictionsClass |=JOB_OBJECT_UILIMIT_EXITWINDOWS;
			jobuir.UIRestrictionsClass |=JOB_OBJECT_UILIMIT_READCLIPBOARD ;
			jobuir.UIRestrictionsClass |=JOB_OBJECT_UILIMIT_WRITECLIPBOARD ;
			jobuir.UIRestrictionsClass |=JOB_OBJECT_UILIMIT_SYSTEMPARAMETERS;
			jobuir.UIRestrictionsClass |=JOB_OBJECT_UILIMIT_HANDLES;

			if(SetInformationJobObject(hjob,JobObjectBasicUIRestrictions,&jobuir,sizeof(jobuir)))
			{
				return hjob;
			}
			else
			{
				write_log(SYSTEM_ERROR,"SetInformationJobObject  JOBOBJECT_BASIC_UI_RESTRICTIONS   [Error:%d]\n",GetLastError());
			}
		}
		else
		{
			write_log(SYSTEM_ERROR,"SetInformationJobObject  JOBOBJECT_BASIC_LIMIT_INFORMATION   [Error:%d]\n",GetLastError());
		}
	}
	else
	{
		write_log(SYSTEM_ERROR,"CreateJobObject     [Error:%d]\n",GetLastError());
	}
	return NULL;
}

bool ProcessToSandbox(HANDLE job,PROCESS_INFORMATION p){
	if(AssignProcessToJobObject(job,p.hProcess))
	{
		//顺便调整本进程优先级为高
		HANDLE   hPS   =   OpenProcess(PROCESS_ALL_ACCESS,   false,  p.dwProcessId); 
		if(!SetPriorityClass(hPS,   HIGH_PRIORITY_CLASS)){
			write_log(SYSTEM_ERROR,"SetPriorityClass        [Error:%d]\n",GetLastError());
		}
		CloseHandle(hPS);
		return true;
	}
	else
	{					
		write_log(SYSTEM_ERROR,"AssignProcessToJobObject Error:%s",GetLastError());
	}	
	return false;
}

DWORD WINAPI RUN_ProgramThread(LPVOID lp) //ac
{
	/// cmd/c solution.exe <data.in >data.out 2>error.txt
	//ChildIn_Write是子进程的输入句柄，ChildIn_Read是父进程用于写入子进程输入的句柄
	HANDLE ChildIn_Read, ChildIn_Write;
	//ChildOut_Write是子进程的输出句柄，ChildOut_Read是父进程用于读取子进程输出的句柄
	HANDLE ChildOut_Read, ChildOut_Write;

	SECURITY_ATTRIBUTES saAttr = {0};
	saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
	saAttr.bInheritHandle = TRUE; 
	saAttr.lpSecurityDescriptor = NULL;

	CreatePipe(&ChildIn_Read, &ChildIn_Write, &saAttr, 0);
	SetHandleInformation(ChildIn_Write, HANDLE_FLAG_INHERIT, 0);
	CreatePipe(&ChildOut_Read, &ChildOut_Write, &saAttr, 0);
	SetHandleInformation(ChildOut_Read, HANDLE_FLAG_INHERIT, 0);
	
	SetErrorMode(SEM_NOGPFAULTERRORBOX );

	STARTUPINFO StartupInfo = {0};
	StartupInfo.cb = sizeof(STARTUPINFO);
	//	StartupInfo.hStdError = h_ErrorFile;		
	StartupInfo.hStdOutput = ChildOut_Write;
	StartupInfo.hStdInput = ChildIn_Read;
	StartupInfo.dwFlags |= STARTF_USESTDHANDLES;

	if(CreateProcess(NULL,runCmd_str,NULL,NULL,TRUE,CREATE_SUSPENDED|CREATE_NEW_CONSOLE,NULL,NULL,&StartupInfo,&G_pi))
	{
		
		G_job = CreateSandBox();
		if(G_job!=NULL)
		{
			if(ProcessToSandbox(G_job,G_pi))
			{
				ResumeThread(G_pi.hThread); 
				CloseHandle(G_pi.hThread);

				InputFile= CreateFile(inFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_READONLY, NULL);
				BOOL flag = FALSE;
				while (true)
				{
					char buffer[BUFSIZE] = {0};
					DWORD BytesRead, BytesWritten;
					flag = ReadFile(InputFile, buffer, BUFSIZE, &BytesRead, NULL);
					if (!flag || (BytesRead == 0)) break;
					flag = WriteFile(ChildIn_Write, buffer, BytesRead, &BytesWritten, NULL);

					if (!flag){ break;}
				}

				CloseHandle(InputFile);InputFile=NULL;
				CloseHandle(ChildIn_Write);ChildIn_Write=NULL;
				CloseHandle(ChildOut_Write);ChildOut_Write=NULL;

				//读取子进程的标准输出，并将其传递给文件输出
				OutputFile= CreateFile(outFileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
				
				startt = clock();

				DWORD limit_output =0; 
				while (true)
				{   
					char buffer[BUFSIZE] = {0};
					DWORD BytesRead, BytesWritten;				
					flag = ReadFile(ChildOut_Read, buffer, BUFSIZE, &BytesRead, NULL);
					if (!flag || (BytesRead == 0)) break;
					flag = WriteFile(OutputFile, buffer, BytesRead, &BytesWritten, NULL);
					if (!flag) break;

					limit_output+=BytesWritten;
					if(limit_output>OutputLimit){
						write_log(INFO,"OLE");
						GL_verdictId = V_OLE;
						//CloseHandle(pi.hProcess);								
						break;
					}
				}
				endt = clock();

				CloseHandle(ChildIn_Read);ChildIn_Read=NULL;
				CloseHandle(ChildOut_Read);ChildOut_Read=NULL;
				CloseHandle(OutputFile);OutputFile=NULL;
				//printf("OK\n");
				return 1;
			}else{
				write_log(SYSTEM_ERROR,"ProcessToSandBox Error:%s",GetLastError());
			}
		}
		else{
			write_log(SYSTEM_ERROR,"CreateSandBox Error:%s",GetLastError());
		}
	}
	else
	{
		write_log(SYSTEM_ERROR,"CreateProcess       [Error:%d]\n",GetLastError());
	}
	GL_verdictId = V_SE;
	return 0;
}

//spj死循环风险、崩溃风险||应做时间限制
int special_judge(const char *inFile,const char *uOutFile){ //return 0 ====Error,return 1 ====Accepted
	int judge ;
	char spj_path[MAX_PATH];
	sprintf(spj_path,"%s%d\\spj_%d.exe %s %s",dataPath,GL_problemId,GL_problemId,inFile,uOutFile);
	judge = system(spj_path) ;
	//cout<<judge<<endl;
	if (judge == -1){
		//printf("system error!") ;
		return 0;
	}
	if(judge == 1){  //spj返回1,表示程序正确
		return 1;
	}
	return 0;
}

int RUN_solution(int solutionId){ //有跳过测试样例的风险
	long caseTime=0;
	int i,case_;
	dw_code=0;

	for(i=0;;++i){
		if(i==0){
			case_=1;
		}else{
			case_=i;
		}
		sprintf(inFileName,"%s%d\\data%d.in",dataPath,GL_problemId,case_);   
		sprintf(outFileName,"%s%d%_%d.out",workPath,solutionId,case_);
		
		if( (_access(inFileName, 0 )) == -1 )   {
			write_log(INFO,"Test over..");
			break ;
		}
		GL_testcase = case_;
		
		SQL_updateSolution(solutionId,V_RUN,case_,GL_time-GL_time%10,GL_memory);

		HANDLE hThread_run;
		hThread_run=CreateThread(NULL,NULL,RUN_ProgramThread,NULL,0,NULL);
		if(hThread_run==NULL) {
			write_log(ERROR,"Create thread error");
			CloseHandle(hThread_run);
		}
		
		DWORD status_ = WaitForSingleObject(hThread_run,GL_time_limit+2000);   //放宽时限2S,返回值大于零说明超时.
		if(status_>0){
			puts("hThread_run TIME LIMIT");
			GL_time = GL_time_limit;
			if(GL_verdictId==V_AC)
			{
				GL_verdictId = V_TLE;
			}
		}
	
		if(InputFile!=NULL) CloseHandle(InputFile);
		if(OutputFile!=NULL) CloseHandle(OutputFile);
		
 
		

		//get memory info
		PROCESS_MEMORY_COUNTERS   pmc; 
		unsigned long tmp_memory=0;
		if(GetProcessMemoryInfo(G_pi.hProcess,&pmc,sizeof(pmc))) { 
			tmp_memory=pmc.PeakWorkingSetSize/1024;
			if(tmp_memory>GL_memory) GL_memory=tmp_memory;
		}

		//get process state
		GetExitCodeProcess(G_pi.hProcess, &dw_code);
		if(RUN_existException(dw_code))//是否在运行中出现"常见"异常，若存在，修正评判结果为Runtime error	
		{
			GL_verdictId=V_RE;
			goto l;
		}
		else if(dw_code==STILL_ACTIVE)
		{	//超时	
			puts("TIME LIMIT");
			TerminateProcess(G_pi.hProcess, 0); 
			if(GL_verdictId==V_AC)
			{
				//TLE 超时
				GL_verdictId=V_TLE;
				GL_time = GL_time_limit;
				goto l;
			}
			
		}
		caseTime = endt - startt;
		if(caseTime<0){
			caseTime = GL_time_limit;
		}

		TerminateJobObject(G_job,0);//exit
		CloseHandle(G_pi.hProcess);
		CloseHandle(G_job);G_job=NULL;


		GL_time = (caseTime>GL_time)?caseTime:GL_time;

		if(GL_time>=GL_time_limit){
			GL_verdictId=V_TLE;
			GL_time = GL_time_limit;
			goto l;
		}
		if(GL_memory>=GL_memory_limit){
			GL_verdictId=V_MLE;
			GL_memory = GL_memory_limit;
			goto l;
		}
		//OLE
		if(GL_verdictId!=V_AC){
			goto l;
		}
		//judge file，spj or not
		
		if(GL_spj==1){
			int verdict_ = special_judge(inFileName,outFileName);
			if(verdict_) GL_verdictId=V_AC;
			else GL_verdictId=V_WA;
		}else{
			char srcPath[MAX_PATH];
			char ansPath[MAX_PATH];
			sprintf(srcPath, "%s",outFileName);
			sprintf(ansPath, "%s%d\\data%d.out",dataPath,GL_problemId,case_);
			GL_verdictId = compare(srcPath,ansPath);
		}
		
l:		write_log(INFO,"ID:%d Test%d ,%s ,%dms %dkb ,Return code:%u",GL_solutionId,i,VERDICT_NAME[GL_verdictId],caseTime,tmp_memory,dw_code);
		
		if(GL_verdictId!=V_AC){
			break;
		}
		if(i==0){
			GL_time=0;
			GL_memory=0;
		}
	}
	return 0;

}

void resetVal(){
	GL_verdictId=V_AC;
	GL_contestId=0;
	GL_time=0;
	GL_memory=0;
	GL_time_limit=1000;
	GL_memory_limit=65535;
	GL_reJudge=0;
	GL_testcase=0;
}

int work(int solutionId){  //开始工作
	
	GL_solutionId = solutionId;

	resetVal();//重置

	SQL_getSolutionInfo();

	InitPath();  //包含sourcePath,所以在SQL_getSolutionSource之前

	SQL_getSolutionSource();  //取出source，并保存到sourcePath

	SQL_getProblemInfo();	//problem info

	GL_time_limit*=limitIndex;
	GL_memory_limit*=limitIndex;

	//write_log(INFO,"Time_Limit:%d ms , Memory_Limit:%d kb",GL_time_limit,GL_memory_limit);

	//compile
	write_log(INFO,"Start Compile...");

	if(0==compile()){
		//compile error...
		write_log(INFO,"Compile Error");
		GL_verdictId=V_CE;
		SQL_updateCompileInfo(GL_solutionId);
	}else{
		//start run
		write_log(INFO,"Start Run...");
		RUN_solution(GL_solutionId);
	}

	string time_string_;
	API_TimeToString(time_string_,GL_submitDate);
	//update MySQL............
	write_log(INFO,"ID:%d ->Rusult:%s Case:%d %dms %dkb ,Return code:%u at %s by %s",GL_solutionId,VERDICT_NAME[GL_verdictId],GL_testcase,GL_time-GL_time%10,GL_memory,dw_code,time_string_.c_str(),GL_username);
	SQL_updateSolution(GL_solutionId,GL_verdictId,GL_testcase,GL_time-GL_time%10,GL_memory);
	SQL_updateProblem(GL_problemId);
	SQL_updateUser(GL_username);

	//contest or not 
	if(GL_contestId>0){ 

		time_t contest_s_time,contest_e_time;
		//contest judge
		char num[10]={0};  
		SQL_getProblemInfo_contest(GL_contestId,GL_problemId,num);  //获取contest problem题目标号
		SQL_getContestInfo(GL_contestId,contest_s_time,contest_e_time);
		if(contest_e_time>GL_submitDate){  //比赛running ，修改Attend
			SQL_updateAttend_contest(GL_contestId,GL_verdictId,GL_problemId,num,GL_username,contest_s_time,contest_e_time);
		}
		SQL_updateProblem_contest(GL_contestId,GL_problemId);
	}

	//删除文件
	DeleteFile(sourcePath);
	DeleteFile(DebugFile);
	DeleteFile(exePath);

	return 1;
}

DWORD WINAPI WorkThread(LPVOID lpParam)
{
	JUDGE_DATA jd;
	time_t first_t,second_t;
	string str_time;
    time(&first_t);
	while(1)
	{
		//cout<<"size"<<Q.size()<<endl;
		if(Q.size()>limitJudge) {int a=0;cout<<2/a<<endl; return 0;} //保护系统,故作此限制,最大接受limitJudge个同时等待,异常退出重启
		if(!Q.empty())
		{
				jd=Q.front();
			  	GL_currentId=jd.solutionId;
				//启动评判
				work(GL_currentId); 
				Q.pop();
				//cout<<"judge over"<<endl;
		}
		Sleep(1);
	}  
	write_log(ERROR,"WorkThread Crash");
	return 0;
} 
DWORD WINAPI ListenThread(LPVOID lpParam)
{
	// 循环接受客户的连接请求
	sockaddr_in remoteAddr; 
	SOCKET sClient;
	//初始化客户地址长度
	int nAddrLen = sizeof(remoteAddr);
	JUDGE_DATA j;
	while(TRUE)
	{
		sClient = accept(sListen, (SOCKADDR*)&remoteAddr, &nAddrLen);
		if(sClient == INVALID_SOCKET){
			write_log(ERROR,"Accept() Error");
			continue;
		}
		int ret=recv(sClient,(char*)&j,sizeof(j),0);
		if(ret>0){
			write_log(INFO,"Push SolutionId:%d into Judge Queue....",j.solutionId);
			Q.push(j);
		}
		Sleep(1);
	}
	write_log(ERROR,"ListenThread Crash");
	closesocket(sClient);
	return 0;
}

long WINAPI ExceptionFilter(EXCEPTION_POINTERS * lParam)
{
	cout<<"System Error!"<<endl<<"System may restart after 2 seconds..."<<endl;
	Sleep(2000);
	ShellExecuteA(NULL,"open",judgePath,NULL,NULL,SW_SHOWNORMAL); 
	return EXCEPTION_EXECUTE_HANDLER;
}

int main(int argc, char **argv)
{

	SetUnhandledExceptionFilter(ExceptionFilter);
 	SetErrorMode(SEM_NOGPFAULTERRORBOX ); 
	
	if( (_access(logPath, 0 )) == -1 )   {
		CreateDirectory(logPath,NULL);
	}
	write_log(INFO,"===>Running Judge Core<===");
	InitMySqlConfig();
	if(InitMySQL()==0){//初始化mysql
		write_log(ERROR,"Init MySQL ERROR...");
		return 0;
	}	
	write_log(INFO,"Init MySQL Success...");
	if(initSocket()==0){         // 初始化socket
		write_log(ERROR,"Init Socket ERROR...");
		return 0;
	}
	//创建工作线程
	HANDLE hThreadW;
	hThreadW=CreateThread(NULL,NULL,WorkThread,0,0,0);
	//进入循环，等待客户连接请求
	HANDLE hThreadR=CreateThread(NULL,NULL,ListenThread,0,0,0);

	write_log(INFO,"End of initialization...");

	while(TRUE){
		Sleep(1);
	}
	write_log(ERROR,"Crash!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
	closesocket(sListen);
	WSACleanup();
	return 0;
}