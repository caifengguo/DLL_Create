

#include "Device_State.h"

#include <Iphlpapi.h>
#include <atlconv.h> 
#include"WinSock2.h"
#include <WinCrypt.h>
#include <winbase.h> 
#include <string> 
#include<sstream>
#include <cassert>
#include <comdef.h>
#include <WbemCli.h>
#include<Winnt.h>

#include <ace/OS_NS_stdio.h>

#pragma comment(lib, "Iphlpapi.lib")



Device_State_t::Device_State_t()
{

}

Device_State_t::~Device_State_t()
{

}




// 获取程序位数（被编译为多少位的代码）
int get_program_bits()
{
	return sizeof(int*) * 8;
}

// 安全的取得真实系统信息
VOID SafeGetNativeSystemInfo(__out LPSYSTEM_INFO lpSystemInfo)
{
	if (NULL==lpSystemInfo)    return;
	typedef VOID (WINAPI *LPFN_GetNativeSystemInfo)(LPSYSTEM_INFO lpSystemInfo);
	LPFN_GetNativeSystemInfo fnGetNativeSystemInfo = (LPFN_GetNativeSystemInfo)GetProcAddress( GetModuleHandle(ACE_TEXT("kernel32")), "GetNativeSystemInfo");;
	if (NULL != fnGetNativeSystemInfo)
	{
		fnGetNativeSystemInfo(lpSystemInfo);
	}
	else
	{
		GetSystemInfo(lpSystemInfo);
	}
}

// 获取操作系统位数
int get_system_bits()
{
	SYSTEM_INFO si;
	SafeGetNativeSystemInfo(&si);
	if (si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64 ||
		si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_IA64 )
	{
		return 64;
	}
	return 32;
}


//<< 获取CPU核心数目 
int  Device_State_t::get_cpu_num()
{
	SYSTEM_INFO si; 
	memset(&si,0,sizeof(SYSTEM_INFO));  
	GetSystemInfo(&si);                //核数
	return  si.dwNumberOfProcessors;
}

//<< cpu型号
DWORD Device_State_t::get_cpu_type()
{
	SYSTEM_INFO si; 
	memset(&si,0,sizeof(SYSTEM_INFO));  
	GetSystemInfo(&si);  
	m_ProcessorType = si.dwProcessorType;          //类型
	return  m_ProcessorType;
}

//<< cpu级数
DWORD Device_State_t::get_cpu_grade()
{
	SYSTEM_INFO si; 
	memset(&si,0,sizeof(SYSTEM_INFO));  
	GetSystemInfo(&si);  
	m_ProcessorLevel = si.wProcessorLevel;         //级数
	return  m_ProcessorLevel;
}


//获取可用物理内存 
int Device_State_t::get_AvailPhys_mem_size()
{
	MEMORYSTATUS   Mem;
	GlobalMemoryStatus(&Mem);

	DWORD  dwAvailPhys;
	dwAvailPhys = Mem.dwAvailPhys/1024;      //可用物理内存 
	m_dwAvailPhys = dwAvailPhys/1024;
	return  m_dwAvailPhys;	
}

//获取总的物理内存(实际内存)
int Device_State_t::get_TotalPhys_mem_size()
{
	MEMORYSTATUS   Mem;
	GlobalMemoryStatus(&Mem);

	DWORD        dwSize; 
	dwSize = Mem.dwTotalPhys/1024;           //总的物理内存   
	m_dwSize = dwSize/1024;

	return  m_dwSize;	    
}

//获取总的虚拟内存 
int Device_State_t::get_VirtSize_mem_size()
{
	MEMORYSTATUS   Mem;
	GlobalMemoryStatus(&Mem);

	DWORD   dwVirtSize;
	dwVirtSize = Mem.dwTotalVirtual/1024;      //总的虚拟内存
	m_dwVirtSize = dwVirtSize/1024;

	return  m_dwVirtSize;	  
}



