#include <iostream>
#include <cstdlib>
//#include "UnorderedLinkedList.h"
#include "OrderedLinkedList.h"
#include <memory>
//#include "unistd.h"
#include "boost/thread.hpp"
using namespace std;
//������ ���-�� ������� ��� ������������ ����������
static const int num_threads = 20;
orderedLinkedList<int> list1;
nodeType<int>* node;


//���� ��� ������ ������ �� �������
 void print_list()

     {


        list1.print();



     }

//������� ��������
  void insert_node_test(int i)

    {
              // std::cout<<"Inserting node with key:"<<i<<std::endl;

              list1.insert(i);


    }
//������� �������� � ��������� ������� �������� �������
  void step_write_out_insert_test(int i)

    {


         list1.insert(i);
         list1.print();


    }

 //�������� prev ����
 void delete_node_test(int i)

       {

         // std::cout<<"Deleting node with key:"<<i<<std::endl;
          list1.deleteNode(i);


       }

 int main()

    {
        std::cout << "Launching multi-threaded tests\n";
        //������ ������� ��� ������������ �������� �������
        boost::thread t_ins[num_threads];
        //������ ������� ��� ������������ �������� ��������
        boost::thread t_del[num_threads];
        //��������� 2 ������ parallel �������: 1 - ������� ��������, 2 - ��������

        for (int i = 0; i < num_threads; ++i)
        {
            t_ins[i] = boost::thread(insert_node_test,i);

        }
          for (int i = 0; i < num_threads/2; ++i)
        {

            t_del[i] = boost::thread(delete_node_test,i);
        }

        //������� ��������� ������ � �������
        for (int i = 0; i < num_threads; ++i)
        {
            t_ins[i].join();

        }
         for (int i = 0; i < num_threads/2; ++i)
        {
            t_del[i].join();
        }
         //��������� �������� ������� (���� ������� �� ����������� ������ ��� ���� � ������, �� �� ��� �� ���������)

         //����� ��� ������ ��������� ������
         boost::thread workerThread_sec(print_list);
         workerThread_sec.join();
         return 0;

    }
