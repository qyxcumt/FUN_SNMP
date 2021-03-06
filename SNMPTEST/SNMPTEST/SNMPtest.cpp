/**
*
* ━━━━━━神兽出没━━━━━━
* 　　　┏┓　　　┏┓
* 　　┏┛┻━━━┛┻┓
* 　　┃　　　　　　　┃
* 　　┃　　　━　　　┃
* 　　┃　┳┛　┗┳　┃
* 　　┃　　　　　　　┃
* 　　┃　　　┻　　　┃
* 　　┃　　　　　　　┃
* 　　┗━┓　　　┏━┛Code is far away from bug with the animal protecting
* 　　　　┃　　　┃ 神兽保佑,代码无bug
* 　　　　┃　　　┃
* 　　　　┃　　　┗━━━┓
* 　　　　┃　　　　　　　┣┓
* 　　　　┃　　　　　　　┏┛
* 　　　　┗┓┓┏━┳┓┏┛
* 　　　　　┃┫┫　┃┫┫
* 　　　　　┗┻┛　┗┻┛
*
* ━━━━━━感觉萌萌哒━━━━━━
*/

#include "SNMPtest.h"

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

bool MySNMP::Send(smiVALUE sValue, smiINT sPDUType)
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

	SnmpSetVb(hVbl, 0, &sOid, &sValue);
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
bool MySNMP::SetValue(const string& strOID, int setVal)
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
		return false;
	}
	//获取变量值
	smiOID sOIDRecv;
	smiVALUE sValue;
	if (SnmpGetVb(hVbl, 1, &sOIDRecv, &sValue) == SNMPAPI_FAILURE){
		return false;
	}
	//绑定变量
	
	switch (sValue.syntax){
	case SNMP_SYNTAX_INT:{
		sValue.value.sNumber = setVal;
		break;
	}
	case SNMP_SYNTAX_UINT32:{
		sValue.value.uNumber = setVal;
		break;
	}
	}
	//设置变量
	Send(sValue, SNMP_PDU_SET);
}

bool MySNMP::GetTable(strvec strOIDArray, strvec_vec& strResultTable)
{
	//没有OID还查毛线
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
			SnmpOidCompare(&sOIDSend, &sOIDRecv, iOIDLen, &lIfEnd);
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
		//ss << sValue.value.hNumber.hipart << sValue.value.hNumber.lopart;
		ss << "实现方法错辣~~~先注释掉~~~";
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
