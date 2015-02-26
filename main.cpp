#include <iostream>
#include <cstdlib>
//#include "UnorderedLinkedList.h"
#include "OrderedLinkedList.h"
#include <memory>
//#include "unistd.h"
#include "boost/thread.hpp"
using namespace std;
//задаем кол-во потоков для тестирования контейнера
static const int num_threads = 20;
orderedLinkedList<int> list1;
nodeType<int>* node;


//таск для вывода списка на консоль
 void print_list()

     {


        list1.print();



     }

//вставка элемента
  void insert_node_test(int i)

    {
              // std::cout<<"Inserting node with key:"<<i<<std::endl;

              list1.insert(i);


    }
//вставка элемента с пошаговым выводом сотояния спиская
  void step_write_out_insert_test(int i)

    {


         list1.insert(i);
         list1.print();


    }

 //удаление prev узла
 void delete_node_test(int i)

       {

         // std::cout<<"Deleting node with key:"<<i<<std::endl;
          list1.deleteNode(i);


       }

 int main()

    {
        std::cout << "Launching multi-threaded tests\n";
        //массив потоков для тестирования операции вставки
        boost::thread t_ins[num_threads];
        //массив потоков для тестирования операции удаления
        boost::thread t_del[num_threads];
        //запускаем 2 группу parallel потоков: 1 - вставка элемента, 2 - удаление

        for (int i = 0; i < num_threads; ++i)
        {
            t_ins[i] = boost::thread(insert_node_test,i);

        }
          for (int i = 0; i < num_threads/2; ++i)
        {

            t_del[i] = boost::thread(delete_node_test,i);
        }

        //джоиним созданные потоки с главным
        for (int i = 0; i < num_threads; ++i)
        {
            t_ins[i].join();

        }
         for (int i = 0; i < num_threads/2; ++i)
        {
            t_del[i].join();
        }
         //проверяем операцию вставки (если элемент со вставляемым ключом уже есть в списке, то мы его не вставляем)

         //поток для вывода состояния списка
         boost::thread workerThread_sec(print_list);
         workerThread_sec.join();
         return 0;

    }
