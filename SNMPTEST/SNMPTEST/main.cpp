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
#include <time.h>
#include "SNMPtest.h"


using namespace std;


int main()
{
	
	MySNMP SNMPTest("192.168.196.3", "SNMPtestNULL");
	//��ȡWinSNMP��Ϣ
	SNMPTest.GetSNMPInfo();
	string result;
	//��ȡϵͳ����
	cout << endl;
	cout << "---------------System Description---------------" << endl;
	if (SNMPTest.GetValue("1.3.6.1.2.1.1.1", result) == false){
		cout << "GetValueErr" << endl;
	}
	else{
		cout << result << endl;
	}
	cout << endl;
	//��ȡϵͳʶ���
	cout << "------------System Object Identifier------------" << endl;
	if (SNMPTest.GetValue("1.3.6.1.2.1.1.2", result) == false){
		cout << "GetValueErr" << endl;
	}
	else{
		cout << result << endl;
	}
	cout << endl;
	//��ȡ·�ɱ�
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
	//TTL
	cout << endl;
	cout << "----------------------TTL-----------------------" << endl;
	if (SNMPTest.GetValue("1.3.6.1.2.1.4.2", result) == false){
		cout << "GetValueErr" << endl;
	}
	else{
		cout << result << endl;
	}
	cout << endl;
	int ttl = 0;
	for (ttl = 16; ttl >= 0; --ttl){
		cout << "--------------------------------->set TTL to " << ttl << endl;
		SNMPTest.SetValue("1.3.6.1.2.1.4.2", ttl);
		Sleep(1000);
	}

	SNMPTest.SetValue("1.3.6.1.2.1.4.2", 64);

	cout << endl;
	cout << "----------------------TTL-----------------------" << endl;
	if (SNMPTest.GetValue("1.3.6.1.2.1.4.2", result) == false){
		cout << "GetValueErr" << endl;
	}
	else{
		cout << result << endl;
	}
	
	system("pause");
}
