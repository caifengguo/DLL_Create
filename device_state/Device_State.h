
#include <vector>

#include <ace/ACE.h>
#include <ace/SString.h>
#include "Device_State_Export.h"

using namespace std;


class device_state_Export Device_State_t      //������
{
public:
	Device_State_t();
	~Device_State_t();

public:
    ///<<  ��ȡ�����λ��
	int get_program_bits();
    ///<<  ��ȡ����ϵͳ��λ��
	int get_system_bits();
	///<<  ��ȡCPU����
	int get_cpu_num();
	//<< cpu�ͺ�
	DWORD get_cpu_type();
	//<< cpu����
	DWORD get_cpu_grade();
	//<<  ��ȡ�ڴ��С
	int get_TotalPhys_mem_size();
	int get_VirtSize_mem_size();
	int get_AvailPhys_mem_size();

	//<<��ȡ������Ϣ
	int get_disk_info(int &m_disk_count,int &m_DiskCount,int &m_Drive_num,vector<ACE_TString> &m_drive_info_vec_);
	//<<  ��ȡ������
	ACE_TCHAR* get_main_machine_name();

	//<< ��ȡH����IP
	ACE_TCHAR*  get_main_ip();

	//<< ��ȡCPUʹ����
	int  get_cpu_use_rate();

	//<< ��ȡ�����ڴ�ʹ���ʡ������ڴ�ʹ�����
	int  get_mem_phy_rate();

	//<< ��ȡ����ϵͳ�汾��Ϣ
	ACE_TString get_version_info();

protected:
	ACE_INT64  compare_file_time ( FILETIME* time1, FILETIME* time2 );

private: 
	WORD         m_ProcessorLevel;
	WORD         m_ProcessorType;
	DWORD        m_dwUsePre;                //�����ڴ�ʹ����
	int          m_cpu;                     //cpuռ����
	int          m_cpuidle;                 //cpu������
	ACE_TCHAR    m_Ip[15];                   //ip��ַ
	int          m_DiskCount;                //������Ŀ
	int          m_Drive_num;                //������
	WORD         m_dwSize;                   //�����ڴ��С
	DWORD        m_dwVirtSize;              //�����ڴ��С
	DWORD        m_dwAvailPhys;             //���������ڴ��С
	ACE_TCHAR    m_Descripte[50];         //������������
	ACE_TCHAR    m_MDescripte[30];        //����������
	ACE_TCHAR    m_Group[30];             //״̬
};