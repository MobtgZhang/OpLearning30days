#ifndef JOS_IN_TYPES_H
#define JOS_IN_TYPES_H

#ifndef NULL
#define NULL ((void*) 0)
#endif

/* 布尔变量 */
typedef int bool;
/* 明确定义整型变量 */
typedef __signed char int8_t;
typedef unsigned char uint8_t;
typedef short int16_t;
typedef unsigned short uint16_t;
typedef int int32_t;
typedef unsigned int uint32_t;
typedef long long int64_t;
typedef unsigned long long uint64_t;

/*特别地，指针和地址均为32位长，使用以下定义作为虚拟地址*/
typedef int32_t intptr_t;
typedef uint32_t uintptr_t;
/*物理地址*/
typedef uint32_t physaddr_t;

/*页表地址*/
typedef uint32_t ppn_t;

/*内存对象大小*/
typedef uint32_t size_t;
/*有符号类型，用于返回错误类型*/
typedef int32_t ssize_t;

/*用于偏移量和长度*/
typedef int32_t off_t;

/*最大值与最小值比较函数*/
#define MIN(_a, _b)						\
({								\
	typeof(_a) __a = (_a);					\
	typeof(_b) __b = (_b);					\
	__a <= __b ? __a : __b;					\
})
#define MAX(_a, _b)						\
({								\
	typeof(_a) __a = (_a);					\
	typeof(_b) __b = (_b);					\
	__a >= __b ? __a : __b;					\
})

/*rounding函数，用于寻找数值N最接近的整数*/
/*向下定义函数*/
#define ROUNDDOWN(a, n)						\
({								\
	uint32_t __a = (uint32_t) (a);				\
	(typeof(a)) (__a - __a % (n));				\
})
/*向上定义函数*/
#define ROUNDUP(a, n)						\
({								\
	uint32_t __n = (uint32_t) (n);				\
	(typeof(a)) (ROUNDDOWN((uint32_t) (a) + __n - 1, __n));	\
})


/*返回member相对于结构类型开头的偏移量*/
#define offsetof(type, member)  ((size_t) (&((type*)0)->member))

#endif
