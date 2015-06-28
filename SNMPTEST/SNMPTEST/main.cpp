//                            _ooOoo_  
//                           o8888888o  
//                           88" . "88  
//                           (| -_- |)  
//                            O\ = /O  
//                        ____/`---'\____  
//                      .   ' \\| |// `.  
//                       / \\||| : |||// \  
//                     / _||||| -:- |||||- \  
//                       | | \\\ - /// | |  
//                     | \_| ''\---/'' | |  
//                      \ .-\__ `-` ___/-. /  
//                   ___`. .' /--.--\ `. . __  
//                ."" '< `.___\_<|>_/___.' >'"".  
//               | | : `- \`.;`\ _ /`;.`/ - ` : | |  
//                 \ \ `-. \_ __\ /__ _/ .-` / /  
//         ======`-.____`-.___\_____/___.-`____.-'======  
//                            `=---='  
//  
//         .............................................  
//                  佛祖保佑             永无BUG 
//          佛曰:  
//                  写字楼里写字间，写字间里程序员；  
//                  程序人员写程序，又拿程序换酒钱。  
//                  酒醒只在网上坐，酒醉还来网下眠；  
//                  酒醉酒醒日复日，网上网下年复年。  
//                  但愿老死电脑间，不愿鞠躬老板前；  
//                  奔驰宝马贵者趣，公交自行程序员。  
//                  别人笑我忒疯癫，我笑自己命太贱；  
//                  不见满街漂亮妹，哪个归得程序员？ 

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
};

MySNMP::MySNMP()
{
	//这是空哒~
}

MySNMP::MySNMP(string strIP, string strCommunity)
	:nMajorVersion(0), nMinorVersion(0), nLevel(0), nTranslateMode(0), nRetransmitMode(0), IPAddr(strIP)	
{
	//设置团体名
	Community.len = strCommunity.length();
	Community.ptr = new smiBYTE[strCommunity.length()];
	memcpy(Community.ptr, strCommunity.c_str(), strCommunity.length());
	//初始化WinSNMP
	InitSnmp();
	//设置事件
	hEvent = CreateEvent(NULL, true, false, NULL);
}

bool MySNMP::InitSnmp()
{
	//初始化WinSNMP
	if (SnmpStartup(&nMajorVersion, &nMinorVersion, &nLevel, &nTranslateMode, &nRetransmitMode) == SNMPAPI_FAILURE){
		cout << "加载失败" << endl;
		return false;
	}
	//设置传输模式
	if (SnmpSetTranslateMode(nTranslateMode) == SNMPAPI_FAILURE){
		cout << "设置传输模式失败" << endl;
		return false;
	}
	//设置重传模式
	if (SnmpSetRetransmitMode(nRetransmitMode) == SNMPAPI_FAILURE){
		cout << "设置重传模式失败" << endl;
		return false;
	}
	//建立会话
	hSession = SnmpCreateSession(NULL, NULL, MySNMP::CallBack, (LPVOID)this);
	if (hSession == SNMPAPI_FAILURE){
		cout << "建立会话失败" << endl;
		return false;
	}
	//创建实体
	HSNMP_ENTITY hEntity;
	if ((hEntity = SnmpStrToEntity(hSession, IPAddr.c_str())) == SNMPAPI_FAILURE){
		cout << "创建实体失败" << endl;
		return false;
	}
	//建立上下文
	if ((hContext = SnmpStrToContext(hSession, &Community)) == SNMPAPI_FAILURE){
		cout << "建立上下文失败" << endl;
		return false;
	}
	//设置超时
	if (SnmpSetTimeout(hEntity, 10) == SNMPAPI_FAILURE){
		cout << "设置超时失败" << endl;
		return false;
	}
	//设置重传次数
	if (SnmpSetRetry(hEntity, 1) == SNMPAPI_FAILURE){
		cout << "设置重传次数失败" << endl;
		return false;
	}

	return true;
}