//<< 获取物理内存使用率
int  Device_State_t::get_mem_phy_rate()
{
	MEMORYSTATUS   Mem;
	GlobalMemoryStatus(&Mem);
	m_dwUsePre = (DWORD)Mem.dwMemoryLoad;                   //内存使用百分比

	return m_dwUsePre;
}


ACE_INT64 Device_State_t::compare_file_time ( FILETIME* time1, FILETIME* time2 )
{
	ACE_INT64 a = time1->dwHighDateTime << 32 | time1->dwLowDateTime ;
	ACE_INT64 b = time2->dwHighDateTime << 32 | time2->dwLowDateTime ;

	return   (b - a);
}


//<< 获取cpu使用率以及空闲率
int Device_State_t::get_cpu_use_rate()
{
	HANDLE hEvent;
	BOOL res ;

	FILETIME preidleTime;
	FILETIME prekernelTime;
	FILETIME preuserTime;

	FILETIME idleTime;
	FILETIME kernelTime;
	FILETIME userTime;

	//获取系统时间 
	res = GetSystemTimes( &idleTime, &kernelTime, &userTime );   //参数分别为空闲时间  内核时间  用户时间

	preidleTime = idleTime;       //空闲时间
	prekernelTime = kernelTime;   //内核时间
	preuserTime = userTime;       //用户时间

	hEvent = CreateEvent (NULL,FALSE,FALSE,NULL); // 初始值为 nonsignaled ，并且每次触发后自动设置为nonsignaled

	// while(1)
	//  {
	WaitForSingleObject( hEvent,100);        //等待100毫秒
	res = GetSystemTimes( &idleTime, &kernelTime, &userTime );

	int idle = compare_file_time( &preidleTime,&idleTime);
	int kernel = compare_file_time( &prekernelTime, &kernelTime);
	int user = compare_file_time(&preuserTime, &userTime);

	m_cpu = (kernel +user - idle) *100/(kernel+user);     //占有率         
	m_cpuidle = 100 - m_cpu;                     //空闲率

	preidleTime = idleTime;
	prekernelTime = kernelTime;
	preuserTime = userTime ;
	//  }
	return  m_cpu;
}


//<< 获取本地主机名
ACE_TCHAR*  Device_State_t::get_main_machine_name()
{
	// 声明变量   
	ULONG    ulOutBufLen;       // 保存获取到的本地计算机网络参数信息结构体链表的长度  
	DWORD    dwRetVal;          // 调用GetNetworkParams()函数的返回值   

	FIXED_INFO*  FixedInfo;     // 定义保存本地计算机网络参数信息的结构体指针
	// 获取信息        
	FixedInfo = (FIXED_INFO *) GlobalAlloc( GPTR, sizeof( FIXED_INFO ) );   // 为FixedInfo结构体分配内存空间  
	ulOutBufLen = sizeof( FIXED_INFO );                // 初始化ulOutBufLen变量值  

	// 第1次调用GetNetworkParams()函数，获取返回结果的大小到ulOutBufLen中  
	if( ERROR_BUFFER_OVERFLOW == GetNetworkParams( FixedInfo, &ulOutBufLen ) ) 
	{  
		GlobalFree( FixedInfo );  
		FixedInfo = (FIXED_INFO *) GlobalAlloc( GPTR, ulOutBufLen );  
	}  

	// 第2次调用GetNetworkParams()函数，以前面获取的ulOutBufLen作为参数，  
	if ( dwRetVal = GetNetworkParams(FixedInfo, &ulOutBufLen ) != ERROR_SUCCESS)
	{  
		return 0;  
	}  
	return FixedInfo->HostName;    
}




