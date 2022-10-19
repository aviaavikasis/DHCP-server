#include <assert.h> /* assert */
#include <stdlib.h> /* calloc, free */
#include <string.h> /* strcat */
#include <stdio.h>
#include <math.h>   /* pow */

#include "dhcp.h"   /* dhcp api */
#include "btrie.h"  /* btrie functions */

#define BINARY 2
#define IP_SIZE 32
#define OCTET_NUM 4
#define OCTET_SIZE 8
#define HOST_PART_SIZE(dhcp) (IP_SIZE - dhcp->subnet_size)

struct dhcp
{
    btrie_ty *btrie;
    char subnet_ip[IP_SIZE + 1];
    size_t subnet_size;
};


/* static functions */
static btrie_ty *GetTrie(dhcp_ty *dhcp);
char* BinaryItoa(unsigned char num, char* dest);
static void ConvertToStr(const unsigned char *ip, char *str_ip);
static void ConvertStrToNums(char *alloc_ip, unsigned char *o_ip_address);
static void InitSubnetIp(char *subnet_ip);

/*----------- getters and setters------------*/

static btrie_ty *GetTrie(dhcp_ty *dhcp)
{
	assert(NULL != dhcp);
	
	return dhcp->btrie;
}

/*------------------------------------------*/


dhcp_ty *DHCPCreate(const unsigned char *subnet_ip, size_t subnet_size)
{

	dhcp_ty *dhcp = NULL;
	char *convert_str = NULL;
	return_status_ty alloc_status = 0;
	int i = 0;
	char special_ip_str[IP_SIZE + 1] = {'\0'}; 
	unsigned char o_ip_address[OCTET_NUM] = {0};
	unsigned char special_addresses[3][4] = {{0, 0, 0, 0},
											 {255, 255, 255, 254},
											 {255, 255, 255, 255}}; 
	assert(NULL != subnet_ip);
	 
	dhcp = (dhcp_ty*)malloc(sizeof(dhcp_ty));
	if(NULL == dhcp)
	{
		return NULL;
	}
	
	dhcp->btrie = BTrieCreate();
	if(NULL == dhcp->btrie)
	{
		free(dhcp);
		dhcp = NULL;
		
		return NULL;
	}
	
	InitSubnetIp(dhcp->subnet_ip);
	ConvertToStr(subnet_ip, dhcp->subnet_ip);
	dhcp->subnet_size = subnet_size;
	
	/* alloc special ips */
	for(i = 0 ; i < 3 ; ++i)
	{
		ConvertToStr(special_addresses[i], special_ip_str);
		strncpy(special_ip_str, dhcp->subnet_ip, dhcp->subnet_size);
		ConvertStrToNums(special_ip_str, special_addresses[i]);
		alloc_status = DHCPAllocateIp(dhcp, o_ip_address, special_addresses[i]);
		if(FAILED_TO_ALLOCATE == alloc_status)
		{
			DHCPDestroy(dhcp);
			
			return NULL;
		}
	}
	
	return dhcp;
}


void DHCPDestroy(dhcp_ty *dhcp)
{
	assert(NULL != dhcp);
	
	BTrieDestroy(dhcp->btrie);
	dhcp->btrie = NULL;
	
	free(dhcp);
	dhcp = NULL;
}


return_status_ty DHCPAllocateIp(dhcp_ty *dhcp,
								 unsigned char *o_ip_address,
								 const unsigned char *requested_ip_address)
{
	char *req_ip_str_ptr = NULL;
	char req_ip_str[IP_SIZE + 1] = {'\0'};
	char alloc_ip[IP_SIZE + 1] = {'\0'};
	return_status_ty suggest_status = 0;
	status_ty alloc_status = 0;

	assert(NULL != dhcp);
	assert(NULL != o_ip_address);
	assert(NULL != requested_ip_address);

	ConvertToStr(requested_ip_address, req_ip_str);
	
	suggest_status = BTrieFindEqualOrLargeStr(dhcp->btrie,
												 alloc_ip,
												 req_ip_str + dhcp->subnet_size);
	if(FAILED_TO_ALLOCATE == suggest_status)
	{
		free(alloc_ip);
		
		return FAILED_TO_ALLOCATE;
	}
	
	alloc_status = BTrieInsert(GetTrie(dhcp), alloc_ip);
	{
		if(FAIL == alloc_status)
		{
			free(alloc_ip);
		
			return FAILED_TO_ALLOCATE;
		}
	}
	
	strncpy(alloc_ip + dhcp->subnet_size, alloc_ip, HOST_PART_SIZE(dhcp));
	strncpy(alloc_ip, req_ip_str, dhcp->subnet_size);
	
	ConvertStrToNums(alloc_ip, o_ip_address);
	
	return suggest_status;

}


status_ty DHCPFreeIp(dhcp_ty *dhcp, const unsigned char *ip_to_free)
{

	char free_ip_str[IP_SIZE + 1] = {'\0'};
	
	assert(NULL != dhcp);
	assert(NULL != ip_to_free);
	
	ConvertToStr(ip_to_free, free_ip_str);
	
	printf("\n%s\n",free_ip_str);
	
	return BTrieRemove(dhcp->btrie, free_ip_str + dhcp->subnet_size);

}


size_t DHCPCountFree(const dhcp_ty *dhcp)
{
	assert(NULL != dhcp);
	
	return (pow(2, IP_SIZE - dhcp->subnet_size) - BTrieSize(dhcp->btrie));
}



/*---------------- static functions --------------*/


static void InitSubnetIp(char *subnet_ip)
{
	int i = 0;
	
	assert(NULL != subnet_ip);
	
	for(i = 0 ; i < IP_SIZE + 1 ; ++i)
	{
		subnet_ip[i] = '\0';
	}
}


char* BinaryItoa(unsigned char num, char* str)
{

	const char digits[BINARY] = "01";
	
	unsigned int len = 8; /* amount of digits that will be in new base */
	unsigned int end = 0;	
	                                         
	str[len] = '\0';                                     
	for ( ; len > end ; --len)
	{
		str[len-1] = digits[num % 2];
		num /= 2;
	}
	
	return str;
}


static void ConvertToStr(const unsigned char *ip, char *str_ip)
{
	char help_str[OCTET_SIZE + 1] = {'\0'};
	int i = 0;
	unsigned char num = 0;
		
	assert(NULL != ip);
	
	for(; i < OCTET_NUM ; ++i)
	{
		num = ip[i];
		strcat(str_ip, BinaryItoa(num, help_str));
	}
}


static void ConvertStrToNums(char *alloc_ip, unsigned char *o_ip_address)
{
	int i = OCTET_NUM - 1;
	assert(NULL != alloc_ip);
	assert(NULL != o_ip_address);
	
	for( ; i >= 0 ; --i)
	{
		o_ip_address[i] = strtol(alloc_ip + i * OCTET_SIZE, NULL, 2);
		*(alloc_ip + i * OCTET_SIZE) = '\0';
	} 	
}