MySNMP::~MySNMP(){
	SnmpFreeContext(hContext);
	SnmpClose(hSession);
	SnmpCleanup();
}

SNMPAPI_STATUS CALLBACK MySNMP::CallBack(HSNMP_SESSION hSession, HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam, LPVOID lpClientData)
{
	//((MySNMP*)lpClientData)->Setevent();
	SetEvent(((MySNMP*)lpClientData)->hEvent);
	return 1;
}

bool MySNMP::Send(const strvec& strOIDArray, smiINT sPDUType)
{
	//创建实体
	hSrcEntity = SnmpStrToEntity(hSession, "192.168.196.1");
	hDestEntity = SnmpStrToEntity(hSession, IPAddr.c_str());
	//设置实体超时时间
	if (SnmpSetTimeout(hSrcEntity, 10) == SNMPAPI_FAILURE){
		cout << GetLastError() << endl;
		cout << "设置超时失败，在Send函数内,源" << endl;
		return false;
	}
	//设置实体超时
	if (SnmpSetTimeout(hDestEntity, 10) == SNMPAPI_FAILURE){
		cout << "设置超时失败，在Send函数内,目的" << endl;
		return false;
	}
	//实体建立成功？
	if (hSrcEntity == SNMPAPI_FAILURE || hDestEntity == SNMPAPI_FAILURE){
		return false;
	}
	//创建变量绑定表
	if ((hVbl = SnmpCreateVbl(hSession, NULL, NULL)) == SNMPAPI_FAILURE){
		return false;
	}
	//绑定变量
	for (strvec::const_iterator it = strOIDArray.begin(); it != strOIDArray.end(); it++){
		SnmpStrToOid((*it).c_str(), &sOid);		//OID字符串转换成smiOID类型
		SnmpSetVb(hVbl, 0, &sOid, NULL);		//绑定变量
	}
	//设置PDU
	if ((hPdu = SnmpCreatePdu(hSession, sPDUType, 0, NULL, NULL, hVbl)) == SNMPAPI_FAILURE){
		return false;
	}
	//biu~biu~~biu~~~
	if (SnmpSendMsg(hSession, hSrcEntity, hDestEntity, hContext, hPdu) == SNMPAPI_FAILURE){
		return false;
	}
	//释放资源
	SnmpFreePdu(hPdu);
	return true;
}

bool MySNMP::Receive(HSNMP_VBL& hVbl)
{
	//等待Response报文——等待hEvent事件被触发
	if (WaitForSingleObject(hEvent, 10000) == WAIT_TIMEOUT){
		cout << "Wait time out" << endl;
		return false;
	}
	//重置事件
	ResetEvent(hEvent);
	//今年过节不收礼啊~~~不收礼啊不收礼~~~收礼只收Response报文~~~
	if (SnmpRecvMsg(hSession, &hSrcEntity, &hDestEntity, &hContext, &hPdu) == SNMPAPI_FAILURE)
		return false;
	//解析报文，获取PDU数据
	smiINT PDU_type, error_status, error_index;
	if (SnmpGetPduData(hPdu, &PDU_type, NULL, &error_status, &error_index, &hVbl) == SNMPAPI_FAILURE)
		return false;

	return true;
}

bool MySNMP::GetValue(const string& strOID, string& strResult)
{
	//OID.0
	strvec strOIDArray;
	strOIDArray.push_back(strOID + ".0");
	//发送Request报文
	if (!Send(strOIDArray, SNMP_PDU_GET))
		return false;
	//接收报文
	if (!Receive(hVbl))
		return false;

	SNMPAPI_STATUS iCount = SnmpCountVbl(hVbl);
	//应该只有1个值
	if (iCount != 1){
		strResult = "SnmoCountVbl error";
		return false;
	}
	//获取变量值
	smiOID sOIDRecv;
	smiVALUE sValue;
	if (SnmpGetVb(hVbl, 1, &sOIDRecv, &sValue) == SNMPAPI_FAILURE){
		strResult = "SnmpGetVb error";
		return false;
	}
	//解析变量
	strResult = ValueToString(sValue);
	SnmpFreeVbl(hVbl);
	return true;
}

