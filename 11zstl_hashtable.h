#ifndef _ZSTL_HASHTABLE_
#define _ZSTL_HASHTABLE_

#include <iostream>
#include "01zstl_vector.h"
#include "00zstl_algobase.h"
#include <cstddef>		//ptrdiff_t
#include "00zstl_pair.h"

__MYSTL_NAMESPACE_BEGIN_
	//基本配置---------------------------------------------------------------------------
	static const int __num_primes = 28;		//用于维护 hash table 大小的质数个数
	// 质数数组(逐渐呈现大约两倍的关系)
	static const unsigned long __prime_list[__num_primes] = {
		53,         97,           193,         389,       769,
		1543,       3079,         6151,        12289,     24593,
		49157,      98317,        196613,      393241,    786433,
		1572869,    3145739,      6291469,     12582917,  25165843,
		50331653,   100663319,    201326611,   402653189, 805306457,
		1610612741, 3221225473ul, 4294967291ul
	};
	//以下找出28个质数中，最接近并大于 n 的那个质数
	inline unsigned long __next_prime(unsigned long n) {
		const unsigned long * first = __prime_list;
		const unsigned long * last = __prime_list + __num_primes;
		const unsigned long * pos = lower_bound(first, last, n);

		return pos == last ? *(last - 1) : *pos;
	}

	//计算元素位置的函数，对于字符串设计了转换函数，其余忠实返回原值
	template <class Key> struct hash { };
	inline size_t __hash_string(const char* s) {
		unsigned long h = 0;
		for (; *s; ++s) {
			h = 5 * h + *s;
		}
		return size_t(h);
	}

	__STL_TEMPLATE_NULL struct hash<char*>
	{
		size_t operator()(const char* s) const { return __hash_string(s); }
	};

	__STL_TEMPLATE_NULL struct hash<const char*>
	{
		size_t operator()(const char* s) const { return __hash_string(s); }
	};

	__STL_TEMPLATE_NULL struct hash<char> {
		size_t operator()(char x) const { return x; }
	};
	__STL_TEMPLATE_NULL struct hash<unsigned char> {
		size_t operator()(unsigned char x) const { return x; }
	};
	__STL_TEMPLATE_NULL struct hash<signed char> {
		size_t operator()(unsigned char x) const { return x; }
	};
	__STL_TEMPLATE_NULL struct hash<short> {
		size_t operator()(short x) const { return x; }
	};
	__STL_TEMPLATE_NULL struct hash<unsigned short> {
		size_t operator()(unsigned short x) const { return x; }
	};
	__STL_TEMPLATE_NULL struct hash<int> {
		size_t operator()(int x) const { return x; }
	};
	__STL_TEMPLATE_NULL struct hash<unsigned int> {
		size_t operator()(unsigned int x) const { return x; }
	};
	__STL_TEMPLATE_NULL struct hash<long> {
		size_t operator()(long x) const { return x; }
	};
	__STL_TEMPLATE_NULL struct hash<unsigned long> {
		size_t operator()(unsigned long x) const { return x; }
	};
	//-----------------------------------------------------------------------------------

	//节点
	template <class Value>
	struct __hashtable_node {
		__hashtable_node* next;
		Value val;
	};

	//迭代器声明
	template<class Value, class Key, class HashFunc, class ExtractKey, class EqualKey, class Alloc = alloc>
	struct __hashtable_iterator;
	template<class Value, class Key, class HashFunc, class ExtractKey, class EqualKey, class Alloc = alloc>
	struct __hashtable_const_iterator;

	//数据结构声明
	template<class Value, class Key, class HashFunc, class ExtractKey, class EqualKey, class Alloc = alloc>
	class hashtable;

	//迭代器
	template <class Value, class Key, class HashFcn, class ExtractKey, class EqualKey, class Alloc>
	struct __hashtable_iterator {
		typedef hashtable<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>						hashtable;
		typedef __hashtable_iterator<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>			iterator;
		typedef __hashtable_const_iterator<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>	const_iterator;
		typedef __hashtable_node<Value> node;

		typedef forward_iterator_tag	iterator_category;
		typedef Value					value_type;
		typedef ptrdiff_t				difference_type;
		typedef size_t					size_type;
		typedef Value&					reference;
		typedef Value*					pointer;

		node* cur;			//迭代器目前所指的节点
		hashtable* ht;		//保持对容器的连结关系(因为可能需要从 bucket 跳到 bucket )

		__hashtable_iterator(node* n, hashtable* tab) : cur(n), ht(tab) { }
		__hashtable_iterator() { }
		reference operator*() const { return cur->val; }
		pointer operator->() const { return &(operator*()); }
		iterator& operator++();
		iterator operator++(int);
		bool operator==(const iterator& it) const { return cur == it.cur; }
		bool operator!=(const iterator& it) const { return cur != it.cur; }
	};

	template <class V, class K, class HF, class ExK, class EqK, class A>
	__hashtable_iterator<V, K, HF, ExK, EqK, A>&
		__hashtable_iterator<V, K, HF, ExK, EqK, A>::operator++() {
		const node* old = cur;
		cur = cur->next;			//如果存在，就是它，否则进入以下 if 流程
		if (!cur) {
			//根据元素值，定位下一个 bucket ，其起头处就是目的地
			size_type bucket = ht->bkt_num(old->val);
			while (!cur && (++bucket < ht->buckets.size())) {
				cur = ht->buckets[bucket];
			}
		}
		return *this;
	}

	template <class V, class K, class HF, class ExK, class EqK, class A>
	inline __hashtable_iterator<V, K, HF, ExK, EqK, A>
		__hashtable_iterator<V, K, HF, ExK, EqK, A>::operator++(int) {
		iterator tmp = *this;
		++*this;		//调用 operator++()
		return tmp;
	}

	template <class Value, class Key, class HashFcn, class ExtractKey, class EqualKey, class Alloc>
	struct __hashtable_const_iterator {
		typedef hashtable<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>						hashtable;
		typedef __hashtable_iterator<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>			iterator;
		typedef __hashtable_const_iterator<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>	const_iterator;
		typedef __hashtable_node<Value> node;

		typedef forward_iterator_tag	iterator_category;
		typedef Value					value_type;
		typedef ptrdiff_t				difference_type;
		typedef size_t					size_type;
		typedef const Value&			reference;
		typedef const Value*			pointer;

		const node* cur;			//迭代器目前所指的节点
		const hashtable* ht;		//保持对容器的连结关系(因为可能需要从 bucket 跳到 bucket )

		__hashtable_const_iterator(const node* n, const hashtable* tab) : cur(n), ht(tab) { }
		__hashtable_const_iterator() { }
		__hashtable_const_iterator(const iterator& it) : cur(it.cur), ht(it.ht) { }
		reference operator*() const { return cur->val; }
		pointer operator->() const { return &(operator*()); }
		const_iterator& operator++();
		const_iterator operator++(int);
		bool operator==(const iterator& it) const { return cur == it.cur; }
		bool operator!=(const iterator& it) const { return cur != it.cur; }
	};

	template <class V, class K, class HF, class ExK, class EqK, class A>
	__hashtable_const_iterator<V, K, HF, ExK, EqK, A>&
	__hashtable_const_iterator<V, K, HF, ExK, EqK, A>::operator++() {
		const node* old = cur;
		cur = cur->next;			//如果存在，就是它，否则进入以下 if 流程
		if (!cur) {
			//根据元素值，定位下一个 bucket ，其起头处就是目的地
			size_type bucket = ht->bkt_num(old->val);
			while (!cur && ++bucket < ht->buckets.size()) {
				cur = ht->buckets[bucket];
			}
			return *this;
		}
	}

	template <class V, class K, class HF, class ExK, class EqK, class A>
	inline __hashtable_const_iterator<V, K, HF, ExK, EqK, A>
		__hashtable_const_iterator<V, K, HF, ExK, EqK, A>::operator++(int) {
		iterator tmp = *this;
		++*this;		//调用 operator++()
		return tmp;
	}

	template <class V, class K, class HF, class ExK, class EqK, class A>
	bool operator==(const hashtable<V, K, HF, ExK, EqK, A> & ht1,
					const hashtable<V, K, HF, ExK, EqK, A> & ht2) {
		typedef typename hashtable<V, K, HF, ExK, EqK, A>::node node;
		if (ht1.buckets.size() != ht2.buckets.size()) {
			return false;
		}
		for (int n = 0; n < ht1.buckets.size(); ++n) {
			node* cur1 = ht1.buckets[n];
			node* cur2 = ht2.buckets[n];
			for (; cur1 && cur2 && cur1->val == cur2->val;
				cur1 = cur1->next, cur2 = cur2->next) {	}
			if (cur1 || cur2) return false;
		}

		return true;
	}

	template <class V, class K, class HF, class ExK, class EqK, class A>
	bool operator!=(const hashtable<V, K, HF, ExK, EqK, A> & ht1,
		const hashtable<V, K, HF, ExK, EqK, A> & ht2) {
		return !(ht1 == ht2);
	}

	/*数据结构
		Value			节点的实值型别
		Key：			节点的键值型别
		HashFcn：		hash function 的函数型别
		ExtractKey：	从节点中取出键值的方法
		Alloc：			空间配置器，缺省使用 alloc (在声明时已经给予了 Alloc 默认值 alloc)
	*/
	template <class Value, class Key, class HashFcn, class ExtractKey, class EqualKey, class Alloc>
	class hashtable {
	public:
		typedef HashFcn		hasher;			//为 template 型别参数重新定义一个名称
		typedef EqualKey	key_equal;		//为 template 型别参数重新定义一个名称
		typedef size_t			size_type;
		typedef Value			value_type;
		typedef Key				key_type;
		typedef ptrdiff_t		difference_type;
		typedef Value*			pointer;
		typedef const Value*	const_pointer;
		typedef Value&			reference;
		typedef const Value		const_reference;
		typedef __hashtable_iterator<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>			iterator;
		typedef __hashtable_const_iterator<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>	const_iterator;
		typedef __hashtable_node<Value>		node;
		typedef simple_alloc<node, Alloc>	node_allocator;

		friend struct __hashtable_iterator<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>;
		friend struct __hashtable_const_iterator<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>;

		hasher hash_funct() const { return hash; }
		key_equal key_eq() const { return equals; }
		vector<node*, Alloc> buckets;		//以 vector 完成

	private:
		//以下三者都是 function objects
		hasher		hash;
		key_equal	equals;
		ExtractKey	get_key;

		size_type num_elements;

	protected:
		//节点配置
		node* new_node(const value_type& obj) {
			node* n = node_allocator::allocate();
			n->next = nullptr;
			try {
				construct(&n->val, obj);
				return n;
			}
			catch (...) {
				node_allocator::deallocate(n);
				throw;
			}
		}
		//节点释放
		void delete_node(node* n) {
			destroy(&n->val);
			node_allocator::deallocate(n);
		}

	public:
		hashtable(size_type n, const HashFcn& hf, const EqualKey& eql): hash(hf), equals(eql), get_key(ExtractKey()), num_elements(0) {
			initialize_buckets(n);
		}
		void initialize_buckets(size_type n) {
			const size_type n_buckets = next_size(n);
			buckets.reserve(n_buckets);
			buckets.insert(buckets.end(), n_buckets, (node*)0);
			num_elements = 0;
		}
		hashtable& operator= (const hashtable& ht) {
			if (&ht != this) {
				clear();
				hash = ht.hash;
				equals = ht.equals;
				get_key = ht.get_key;
				copy_from(ht);
			}
			return *this;
		}
		~hashtable() { clear(); }

		size_type size() const { return num_elements; }
		size_type max_size() const { return size_type(-1); }
		bool empty() const { return size() == 0; }

		void swap(hashtable& ht) {
			std::swap(hash, ht.hash);
			std::swap(equals, ht.equals);
			std::swap(get_key, ht.get_key);
			buckets.swap(ht.buckets);
			std::swap(num_elements, ht.num_elements);
		}

		iterator begin() {
			for (size_type n = 0; n < buckets.size(); ++n) {
				if (buckets[n]) return iterator(buckets[n], this); 
			}
			return end();
		}
		iterator end() { return iterator(0, this); }

		const_iterator begin() const {
			for (size_type n = 0; n < buckets.size(); ++n) {
				if (buckets[n]) return const_iterator(buckets[n], this);
			}
			return end();
		}
		const_iterator end() const { return const_iterator(0, this); }

		// 返回最接近 n 并大于 n 的质数
		size_type next_size(size_type n) const { return __next_prime(n); }
		// bucket 个数即 buckets vector 的大小
		size_type bucket_count() const { return buckets.size(); }

		// buckets 总数
		size_type max_bucket_count() const {
			return __prime_list[__num_primes - 1];	//其值将为 4294967291
		}

		int elems_in_bucket(size_type bucket) {
			int result = 0;
			for (node* cur = buckets[bucket]; cur; cur = cur->next)
				++result;
			return result;
		}

		//插入元素，不允许重复
		pair<iterator, bool> insert_unique(const value_type& obj) {
			resize(num_elements + 1);			//判断是否需要重建表格，如需要就扩充
			return insert_unique_noresize(obj);
		}
		//插入元素，允许重复
		iterator insert_equal(const value_type& obj) {
			resize(num_elements + 1);			//判断是否需要重建表格，如需要就扩充
			return insert_equal_noresize(obj);
		}

		//以下函数判断是否需要重建表格，如果不需要，立刻返回，如果需要，就处理
		void resize(size_type num_elements_hint);
		pair<iterator, bool> insert_unique_noresize(const value_type& obj);
		iterator insert_equal_noresize(const value_type& obj);

		template<class InputIterator>
		void insert_equal(InputIterator f, InputIterator l) {
			insert_equal(f, l, iterator_category(f));
		}
		template<class InputIterator>
		void insert_unique(InputIterator f, InputIterator l) {
			insert_unique(f, l, iterator_category(f));
		}
		
		template <class InputIterator>
		void insert_equal(InputIterator f, InputIterator l, input_iterator_tag) {
			for (; f != l; ++f) {
				insert_equal(*f);
			}
		}
		template <class InputIterator>
		void insert_unique(InputIterator f, InputIterator l, input_iterator_tag) {
			for (; f != l; ++f) {
				insert_unique(*f);
			}
		}

		template <class ForwardIterator>
		void insert_equal(ForwardIterator f, ForwardIterator l, forward_iterator_tag) {
			size_type n = 0;
			distance(f, l, n);
			resize(num_elements + n);
			for (; n > 0; --n, ++f) {
				insert_equal_noresize(*f);
			}
		}
		template <class ForwardIterator>
		void insert_unique(ForwardIterator f, ForwardIterator l, forward_iterator_tag) {
			size_type n = 0;
			distance(f, l, n);
			resize(num_elements + n);
			for (; n > 0; --n, ++f) {
				insert_unique_noresize(*f);
			}
		}

		void insert_equal(const value_type* f, const value_type* l) {
			size_type n = l - f;
			resize(num_elements + n);
			for (; n > 0;--n, ++f) {
				insert_equal_noresize(*f);
			}
		}
		void insert_unique(const value_type* f, const value_type* l) {
			size_type n = l - f;
			resize(num_elements + n);
			for (; n > 0;--n, ++f) {
				insert_unique_noresize(*f);
			}
		}

		void insert_equal(const_iterator f, const_iterator l) {
			size_type n = 0;
			distance(f, l, n);
			resize(num_elements + n);
			for (; n > 0; --n, ++f) {
				insert_equal_noresize(*f);
			}
		}
		void insert_unique(const_iterator f, const_iterator l) {
			size_type n = 0;
			distance(f, l, n);
			resize(num_elements + n);
			for (; n > 0; --n, ++f) {
				insert_unique_noresize(*f);
			}
		}

		//判断某个元素落脚于哪一个 bucket 之内
		//版本1：接收实值( value )和 buckets 个数
		size_type bkt_num(const value_type& obj, size_t n) const {
			return bkt_num_key(get_key(obj), n);		//调用版本4
		}
		//版本2：只接受实值( value )
		size_type bkt_num(const value_type& obj) const {
			return bkt_num_key(get_key(obj));			//调用版本3
		}
		//版本3：只接受键值( key )
		size_type bkt_num_key(const key_type& key) const {
			return bkt_num_key(key, buckets.size());			//调用版本4
		}
		//版本4：接受键值和 buckets 个数
		size_type bkt_num_key(const key_type& key, size_t n) const {
			return hash(key) % n;
		}

		pair<iterator, iterator>
			equal_range(const key_type& key);

		pair<const_iterator, const_iterator>
			equal_range(const key_type& key) const;

		size_type erase(const key_type& key);
		void erase(const iterator& it);
		void erase(iterator first, iterator last);

		void erase(const const_iterator& it);
		void erase(const_iterator first, const_iterator last);

		void clear();

		void erase_bucket(const size_type n, node* first, node* last);
		void erase_bucket(const size_type n, node* last);

		void copy_from(const hashtable& ht);

		value_type find_or_insert(const value_type& obj);

		iterator find(const key_type& key) {
			size_type n = bkt_num_key(key);			//先寻找落在哪一个 bucket 内
			node* first;
			//以下，从 bucket list 的头开始，一一比对每个元素的键值，比对成功就跳出
			for (first = buckets[n];
				first && !equals(get_key(first->val), key);
				first = first->next) {	}
			return iterator(first, this);
		}

		const_iterator find(const key_type& key) const {
			size_type n = bkt_num_key(key);			//先寻找落在哪一个 bucket 内
			node* first;
			//以下，从 bucket list 的头开始，一一比对每个元素的键值，比对成功就跳出
			for (first = buckets[n]; 
				first && !equals(get_key(first->val), key); 
				first = first->next) { }
			return const_iterator(first, this);
		}

		size_type count(const key_type& key) {
			const size_type n = bkt_num_key(key);			//先寻找落在哪一个 bucket 内
			const node* cur;
			size_type result = 0;

			//以下，从 bucket list 的头开始，一一比对每一个元素的键值，比对成功就累加1
			for (cur = buckets[n]; cur; cur = cur->next) {
				if (equals(get_key(cur->val), key)) ++result;
			}
			return result;
		}

		template<class V1, class Ky, class HF, class Ex, class Eq, class A>
		friend bool operator==(const hashtable<V1, Ky, HF, Ex, Eq, A>&,
			const hashtable<V1, Ky, HF, Ex, Eq, A>&);
	};

	template <class V, class K, class HF, class Ex, class Eq, class A>
	void hashtable<V, K, HF, Ex, Eq, A>::resize(size_type num_elements_hint) {
		//以下，“表格重建与否”的判断原则：拿元素个数(把新增元素计入后)和
		// bucket vector 的大小来比，如果前者大于后者，就重建表格
		//由此可判知，每个 bucket ( list ) 的最大容量和 bucket vector 的大小相同
		const size_type old_n = buckets.size();
		if (num_elements_hint > old_n) {			//确定真的需要重新配置
			const size_type n = next_size(num_elements_hint);		//找出下一个质数
			if (n > old_n) {
				vector<node*, A> tmp(n, (node*)0);				//设立新的 buckets
				try {
					//以下处理每一个旧的 bucket
					for (size_type bucket = 0; bucket < old_n; ++bucket) {
						node* first = buckets[bucket];			//指向节点所对应的串行的起始节点
						//以下处理每一个旧 bucket 所含(串行)的每一个节点
						while (first) {							//串行还没结束时
							//以下找出节点落在哪一个新的 bucket 内
							size_type new_bucket = bkt_num(first->val, n);
							
							//令旧 bucket 指向其所对应之串行的下一个节点(以便迭代处理)
							buckets[bucket] = first->next;
							//将当前节点插入到新 bucket 内，成为其对应串行的第一个节点
							first->next = tmp[new_bucket];
							tmp[new_bucket] = first;
							//回到旧 bucket 所指的待处理串行，准备下一个节点
							first = buckets[bucket];
						}
					}
					buckets.swap(tmp);		// vector::swap 新旧两个 buckets 对调
					//对调两方如果大小不同，大的会变小，小的会变大
					//离开时释放 local tmp 的内存
				}
				catch (...) {
					for (size_type bucket = 0; bucket < tmp.size(); ++bucket) {
						while (tmp[bucket]) {
							node* next = tmp[bucket]->next;
							delete_node(tmp[bucket]);
							tmp[bucket] = next;
						}
					}
					throw;
				}
			}
		}
	}

	template<class V, class K, class HF, class Ex, class Eq, class A>
	pair<typename hashtable<V, K, HF, Ex, Eq, A>::iterator, bool>
		hashtable<V, K, HF, Ex, Eq, A>::insert_unique_noresize(const value_type& obj) {
		const size_type n = bkt_num(obj);		//决定 obj 应位于 #n bucket
		node* first = buckets[n];				//令 first 指向 bucket 对应的串行头部

		//如果 buckets[n] 已被占用，此时 first 将不为0， 于是进入以下循环
		//走过 bucket 所对应的整个链表
		for (node* cur = first; cur; cur = cur->next) {
			if (equals(get_key(cur->val), get_key(obj))) {
				//如果发现与链表中的某键值相同，就不插入，立刻返回
				return pair<iterator, bool>(iterator(cur, this), false);
			}
		}
		//离开以上循环(或根本未进入循环)时，first 指向 bucket 所指链表的头部节点
		node* tmp = new_node(obj);		//产生新节点
		tmp->next = first;
		buckets[n] = tmp;				//令新节点称为链表的第一个节点
		++num_elements;					//节点个数累加1
		return pair<iterator, bool>(iterator(tmp, this), true);
	}

	template <class V, class K, class HF, class Ex, class Eq, class A>
	typename hashtable<V, K, HF, Ex, Eq, A>::iterator
		hashtable<V, K, HF, Ex, Eq, A>::insert_equal_noresize(const value_type& obj) {
		const size_type n = bkt_num(obj);		//决定 obj 应位于 #n bucket
		node* first = buckets[n];				//令 first 指向 bucket 对应的链表头部

		//如果 buckets[n] 已被占用，此时 first 将不为0，于是进入以下循环
		//走过 bucket 对应的整个链表
		for (node* cur = first; cur; cur = cur->next) {
			if (equals(get_key(cur->val), get_key(obj))) {
				//如果发现与链表中的某键值相同，就马上插入，然后返回
				node* tmp = new_node(obj);		//产生新节点
				tmp->next = cur->next;			//将新节点插入于目前位置
				cur->next = tmp;
				++num_elements;					//节点个数累加1
				return iterator(tmp, this);		//返回一个迭代器，指向新增节点
			}
		}
		//进行至此，表示没有发现重复的键值
		node* tmp = new_node(obj);		//产生新节点
		tmp->next = first;				//将新节点插入于链表头部
		buckets[n] = tmp;
		++num_elements;					//节点个数累加1
		return iterator(tmp, this);		//返回一个迭代器，指向新增节点
	}
	template <class V, class K, class HF, class Ex, class Eq, class A>
	typename hashtable<V, K, HF, Ex, Eq, A>::value_type
		hashtable<V, K, HF, Ex, Eq, A>::find_or_insert(const value_type& obj)
	{
		resize(num_elements + 1);

		size_type n = bkt_num(obj);
		node* first = buckets[n];

		for (node* cur = first; cur; cur = cur->next)
			if (equals(get_key(cur->val), get_key(obj)))
				return cur->val;

		node* tmp = new_node(obj);
		tmp->next = first;
		buckets[n] = tmp;
		++num_elements;
		return tmp->val;
	}

	template <class V, class K, class HF, class Ex, class Eq, class A>
	void hashtable<V, K, HF, Ex, Eq, A>::clear() {
		//针对每一个 bucket
		for (size_type i = 0; i < buckets.size(); ++i) {
			node* cur = buckets[i];
			//将 bucket list 中的每一个节点删除掉
			while (cur) {
				node* next = cur->next;
				delete_node(cur);
				cur = next;
			}
			buckets[i] = nullptr;		//令 bucket 内容为 nullptr 指针
		}
		num_elements = 0;				//令总节点个数为0

		//注意：buckets vector 并未释放掉空间，仍保有原来大小
	}

	template<class V, class K, class HF, class Ex, class Eq, class A>
	void hashtable<V, K, HF, Ex, Eq, A>::copy_from(const hashtable& ht) {
		//先清除己方的 buckets vector，这操作是调用 vector::clear，造成所有元素为0
		buckets.clear();
		//为己方的 buckets vector 保留空间，使与对方相同
		//如果己方空间大于对方，就不动，如果己方空间小于对方，就增大
		buckets.reserve(ht.buckets.size());
		//从己方的 buckets vector 尾端开始，插入 n 个元素，其值为 nullptr 指针
		//注意，此时 buckets vector 为空，所以所谓尾端，就是起头处
		buckets.insert(buckets.end(), ht.buckets.size(), (node*)0);
		try {
			//针对 buckets vector
			for (size_type i = 0; i < ht.buckets.size(); ++i) {
				//复制 vector 的每一个元素(是个指针，指向 hashtable 节点)
				if (const node* cur = ht.buckets[i]) {
					node* copy = new_node(cur->val);
					buckets[i] = copy;

					//针对同一个 bucket list，复制每一个节点
					for (node* next = cur->next; next; cur = next, next = cur->next) {
						copy->next = new_node(next->val);
						copy = copy->next;
					}
				}
			}
			num_elements = ht.num_elements;		//重新登录节点个数( hashtable 的大小)
		}
		catch (...) {
			clear();
			throw;
		}
	}

	template<class Val, class Key, class HF, class Ex, class Eq, class All>
	typename hashtable<Val, Key, HF, Ex, Eq, All>::size_type
		hashtable < Val, Key, HF, Ex, Eq, All>::erase(const key_type& key) {
		const size_type n = bkt_num_key(key);
		node* first = buckets[n];
		size_type erased = 0;

		if (first) {
			node* cur = first;
			node* next = cur->next;
			while (next) {
				if (equals(get_key(next->val), key)) {
					cur->next = next->next;
					delete_node(next);
					next = cur->next;
					++erased;
					--num_elements;
				}
				else {
					cur = next;
					next = cur->next;
				}
			}
			if (equals(get_key(first->val), key)) {
				buckets[n] = first->next;
				delete_node(first);
				++erased;
				--num_elements;
			}
		}

		return erased;
	}

	template<class Val, class Key, class HF, class Ex, class Eq, class All>
	void hashtable < Val, Key, HF, Ex, Eq, All>::erase(const iterator& it) {
		node* p = it.cur;

		if (p) {
			const size_type n = bkt_num(p->val);
			node* cur = buckets[n];

			if (cur == p) {
				buckets[n] = cur->next;
				delete_node(cur);
				--num_elements;
			}
			else {
				node* next = cur->next;
				while (next) {
					if (next == p) {
						cur->next = next->next;
						delete_node(next);
						--num_elements;
						break;
					}
					else {
						cur = next;
						next - cur->next;
					}
				}
			}
		}
	}

	template<class Val, class Key, class HF, class Ex, class Eq, class All>
	void hashtable < Val, Key, HF, Ex, Eq, All>::erase(iterator first, iterator last) {
		size_type f_bucket = first.cur ? bkt_num(first.cur->val) : buckets.size();
		size_type l_bucket = last.cur ? bkt_num(last.cur->val) : buckets.size();

		if (first.cur == last.cur) { return; }
		else if (f_bucket == l_bucket) {
			erase_bucket(f_bucket, first.cur, last.cur);
		}
		else {
			erase_bucket(f_bucket, first.cur, 0);
			for (size_type n = f_bucket + 1; n < l_bucket; ++n) {
				erase_bucket(n, 0);
			}
			if (l_bucket != buckets.size()) {
				erase_bucket(l_bucket, last.cur);
			}
		}
	}

	template<class Val, class Key, class HF, class Ex, class Eq, class All>
	pair<typename hashtable<Val, Key, HF, Ex, Eq, All>::iterator,
		typename hashtable<Val, Key, HF, Ex, Eq, All>::iterator>
		hashtable < Val, Key, HF, Ex, Eq, All>::equal_range(const key_type& key) {
		typedef pair<iterator, iterator> _p;
		const size_type _n = bkt_num_key(key);

		for (node* first = buckets[_n]; first; first = first->next) {
			if (equals(get_key(first->val), key)) {
				for (node* cur = first->next; cur; cur = cur->next) {
					if (!equals(get_key(cur->val), key)) {
						return _p(iterator(first, this), iterator(cur, this));
					}
				}
				for (size_type m = _n + 1; m < buckets.size(); ++m) {
					if (buckets[m]) {
						return _p(iterator(first, this), iterator(buckets[m], this));
					}
				}
				return _p(iterator(first, this), end());
			}
		}
		return _p(end(), end());
	}

	template<class Val, class Key, class HF, class Ex, class Eq, class All>
	pair<typename hashtable<Val, Key, HF, Ex, Eq, All>::const_iterator,
		typename hashtable<Val, Key, HF, Ex, Eq, All>::const_iterator>
		hashtable < Val, Key, HF, Ex, Eq, All>::equal_range(const key_type& key) const {
		typedef pair<const_iterator, const_iterator> _p;
		const size_type _n = bkt_num_key(key);

		for (const node* first = buckets[_n]; first; first = first->next) {
			if (equals(get_key(first->val), key)) {
				for (const node* cur = first->next; cur; cur = cur->next) {
					if (!equals(get_key(cur->val), key)) {
						return _p(const_iterator(first, this), const_iterator(cur, this));
					}
				}
				for (size_type m = _n + 1; m < buckets.size(); ++m) {
					if (buckets[m]) {
						return _p(const_iterator(first, this), const_iterator(buckets[m], this));
					}
				}
				return _p(const_iterator(first, this), end());
			}
		}
		return _p(end(), end());
	}

	template<class Val, class Key, class HF, class Ex, class Eq, class All>
	inline void hashtable < Val, Key, HF, Ex, Eq, All>::erase(const_iterator first, const_iterator last) {
		erase(iterator(const_cast<node*>(first.cur), const_cast<hashtable*>(first.ht)),
			iterator(const_cast<node*>(last.cur), const_cast<hashtable*>(last.ht)));
	}

	template<class Val, class Key, class HF, class Ex, class Eq, class All>
	inline void hashtable < Val, Key, HF, Ex, Eq, All>::erase(const const_iterator& it) {
		erase(iterator(const_cast<node*>(it.cur), const_cast<hashtable*>(it.ht)));
	}

	template<class Val, class Key, class HF, class Ex, class Eq, class All>
	void hashtable < Val, Key, HF, Ex, Eq, All>::erase_bucket(const size_type n, node* first, node* last) {
		node* cur = buckets[n];
		if (cur == first) {
			erase_bucket(n, last);
		}
		else {
			node* next;
			for (next = cur->next; next != first; cur = next, next = cur->next) { }
			while (next != last) {
				cur->next = next->next;
				delete_node(next);
				next = cur->next;
				--num_elements;
			}
		}
	}

	template<class Val, class Key, class HF, class Ex, class Eq, class All>
	void hashtable < Val, Key, HF, Ex, Eq, All>::erase_bucket(const size_type n, node* last) {
		node* cur = buckets[n];
		while (cur != last) {
			node* next = cur->next;
			delete_node(cur);
			cur = next;
			buckets[n] = cur;
			--num_elements;
		}
	}

	template <class V, class K, class HF, class Ex, class Eq, class A>
		inline forward_iterator_tag
		iterator_category(const __hashtable_iterator<V, K, HF, Ex, Eq, A>&) {
		return forward_iterator_tag();
	}

	template <class V, class K, class HF, class Ex, class Eq, class A>
		inline V*
		value_type(const __hashtable_iterator<V, K, HF, Ex, Eq, A>&) {
		return (V*)0;
	}

	template <class V, class K, class HF, class Ex, class Eq, class A>
		inline typename hashtable<V, K, HF, Ex, Eq, A>::difference_type*
		distance_type(const  __hashtable_iterator<V, K, HF, Ex, Eq, A>&) {
		return (typename hashtable<V, K, HF, Ex, Eq, A>::difference_type*) 0;
	}

	template <class V, class K, class HF, class Ex, class Eq, class A>
		inline forward_iterator_tag
		iterator_category(const  __hashtable_const_iterator<V, K, HF, Ex, Eq, A>&) {
		return forward_iterator_tag();
	}

	template <class V, class K, class HF, class Ex, class Eq, class A>
		inline V*
		value_type(const __hashtable_const_iterator<V, K, HF, Ex, Eq, A>&) {
		return (V*)0;
	}

	template <class V, class K, class HF, class Ex, class Eq, class A>
		inline typename hashtable<V, K, HF, Ex, Eq, A>::difference_type*
		distance_type(const __hashtable_const_iterator<V, K, HF, Ex, Eq, A>&) {
		return (typename hashtable<V, K, HF, Ex, Eq, A>::difference_type*) 0;
	}

__MYSTL_NAMESPACE_END_

#endif // !_ZSTL_HASHTABLE_
