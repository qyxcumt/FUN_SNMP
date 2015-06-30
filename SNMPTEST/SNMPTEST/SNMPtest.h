/**
* ������������������������������
* �������������������ߩ��������ߩ�
* �������������������������������� ��
* ��������������������������������
* ��������������������������������
* ��������������������������������
* ����������������...���С�...����
* ��������������������������������
* ��������������������������������
* ������������������������������Code is far away from bug with the animal protecting��������������������
* ���������������������������� ���ޱ���,������bug
* ��������������������������������������������������
* ���������������������������� ������������
* ����������������������������
* ��������������������������������������������������
* ������������������������������������
* �����������������������������������ǩ�
* ��������������������������������������
* �����������������������������ש�����
* �����������������������ϩϡ����ϩ�
* �����������������������ߩ������ߩ�
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
	bool Send(smiVALUE, smiINT);
	bool SetValue(const string&, int);
};