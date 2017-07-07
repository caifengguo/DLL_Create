

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




// ��ȡ����λ����������Ϊ����λ�Ĵ��룩
int get_program_bits()
{
	return sizeof(int*) * 8;
}

// ��ȫ��ȡ����ʵϵͳ��Ϣ
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

// ��ȡ����ϵͳλ��
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


//<< ��ȡCPU������Ŀ 
int  Device_State_t::get_cpu_num()
{
	SYSTEM_INFO si; 
	memset(&si,0,sizeof(SYSTEM_INFO));  
	GetSystemInfo(&si);                //����
	return  si.dwNumberOfProcessors;
}

//<< cpu�ͺ�
DWORD Device_State_t::get_cpu_type()
{
	SYSTEM_INFO si; 
	memset(&si,0,sizeof(SYSTEM_INFO));  
	GetSystemInfo(&si);  
	m_ProcessorType = si.dwProcessorType;          //����
	return  m_ProcessorType;
}

//<< cpu����
DWORD Device_State_t::get_cpu_grade()
{
	SYSTEM_INFO si; 
	memset(&si,0,sizeof(SYSTEM_INFO));  
	GetSystemInfo(&si);  
	m_ProcessorLevel = si.wProcessorLevel;         //����
	return  m_ProcessorLevel;
}


//��ȡ���������ڴ� 
int Device_State_t::get_AvailPhys_mem_size()
{
	MEMORYSTATUS   Mem;
	GlobalMemoryStatus(&Mem);

	DWORD  dwAvailPhys;
	dwAvailPhys = Mem.dwAvailPhys/1024;      //���������ڴ� 
	m_dwAvailPhys = dwAvailPhys/1024;
	return  m_dwAvailPhys;	
}

//��ȡ�ܵ������ڴ�(ʵ���ڴ�)
int Device_State_t::get_TotalPhys_mem_size()
{
	MEMORYSTATUS   Mem;
	GlobalMemoryStatus(&Mem);

	DWORD        dwSize; 
	dwSize = Mem.dwTotalPhys/1024;           //�ܵ������ڴ�   
	m_dwSize = dwSize/1024;

	return  m_dwSize;	    
}

//��ȡ�ܵ������ڴ� 
int Device_State_t::get_VirtSize_mem_size()
{
	MEMORYSTATUS   Mem;
	GlobalMemoryStatus(&Mem);

	DWORD   dwVirtSize;
	dwVirtSize = Mem.dwTotalVirtual/1024;      //�ܵ������ڴ�
	m_dwVirtSize = dwVirtSize/1024;

	return  m_dwVirtSize;	  
}



//<< ��ȡ�����ڴ�ʹ����
int  Device_State_t::get_mem_phy_rate()
{
	MEMORYSTATUS   Mem;
	GlobalMemoryStatus(&Mem);
	m_dwUsePre = (DWORD)Mem.dwMemoryLoad;                   //�ڴ�ʹ�ðٷֱ�

	return m_dwUsePre;
}


ACE_INT64 Device_State_t::compare_file_time ( FILETIME* time1, FILETIME* time2 )
{
	ACE_INT64 a = time1->dwHighDateTime << 32 | time1->dwLowDateTime ;
	ACE_INT64 b = time2->dwHighDateTime << 32 | time2->dwLowDateTime ;

	return   (b - a);
}


//<< ��ȡcpuʹ�����Լ�������
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

	//��ȡϵͳʱ�� 
	res = GetSystemTimes( &idleTime, &kernelTime, &userTime );   //�����ֱ�Ϊ����ʱ��  �ں�ʱ��  �û�ʱ��

	preidleTime = idleTime;       //����ʱ��
	prekernelTime = kernelTime;   //�ں�ʱ��
	preuserTime = userTime;       //�û�ʱ��

	hEvent = CreateEvent (NULL,FALSE,FALSE,NULL); // ��ʼֵΪ nonsignaled ������ÿ�δ������Զ�����Ϊnonsignaled

	// while(1)
	//  {
	WaitForSingleObject( hEvent,100);        //�ȴ�100����
	res = GetSystemTimes( &idleTime, &kernelTime, &userTime );

	int idle = compare_file_time( &preidleTime,&idleTime);
	int kernel = compare_file_time( &prekernelTime, &kernelTime);
	int user = compare_file_time(&preuserTime, &userTime);

	m_cpu = (kernel +user - idle) *100/(kernel+user);     //ռ����         
	m_cpuidle = 100 - m_cpu;                     //������

	preidleTime = idleTime;
	prekernelTime = kernelTime;
	preuserTime = userTime ;
	//  }
	return  m_cpu;
}


