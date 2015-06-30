/**
* 　　　　　　　　┏┓　　　┏┓
* 　　　　　　　┏┛┻━━━┛┻┓
* 　　　　　　　┃　　　　　　　┃ 　
* 　　　　　　　┃　　　━　　　┃
* 　　　　　　　┃　＞　　　＜　┃
* 　　　　　　　┃　　　　　　　┃
* 　　　　　　　┃...　⌒　...　┃
* 　　　　　　　┃　　　　　　　┃
* 　　　　　　　┗━┓　　　┏━┛
* 　　　　　　　　　┃　　　┃　Code is far away from bug with the animal protecting　　　　　　　　　　
* 　　　　　　　　　┃　　　┃ 神兽保佑,代码无bug
* 　　　　　　　　　┃　　　┃　　　　　　　　　　　
* 　　　　　　　　　┃　　　┃ 　　　　　　
* 　　　　　　　　　┃　　　┃
* 　　　　　　　　　┃　　　┃　　　　　　　　　　　
* 　　　　　　　　　┃　　　┗━━━┓
* 　　　　　　　　　┃　　　　　　　┣┓
* 　　　　　　　　　┃　　　　　　　┏┛
* 　　　　　　　　　┗┓┓┏━┳┓┏┛
* 　　　　　　　　　　┃┫┫　┃┫┫
* 　　　　　　　　　　┗┻┛　┗┻┛
*/

#include <iostream>
#include <winsnmp.h>
#include <stdlib.h>
#include <Windows.h>
#include <string>
#include <vector>
#include <stdlib.h>
#include <sstream>
#include <iomanip>

using namespace std;

class MySNMP{
#define	strvec		vector< string >
#define strvec_vec	vector< strvec >
private:
	smiUINT32			nMajorVersion;		//主版本号
	smiUINT32			nMinorVersion;		//次版本号	
	smiUINT32			nLevel;				//支持层次
	smiUINT32			nTranslateMode;		//Translate模式
	smiUINT32			nRetransmitMode;	//重传模式
	smiOCTETS			Community;			//团体名
	smiOID				sOid;
	string				IPAddr;
	HSNMP_SESSION		hSession;			//会话句柄
	HSNMP_CONTEXT		hContext;			//上下文句柄
	HANDLE				hEvent;				//实践句柄
	HSNMP_ENTITY		hSrcEntity;			//源实体句柄
	HSNMP_ENTITY		hDestEntity;		//目的实体句柄
	HSNMP_VBL			hVbl;				//变量句柄
	HSNMP_PDU			hPdu;				//PDU句柄

public:
	MySNMP();								//构造函数
	MySNMP(string, string);					//同上
	~MySNMP();								//析构函数
	bool InitSnmp();						//初始化WinSNMP
	static SNMPAPI_STATUS CALLBACK CallBack(HSNMP_SESSION, HWND, UINT, WPARAM, LPARAM, LPVOID);	//回调函数
	bool Send(const strvec&, smiINT);		//发送
	bool Receive(HSNMP_VBL&);				//接收
	void Setevent(){ SetEvent(hEvent); }	//脱裤子放屁，但是懒得改
	bool GetValue(const string&, string&);	//获得变量值
	string ValueToString(smiVALUE);			//变量值转换成字符串
	bool GetTable(strvec, strvec_vec&);		//获取表
	void GetSNMPInfo();						//获取WinSNMP信息
	bool Send(smiVALUE, smiINT);
	bool SetValue(const string&, int);
};