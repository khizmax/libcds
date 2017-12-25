#ifndef CDSLIB_CONTAINER_EAWfPAD_HASHTABLE_H
#define CDSLIB_CONTAINER_EAWfPAD_HASHTABLE_H

namespace cds { namespace container {

/*
	Efficient Almost wait-free Parallel Accessible Dynamic Hashtables
	Gao, Groote, Hesselink (2003)
*/

class WfHashtable
{

	struct EValue {

	private:
		int ADR;
		double value;
		bool del;
		bool old;

	public:
		void setValue(double value) {
			this->value = value;
		}

		int getADR() {
			return ADR;
		}

		void setDel() {
			this->del = true;
		}

		double val() {
			if (del == true) {
				return 0;
			}
			else {
				return value;
			}
		}

		bool oldp() {
			return old;
		}

		bool done() {
			return oldp() && val() == 0;
		}
	};

	struct Hashtable {
		int size;
		int occ;
		int dels;
		int bound;
		EValue* table;
	};

	Hashtable** H; // 1..2P
	int* busy; // 1..2P
	int* prot; // 1..2P
	Hashtable* next; // 1..2P
	int currInd; // 1..2P

	class Process {
		int index; // 1..2P

		// ----------- HASHTABLE METHODS -----------

		int key(int a, int l, int n) {
			return a % l;
			// ÇÄÅÑÜ ×ÒÎ-òî íåïîíÿòíîå Inc(a,l,n-1)
		}

		double find(int a) {
			EValue r;
			int n, l;
			Hashtable* h;

			h = H[index];
			n = 0;
			l = h->size;

			do {
				r = h->table[key(a, l, n)]; // ATOMIC
				if (r.done()) {
					refresh();
					h = H[index];
					l = h->size;
				}
				else {
					n++;
				}

			} while (r.val() != 0 && a != r.getADR());

			return r.val();
		}

		bool del(int a)
		{
			EValue r;
			int k, l, n;
			Hashtable* h;
			bool suc;

			h = H[index];
			suc = false;
			l = h->size;
			n = 0;

			do {
				k = key(a, l, n);
				r = h->table[k]; // ATOMIC
				if (r.oldp()) {
					refresh();
					h = H[index];
					l = h->size;
					n = 0;
				}
				else if (a == r.getADR()) {
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

		bool insert(double v) {
		EValue r; int k,l,n;Hashtable* h;
		bool suc;
		int a=v.getADR;//??? 
		h=H[index];
		if h.occ>h.bound {
			newTable();
			h=H[index];
		}
		n=0;l=h.size;suc=false;
		do{
			k=key(a,l,n);
			r=h.table[k];//atomic
			if (r.oldp()){
				refresh();
				h=H[index];
				n=0;l=h.size;
			}else {
				if(r.val()==0){
					if(h.table[k]->val()==0){//atmic
					suc=true;h.table[k]=v;//atmic
					}//atmic
				}
				else{
				n++;
				}
			}
		}while(!(suc || a != r.getADR()));
		if (suc) {
				h->occ++;
			}
			return suc;
		}

		void assign(double v) {
		EValue r; int k,l,n;Hashtable* h;
		bool suc;
		int a=v.getADR;//???
		h=H[index];
		if h.occ>h.bound {
			newTable();
			h=H[index];
		}
		n=0;l=h.size;suc=false;
		do{
			k=key(a,l,n);
			r=h.table[k];//atomic
			if (r.oldp()){
				refresh();
				h=H[index];
				n=0;l=h.size;
			}else {
				if(r.val()==0 || a=r.getADR()){
					if(h.table[k]==r){//atmic
					suc=true;h.table[k]=v;//atmic
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

		Hashtable* allocate(int s, int b) {
			Hashtable* h=(Hashtable*)void *malloc(sizeof(Hashtable));//atomic
			h->table=(EValue*) void *malloc(s*sizeof(EValue));//atomic
			h->size=s;//atomic
			h->bound=b;//atomic
			h->occ=0;//atomic
			h->dels=0;//atomic
		}

		void deAlloc(int h) {
			if(H[h]==null)//atomic
			{
			free(H[h]);//atomic
			H[h]==null;//atomic
			}		//atomic
		}

		void getAccess() {
			while (true) {
				index = currInd;
				prot[index] ++;
				if (index == currInd) {
					busy[index]++;
					if (index == currInd) {
						return;
					}
					else {
						releaseAccess(index);
					}
				}
				else {
					prot[index]--;
				}
			}
		}

		void releaseAccess(int i) {
			Hashtable* h;
			h = H[i];
			busy[i]--;
			if (h != 0 && busy[i] == 0) {
				if (H[i] = h) { // ATOMIC
					H[i] = 0; // ATOMIC
					deAlloc(i);//change h
				}
			}
			prot[i]--;
		}

		void newTable() {
			int i;
			bool b, bb;

			// TODO implement
		}

		void migrate() {
			int i; // 0..2P
			Hashtable* h;
			bool b;
			i = next[index];
			prot[i]++;
			if (index != currInd) {
				prot[i]--;
			}
			else {
				busy[i]++;
				h = H[i];
				if (index == currInd) {
					moveContents(H[index], h);
					b = (currInd == index); // ATOMIC
					if (b) currInd = i; // ATOMIC
				
					if (b) {
						busy[index]--;
						prot[index]--;
					}
				}
				releaseAccess(i);
			}
		}

		void refresh() {
			if (index != currInd) {
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

			// TODO implement
		}

		void moveElement(double v, Hashtable* to) {
			int a;
			int k, m, n;
			EValue w;
			bool b;

			n = 0;
			b = false;
			a = w.getADR();
			m = to->size;

			do{
				k = key(a, m, n);
				w = to->table[k];
				if (w.val() == 0) {
					{	// ATOMIC
						b = to->table[k].val() == 0;
						if (b) to->table[k].setValue(v); 
					}
				}
				else {
					n++;
				}

			} while (!(b || a == w.getADR() || currInd != index));
			if (b) to->occ++;
		}
	};

}
}}