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

uint16_t  g_clockcount;
int main() {

	while(1){

		if(g_clockcount > 100)
			break;
		g_clockcount++;
	}

}