bool MySNMP::GetTable(strvec strOIDArray, strvec_vec& strResultTable)
{
	//为空还查毛线
	if (strOIDArray.empty())
		return false;
	//OID个数
	size_t iOIDLen = count((strOIDArray[0].begin()), (strOIDArray[0].end()), '.') + 1;

	while (true){
		//发送get-next-request请求
		if (!Send(strOIDArray, SNMP_PDU_GETNEXT))
			return false;
		//接收报文
		if (!Receive(hVbl))
			return false;
		//变量个数
		size_t iCount = SnmpCountVbl(hVbl);

		smiOID sOIDRecv, sOIDSend;
		smiVALUE sValue;
		smiINT lIfEnd;
		char* buff = new char[100];
		//分别获取每个变量值
		for (size_t i = 0; i < iCount; ++i){
			if (SnmpGetVb(hVbl, i + 1, &sOIDRecv, &sValue) == SNMPAPI_FAILURE){
				cout << "获取结果失败" << endl;
				return false;
			}
			//下一个OID…表是否结束
			memset(buff, 0, 100);
			SnmpStrToOid(strOIDArray[i].c_str(), &sOIDSend);
			SnmpOidToStr(&sOIDRecv, 100, buff);
			strOIDArray[i] = string(buff);
			SnmpOidCompare(&sOIDSend, &sOIDRecv, iOIDLen , &lIfEnd);
			//结束，退出循环
			if (lIfEnd != 0)
				break;
			//否则把变量值放入返回向量中
			strResultTable[i].push_back(ValueToString(sValue));
		}
		if (lIfEnd != 0)
			break;
	}
	//释放资源
	SnmpFreeVbl(hVbl);
	return true;
}

string MySNMP::ValueToString(smiVALUE sValue)
{
	stringstream ss;
	switch (sValue.syntax){
	case SNMP_SYNTAX_INT:{
		/*
		char* buff = new char[12];
		_itoa_s(sValue.value.sNumber, buff, 12, 10);
		string value(buff);
		return value;*/
		ss << sValue.value.sNumber;
		break;
	}
	case SNMP_SYNTAX_CNTR32:
	case SNMP_SYNTAX_GAUGE32:
	case SNMP_SYNTAX_TIMETICKS:
	case SNMP_SYNTAX_UINT32:{
		char* buff = new char[12];
		_ultoa_s(sValue.value.uNumber, buff, 12, 10);
		string value(buff);
		return value;
		ss << sValue.value.uNumber;
		break;
	}
	case SNMP_SYNTAX_CNTR64:{
		/*
		char* buff = new char[22];
		char* lbuff = new char[12];
		_ultoa_s(sValue.value.hNumber.lopart, lbuff, 12, 10);
		_ultoa_s(sValue.value.hNumber.hipart, buff, 12, 10);
		size_t len = strlen(lbuff);
		for (unsigned int i = 0; i <= len; ++i){
			lbuff[10 - i] = lbuff[len - i];
		}
		for (int i = 10 - len; i >= 0; --i){
			lbuff[i] = '0';
		}
		len = strlen(buff);
		memcpy(buff + len, lbuff, 11);
		string value(buff);
		return value;*/
		ss << sValue.value.hNumber.hipart << sValue.value.hNumber.lopart;
		break;
	}
	case SNMP_SYNTAX_BITS:
	case SNMP_SYNTAX_OPAQUE:
	case SNMP_SYNTAX_OCTETS:{
		char* buff = new char[sValue.value.string.len + 1];
		memset(buff, 0, sValue.value.string.len + 1);
		memcpy(buff, sValue.value.string.ptr, sValue.value.string.len);
		ss << buff;
		break;
	}
	case SNMP_SYNTAX_OID:{
		char* buff = new char[100];
		memset(buff, 0, 100);
		SnmpOidToStr(&sValue.value.oid, 99, buff);
		//string value(buff);
		//return value;
		ss << buff;
		break;
	}
	case SNMP_SYNTAX_NULL:{
		//string value("NULL");
		//return value;
		ss << "NULL";
		break;
	}
	case SNMP_SYNTAX_NOSUCHINSTANCE:{
		ss << "no such instance";
		break;
	}
	case SNMP_SYNTAX_NOSUCHOBJECT:{
		ss << "no such object";
		break;
	}
	case SNMP_SYNTAX_ENDOFMIBVIEW:{
		ss << "end of MIB view";
		break;
	}
	case SNMP_SYNTAX_NSAPADDR:
	case SNMP_SYNTAX_IPADDR:{
		ss << (int)sValue.value.string.ptr[0] << "."
			<< (int)sValue.value.string.ptr[1] << "."
			<< (int)sValue.value.string.ptr[2] << "."
			<< (int)sValue.value.string.ptr[3];
		break;
	}
	default:
		ss << "我也不知道是啥  " << "SyntaxValue:" << sValue.syntax;
	}
	string value(ss.str());
	return value;
}

