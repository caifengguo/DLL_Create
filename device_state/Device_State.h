
#include <vector>

#include <ace/ACE.h>
#include <ace/SString.h>
#include "Device_State_Export.h"

using namespace std;


class device_state_Export Device_State_t      //导出类
{
public:
	Device_State_t();
	~Device_State_t();

public:
    ///<<  获取程序的位数
	int get_program_bits();
    ///<<  获取操作系统的位数
	int get_system_bits();
	///<<  获取CPU核数
	int get_cpu_num();
	//<< cpu型号
	DWORD get_cpu_type();
	//<< cpu级数
	DWORD get_cpu_grade();
	//<<  获取内存大小
	int get_TotalPhys_mem_size();
	int get_VirtSize_mem_size();
	int get_AvailPhys_mem_size();

	//<<获取磁盘信息
	int get_disk_info(int &m_disk_count,int &m_DiskCount,int &m_Drive_num,vector<ACE_TString> &m_drive_info_vec_);
	//<<  获取主机名
	ACE_TCHAR* get_main_machine_name();

	//<< 获取H主机IP
	ACE_TCHAR*  get_main_ip();

	//<< 获取CPU使用率
	int  get_cpu_use_rate();

	//<< 获取物理内存使用率、物理内存使用情况
	int  get_mem_phy_rate();

	//<< 获取操作系统版本信息
	ACE_TString get_version_info();

protected:
	ACE_INT64  compare_file_time ( FILETIME* time1, FILETIME* time2 );

private: 
	WORD         m_ProcessorLevel;
	WORD         m_ProcessorType;
	DWORD        m_dwUsePre;                //物理内存使用率
	int          m_cpu;                     //cpu占有率
	int          m_cpuidle;                 //cpu空闲率
	ACE_TCHAR    m_Ip[15];                   //ip地址
	int          m_DiskCount;                //磁盘数目
	int          m_Drive_num;                //驱动数
	WORD         m_dwSize;                   //物理内存大小
	DWORD        m_dwVirtSize;              //虚拟内存大小
	DWORD        m_dwAvailPhys;             //可用物理内存大小
	ACE_TCHAR    m_Descripte[50];         //处理器描述符
	ACE_TCHAR    m_MDescripte[30];        //主机描述符
	ACE_TCHAR    m_Group[30];             //状态
};