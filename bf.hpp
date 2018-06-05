#ifndef __BF__
#define __BF__

#include <vector>

template <class T>
struct MemoryTree{
	T *data = nullptr;
	MemoryTree *left = nullptr;
	MemoryTree *right = nullptr;
};

template <class T>
void deleteTree(MemoryTree<T>*);

template <class T>
void printTree(MemoryTree<T>*, T);

template <class T>
class _BFEngine{
public:
	_BFEngine(int);
	~_BFEngine();
	void process(const char*, int);
private:
    T pop();

	MemoryTree<T>* allocate(T);
	T get(T);
	void set(T, T);
	MemoryTree<T>* getBlock(T);
	void printMemBlocks()const;

	MemoryTree<T>* head;
	bool cacheValid;
	T cachePoint;
	MemoryTree<T>* cache;
	T ptr;
	int size;
	int blocksize;
	unsigned long long int cptr;
	std::vector<T> callstack;
};

//Wrapper class
class BFEngine{
public:
	BFEngine(int, int);
	~BFEngine();
	void process(const char*, int);
private:
	_BFEngine<unsigned char> *e8;
	_BFEngine<unsigned short int> *e16;
	_BFEngine<unsigned long int> *e32;
	_BFEngine<unsigned long long int> *e64;
	int arch;
};

#endif