//<< 获取主机IP地址
ACE_TCHAR* Device_State_t::get_main_ip()
{
	PMIB_IPADDRTABLE pIPAddrTable;   // 网络接口与IP地址映射表  
	DWORD dwSize = 0;     // 获取数据的大小  
	DWORD dwRetVal = 0;    // 调用GetIPAddrTable()函数的返回值  
	IN_ADDR IPAddr;        // 保存IP地址的结构体  
	LPVOID lpMsgBuf;       // 用于获取错误信息 

	// 分配内存空间 
	pIPAddrTable = (MIB_IPADDRTABLE *) malloc(sizeof (MIB_IPADDRTABLE));

	// 第1次调用GetIpAddrTable()函数，获取数据的大小到dwSize  
	if (pIPAddrTable)
	{  
		if (GetIpAddrTable(pIPAddrTable, &dwSize, 0) ==  ERROR_INSUFFICIENT_BUFFER)
		{  
			free(pIPAddrTable);  
			pIPAddrTable = (MIB_IPADDRTABLE *) malloc(dwSize);  
		}  
		if (pIPAddrTable == NULL) 
		{  
			return 0;    
		}  
	} 
	// 第2次调用GetIpAddrTable()函数，获取实际数据  
	if ( (dwRetVal = GetIpAddrTable( pIPAddrTable, &dwSize, 0 )) != NO_ERROR ) 
	{ 
		if (FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL,   
			dwRetVal, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),(LPTSTR) & lpMsgBuf, 0, NULL)) 
		{  
			return 0;  
		}   
	} 

	IPAddr.S_un.S_addr = (u_long) pIPAddrTable->table[0].dwAddr;

	memcpy(m_Ip,inet_ntoa(IPAddr),15); 

	if (pIPAddrTable) 
	{  
		free(pIPAddrTable);  
		pIPAddrTable = NULL;  
	} 
	return m_Ip;
}


//获取操作系统版本信息
ACE_TString Device_State_t::get_version_info()
{
	//Operating system	Version number	dwMajorVersion	dwMinorVersion	Other
	//Windows 8.1	6.3* 6	3	OSVERSIONINFOEX.wProductType == VER_NT_WORKSTATION
	//Windows Server 2012 R2	6.3*	6	3	OSVERSIONINFOEX.wProductType != VER_NT_WORKSTATION
	//Windows 8	6.2	6	2	OSVERSIONINFOEX.wProductType == VER_NT_WORKSTATION
	//Windows Server 2012	6.2	6	2	OSVERSIONINFOEX.wProductType != VER_NT_WORKSTATION
	//Windows 7
	//	6.1
	//	6
	//	1
	//	OSVERIONINFOEX.wProductType == VER_NT_WORKSTATION
	//	Windows Server 2008 R2
	//	6.1
	//	6
	//	1
	//	OSVERIONINFOEX.wProductType != VER_NT_WORKSTATION
	//	Windows Server 2008
	//	6.0
	//	6
	//	0
	//	OSVERIONINFOEX.wProductType != VER_NT_WORKSTATION
	//	Windows Vista
	//	6.0
	//	6
	//	0
	//	OSVERIONINFOEX.wProductType == VER_NT_WORKSTATION
	//	Windows Server 2003 R2
	//	5.2
	//	5
	//	2
	//	GetSystemMetrics(SM_SERVERR2) != 0
	//	Windows Server 2003
	//	5.2
	//	5
	//	2
	//	GetSystemMetrics(SM_SERVERR2) == 0
	//	Windows XP
	//	5.1
	//	5
	//	1
	//	Not applicable
	//	Windows 2000
	//	5.0
	//	5
	//	0
	//	Not applicable
	OSVERSIONINFO lpVersionInformation;
	lpVersionInformation.dwOSVersionInfoSize=sizeof(OSVERSIONINFO);
	GetVersionEx(&lpVersionInformation);     // 指向版本信息结构体的指针

	ACE_TCHAR version[1024] = {'\0'};
	ACE_OS::sprintf(version,ACE_TEXT("MajorVersion[%u] MinorVersion[%u] BuildNumber[%u] PlatformId[%u] CSDVersion[%s]"),
		lpVersionInformation.dwMajorVersion,
		lpVersionInformation.dwMinorVersion,
		lpVersionInformation.dwBuildNumber,
		lpVersionInformation.dwPlatformId,
		lpVersionInformation.szCSDVersion);
	return version;
} 



