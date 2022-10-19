#include <assert.h> /* assert */
#include <stdio.h>  /* printf */
#include <math.h>   /* pow */

#include "dhcp.h"

void DHCPCreateAndDestroyTest(void);
void DHCPAllocateIpTest(void); 
void DHCPFreeIpTest(void);
void DHCPCountFreeTest(void);


int main(void)
{
	DHCPCreateAndDestroyTest();
	DHCPAllocateIpTest();
	DHCPFreeIpTest();
	DHCPCountFreeTest();
	
	printf("\n->->->->->-> success!! <-<-<-<-<-<-\n\n");	
	
	return 0;
}



void DHCPCreateAndDestroyTest(void)
{
	dhcp_ty *dhcp = NULL;
	size_t subnet_size = 16;
	unsigned char subnet_ip[4] = {0};
	subnet_ip[0] = 20;
	subnet_ip[1] = 187;
	subnet_ip[2] = 0;
	subnet_ip[3] = 0;
	
	dhcp = DHCPCreate(subnet_ip, subnet_size);
	
	assert((pow(2, 16) - 3) == DHCPCountFree(dhcp));
	
	DHCPDestroy(dhcp);
}

void DHCPAllocateIpTest(void)
{
	dhcp_ty *dhcp = NULL;
	int i = 0;
	int j = 0;
	size_t subnet_size = 16;
	unsigned char subnet_ip[4] = {0};
	unsigned char o_ip_address[4] = {0};
	const unsigned char requested_ip_address[4] = {192, 187, 0, 0};
	
	dhcp = DHCPCreate(subnet_ip, subnet_size);
	
	assert(ALLOCATED_AVAILABLE_ADDRESS == 
				DHCPAllocateIp(dhcp, o_ip_address, requested_ip_address));
	
	for(i = 0 ; i < 4 ; ++i)
	{
		printf("%d.", o_ip_address[i]);
	}
	
	printf("\n\n");
	
	for( ; j < 20 ; ++j)
	{
		assert(ALLOCATED_AVAILABLE_ADDRESS == 
				DHCPAllocateIp(dhcp, o_ip_address, requested_ip_address));
		
		for(i = 0 ; i < 4 ; ++i)
		{
			printf("%d.", o_ip_address[i]);
		}
		printf("\n\n");
	}
	
	DHCPDestroy(dhcp);	

} 


void DHCPFreeIpTest(void)
{
	
	dhcp_ty *dhcp = NULL;
	int i = 0;
	size_t subnet_size = 16;
	unsigned char subnet_ip[4] = {192, 187, 0, 0};
	unsigned char o_ip_address[4] = {0};
	const unsigned char requested_ip_address[4] = {192, 187, 0, 0};
	
	dhcp = DHCPCreate(subnet_ip, subnet_size);
	
	for( ; i < 20 ; ++i)
	{
		DHCPAllocateIp(dhcp, o_ip_address, requested_ip_address);
	}
	
	for(i = 0 ; i < 4 ; ++i)
	{
		printf("%d.", o_ip_address[i]);
	}

	assert(SUCCESS == DHCPFreeIp(dhcp, o_ip_address));
	o_ip_address[3] = 128;
	assert(FAIL == DHCPFreeIp(dhcp, o_ip_address));

	DHCPDestroy(dhcp);	
}


void DHCPCountFreeTest(void)
{
	
	dhcp_ty *dhcp = NULL;
	int i = 0;
	size_t subnet_size = 16;
	unsigned char subnet_ip[4] = {0};
	unsigned char o_ip_address[4] = {0};
	const unsigned char requested_ip_address[4] = {192, 187, 0, 0};
	
	dhcp = DHCPCreate(subnet_ip, subnet_size);
	
	for( ; i < 200 ; ++i)
	{
		DHCPAllocateIp(dhcp, o_ip_address, requested_ip_address);
	}

	assert((pow(2, 16) - 203) == DHCPCountFree(dhcp));

	DHCPDestroy(dhcp);	

}














