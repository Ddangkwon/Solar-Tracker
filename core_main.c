/*
 * core_main.c
 *
 *  Created on: 2021. 4. 5.
 *      Author: dram1
 */


#include <stdio.h>
#include <string.h>
#include "Yk_Typedef.h";

/*global variable*/
uint16_t   g_su16clockcount;
CalData_t* g_pf32pointer;
int main() {
	printf("START\n");
	CalData_t* l_pf32pointer;
	//값을 직접 대입시켜줘야하므로 malloc과 memcpy를 수행한다.
	CalData_t* l_pf32data = malloc(sizeof(CalData_t));
	memset(l_pf32data, 0x00, sizeof(CalData_t));
	l_pf32data->m_su16header = 16;
	l_pf32data->m_asu8Dummy[0] = 5;
	memcpy(l_pf32pointer, l_pf32data, sizeof(CalData_t));

	printf("%d\n", l_pf32pointer->m_su16header);
	printf("%d\n", l_pf32pointer->m_asu8Dummy[0]);
	while(1){

		if(g_su16clockcount > 100U)
			break;
		g_su16clockcount++;
	}

	printf("FIN");
}
