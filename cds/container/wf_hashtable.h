#ifndef CDSLIB_CONTAINER_EAWfPAD_HASHTABLE_H
#define CDSLIB_CONTAINER_EAWfPAD_HASHTABLE_H

#include <stdlib.h>
#include <atomic>  



namespace cds { namespace container {

/*
	Efficient Almost wait-free Parallel Accessible Dynamic Hashtables
	Gao, Groote, Hesselink (2003)
*/

template <typename KEY, typename T>
class WfHashtable
{
protected:
	typedef enum { DEL, VALUE, OLDV } eType;

        template <typename KEY1, typename T1>
	struct EValue {

	private:
		eType type;
		int address;
		T* value;

	public:
		EValue(){
			type = VALUE;
			address = 0;
			value = NULL;
		}

		EValue(int address, T* value) {
			this->type = VALUE;
			this->address = address;
			this->value = value;
		}

		// ----------- Setters -----------

		void setValue(int address, T* value) {
			this->address = address;
			this->value = value;
		}

		void setDel(){
			type = DEL;
		}

		void setOld(){
			type = OLDV;
		}

		void setDone() {
			type = OLDV;
			value = NULL;
		}

		// ----------- Getters -----------

		int ADR() {
			return address;
		}

		T* val() {
			if(type == DEL){
				return NULL;
			}
			else{
				return value;
			}
		}

		bool oldp() {
			return type == OLDV;
		}

		bool done() {
			return oldp() && val() == NULL;
		}
	};

	struct Hashtable {
		int size; // size of the hashtable
		int occ; // number of occupied positions in the table
		int dels; // number of deleted positions
		int bound; // the maximal number of places that can be occupied before refreshing the table
		std::atomic<EValue<KEY, T>>* table;
		
		Hashtable(int size, int bound){
			this->size = size;
                        EValue<KEY, T>* temp= new EValue<KEY,T>[size];
                        std::atomic_init(this->table,*temp);
			this->bound = bound;
			this->occ = 0;
			this->dels = 0;
		}
		~Hashtable(){
			delete table;
		}
	};

	int P;
	std::atomic<Hashtable*>* H; // 1..2P
	std::atomic<int> currInd; // 1..2P = index of the currently valid hashtable
	int* busy; // 1..2P = number of processes that are using a hashtable
	std::atomic<int>* next; // 1..2P = next hashtable to which the contents of hashtable H[i] is being copied
	std::atomic<int>* prot; // 1..2P = is used to guard the variables busy[i], next[i] and H[i]
			   // against being reused for a new table, before all processes have discarded these

public:
        template <typename KEY2, typename T2>
	class WfHashtableProcess {
	protected:
		WfHashtable* wh;
		int index; // 1..2P

	public:

		WfHashtableProcess(WfHashtable* wh){
			this->wh = wh;
			getAccess();
		}
		
		~WfHashtableProcess(){
			releaseAccess(index);
		}

		// ----------- HASHTABLE METHODS -----------

		T* find(int a) {
			EValue<KEY,T> r;
			int n, l, k;
			Hashtable* h;

			h = wh->H[index];
			n = 0;
			l = h->size;

			do {
				k = key(a, l, n);
                                r=h->table[k].load(); // ATOMIC
				if (r.done()) {
					refresh();
					h = wh->H[index];
					l = h->size;
				}
				else {
					n++;
				}

			} while (r.val() != 0 && a != r.ADR());

			return r.val();
		}

		bool del(int a)
		{
			EValue<KEY,T> r;
			int k, l, n;
			Hashtable* h;
			bool suc;

			h = wh->H[index];
			suc = false;
			l = h->size;
			n = 0;

			do {
				k = key(a, l, n);
                                r=h->table[k].load(); // ATOMIC
				if (r.oldp()) {
					refresh();
					h = wh->H[index];
					l = h->size;
					n = 0;
				}
				else if (a == r.ADR()) {
                                        EValue<KEY, T> newValue(a, NULL);
                                        newValue.setDel();
                                        suc =std::atomic_compare_exchange_strong(&h->table[k] , &r, newValue);//atmic
				}
				else {
					n++;
				}

			} while (!(suc || r.val() == 0));
			if (suc) {
				h->dels++;
			}
			return suc;
		}

		bool insert(int a, T* v) {
                        EValue<KEY, T> r;
			int k,l,n;
			Hashtable* h;
			bool suc;
			h = wh->H[index];
			if (h->occ > h->bound) {
				newTable();
				h = wh->H[index];
			}
			n=0; l=h->size; suc=false;
			do{
				k=key(a,l,n);
                                r = h->table[k].load();

                                if (r.oldp()){
					refresh();
					h = wh->H[index];
					n = 0; l = h->size;
				}else {
                                        if(r.val() == NULL){
						EValue<KEY, T>* null_ptr = NULL;
						EValue<KEY, T> newValue(a, v);
                                                suc=std::atomic_compare_exchange_strong(&h->table[k] , null_ptr, newValue);//atmic
					}
					else{
						n++;
					}
				}
                        }while(!(suc || a != r.ADR()));
			if (suc) {
					h->occ++;
				}
				return suc;
			}

		void assign(int a, T* v) {
			EValue<KEY,T> r; 
			int k,l,n;
			Hashtable* h;
			bool suc;

			h = wh->H[index];
			if (h->occ > h->bound) {
				newTable();
				h = wh->H[index];
			}
			n=0; l=h->size; suc=false;
			do{
				k = key(a,l,n);
                                r=h->table[k].load();//atomic
				if (r.oldp()){
					refresh();
					h = wh->H[index];
					n=0; l=h->size;
				}else {
                                        if(r.val()==0 || a==r.ADR()){
                                                EValue<KEY, T> newValue(a, v);
                                                suc=std::atomic_compare_exchange_strong(&h->table[k] , &r, newValue);//atmic

					}
					else{
						n++;
					}
				}
			}while(!(suc));
			if(r.val()==0){
				h->occ++;
			}
		}
		
