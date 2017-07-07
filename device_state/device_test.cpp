
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


	cout<<"�����������Ϣ��"<<endl;
//<< ��ȡ����������
	cout<< "�������� "<<ds_obj.get_main_machine_name()<<endl; 
//<< ��ȡ����IP��ַ
	cout<<"����ip�� "<<ds_obj.get_main_ip()<<endl<<endl;

//<< ��ȡ����ϵͳ�汾��Ϣ
	cout<<"����ϵͳ�汾��Ϣ��"<<ds_obj.get_version_info()<<endl<<endl;


	cout<<"cpu�������Ϣ��"<<endl;
//<< ��ȡcpuʹ�����Լ�������
	int m_cpu = ds_obj.get_cpu_use_rate();
	cout<< "cpuʹ���ʣ� "<<m_cpu<<"%"<<endl;
	int m_nouse_cpu;
	m_nouse_cpu = 100 - m_cpu;
	cout<< "cpu�����ʣ� "<<m_nouse_cpu<<"%"<<endl;
//<<cpu�ں���
	cout<<"cpu�ں����� "<< ds_obj.get_cpu_num() <<endl;
//<< cpu�ͺ�
     cout<<"cpu�ͺţ� "<< ds_obj.get_cpu_type()<<"�ͺ�" <<endl;
//<< cpu�ȼ�
	 cout<<"cpu�ȼ��� "<< ds_obj.get_cpu_grade()<<"��"<< endl<<endl;



//<< �����ڴ�ʹ�����
	 cout<<"�����ڴ�ʹ��������£�"<<endl;
	 int  usePrecent = ds_obj.get_mem_phy_rate();
	 int  TotalPhys = ds_obj.get_TotalPhys_mem_size();
	 int  AvailPhys = ds_obj.get_AvailPhys_mem_size();

	 int  UsePhys   = TotalPhys * usePrecent;
	      UsePhys   = UsePhys / 100;
	  int  RemainPhys =  AvailPhys -  UsePhys;

	 cout<<"ʵ���ܵ������ڴ棺 "<<TotalPhys<<" MB"<<endl;
	 cout<<"���������ڴ棺 "<<AvailPhys<<" MB"<<endl;
	 cout<<"���������ڴ棺 "<<UsePhys<<" MB"<<endl;
	 cout<<"ʣ�������ڴ棺 "<<RemainPhys<<" MB"<<endl;
	 cout<<"�����ڴ�ʹ���ʣ� "<<usePrecent<<"%"<<endl<<endl;

//<< ��ȡ������Ϣ
	 int   m_disk_num = 0;
	 int   m_drive_num = 0;
	 int   m_disk_count = 0;
	 vector<ACE_TString>  m_drive_info_vec_;

	 ds_obj.get_disk_info(m_disk_count,m_disk_num,m_drive_num,m_drive_info_vec_);
	 cout<<"������Ϣ��ʾ���£�"<<endl;
	 cout<<"�ܵ��̷�����"<<m_disk_count<<endl;
	 cout<<"���̸�����"<<m_disk_num<<endl;
	 cout<<"����������"<<m_drive_num<<endl;
	 cout<<"����ʹ�������"<<endl;
	 vector<ACE_TString>::iterator  m_iter;
	 for(m_iter = m_drive_info_vec_.begin(); m_iter != m_drive_info_vec_.end(); m_iter ++)
	 {
          cout<< *m_iter <<endl;
	 }
	 cout<<endl;

	return 0;
}