//<< ��ȡ����������
ACE_TCHAR*  Device_State_t::get_main_machine_name()
{
	// ��������   
	ULONG    ulOutBufLen;       // �����ȡ���ı��ؼ�������������Ϣ�ṹ������ĳ���  
	DWORD    dwRetVal;          // ����GetNetworkParams()�����ķ���ֵ   

	FIXED_INFO*  FixedInfo;     // ���屣�汾�ؼ�������������Ϣ�Ľṹ��ָ��
	// ��ȡ��Ϣ        
	FixedInfo = (FIXED_INFO *) GlobalAlloc( GPTR, sizeof( FIXED_INFO ) );   // ΪFixedInfo�ṹ������ڴ�ռ�  
	ulOutBufLen = sizeof( FIXED_INFO );                // ��ʼ��ulOutBufLen����ֵ  

	// ��1�ε���GetNetworkParams()��������ȡ���ؽ���Ĵ�С��ulOutBufLen��  
	if( ERROR_BUFFER_OVERFLOW == GetNetworkParams( FixedInfo, &ulOutBufLen ) ) 
	{  
		GlobalFree( FixedInfo );  
		FixedInfo = (FIXED_INFO *) GlobalAlloc( GPTR, ulOutBufLen );  
	}  

	// ��2�ε���GetNetworkParams()��������ǰ���ȡ��ulOutBufLen��Ϊ������  
	if ( dwRetVal = GetNetworkParams(FixedInfo, &ulOutBufLen ) != ERROR_SUCCESS)
	{  
		return 0;  
	}  
	return FixedInfo->HostName;    
}




//<< ��ȡ����IP��ַ
ACE_TCHAR* Device_State_t::get_main_ip()
{
	PMIB_IPADDRTABLE pIPAddrTable;   // ����ӿ���IP��ַӳ���  
	DWORD dwSize = 0;     // ��ȡ���ݵĴ�С  
	DWORD dwRetVal = 0;    // ����GetIPAddrTable()�����ķ���ֵ  
	IN_ADDR IPAddr;        // ����IP��ַ�Ľṹ��  
	LPVOID lpMsgBuf;       // ���ڻ�ȡ������Ϣ 

	// �����ڴ�ռ� 
	pIPAddrTable = (MIB_IPADDRTABLE *) malloc(sizeof (MIB_IPADDRTABLE));

	// ��1�ε���GetIpAddrTable()��������ȡ���ݵĴ�С��dwSize  
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
	// ��2�ε���GetIpAddrTable()��������ȡʵ������  
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


//��ȡ����ϵͳ�汾��Ϣ
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
	GetVersionEx(&lpVersionInformation);     // ָ��汾��Ϣ�ṹ���ָ��

	ACE_TCHAR version[1024] = {'\0'};
	ACE_OS::sprintf(version,ACE_TEXT("MajorVersion[%u] MinorVersion[%u] BuildNumber[%u] PlatformId[%u] CSDVersion[%s]"),
		lpVersionInformation.dwMajorVersion,
		lpVersionInformation.dwMinorVersion,
		lpVersionInformation.dwBuildNumber,
		lpVersionInformation.dwPlatformId,
		lpVersionInformation.szCSDVersion);
	return version;
} 



//<< ��ȡ����ʹ�����
int  Device_State_t::get_disk_info(int &m_disk_count,int &disk_num,int &drive_num,vector<ACE_TString> &drive_info_vec_)
{
	DWORD DiskCount = 0;  
	//��ȡϵͳ���߼����������������������ص���һ��32λ�޷����������ݡ�  
	DWORD DiskInfo = GetLogicalDrives(); 
	//���Ϊ1�����Ϊ��,���Ϊ0����̲����ڡ�  
	while(DiskInfo)  
	{  
		//ͨ��λ������߼���������ж��Ƿ�Ϊ1   
		if(DiskInfo&1)  
		{  
			DiskCount++;  
		}  
		DiskInfo = DiskInfo >> 1;  //ͨ��λ��������Ʋ�����֤ÿѭ��һ��������λ�������ƶ�һλ��*/  
	} 
	m_disk_count = DiskCount;

	//ͨ��GetLogicalDriveStrings()������ȡ�����������ַ�����Ϣ����  
	int DSLength = GetLogicalDriveStrings(0,NULL);  
	CHAR*  DStr = new CHAR[DSLength];  
	memset(DStr,0,DSLength);  

	//ͨ��GetLogicalDriveStrings���ַ�����Ϣ���Ƶ�����������,���б�������������������Ϣ��  
	GetLogicalDriveStrings(DSLength,DStr);  

	//��ȡ��������������
	int DType; 
	ACE_TString strdriver = DStr; 
	DType = GetDriveType(strdriver.c_str());   //GetDriveType���������Ի�ȡ���������ͣ�����Ϊ�������ĸ�Ŀ¼  
	disk_num  =  DType;    //������
	drive_num  = DiskCount - disk_num;   //������

	//��ȡ����������Ϣ������DStr�ڲ����ݸ�ʽ��A:\NULLB:\NULLC:\NULL������DSLength/4���Ի�þ����ѭ����Χ  
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
		DType = GetDriveType(strdriver.c_str());   //GetDriveType���������Ի�ȡ���������ͣ�����Ϊ�������ĸ�Ŀ¼  

		m_DiskCount  =  DType;    //������
		m_Drive_num  = DiskCount - disk_num;   //������

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