	protected:

		// ----------- ACCESS METHODS -----------

		void getAccess() {
			while (true) {
				index = wh->currInd;
				wh->prot[index] ++;
				if (index == wh->currInd) {
					wh->busy[index]++;
					if (index == wh->currInd) {
						return;
					}
					else {
						releaseAccess(index);
					}
				}
				else {
					wh->prot[index]--;
				}
			}
		}

		void releaseAccess(int i) {
			Hashtable* h;
			h = wh->H[i];
			wh->busy[i]--;
			if (h != NULL && wh->busy[i] == 0) {
				Hashtable* null_ptr = NULL;
				if (std::atomic_compare_exchange_strong(&wh->H[i], &h, null_ptr)) {
					deAlloc(i);
				}
			}
			wh->prot[i]--;
		}

		int key(int a, int l, int n) {
			return a % l;
			// ?????? ????? Inc(a,l,n-1)
		}

		// ----------- HEAP methods -----------

		void allocate(int i, int s, int b) {
			Hashtable* tmp = new Hashtable(s, b);
			std::atomic_exchange(&wh->H[i], tmp);
			if (wh->H[i] != tmp) delete tmp;
		}

		void deAlloc(int h) {
                        Hashtable* tmp=new Hashtable(0,0);
                        std::atomic_compare_exchange_strong(&wh->H[h],&tmp,tmp);
		}

		void newTable() {
			int i; // 1..2P
			bool b, bb;
			int temp = 0;
                        while(wh->next[index] == 0){
				i = rand() % (2*wh->P) + 1;
				
				if(std::atomic_compare_exchange_strong(&wh->prot[i] , &temp, 0)){// ATOMIC
					wh->busy[i] = 1;
					int bound = (wh->H[index].load())->bound - (wh->H[index].load())->dels + 2 * wh->P + 1;
					int size = bound + 2 * wh->P + 1;
					allocate(i, size, bound);
					wh->next[i] = 0;
					bb = std::atomic_compare_exchange_strong(&wh->next[index] , &temp, 1);//atomic
					if(!bb) releaseAccess(i);
				}
			}
			refresh();
		}

		void migrate() {
			int i; // 0..2P
			Hashtable* h;
			bool b;
			i = wh->next[index];
			wh->prot[i]++;
			if (index != wh->currInd) {
				wh->prot[i]--;
			}
			else {
				wh->busy[i]++;
				h = wh->H[i];
				if (index == wh->currInd) {
						moveContents(wh->H[index], h);
						if (std::atomic_compare_exchange_strong(&wh->currInd , &index, i)) { // ATOMIC
						wh->busy[index]--;
						wh->prot[index]--;
					}
				}
				releaseAccess(i);
			}
		}

		void refresh() {
			if (index != wh->currInd) {
				releaseAccess(index);
				getAccess();
			}
			else {
				migrate();
			}
		}

		void moveContents(Hashtable* from, Hashtable* to) {
			int i;
			bool b;
			EValue<KEY,T> v;
			int* toBeMoved;
			toBeMoved = new int[from->size];
			for(int j=0; j<from->size; ++j){
				toBeMoved[j] = j;
			}
			int toBeMovedSize = from->size;
			while(wh->currInd == index && toBeMovedSize > 0){
				i = toBeMoved[toBeMovedSize - 1];
				EValue<KEY,T> v = from->table[i];
				if(from->table[i].load().done()){
					toBeMovedSize--;
				}
				else{
					EValue<KEY, T> tmp(v.ADR(), v.val());
					tmp.setOld();
					if(std::atomic_compare_exchange_strong(&from->table[i], &v, tmp)){

                                                if(v.val() != NULL) moveElement(v.ADR(),v.val(), to);
						from->table[i].load().setDone();
						toBeMovedSize--;
					}
				}	
			}
		}

                void moveElement(int a,T* v, Hashtable* to)
		{
			int k, m, n;
			EValue<KEY,T> w;
			bool b;

			n = 0;
			b = false;
			m = to->size;
                        EValue<KEY, T> temp(a,NULL);
			do{
				k = key(a, m, n);
				w = to->table[k];
				if (w.val() == NULL) {
                                                EValue<KEY, T> newValue(a, v);
                                                EValue<KEY, T>* null_ptr = NULL;
                                                b=std::atomic_compare_exchange_strong(&to->table[k] , null_ptr, newValue); // ATOMIC
				}
				else {
					n++;
				}

                        } while (!(b || a == w.ADR() || wh->currInd != index));
			if (b) to->occ++;
		}
	};

	// ----------- WfHashtable API -----------

	typedef WfHashtableProcess<KEY, T> process;

	WfHashtable(int P) {
		this->P = P;
		this->H = new std::atomic<Hashtable*>[2 * P];
		this->busy = new int[2 * P];
		this->prot = new std::atomic<int>[2 * P];
		this->next = new std::atomic<int>[2 * P];
	};

	~WfHashtable() {
		for (int i = 0; i<2 * P; ++i) {
			delete H[i];
		}
		delete H;
		delete busy;
		delete prot;
		delete next;
	}

	WfHashtableProcess<KEY, T>* getProcess() {
		WfHashtableProcess<KEY, T>* process = new WfHashtableProcess<KEY, T>(this);
		return process;
	}

};
}}

#endif // #ifndef CDSLIB_CONTAINER_EAWfPAD_HASHTABLE_H
