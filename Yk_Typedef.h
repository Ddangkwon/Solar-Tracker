/*
 * Yk_Typedef.h
 *
 *  Created on: 2021. 4. 5.
 *      Author: dram1
 */

#ifndef YK_TYPEDEF_H_
#define YK_TYPEDEF_H_

/*define*/
#define YK_DUMMY_LENGTH  4;
/*Signed*/
typedef signed char           int8_t;
typedef short              	  int16_t;
typedef int                   int32_t;
typedef float                 float32_t;
typedef double                float64_t;
typedef long long             int64_t;
/*UnSigned*/
typedef unsigned char         uint8_t;
typedef unsigned short 		  uint16_t;
typedef unsigned int          uint32_t;
typedef unsigned long long    uint64_t;

/*struct*/
typedef struct{
	uint16_t m_su16header;
	uint8_t  m_asu8Dummy[4];
}CalData_t;
#endif /* YK_TYPEDEF_H_ */