//<< 获取磁盘使用情况
int  Device_State_t::get_disk_info(int &m_disk_count,int &disk_num,int &drive_num,vector<ACE_TString> &drive_info_vec_)
{
	DWORD DiskCount = 0;  
	//获取系统中逻辑驱动器的数量，函数返回的是一个32位无符号整型数据。  
	DWORD DiskInfo = GetLogicalDrives(); 
	//如果为1则磁盘为真,如果为0则磁盘不存在。  
	while(DiskInfo)  
	{  
		//通过位运算的逻辑与操作，判断是否为1   
		if(DiskInfo&1)  
		{  
			DiskCount++;  
		}  
		DiskInfo = DiskInfo >> 1;  //通过位运算的右移操作保证每循环一次所检查的位置向右移动一位。*/  
	} 
	m_disk_count = DiskCount;

	//通过GetLogicalDriveStrings()函数获取所有驱动器字符串信息长度  
	int DSLength = GetLogicalDriveStrings(0,NULL);  
	CHAR*  DStr = new CHAR[DSLength];  
	memset(DStr,0,DSLength);  

	//通过GetLogicalDriveStrings将字符串信息复制到堆区数组中,其中保存了所有驱动器的信息。  
	GetLogicalDriveStrings(DSLength,DStr);  

	//获取磁盘数、驱动数
	int DType; 
	ACE_TString strdriver = DStr; 
	DType = GetDriveType(strdriver.c_str());   //GetDriveType函数，可以获取驱动器类型，参数为驱动器的根目录  
	disk_num  =  DType;    //磁盘数
	drive_num  = DiskCount - disk_num;   //驱动数

	//读取各驱动器信息，由于DStr内部数据格式是A:\NULLB:\NULLC:\NULL，所以DSLength/4可以获得具体大循环范围  
	for(int i=0;i<DSLength/4;++i)  
	{ 
		int si=0;
		BOOL fResult;
		ACE_UINT64 i64FreeBytesToCaller;  
		ACE_UINT64 i64TotalBytes;  
		ACE_UINT64 i64FreeBytes; 
		ACE_TCHAR TotalBytes[10],FreeBytes[10];
		ACE_TString strTotalBytes,strFreeBytes; 

		strdriver = DStr + i * 4; 
		DType = GetDriveType(strdriver.c_str());   //GetDriveType函数，可以获取驱动器类型，参数为驱动器的根目录  

		m_DiskCount  =  DType;    //磁盘数
		m_Drive_num  = DiskCount - disk_num;   //驱动数

		switch (DType)  
		{  
		case DRIVE_FIXED:  
			{   
				fResult = GetDiskFreeSpaceEx (strdriver.c_str(),  
					(PULARGE_INTEGER)&i64FreeBytesToCaller,  
					(PULARGE_INTEGER)&i64TotalBytes,  
					(PULARGE_INTEGER)&i64FreeBytes);  

				if(fResult)  
				{ 
					int  m_TotalBytes,m_FreeBytes;
					m_TotalBytes = (float)i64TotalBytes/1024/1024/1024;
					m_FreeBytes  = (float)i64FreeBytesToCaller/1024/1024/1024;

					sprintf(TotalBytes,"%d",m_TotalBytes);
					sprintf(FreeBytes,"%d",m_FreeBytes);

					strTotalBytes = TotalBytes;
					strFreeBytes  = FreeBytes;
				}  
				else  
				{  
					strTotalBytes = "";  
					strFreeBytes = "";  
				} 
				ACE_TString  str = strdriver + strTotalBytes + "GB" +"/"+ strFreeBytes + "GB";  
				drive_info_vec_.push_back(str);
				si+=4; 
			}  
			break;
		default:
			break;
		}           
	}
	return 0;  
}