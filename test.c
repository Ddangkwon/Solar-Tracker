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

	ù��° ���� void* ptr�� �����ϰ��� �ϴ� �޸��� ���� �ּ�.
	��, �� �ּҸ� ����Ű�� �ִ� �����Ͱ� ��ġ�ϴ� �ڸ� �Դϴ�.

	�ι�° ���� value�� �޸𸮿� �����ϰ��� �ϴ� ���� ���� ������ �˴ϴ�.
	int Ÿ������ ������ ���ο����� unsigned char �� ��ȯ�Ǿ ����˴ϴ�. �� 'a' �̷����� �־ �����ϴٴ� ���Դϴ�.

	����° ���� size_t num�� ���̸� ���մϴ�. �� ���̴� ����Ʈ �����ν� �޸��� ũ�� ������ ������ ���̸� ���մϴ�. �̴� ���� "���� * sizeof(������Ÿ��)" �� ���·� �ۼ��ϸ� �˴ϴ�.

	��ȯ���� �����ϸ� ù��° ���ڷ� �� ptr�� ��ȯ�ϰ�, �����Ѵٸ� NULL�� ��ȯ�մϴ�.*/



  char carr1[] = "blockdmask blog";
  memset(carr1, 'c', 5 * sizeof(char));
  printf(carr1);
  printf("\n");

  char str[50] = "I love Chewing C hahaha";
  char str2[50];
  char str3[50];
  //memcpy����
  /*void* memcpy (void* dest, const void* source, size_t num)

  ù��° ���� void* dest
  = ���� ���� �޸𸮸� ����Ű�� ������

  �ι�° ���� const void* source
  = ������ �޸𸮸� ����Ű�� �ִ� ������

  ����° ���� size_t num
  = ������ ������(��)�� ����(����Ʈ ����)*/


  memcpy(str2, str, strlen(str) + 1);
  memcpy(str3, "hello", 6);

  printf("%s \n", str);
  printf("%s \n", str2);
  printf("%s \n", str3);

  //memmove����
  /*void* memmove (void* dest, const void* src, size_t num);

  ù��° ���� void* dest
  = destination.
  ���� �Ѱ� �ٿ����� �޸𸮸� ����Ű�� ������ �Դϴ�. (������)

  �ι�° ���� void* src
  = source.
  ���� �� �޸𸮸� ����Ű�� ������ �Դϴ�. (�����)

  ����° ���� size_t num
  = ������ ����Ʈ ���� �Դϴ�.

  ��ȯ��
  = ù��° ������ dest�� ��ȯ �մϴ�*/

  printf("%s \n", str);
  printf("memmove ���� \n");
  memmove(str + 23, str + 17, 6);
  printf("%s", str);

  int arr[10] = {1, 2, 3, 4, 5};
  int arr2[10] = {1, 2, 3, 4, 5};

  //memcmp ����
  if (memcmp(arr, arr2, 5) == 0)
    printf("arr �� arr2 �� ��ġ! \n");
  else
    printf("arr �� arr2 �� ��ġ ���� \n");
  return 0;
}
