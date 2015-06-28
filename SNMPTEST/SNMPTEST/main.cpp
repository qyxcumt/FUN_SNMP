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
//                  ���汣��             ����BUG 
//          ��Ի:  
//                  д��¥��д�ּ䣬д�ּ������Ա��  
//                  ������Աд�������ó��򻻾�Ǯ��  
//                  ����ֻ���������������������ߣ�  
//                  ��������ո��գ����������긴�ꡣ  
//                  ��Ը�������Լ䣬��Ը�Ϲ��ϰ�ǰ��  
//                  ���۱������Ȥ���������г���Ա��  
//                  ����Ц��߯��񲣬��Ц�Լ���̫����  
//                  ��������Ư���ã��ĸ���ó���Ա�� 

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
	smiUINT32			nMajorVersion;		//���汾��
	smiUINT32			nMinorVersion;		//�ΰ汾��	
	smiUINT32			nLevel;				//֧�ֲ��
	smiUINT32			nTranslateMode;		//Translateģʽ
	smiUINT32			nRetransmitMode;	//�ش�ģʽ
	smiOCTETS			Community;			//������
	smiOID				sOid;				
	string				IPAddr;
	HSNMP_SESSION		hSession;			//�Ự���
	HSNMP_CONTEXT		hContext;			//�����ľ��
	HANDLE				hEvent;				//ʵ�����
	HSNMP_ENTITY		hSrcEntity;			//Դʵ����
	HSNMP_ENTITY		hDestEntity;		//Ŀ��ʵ����
	HSNMP_VBL			hVbl;				//�������
	HSNMP_PDU			hPdu;				//PDU���

public:
	MySNMP();								//���캯��
	MySNMP(string, string);					//ͬ��
	~MySNMP();								//��������
	bool InitSnmp();						//��ʼ��WinSNMP
	static SNMPAPI_STATUS CALLBACK CallBack(HSNMP_SESSION, HWND, UINT, WPARAM, LPARAM, LPVOID);	//�ص�����
	bool Send(const strvec&, smiINT);		//����
	bool Receive(HSNMP_VBL&);				//����
	void Setevent(){ SetEvent(hEvent); }	//�ѿ��ӷ�ƨ���������ø�
	bool GetValue(const string&, string&);	//��ñ���ֵ
	string ValueToString(smiVALUE);			//����ֵת�����ַ���
	bool GetTable(strvec, strvec_vec&);		//��ȡ��
	void GetSNMPInfo();						//��ȡWinSNMP��Ϣ
};

MySNMP::MySNMP()
{
	//���ǿ���~
}

MySNMP::MySNMP(string strIP, string strCommunity)
	:nMajorVersion(0), nMinorVersion(0), nLevel(0), nTranslateMode(0), nRetransmitMode(0), IPAddr(strIP)	
{
	//����������
	Community.len = strCommunity.length();
	Community.ptr = new smiBYTE[strCommunity.length()];
	memcpy(Community.ptr, strCommunity.c_str(), strCommunity.length());
	//��ʼ��WinSNMP
	InitSnmp();
	//�����¼�
	hEvent = CreateEvent(NULL, true, false, NULL);
}

