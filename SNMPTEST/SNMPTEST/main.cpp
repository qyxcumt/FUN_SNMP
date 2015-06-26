#include <iostream>
#include <winsnmp.h>
#include <stdlib.h>
#include <Windows.h>
#include <string>
#include <vector>
#include <stdlib.h>


using namespace std;

class MySNMP{
#define	strvec		vector< string >
#define strvec_vec	vector< strvec >
private:
	smiUINT32			nMajorVersion;
	smiUINT32			nMinorVersion;
	smiUINT32			nLevel;
	smiUINT32			nTranslateMode;
	smiUINT32			nRetransmitMode;
	smiOCTETS			Community;
	smiOID				sOid;
	string				IPAddr;
	HSNMP_SESSION		hSession;
	HSNMP_CONTEXT		hContext;
	HANDLE				hEvent;
	HSNMP_ENTITY		hSrcEntity;
	HSNMP_ENTITY		hDestEntity;
	HSNMP_VBL			hVbl;
	HSNMP_PDU			hPdu;

public:
	MySNMP();
	MySNMP(string, string);
	~MySNMP();
	bool InitSnmp();
	static SNMPAPI_STATUS CALLBACK CallBack(HSNMP_SESSION, HWND, UINT, WPARAM, LPARAM, LPVOID);
	bool Send(const strvec&, smiINT);
	bool Receive(HSNMP_VBL&);
	void Setevent(){ SetEvent(hEvent); }
	bool GetValue(const string&, string&);
	string ValueToString(smiVALUE);
	bool GetTable(strvec, strvec_vec&);
};

MySNMP::MySNMP()
{
	
}

MySNMP::MySNMP(string strIP, string strCommunity) 
:nMajorVersion(0), nMinorVersion(0), nLevel(0),nTranslateMode(0), nRetransmitMode(0), IPAddr(strIP)
{

	Community.len = strCommunity.length();
	Community.ptr = new smiBYTE[strCommunity.length()];
	memcpy(Community.ptr, strCommunity.c_str(), strCommunity.length());

	InitSnmp();

	hEvent = CreateEvent(NULL, true, false, NULL);
}

bool MySNMP::InitSnmp()
{
	if (SnmpStartup(&nMajorVersion, &nMinorVersion, &nLevel, &nTranslateMode, &nRetransmitMode) == SNMPAPI_FAILURE){
		cout << "加载失败" << endl;
		return false;
	}

	if (SnmpSetTranslateMode(nTranslateMode) == SNMPAPI_FAILURE){
		cout << "设置传输模式失败" << endl;
		return false;
	}

	if (SnmpSetRetransmitMode(nRetransmitMode) == SNMPAPI_FAILURE){
		cout << "设置重传模式失败" << endl;
		return false;
	}

	hSession = SnmpCreateSession(NULL, NULL, MySNMP::CallBack, (LPVOID)this);
	if (hSession == SNMPAPI_FAILURE){
		cout << "建立会话失败" << endl;
		return false;
	}

	HSNMP_ENTITY hEntity;
	if ((hEntity = SnmpStrToEntity(hSession, IPAddr.c_str())) == SNMPAPI_FAILURE){
		cout << "创建实体失败" << endl;
		return false;
	}

	if ((hContext = SnmpStrToContext(hSession, &Community)) == SNMPAPI_FAILURE){
		cout << "建立上下文失败" << endl;
		return false;
	}

	if (SnmpSetTimeout(hEntity, 10) == SNMPAPI_FAILURE){
		cout << "设置超时失败" << endl;
		return false;
	}

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
	((MySNMP*)lpClientData)->Setevent();
	return 1;
}

