#ifndef __ILRD_127_128_DHCP_H__
#define __ILRD_127_128_DHCP_H__

#include <stddef.h>     /* size_t */

#include "utils.h"      /* utils */

typedef enum
{
    ALLOCATED_REQUSTED_ADDRESS =  0,
    ALLOCATED_AVAILABLE_ADDRESS = 1,
    FAILED_TO_ALLOCATE =          2,
    DOUBLE_FREE =                 3
}return_status_ty;

typedef struct dhcp dhcp_ty;

/*
DESCRIPTION : create a new dhcp server for specific 
			  local network
PARAMETERS :  array of unsigned char that represent 
			  the octets of the subnet ip address,
			  size of the subnet part in the ip(size_t)
RETURN :      pointer to the new dhcp server

TIME COMPLEXITY : O(1)
SPACE COMPLEXITY : O(l) where l is the length of
				   host part in the ip address 			 
*/
dhcp_ty *DHCPCreate(const unsigned char *subnet_ip, size_t subnet_size); 

/*
DESCRIPTION : destroy exit dhcp server
PARAMETERS :  pointer to dhcp 
RETURN :      void
TIME COMPLEXITY : O(n) where n is the num of addresses
				  that save in the server
SPACE COMPLEXITY : O(1)  			 
*/
void DHCPDestroy(dhcp_ty *dhcp);

/*
DESCRIPTION : allocate a new ip address for host machine
			  in local network
PARAMETERS :  pointer to dhcp,
			  pointer to the new dhcp server,
			  array of unsigned char that will contain
			  the ip address that allocated in fact
			  by the server, 
			  array of unsigned char that represent 
			  the octets of the request ip address, 
RETURN :      ALLOCATED_REQUSTED_ADDRESS when the request 
			  ip address allocated,
			  ALLOCATED_AVAILABLE_ADDRESS when other ip
			  address allocated,
			  and FAILED_TO_ALLOCATE if the allocate
			  is fail     
			  
TIME COMPLEXITY : O(l) where l is the length of
				   host part in the ip address
SPACE COMPLEXITY : O(l) where l is the length of
				   host part in the ip address 			 
*/
return_status_ty DHCPAllocateIp(dhcp_ty *dhcp, unsigned char *o_ip_address, 
                         const unsigned char *requested_ip_address); 

/*
DESCRIPTION : free exist ip address in the server
PARAMETERS :  pointer to dhcp,
			  pointer to the new dhcp server,
			  array of unsigned char that will contain
			  the ip address that allocated in fact
			  by the server, 
			  array of unsigned char that represent 
			  the octets of the request ip address 
RETURN :      SUCCESS if the ip address is found
			  and FAIL if not    
			  
TIME COMPLEXITY : O(l) where l is the length of
				   host part in the ip address
SPACE COMPLEXITY : O(1) 			 
*/
status_ty DHCPFreeIp(dhcp_ty *dhcp, const unsigned char *ip_to_free);

/*
DESCRIPTION : counting how many available addresses are
			  remained in the server
PARAMETERS :  pointer to dhcp
RETURN :      the num of free addresses(size_t)    
			  
TIME COMPLEXITY : O(l) where l is the length of
				   host part in the ip address
SPACE COMPLEXITY : O(1) 			 
*/
size_t DHCPCountFree(const dhcp_ty *dhcp);

#endif /* __ILRD_127_128_DHCP_H__ */










