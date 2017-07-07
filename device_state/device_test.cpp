
#include<vector>

#include "ace/ACE.h"
#include "ace/OS.h"
#include "ace/Log_Msg.h"
#include "ace/streams.h"
#include "Device_State.h"
#include <ace/SString.h>



int
ACE_TMAIN (int argc, ACE_TCHAR *argv[])
{

	Device_State_t  ds_obj;


	cout<<"主机的相关信息："<<endl;
//<< 获取本地主机名
	cout<< "主机名： "<<ds_obj.get_main_machine_name()<<endl; 
//<< 获取主机IP地址
	cout<<"主机ip： "<<ds_obj.get_main_ip()<<endl<<endl;

//<< 获取操作系统版本信息
	cout<<"操作系统版本信息："<<ds_obj.get_version_info()<<endl<<endl;


	cout<<"cpu的相关信息："<<endl;
//<< 获取cpu使用率以及空闲率
	int m_cpu = ds_obj.get_cpu_use_rate();
	cout<< "cpu使用率： "<<m_cpu<<"%"<<endl;
	int m_nouse_cpu;
	m_nouse_cpu = 100 - m_cpu;
	cout<< "cpu空闲率： "<<m_nouse_cpu<<"%"<<endl;
//<<cpu内核数
	cout<<"cpu内核数： "<< ds_obj.get_cpu_num() <<endl;
//<< cpu型号
     cout<<"cpu型号： "<< ds_obj.get_cpu_type()<<"型号" <<endl;
//<< cpu等级
	 cout<<"cpu等级： "<< ds_obj.get_cpu_grade()<<"级"<< endl<<endl;



//<< 物理内存使用情况
	 cout<<"物理内存使用情况如下："<<endl;
	 int  usePrecent = ds_obj.get_mem_phy_rate();
	 int  TotalPhys = ds_obj.get_TotalPhys_mem_size();
	 int  AvailPhys = ds_obj.get_AvailPhys_mem_size();

	 int  UsePhys   = TotalPhys * usePrecent;
	      UsePhys   = UsePhys / 100;
	  int  RemainPhys =  AvailPhys -  UsePhys;

	 cout<<"实际总的物理内存： "<<TotalPhys<<" MB"<<endl;
	 cout<<"可用物理内存： "<<AvailPhys<<" MB"<<endl;
	 cout<<"已用物理内存： "<<UsePhys<<" MB"<<endl;
	 cout<<"剩余物理内存： "<<RemainPhys<<" MB"<<endl;
	 cout<<"物理内存使用率： "<<usePrecent<<"%"<<endl<<endl;

//<< 获取磁盘信息
	 int   m_disk_num = 0;
	 int   m_drive_num = 0;
	 int   m_disk_count = 0;
	 vector<ACE_TString>  m_drive_info_vec_;

	 ds_obj.get_disk_info(m_disk_count,m_disk_num,m_drive_num,m_drive_info_vec_);
	 cout<<"磁盘信息显示如下："<<endl;
	 cout<<"总的盘符数："<<m_disk_count<<endl;
	 cout<<"磁盘个数："<<m_disk_num<<endl;
	 cout<<"驱动个数："<<m_drive_num<<endl;
	 cout<<"磁盘使用情况："<<endl;
	 vector<ACE_TString>::iterator  m_iter;
	 for(m_iter = m_drive_info_vec_.begin(); m_iter != m_drive_info_vec_.end(); m_iter ++)
	 {
          cout<< *m_iter <<endl;
	 }
	 cout<<endl;

	return 0;
}