bool MySNMP::Send(const strvec& strOIDArray, smiINT sPDUType)
{
	hSrcEntity = SnmpStrToEntity(hSession, "192.168.196.1");
	hDestEntity = SnmpStrToEntity(hSession, IPAddr.c_str());
	
	if (SnmpSetTimeout(hSrcEntity, 10) == SNMPAPI_FAILURE){
		cout << GetLastError() << endl;
		cout << "设置超时失败，在Send函数内,源" << endl;
		return false;
	}

	if (SnmpSetTimeout(hDestEntity, 10) == SNMPAPI_FAILURE){
		cout << "设置超时失败，在Send函数内,目的" << endl;
		return false;
	}

	if (hSrcEntity == SNMPAPI_FAILURE || hDestEntity == SNMPAPI_FAILURE){
		return false;
	}

	if ((hVbl = SnmpCreateVbl(hSession, NULL, NULL)) == SNMPAPI_FAILURE){
		return false;
	}

	for (strvec::const_iterator it = strOIDArray.begin(); it != strOIDArray.end(); it++){
		SnmpStrToOid((*it).c_str(), &sOid);
		SnmpSetVb(hVbl, 0, &sOid, NULL);
	}

	if ((hPdu = SnmpCreatePdu(hSession, sPDUType, 0, NULL, NULL, hVbl)) == SNMPAPI_FAILURE){
		return false;
	}

	if (SnmpSendMsg(hSession, hSrcEntity, hDestEntity, hContext, hPdu) == SNMPAPI_FAILURE){
		return false;
	}

	SnmpFreePdu(hPdu);
	return true;
}

bool MySNMP::Receive(HSNMP_VBL& hVbl)
{
	if (WaitForSingleObject(hEvent, 10000) == WAIT_TIMEOUT){
		cout << "Wait time out" << endl;
		return false;
	}

	ResetEvent(hEvent);

	if (SnmpRecvMsg(hSession, &hSrcEntity, &hDestEntity, &hContext, &hPdu) == SNMPAPI_FAILURE)
		return false;

	smiINT PDU_type,error_status,error_index;
	if (SnmpGetPduData(hPdu, &PDU_type, NULL, &error_status, &error_index, &hVbl) == SNMPAPI_FAILURE)
		return false;

	return true;
}

bool MySNMP::GetValue(const string& strOID, string& strResult)
{
	strvec strOIDArray;
	strOIDArray.push_back(strOID + ".0");

	if (!Send(strOIDArray, SNMP_PDU_GET))
		return false;

	if (!Receive(hVbl))
		return false;

	SNMPAPI_STATUS iCount = SnmpCountVbl(hVbl);

	if (iCount != 1){
		strResult = "SnmoCountVbl error";
		return false;
	}
	smiOID sOIDRecv;
	smiVALUE sValue;
	if (SnmpGetVb(hVbl, 1, &sOIDRecv, &sValue) == SNMPAPI_FAILURE){
		strResult = "SnmpGetVb error";
		return false;
	}

	strResult = ValueToString(sValue);
	SnmpFreeVbl(hVbl);
	return true;
}

bool MySNMP::GetTable(strvec strOIDArray, strvec_vec& strResultTable)
{
	if (strOIDArray.empty())
		return false;

	size_t iOIDLen = count((strOIDArray[0].begin()), (strOIDArray[0].end()), '.') + 1;

	while (true){
		if (!Send(strOIDArray,SNMP_PDU_GETNEXT))
			return false;

		if (!Receive(hVbl))
			return false;

		size_t iCount = SnmpCountVbl(hVbl);

		smiOID sOIDRecv,sOIDSend;
		smiVALUE sValue;
		smiINT lIfEnd;
		char* buff = new char(100);

		for (size_t i = 0; i < iCount; ++i){
			if (SnmpGetVb(hVbl, i + 1, &sOIDRecv, &sValue) == SNMPAPI_FAILURE){
				cout << "获取结果失败" << endl;
				return false;
			}
			memset(buff, 0, 100);
			SnmpStrToOid(strOIDArray[i].c_str(), &sOIDSend);
			SnmpOidToStr(&sOIDRecv, 100, buff);
			strOIDArray[i] = string(buff);
			SnmpOidCompare(&sOIDSend, &sOIDRecv, iOIDLen-2, &lIfEnd);

			if (lIfEnd != 0)
				break;
			strResultTable[i].push_back(ValueToString(sValue));
		}
		if (lIfEnd != 0)
			break;
	}

	SnmpFreeVbl(hVbl);
	return true;
}

