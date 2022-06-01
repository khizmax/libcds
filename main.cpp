#include <cds/init.h>  //cds::Initialize и cds::Terminate
#include <cds/gc/hp.h> //cds::gc::HP (Hazard Pointer)
#include <iostream> //cds::gc::HP (Hazard Pointer)

using namespace std;
int main() {
//
    // Инициализируем libcds
    cds::Initialize() ;
    {
        // Инициализируем Hazard Pointer синглтон
        cds::gc::HP hpGC ;

        // Если main thread использует lock-free контейнеры
        // main thread должен быть подключен
        // к инфраструктуре libcds
        cds::threading::Manager::attachThread() ;

        // Всё, libcds готова к использованию
        // Далее располагается ваш код
    }

//    OptimisticQueue< cds::gc::HP, int > queue;
//    cout << queue.push(1) << endl;
//    cout << queue.push(2) << endl;
//    cout << queue.push(3) << endl;
//
//
//    // Завершаем libcds
//    cds::Terminate() ;

}