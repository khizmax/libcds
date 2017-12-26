#ifndef CDSLIB_CONTAINER_EAWfPAD_HASHTABLE_H
#define CDSLIB_CONTAINER_EAWfPAD_HASHTABLE_H

#include <stdlib.h>

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

	template <typename KEY, typename T>
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

		// ----------- Setters -----------

		void setValue(T* value) {
			this->value = value;
		}

		void setDel(){
			type = DEL;
			//value = NULL; // ???
		}

		void setOld(){
			type = OLDV;
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
		EValue* table;
		
		Hashtable(int size, int bound){
			this->size = size;
			this->table = new EValue[size];
			this->bound = bound;
			this->occ = 0;
			this->dels = 0;
		}
		~Hashtable(){
			delete table;
		}
	};

	int P;
	Hashtable** H; // 1..2P
	int currInd; // 1..2P = index of the currently valid hashtable
	int* busy; // 1..2P = number of processes that are using a hashtable
	Hashtable* next; // 1..2P = next hashtable to which the contents of hashtable H[i] is being copied
	int* prot; // 1..2P = is used to guard the variables busy[i], next[i] and H[i]
			   // against being reused for a new table, before all processes have discarded these

public:
	WfHashtable(int P){
		this->P  = P;
		this->H = new Hashtable[2*P];
		this->busy = new int[2*P];
		this->prot = new int[2*P];
	};

	~WfHashtable(){
		for(int i=0; i<2*P; ++i){
			delete H[i];
		}
		delete H;
		delete busy;
		delete prot;
	}

	WfHashtableProcess* getProcess(){
		WfHashtableProcess* process = new WfHashtableProcess(this);
		return process;
	}

	class WfHashtableProcess {
	protected
		WfHashtable* wh;
		int index; // 1..2P

	public:

		WfHashtableProcess(WfHashtable* wh){
			this->wh = wh;
		}
		
		~WfHashtableProcess(){}

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
			if (h != 0 && wh->busy[i] == 0) {
				if (wh->H[i] = h) { // ATOMIC
					wh->H[i] = 0; // ATOMIC
					deAlloc(i);//change h
				}
			}
			wh->prot[i]--;
		}

		// ----------- HASHTABLE METHODS -----------

		int key(int a, int l, int n) {
			return a % l;
			// ÇÄÅÑÜ ×ÒÎ-òî íåïîíÿòíîå Inc(a,l,n-1)
		}

		T* find(int a) {
			EValue r;
			int n, l;
			Hashtable* h;

			h = wh->H[index];
			n = 0;
			l = h->size;

			do {
				r = h->table[key(a, l, n)]; // ATOMIC
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
			EValue r;
			int k, l, n;
			Hashtable* h;
			bool suc;

			h = wh->H[index];
			suc = false;
			l = h->size;
			n = 0;

			do {
				k = key(a, l, n);
				r = h->table[k]; // ATOMIC
				if (r.oldp()) {
					refresh();
					h = wh->H[index];
					l = h->size;
					n = 0;
				}
				else if (a == r.ADR()) {
					if (r == h->table[k]) { // ATOMIC
						suc = true; // ATOMIC
						h->table[k].setDel(); // ATOMIC
					}
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
			EValue r; 
			int k,l,n;
			Hashtable* h;
			bool suc;

			h = wh->H[index];
			if h.occ > h.bound {
				newTable();
				h = wh->H[index];
			}
			n=0; l=h.size; suc=false;
			do{
				k=key(a,l,n);
				r=h.table[k];//atomic
				if (r.oldp()){
					refresh();
					h = wh->H[index];
					n = 0; l = h.size;
				}else {
					if(r.val()==0){
						if(h.table[k]->val() == 0){//atmic
							suc=true; h.table[k].setValue(a, v);//atmic
						}//atmic
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
			EValue r; 
			int k,l,n;
			Hashtable* h;
			bool suc;

			h = wh->H[index];
			if h.occ>h.bound {
				newTable();
				h = wh->H[index];
			}
			n=0; l=h.size; suc=false;
			do{
				k = key(a,l,n);
				r = h.table[k]; //atomic
				if (r.oldp()){
					refresh();
					h = wh->H[index];
					n=0; l=h.size;
				}else {
					if(r.val()==0 || a=r.ADR()){
						if(h.table[k] == r){//atmic
							suc=true; h.table[k].setValue(a, v);//atmic
						}//atmic
					}
					else{
						n++;
					}
				}
			}while(!(suc));
			if(r.val()==0){
				h.occ++;
			}
		}

		// ----------- HEAP methods -----------
	protected:
		Hashtable* allocate(int s, int b) {
			Hashtable* h = new Hashtable(s, b);
			return h;
		}

		void deAlloc(int h) {
			if(wh->H[h] == null) //atomic
			{
				delete wh->H[h]; //atomic
				wh->H[h]==null; //atomic
			}		//atomic
		}

		void newTable() {
			int i; // 1..2P
			bool b, bb;
			while(next[index] == 0){
				i = rand() % (2*wh->P) + 1;
				{ // ATOMIC
					b = prot[i] == 0;
					if(b) prot[i] = 1;
				}
				if(b){
					busy[i] = 1;
					bound = wh->H[index].bound - wh->H[index].dels + 2*P + 1;
					size = bound + 2*P + 1;
					H[i] = allocate(size, bound);
					next[i] = 0;
					{
						bb = next[index] == 0;
						if(bb) next[index] = 1;
					}
					if(!bb) releaseAccess(i);
				}
			}
			refresh();
		}

		void migrate() {
			int i; // 0..2P
			Hashtable* h;
			bool b;
			i = next[index];
			prot[i]++;
			if (index != wh->currInd) {
				wh->prot[i]--;
			}
			else {
				wh->busy[i]++;
				h = wh->H[i];
				if (index == currInd) {
					moveContents(wh->H[index], h);
					b = (wh->currInd == index); // ATOMIC
					if (b) wh->currInd = i; // ATOMIC
				
					if (b) {
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
			EValue v;
			int* toBeMoved;
			toBeMoved = new int[from.size];
			for(int j=0; j<from.size; ++j){
				toBeMoved[j] = j;
			}
			toBeMovedSize = from.size;
			while(currInd == index && toBeMovedSize > 0){
				i = toBeMoved[toBeMovedSize - 1];
				EValue v = from.table[i];
				if(from.table[i].done()){
					toBeMovedSize--;
				}
				else{
					{ // ATOMIC
						b = (v == from.table[i]);
						if(b) from.table[i] = v.setOld();
					}
					if(b){
						if(val(v) != null) moveElement(val(v), to);
						from.table[i].setDone();
						toBeMovedSize--;
					}
				}
			}
		}

		void moveElement(T* v, Hashtable* to) {
			int a;
			int k, m, n;
			EValue w;
			bool b;

			n = 0;
			b = false;
			a = v.ADR();
			m = to->size;

			do{
				k = key(a, m, n);
				w = to->table[k];
				if (w.val() == NULL) {
					{	// ATOMIC
						b = to->table[k].val() == NULL;
						if (b) to->table[k].setValue(a, v); 
					}
				}
				else {
					n++;
				}

			} while (!(b || a == w.ADR() || currInd != index));
			if (b) to->occ++;
		}
	};

};
}}

#endif // #ifndef CDSLIB_CONTAINER_EAWfPAD_HASHTABLE_H