string MySNMP::ValueToString(smiVALUE sValue)
{
	switch (sValue.syntax){
	case SNMP_SYNTAX_INT:{
		char* buff = new char(12);
		_itoa_s(sValue.value.sNumber, buff,12, 10);
		string value(buff);
		return value; 
	}
	case SNMP_SYNTAX_CNTR32:
	case SNMP_SYNTAX_GAUGE32:
	case SNMP_SYNTAX_TIMETICKS:
	case SNMP_SYNTAX_UINT32:{
		char* buff = new char(12);
		_ultoa_s(sValue.value.uNumber, buff,12, 10);
		string value(buff);
		return value; 
	}
	case SNMP_SYNTAX_CNTR64:{
		char* buff = new char(22);
		char* lbuff = new char(12);
		_ultoa_s(sValue.value.hNumber.lopart, lbuff,12, 10);
		_ultoa_s(sValue.value.hNumber.hipart, buff,12, 10);
		size_t len = strlen(lbuff);
		for (unsigned int i = 0; i <= len; ++i){
			lbuff[10 - i] = lbuff[len - i];
		}
		for (int i = 10 - len; i>=0; --i){
			lbuff[i] = '0';
		}
		len = strlen(buff);
		memcpy(buff + len, lbuff, 11);
		string value(buff);
		return value;
	}
	case SNMP_SYNTAX_IPADDR:
	case SNMP_SYNTAX_NSAPADDR:
	case SNMP_SYNTAX_BITS:
	case SNMP_SYNTAX_OPAQUE:
	case SNMP_SYNTAX_OCTETS:{
		char* buff = new char(sValue.value.string.len + 1);
		memset(buff, 0, sValue.value.string.len + 1);
		memcpy(buff, sValue.value.string.ptr, sValue.value.string.len);
		string value(buff);
		return value;
	}/*
	case SNMP_SYNTAX_IPADDR:
	case SNMP_SYNTAX_NSAPADDR:{
		char* buff = new char(16);
		memset(buff, 0, 16);

	}*/
	case SNMP_SYNTAX_OID:{
		char* buff=new char[12];
		_ultoa_s(*sValue.value.oid.ptr, buff,12, 10);
		string value(buff);
		return value;
	}
	case SNMP_SYNTAX_NULL:{
		string value("NULL");
		return value;
	}
	}
}



int main()
{
	MySNMP SNMPTest("192.168.196.141","public");
	string result;

	cout << "----------GetValueTest----------" << endl;
	if (SNMPTest.GetValue("1.3.6.1.2.1.1.1", result) == false){
		cout << "GetValueErr" << endl;
	}
	else{
		cout << result << endl;
	}

	cout << "----------GetTableTest----------" << endl;
	strvec RouteOidArray;
	RouteOidArray.push_back("1.3.6.1.2.1.4.21.1.1");
	RouteOidArray.push_back("1.3.6.1.2.1.4.21.1.2");
	RouteOidArray.push_back("1.3.6.1.2.1.4.21.1.7");
	RouteOidArray.push_back("1.3.6.1.2.1.4.21.1.8");
	RouteOidArray.push_back("1.3.6.1.2.1.4.21.1.11");
	strvec_vec IpResultTable(RouteOidArray.size());
	if (SNMPTest.GetTable(RouteOidArray, IpResultTable) == false){
		cout << "GetTableErr" << endl;
	}
	else{
		for (strvec_vec::iterator it1 = IpResultTable.begin(); it1 != IpResultTable.end(); ++it1){
			strvec AGroup = *it1;
			for (strvec::iterator it2 = AGroup.begin(); it2 != AGroup.end(); ++it2){
				cout << *it2 << "";
			}
			cout << endl;
		}
	}
	system("pause");
}
