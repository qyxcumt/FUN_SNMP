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
#include <time.h>
#include "SNMPtest.h"


using namespace std;


int main()
{
	
	MySNMP SNMPTest("192.168.196.3", "SNMPtestNULL");
	//获取WinSNMP信息
	SNMPTest.GetSNMPInfo();
	string result;
	//获取系统描述
	cout << endl;
	cout << "---------------System Description---------------" << endl;
	if (SNMPTest.GetValue("1.3.6.1.2.1.1.1", result) == false){
		cout << "GetValueErr" << endl;
	}
	else{
		cout << result << endl;
	}
	cout << endl;
	//获取系统识别符
	cout << "------------System Object Identifier------------" << endl;
	if (SNMPTest.GetValue("1.3.6.1.2.1.1.2", result) == false){
		cout << "GetValueErr" << endl;
	}
	else{
		cout << result << endl;
	}
	cout << endl;
	//获取路由表
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
