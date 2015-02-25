#include <iostream>
#include <cstdlib>
//#include "UnorderedLinkedList.h"
#include "OrderedLinkedList.h"
#include <memory>
//#include "unistd.h"
#include "boost/thread.hpp"
using namespace std;

 orderedLinkedList<int> list1;
// nodeType<int> *fst=list1.first;


//таск для вывода списка на консоль
 void print_list()

     {


         list1.print();


     }

//вставка элемента
  void insert_node_test(int i)

    {


        std::cout << "Worker: running insert task" << std::endl;
        list1.insert(i);


        std::cout << "Worker: finished insert task" << std::endl;



    }
//вставка элемента с пошаговым выводом сотояния спиская
  void step_write_out_insert_test(int i)

    {


         std::cout << "Worker: running insert node" << std::endl;
         list1.insert(i);
         std::cout << "Worker: finished insert node" << std::endl;
         list1.print();


    }

 //удаление prev узла
 void delete_node_test(int i)

       {
          std::cout << "Worker: running delete node task" << std::endl;

          list1.deleteNode(i);
          std::cout << "Worker: finished delete node task" << std::endl;

       }

 int main()

    {
         //проверяем операцию вставки (если элемент со вставляемым ключом уже есть в списке, то мы его не вставляем)
         std::cout << "main:Testing List startup" << std::endl;



 for (int i=7000;i>6000;i--)

               {

                   boost::thread workerThread(insert_node_test,i);
                   std::cout << "main: waiting for thread" <<workerThread.get_id()<< std::endl;
                   workerThread.join();
                   std::cout << "\n main: thread done "<< std::endl;


               }

          for (int i=1;i<1000;i++)

              {

                  boost::thread workerThread(insert_node_test,i);

                 std::cout << "main: waiting for thread" <<workerThread.get_id()<< std::endl;
                  workerThread.join();

                  std::cout << "\n main: thread done  " <<std::endl;


              }

           for (int i=5000;i<6000;i++)

               {

                   boost::thread workerThread(insert_node_test,i);
                   std::cout << "main: waiting for thread" <<workerThread.get_id()<< std::endl;
                   workerThread.join();
                   std::cout << "\n main: thread done "<< std::endl;


               }
                 //проверяем операцию удаления узлов
                 for (int i=5000;i<5200;i++)

               {

                   boost::thread workerThread(delete_node_test,i);
                   std::cout << "main: waiting for thread" <<workerThread.get_id()<< std::endl;
                   workerThread.join();
                   std::cout << "\n main: thread done "<< std::endl;


               }
                 for (int i=1;i<1000;i++)

               {

                   boost::thread workerThread(delete_node_test,i);
                   std::cout << "main: waiting for thread" <<workerThread.get_id()<< std::endl;
                   workerThread.join();
                   std::cout << "\n main: thread done "<< std::endl;


               }

         boost::thread workerThread_sec(print_list);
         workerThread_sec.join();
         return 0;

    }