void MySNMP::GetSNMPInfo()
{
	cout << "----------WinSNMP Info----------" << endl
		<< "Version:" << nMajorVersion << "." << nMinorVersion << endl
		<< "Level:" << nLevel << endl
		<< "TranslateMode:" << nTranslateMode << endl
		<< "RetryMode:" << nRetransmitMode << endl;
}

int main()
{
	MySNMP SNMPTest("192.168.196.141", "public");
	SNMPTest.GetSNMPInfo();
	string result;

	cout << endl;
	cout << "---------------System Description---------------" << endl;
	if (SNMPTest.GetValue("1.3.6.1.2.1.1.1", result) == false){
		cout << "GetValueErr" << endl;
	}
	else{
		cout << result << endl;
	}
	cout << endl;
	cout << "------------System Object Identifier------------" << endl;
	if (SNMPTest.GetValue("1.3.6.1.2.1.1.2", result) == false){
		cout << "GetValueErr" << endl;
	}
	else{
		cout << result << endl;
	}
	cout << endl;
	cout << "-----------------IP Route Table-----------------" << endl;
	strvec RouteOidArray;
	RouteOidArray.push_back("1.3.6.1.2.1.4.21.1.1");	//ipRouteDest
	RouteOidArray.push_back("1.3.6.1.2.1.4.21.1.2");	//ipRouteIfIndex
	RouteOidArray.push_back("1.3.6.1.2.1.4.21.1.7");	//ipRouteNextHop
	RouteOidArray.push_back("1.3.6.1.2.1.4.21.1.8");	//ipRouteType
	RouteOidArray.push_back("1.3.6.1.2.1.4.21.1.11");	//ipRouteMask

	strvec_vec IpResultTable(RouteOidArray.size());

	if (SNMPTest.GetTable(RouteOidArray, IpResultTable) == false){
		cout << "GetTableErr" << endl;
	}
	else{
		size_t ResultCount = IpResultTable[0].size();
		if (!ResultCount){
			cout << "路由表为空" << endl;
		}
		else{
			cout << setiosflags(ios::right)
				<< setw(19) << "Network Destination" << "  "
				<< setw(15) << "Netmask" << "  "
				<< setw(15) << "Next Hop" << "  "
				<< setw(9) << "Interface" << "  "
				<< setw(9) << "Type" << endl;
			for (size_t i = 0; i < ResultCount; ++i){
				cout << setiosflags(ios::right)
					<< setw(19) << IpResultTable[0][i] << "  "
					<< setw(15) << IpResultTable[4][i] << "  "
					<< setw(15) << IpResultTable[2][i] << "  "
					<< setw(9) << IpResultTable[1][i] << "  "
					<< setw(9) << IpResultTable[3][i] << endl;
			}
		}
	}
	
	system("pause");
}
