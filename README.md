## mySTL

- 简单实现C++ STL容器和算法

- 测试流程：http://120.26.179.141/?s=mySTL

## 主要内容

- 00zstl_config.h：基本配置

- 00zstl_algobase.h： 实现常用的STL算法

- 00zstl_alloc.h：实现第一级空间配置器、第二级空间配置器和内存池，其中第一级空间配置器直接使用 malloc/free/realloc 等函数，第二级空间配置器维护 16 个 8~128byte 的自由链表(组织形式类似 hash 开链)和一个内存池，每次获取/释放内存时都是从自由链表获取/归还到自由链表，若自由链表空间不足时就从内存池获取，内存池空间不足时再从系统获取

- 00zstl_construct.h：主要实现全局函数 construct 和 destroy，construct 在已申请的内存空间上构造内容，destroy 析构特定内存空间上的内容，他们都利用__type_traits<>求取最恰当的措施进行适当的优化  

- 00zstl_iterator.h：定义一些迭代器相应属性，通过 iterator_traits 很容易萃取出迭代器的特性，distance_type、iterator_category、value_type 是对 iterator_traits 的封装，用于提取常用的迭代器属性

- 01zstl_vector.h：vector 具有 array 的优点同时又有许多高效的优化，主要优点有顺序存储、随机访问、动态扩容、预分配等，它的插入操作和删除操作容易导致迭代器的失效，它使用原生指针作为迭代器  

- 02zstl_list.h：list作为一个双向链表对外表现，其内部实际是一个双向循环链表，初始状态时有一个空的节点组成双向循环链表。为了能融入STL算法，它必须使用定制的迭代器而不能使用原生的指针。由于 STL 的 sort 算法只接受随机迭代器，而list的迭代器是双向迭代器不支持随机访问，所有list需要自己实现排序算法，这里采用快速排序  

- 03zstl_deque.h：deque 是序列式容器中设计难度最大的容器，主要原因在于其特殊的数据结构和配套的迭代器设计。其内部首先以一块连续的空间作为 map ，map 内的每个 node 指向一块连续的缓冲区，这些连续的缓冲区才是真正存放数据的位置，初始状态时它维护一个空的缓冲区，当一块缓冲区满时，需要申请一块新的缓冲区，并通过 map 中的一个 node 记录其位置，当 map 满时需要 reallocate_map，即更换一块更大的 map。由于 deque 内部是段式连续空间，对外表现为逻辑上的连续空间任务就交给了 deque 的迭代器（迭代器支持随机访问），迭代器的设计变得复杂，迭代器内部维护 4 个指针，node 指向 map 中记录当前迭代器所在缓冲区的 node，这是因为当迭代器需要跨缓冲区移动时它必须要通过 map 找到下一块缓冲区在哪，first 指向当前元素所在缓冲区的第一个位置，last 指向当前元素所在缓冲区最后一个位置的下一位置，cur 指向缓冲区中当前迭代器所指元素的位置，通过它获取迭代器所指元素  

- 04zstl_stack.h：stack 实际是一个容器配接器，其实现完全依赖于底层容器：deque 或 vector 或 list

- 05zstl_queue.h：queue 实际是一个容器配接器，其实现完全依赖于底层容器：deque 或 list

- 06zstl_heap.h：heap 并不属于标准库，默认以 vector 作为底部容器，提供 make_heap、push_heap、pop_heap、sort_heap 四个基本操作

- 06zstl_heap_fix.h：heap 的另一套实现，使 heap 成为一个类，接受大小比较标准指定

- 07zstl_priority_queue.h：优先级队列的实现，使用类形式的 heap ，priority_queue 是一个配接器而不是一个容器，完全以底部容器(默认为 vector )为根据，再加上 heap 的处理规则

- 08zstl_slist.h：单向链表，与 list 最大的区别在于，它的迭代器属于单向的ForwardIterator，而 list 是双向迭代器BidirectionalIterator，但slist消耗的空间更少。slist 的插入、移除、接合等操作不会使原有的迭代器失效。

- 00stl_RBTree.h：红黑树的实现：红黑树的数据结构、旋转、插入(键值不可重复的插入和键值可重复的插入)、删除、查找等。

- 09stl_set.h：基于 RBTree 的关联容器，RBTree 提供了几乎所有 set 需要的操作，set 不允许键重复，使用 RBTree 的 unique 操作

## Enviroment
- OS: Win10
- tool: VS2017
