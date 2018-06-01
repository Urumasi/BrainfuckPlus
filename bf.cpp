#include <iostream>
#include "bf.hpp"

template <class T>
void deleteTree(MemoryTree<T>* tree){
	if(tree->left)
		deleteTree(tree->left);
	if(tree->right)
		deleteTree(tree->right);
	if(tree->data)
		delete tree->data;
	delete tree;
} 

template <class T>
_BFEngine<T>::_BFEngine(int bs){
	size = sizeof(T);
	ptr = 0;
	callstack.clear();
	head = new MemoryTree<T>();
	// If head has no children that we can allocate()
	if(size*8==bs)
		head->data = new T[1<<blocksize];
	cacheValid = false;
	blocksize = bs;
}

template <class T>
_BFEngine<T>::~_BFEngine(){
	deleteTree(head);
}

template <class T>
void _BFEngine<T>::process(const char *prog){
	cptr = 0;
	T o;
	unsigned char in;
	while(prog[cptr]){ // Run until \0
		char c = prog[cptr];
		switch(c){
			case '>':
				ptr++;
			break;
			case '<':
				ptr--;
			break;
			case '+':
				set(ptr, get(ptr)+1);
			break;
			case '-':
				set(ptr, get(ptr)-1);
			break;
			case '.':
				o = get(ptr);
				std::cout << *reinterpret_cast<char*>(&o);
			break;
			case ',':
				std::cin >> in;
				set(ptr, static_cast<T>(in));
			break;
			case '[':
				if(get(ptr)){
					callstack.push_back(cptr);
				}else{
					int depth = 1;
					while(depth){
						cptr++;
						c = prog[cptr];
						if(c=='[')
							depth++;
						if(c==']')
							depth--;
					}	
				}
			break;
			case ']':
				if(callstack.empty())
					throw std::logic_error("Attempt to pop empty call stack");
				if(get(ptr))
					cptr = callstack.back();
				else
					pop();//callstack.pop_back();
			break;
			case '&': // goto
				ptr = get(ptr);
			break;
			case '@': // call
				callstack.push_back(cptr);
				cptr = get(ptr);
			break;
			case '$': // ret
				cptr = pop();//callstack.back();
				//callstack.pop_back();
			break;
			case '^': // pop
				pop();//callstack.pop_back();
			break;
			case 'D': // debug
				std::cout << "\nPtr: " << static_cast<unsigned long long int>(ptr) << "\nCPtr: " << cptr << "\nCall stack size: " << callstack.size() << std::endl;
				printMemBlocks();
				std::cout << std::endl;
			break;
		}
		cptr++;
	}
}

template <class T>
T _BFEngine<T>::pop(){
	if(callstack.empty())
		throw std::logic_error("Attempt to pop empty call stack");
	T back = callstack.back();
	callstack.pop_back();
	return back;
}

template <class T>
void _BFEngine<T>::printMemBlocks()const{
	std::cout << "Allocated blocks:";
	printTree(head, static_cast<T>(0));
}

template <class T>
void printTree(MemoryTree<T> *tree, T addr){
	T newaddr = addr<<1;
	if(tree->left)
		printTree(tree->left, newaddr);
	if(tree->right)
		printTree(tree->right, ++newaddr);
	if(tree->data)
		std::cout << " #" << static_cast<unsigned long long int>(addr);
}

// Allocate new block at position bp
template <class T>
MemoryTree<T>* _BFEngine<T>::allocate(T bp){
	T mask = 1;
	int shift = size*8-blocksize-1;
	if(shift<0)
		mask = 0;
	else
		mask <<= shift;
	MemoryTree<T>* tree = head;
	for(; mask; mask>>=1){
		if(bp & mask){
			if(tree->right==nullptr)
				tree->right = new MemoryTree<T>();
			tree = tree->right;
		}else{
			if(tree->left==nullptr)
				tree->left = new MemoryTree<T>();
			tree = tree->left;
		}
	}
	tree->data = new T[1<<blocksize];
	cacheValid = true;
	cachePoint = bp;
	cache = tree;
	return tree;
}

template <class T>
T _BFEngine<T>::get(T addr){
	MemoryTree<T>* block = getBlock(addr>>blocksize);
	if(block==nullptr)
		return 0;
	if(block->data==nullptr){
		if(block->left || block->right)
			throw std::logic_error("Block is not final");
		else
			throw std::logic_error("No data in block");
	}
	T mask = 1;
	mask <<= blocksize;
	mask--; // blocksize amount of 1s
	return block->data[addr & mask];
}

template <class T>
void _BFEngine<T>::set(T addr, T data){
	MemoryTree<T>* block = getBlock(addr>>blocksize);
	if(block==nullptr)
		block = allocate(addr>>blocksize);
	T mask = 1;
	mask <<= blocksize;
	mask--; // blocksize amount of 1s
	block->data[addr & mask] = data;
}

template <class T>
MemoryTree<T>* _BFEngine<T>::getBlock(T bp){
	if(cacheValid && cachePoint==bp)
		return cache;
	T mask = 1;
	int shift = size*8-blocksize-1;
	if(shift<0)
		mask = 0;
	else
		mask <<= shift;
	MemoryTree<T>* tree = head;
	for(; mask; mask>>=1){
		if(bp & mask){
			if(tree->right==nullptr)
				return nullptr;
			tree = tree->right;
		}else{
			if(tree->left==nullptr)
				return nullptr;
			tree = tree->left;
		}
	}
	if(tree!=nullptr){
		cacheValid = true;
		cachePoint = bp;
		cache = tree;
	}
	return tree;
}

BFEngine::BFEngine(int arch, int bs){
	this->arch = arch;
	switch(arch){
		case 1:
			e8 = new _BFEngine<unsigned char>(bs);
		break;
		case 2:
			e16 = new _BFEngine<unsigned short int>(bs);
		break;
		case 4:
			e32 = new _BFEngine<unsigned long int>(bs);
		break;
		case 8:
			e64 = new _BFEngine<unsigned long long int>(bs);
		break;
		default:
			throw std::logic_error("Undefined architecture");
	}
}

BFEngine::~BFEngine(){
	switch(arch){
		case 1:
			delete e8;
		break;
		case 2:
			delete e16;
		break;
		case 4:
			delete e32;
		break;
		case 8:
			delete e64;
		break;
	}
}

void BFEngine::process(const char *c){
	switch(arch){
		case 1:
			e8->process(c);
		break;
		case 2:
			e16->process(c);
		break;
		case 4:
			e32->process(c);
		break;
		case 8:
			e64->process(c);
		break;
	}
}
