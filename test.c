/*
 * Mem_Lib_Test.c
 *
 *  Created on: 2021. 3. 21.
 *      Author: dram1
 */



#include <stdio.h>
#include <string.h>

int main() {

  /*void* memset(void* ptr, int value, size_t num);*/
/*	void* memset(void* ptr, int value, size_t num);

	첫번째 인자 void* ptr은 세팅하고자 하는 메모리의 시작 주소.
	즉, 그 주소를 가리키고 있는 포인터가 위치하는 자리 입니다.

	두번째 인자 value는 메모리에 세팅하고자 하는 값을 집어 넣으면 됩니다.
	int 타입으로 받지만 내부에서는 unsigned char 로 변환되어서 저장됩니다. 즉 'a' 이런것을 넣어도 무방하다는 뜻입니다.

	세번째 인자 size_t num은 길이를 뜻합니다. 이 길이는 바이트 단위로써 메모리의 크기 한조각 단위의 길이를 말합니다. 이는 보통 "길이 * sizeof(데이터타입)" 의 형태로 작성하면 됩니다.

	반환값은 성공하면 첫번째 인자로 들어간 ptr을 반환하고, 실패한다면 NULL을 반환합니다.*/



  char carr1[] = "blockdmask blog";
  memset(carr1, 'c', 5 * sizeof(char));
  printf(carr1);
  printf("\n");

  char str[50] = "I love Chewing C hahaha";
  char str2[50];
  char str3[50];
  //memcpy예제
  /*void* memcpy (void* dest, const void* source, size_t num)

  첫번째 인자 void* dest
  = 복사 받을 메모리를 가리키는 포인터

  두번째 인자 const void* source
  = 복사할 메모리를 가리키고 있는 포인터

  세번째 인자 size_t num
  = 복사할 데이터(값)의 길이(바이트 단위)*/


  memcpy(str2, str, strlen(str) + 1);
  memcpy(str3, "hello", 6);

  printf("%s \n", str);
  printf("%s \n", str2);
  printf("%s \n", str3);

  //memmove예제
  /*void* memmove (void* dest, const void* src, size_t num);

  첫번째 인자 void* dest
  = destination.
  복사 한걸 붙여넣을 메모리를 가리키는 포인터 입니다. (목적지)

  두번째 인자 void* src
  = source.
  복사 할 메모리를 가리키는 포인터 입니다. (출발지)

  세번째 인자 size_t num
  = 복사할 바이트 길이 입니다.

  반환형
  = 첫번째 인자인 dest를 반환 합니다*/

  printf("%s \n", str);
  printf("memmove 이후 \n");
  memmove(str + 23, str + 17, 6);
  printf("%s", str);

  int arr[10] = {1, 2, 3, 4, 5};
  int arr2[10] = {1, 2, 3, 4, 5};

  //memcmp 예제
  if (memcmp(arr, arr2, 5) == 0)
    printf("arr 과 arr2 는 일치! \n");
  else
    printf("arr 과 arr2 는 일치 안함 \n");
  return 0;
}
