struct EValue {
	int ADR;
	double value;
	bool del;
	bool old;

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

	int key(int a, int l, int n) {
		return a % l;
		// ÇÄÅÑÜ ×ÒÎ-òî íåïîíÿòíîå Inc(a,l,n-1)
	}


	// HASHTABLE METHODS

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

		} while (r.val() != 0 && a != r.ADR);

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
			else if (a == r.ADR) {
				if (r == h->table[k]) { // ATOMIC
					suc = true; // ATOMIC
					h->table[k].del = true; // ATOMIC
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

	}

	void assign(double v) {

	}

	// HEAP methods

	int allocate(int s, int b) {

	}

	void deAlloc(int h) {
		// assert(Heap(h) != null);
		// Heap(h) = null;
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
				deAlloc(h);
			}
		}
		prot[i]--;
	}

	void newTable() {

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
				if (b) { // ATOMIC
						 // ATOMIC
				} // ATOMIC
				
				if (b) {

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

	}

	void moveElement(double v, Hashtable* to) {

	}


};