bool MySNMP::InitSnmp()
{
	//��ʼ��WinSNMP
	if (SnmpStartup(&nMajorVersion, &nMinorVersion, &nLevel, &nTranslateMode, &nRetransmitMode) == SNMPAPI_FAILURE){
		cout << "����ʧ��" << endl;
		return false;
	}
	//���ô���ģʽ
	if (SnmpSetTranslateMode(nTranslateMode) == SNMPAPI_FAILURE){
		cout << "���ô���ģʽʧ��" << endl;
		return false;
	}
	//�����ش�ģʽ
	if (SnmpSetRetransmitMode(nRetransmitMode) == SNMPAPI_FAILURE){
		cout << "�����ش�ģʽʧ��" << endl;
		return false;
	}
	//�����Ự
	hSession = SnmpCreateSession(NULL, NULL, MySNMP::CallBack, (LPVOID)this);
	if (hSession == SNMPAPI_FAILURE){
		cout << "�����Ựʧ��" << endl;
		return false;
	}
	//����ʵ��
	HSNMP_ENTITY hEntity;
	if ((hEntity = SnmpStrToEntity(hSession, IPAddr.c_str())) == SNMPAPI_FAILURE){
		cout << "����ʵ��ʧ��" << endl;
		return false;
	}
	//����������
	if ((hContext = SnmpStrToContext(hSession, &Community)) == SNMPAPI_FAILURE){
		cout << "����������ʧ��" << endl;
		return false;
	}
	//���ó�ʱ
	if (SnmpSetTimeout(hEntity, 10) == SNMPAPI_FAILURE){
		cout << "���ó�ʱʧ��" << endl;
		return false;
	}
	//�����ش�����
	if (SnmpSetRetry(hEntity, 1) == SNMPAPI_FAILURE){
		cout << "�����ش�����ʧ��" << endl;
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
	//����ʵ��
	hSrcEntity = SnmpStrToEntity(hSession, "192.168.196.1");
	hDestEntity = SnmpStrToEntity(hSession, IPAddr.c_str());
	//����ʵ�峬ʱʱ��
	if (SnmpSetTimeout(hSrcEntity, 10) == SNMPAPI_FAILURE){
		cout << GetLastError() << endl;
		cout << "���ó�ʱʧ�ܣ���Send������,Դ" << endl;
		return false;
	}
	//����ʵ�峬ʱ
	if (SnmpSetTimeout(hDestEntity, 10) == SNMPAPI_FAILURE){
		cout << "���ó�ʱʧ�ܣ���Send������,Ŀ��" << endl;
		return false;
	}
	//ʵ�彨���ɹ���
	if (hSrcEntity == SNMPAPI_FAILURE || hDestEntity == SNMPAPI_FAILURE){
		return false;
	}
	//���������󶨱�
	if ((hVbl = SnmpCreateVbl(hSession, NULL, NULL)) == SNMPAPI_FAILURE){
		return false;
	}
	//�󶨱���
	for (strvec::const_iterator it = strOIDArray.begin(); it != strOIDArray.end(); it++){
		SnmpStrToOid((*it).c_str(), &sOid);		//OID�ַ���ת����smiOID����
		SnmpSetVb(hVbl, 0, &sOid, NULL);		//�󶨱���
	}
	//����PDU
	if ((hPdu = SnmpCreatePdu(hSession, sPDUType, 0, NULL, NULL, hVbl)) == SNMPAPI_FAILURE){
		return false;
	}
	//biu~biu~~biu~~~
	if (SnmpSendMsg(hSession, hSrcEntity, hDestEntity, hContext, hPdu) == SNMPAPI_FAILURE){
		return false;
	}
	//�ͷ���Դ
	SnmpFreePdu(hPdu);
	return true;
}

bool MySNMP::Receive(HSNMP_VBL& hVbl)
{
	//�ȴ�Response���ġ����ȴ�hEvent�¼�������
	if (WaitForSingleObject(hEvent, 10000) == WAIT_TIMEOUT){
		cout << "Wait time out" << endl;
		return false;
	}
	//�����¼�
	ResetEvent(hEvent);
	//������ڲ�����~~~�����񰡲�����~~~����ֻ��Response����~~~
	if (SnmpRecvMsg(hSession, &hSrcEntity, &hDestEntity, &hContext, &hPdu) == SNMPAPI_FAILURE)
		return false;
	//�������ģ���ȡPDU����
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
	//����Request����
	if (!Send(strOIDArray, SNMP_PDU_GET))
		return false;
	//���ձ���
	if (!Receive(hVbl))
		return false;

	SNMPAPI_STATUS iCount = SnmpCountVbl(hVbl);
	//Ӧ��ֻ��1��ֵ
	if (iCount != 1){
		strResult = "SnmoCountVbl error";
		return false;
	}
	//��ȡ����ֵ
	smiOID sOIDRecv;
	smiVALUE sValue;
	if (SnmpGetVb(hVbl, 1, &sOIDRecv, &sValue) == SNMPAPI_FAILURE){
		strResult = "SnmpGetVb error";
		return false;
	}
	//��������
	strResult = ValueToString(sValue);
	SnmpFreeVbl(hVbl);
	return true;
}

bool MySNMP::GetTable(strvec strOIDArray, strvec_vec& strResultTable)
{
	//Ϊ�ջ���ë��
	if (strOIDArray.empty())
		return false;
	//OID����
	size_t iOIDLen = count((strOIDArray[0].begin()), (strOIDArray[0].end()), '.') + 1;

	while (true){
		//����get-next-request����
		if (!Send(strOIDArray, SNMP_PDU_GETNEXT))
			return false;
		//���ձ���
		if (!Receive(hVbl))
			return false;
		//��������
		size_t iCount = SnmpCountVbl(hVbl);

		smiOID sOIDRecv, sOIDSend;
		smiVALUE sValue;
		smiINT lIfEnd;
		char* buff = new char[100];
		//�ֱ��ȡÿ������ֵ
		for (size_t i = 0; i < iCount; ++i){
			if (SnmpGetVb(hVbl, i + 1, &sOIDRecv, &sValue) == SNMPAPI_FAILURE){
				cout << "��ȡ���ʧ��" << endl;
				return false;
			}
			//��һ��OID�����Ƿ����
			memset(buff, 0, 100);
			SnmpStrToOid(strOIDArray[i].c_str(), &sOIDSend);
			SnmpOidToStr(&sOIDRecv, 100, buff);
			strOIDArray[i] = string(buff);
			SnmpOidCompare(&sOIDSend, &sOIDRecv, iOIDLen , &lIfEnd);
			//�������˳�ѭ��
			if (lIfEnd != 0)
				break;
			//����ѱ���ֵ���뷵��������
			strResultTable[i].push_back(ValueToString(sValue));
		}
		if (lIfEnd != 0)
			break;
	}
	//�ͷ���Դ
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
		ss << "��Ҳ��֪����ɶ  " << "SyntaxValue:" << sValue.syntax;
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
			cout << "·�ɱ�Ϊ��" << endl;
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
