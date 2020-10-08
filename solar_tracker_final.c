#include "nios2_ctrl_reg_macros.h"
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <math.h>

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240
#define PI 3.1415925
#define PIXEL(r,g,b) \
   (short int)((((r)&0x1F)<<11) | (((g)&0x3F)<<5) | (((b)&0x1F)))

#define FILL_PIXEL(x, y, r, g, b) \
   *(short int *)(pixel_buffer_start + (((y)&0xFF)<<10) + (((x)&0x1FF)<<1)) = PIXEL(r, g, b)

#define KEY_BASE 0xFF200050
#define LEDR_BASE 0xFF200000
#define JP1_BASE_Address 0xFF200060
#define LEDR_BASE 0xFF200000
#define ADC_BASE 0xFF204000
#define HEX3_HEX0_BASE 0xFF200020
#define HEX5_HEX4_BASE 0xFF200030
#define PIXEL_BUF_CTRL_BASE   0xFF203020   // for VGA

volatile int* ADC_ptr_ch0 = (int*)ADC_BASE;
volatile int* ADC_ptr_ch1 = (int*)(ADC_BASE + 0x4);
volatile int* ADC_ptr_ch2 = (int*)(ADC_BASE + 0x8);
volatile int* ADC_ptr_ch3 = (int*)(ADC_BASE + 0xC);
volatile int* ADC_ptr_ch4 = (int*)(ADC_BASE + 0x10);
volatile int* ADC_ptr_ch5 = (int*)(ADC_BASE + 0x14);
volatile int* ADC_ptr_ch6 = (int*)(ADC_BASE + 0x18);
volatile int* ADC_ptr_ch7 = (int*)(ADC_BASE + 0x1C);

volatile int *KEY_ptr = (int *)KEY_BASE;
volatile int *GPIO_ptr = (int *)JP1_BASE_Address;
volatile int *HEX3_0 = (int *)HEX3_HEX0_BASE;
volatile int *HEX5_4 = (int *)HEX5_HEX4_BASE;

volatile int pixel_buffer_start;   // for VGA
volatile int *pixel_ctrl_ptr;




void interrupt_handler(void);
void pushbutton_ISR(void);
void timer_ISR(void);

void dht22(void);
void update_dht22(void);
void update_voltage(void);
void update_current(void);
void current_check(void);
void voltage_check(void);
void delay_us(unsigned int counter);

void TEMP_HUM_HEX5_0_DISPLAY(int divtem, int divhumid);
int SEG7_CODE(int index);

void photoresistor(float* ch_value);
unsigned char bit_discrimination();
void servo_yaw_control(double angle);
void servo_pitch_control(double angle);

void clear_screen(int r, int g, int b);            // VGA
void draw_square(int x1, int y1, int x2, int y2, int r, int g, int b);
void draw_line(int x1, int y1, int x2, int y2, int r, int g, int b);
void wait_for_vsync();
void draw_num(int num, int x, int y);
void draw_temp_humid(int x, int y);
void draw_vol_cur(int x, int y);
void swap(int *x1, int *x2);
void relay();
void solar_tracker(void);
void VGA_display(void);

/*--------------------------------------------------------------------------*/

short int front_buffer[512 * 256];
short int back_buffer[512 * 256];

int Temp;   //dht22에 쓰이는 변수
int Humid;      //전역변수로 지정
int checksum;      //파일 나눠서 할때는 extern으로 받을 것

int voltage_100times; // 전압계 측정값 V * 100 한 값
int current_mA; // 전류 측정 결과

int temp_10, temp_1, temp_01;  // VGA 출력을 위한 온도 값
int humid_10, humid_1, humid_01;  // VGA 출력을 위한 습도 값
int vol_1, vol_01, vol_001;      // VGA 출력 위한 전압값 (V)
int cur_100, cur_10, cur_1;      // VGA 출력 위한 전류값 (mA)

float topleft, topright, bottomleft, bottomright, threshold;
int flag_yaw = 1;
int flag_pitch = 1;
double angle_yaw;
double angle_pitch;
int left_tilt;
int right_tilt;
int top_tilt;
int bottom_tilt;
float photon[4] = { 0, };
int check = -1;
 int x,y,j,k;
void the_reset(void) __attribute__((section(".reset")));
void the_reset(void)
/************************************************************************************
 * Reset code. By giving the code a section attribute with the name ".reset" we     *
 * allow the linker program to locate this code at the proper reset vector address. *
 * This code just calls the main program.                                           *
 ***********************************************************************************/
{
   asm(".set      noat");               // Magic, for the C compiler
   asm(".set      nobreak");            // Magic, for the C compiler
   asm("movia      r2, main");            // Call the C language main program
   asm("jmp      r2");
}
/* The assembly language code below handles CPU exception processing. This
 * code should not be modified; instead, the C language code in the function
 * interrupt_handler() can be modified as needed for a given application.
*/
void the_exception(void) __attribute__((section(".exceptions")));
void the_exception(void)
/*******************************************************************************
 * Exceptions code. By giving the code a section attribute with the name       *
 * ".exceptions" we allow the linker program to locate this code at the proper *
 * exceptions vector address.                                                  *
 * This code calls the interrupt handler and later returns from the exception. *
 ******************************************************************************/
{
   asm(".set      noat");                  // Magic, for the C compiler
   asm(".set      nobreak");               // Magic, for the C compiler
   asm("subi      sp, sp, 128");
   asm("stw      et, 96(sp)");
   asm("rdctl   et, ctl4");
   asm("beq      et, r0, SKIP_EA_DEC");   // Interrupt is not external         
   asm("subi      ea, ea, 4");            /* Must decrement ea by one instruction
                                 * for external interupts, so that the
                                 * interrupted instruction will be run */

   asm("SKIP_EA_DEC:");
   asm("stw   r1,  4(sp)");               // Save all registers 
   asm("stw   r2,  8(sp)");
   asm("stw   r3,  12(sp)");
   asm("stw   r4,  16(sp)");
   asm("stw   r5,  20(sp)");
   asm("stw   r6,  24(sp)");
   asm("stw   r7,  28(sp)");
   asm("stw   r8,  32(sp)");
   asm("stw   r9,  36(sp)");
   asm("stw   r10, 40(sp)");
   asm("stw   r11, 44(sp)");
   asm("stw   r12, 48(sp)");
   asm("stw   r13, 52(sp)");
   asm("stw   r14, 56(sp)");
   asm("stw   r15, 60(sp)");
   asm("stw   r16, 64(sp)");
   asm("stw   r17, 68(sp)");
   asm("stw   r18, 72(sp)");
   asm("stw   r19, 76(sp)");
   asm("stw   r20, 80(sp)");
   asm("stw   r21, 84(sp)");
   asm("stw   r22, 88(sp)");
   asm("stw   r23, 92(sp)");
   asm("stw   r25, 100(sp)");            // r25 = bt (skip r24 = et, because it is saved above)
   asm("stw   r26, 104(sp)");            // r26 = gp
   // skip r27 because it is sp, and there is no point in saving this
   asm("stw   r28, 112(sp)");            // r28 = fp
   asm("stw   r29, 116(sp)");            // r29 = ea
   asm("stw   r30, 120(sp)");            // r30 = ba
   asm("stw   r31, 124(sp)");            // r31 = ra
   asm("addi   fp,  sp, 128");

   asm("call   interrupt_handler");      // Call the C language interrupt handler

   asm("ldw   r1,  4(sp)");               // Restore all registers
   asm("ldw   r2,  8(sp)");
   asm("ldw   r3,  12(sp)");
   asm("ldw   r4,  16(sp)");
   asm("ldw   r5,  20(sp)");
   asm("ldw   r6,  24(sp)");
   asm("ldw   r7,  28(sp)");
   asm("ldw   r8,  32(sp)");
   asm("ldw   r9,  36(sp)");
   asm("ldw   r10, 40(sp)");
   asm("ldw   r11, 44(sp)");
   asm("ldw   r12, 48(sp)");
   asm("ldw   r13, 52(sp)");
   asm("ldw   r14, 56(sp)");
   asm("ldw   r15, 60(sp)");
   asm("ldw   r16, 64(sp)");
   asm("ldw   r17, 68(sp)");
   asm("ldw   r18, 72(sp)");
   asm("ldw   r19, 76(sp)");
   asm("ldw   r20, 80(sp)");
   asm("ldw   r21, 84(sp)");
   asm("ldw   r22, 88(sp)");
   asm("ldw   r23, 92(sp)");
   asm("ldw   r24, 96(sp)");
   asm("ldw   r25, 100(sp)");            // r25 = bt
   asm("ldw   r26, 104(sp)");            // r26 = gp
   // skip r27 because it is sp, and we did not save this on the stack
   asm("ldw   r28, 112(sp)");            // r28 = fp
   asm("ldw   r29, 116(sp)");            // r29 = ea
   asm("ldw   r30, 120(sp)");            // r30 = ba
   asm("ldw   r31, 124(sp)");            // r31 = ra

   asm("addi   sp,  sp, 128");

   asm("eret");
}
/*****************************************************************************
 * Interrupt Service Routine
 *  Determines what caused the interrupt and calls the appropriate
 *  subroutine.
 *
 * ipending - Control register 4 which has the pending external interrupts
 *****************************************************************************/
void interrupt_handler(void)
{
   int ipending;
   NIOS2_READ_IPENDING(ipending);

   if (ipending & 0x2)
   {
      pushbutton_ISR();
   }
   
   // else, ignore the interrupt
   return;
}
int main(void)
{
   *(KEY_ptr + 2) = 0b1111;
   NIOS2_WRITE_IENABLE(0b10); //key의 IRQ 번호는 2
   NIOS2_WRITE_STATUS(0b1); // system이 interrupt 처리할 수 있도록

   pixel_ctrl_ptr = (int *)PIXEL_BUF_CTRL_BASE;   //VGA 세팅
   *(pixel_ctrl_ptr + 1) = front_buffer;
   wait_for_vsync();

   pixel_buffer_start = *pixel_ctrl_ptr;
   clear_screen(0x00, 0x00, 0x00);
   *(pixel_ctrl_ptr + 1) = back_buffer;

   int x, y;      //VGA 좌표
   int i, j, k;
   int diff = 3;   // 서보 모터 각 변동 최소 단위
   angle_yaw = 90.0;   // 초기 설정각
   angle_pitch = 90.0;
   threshold = 0.15;//user_setting_value

   *(GPIO_ptr + 1) = 0x000C;//direction bit servo output 
   servo_pitch_control(angle_pitch);
   servo_yaw_control(angle_yaw);
   
   while (1)
   {
      solar_tracker();
      VGA_display();
   }

   return 0;
}
void solar_tracker(void) {
   photoresistor(photon);
  
   int diff = 3;
   printf("photon 0: %f\n", photon[0]);
   printf("photon 1: %f\n", photon[1]);
   printf("photon 2: %f\n", photon[2]);
   printf("photon 3: %f\n", photon[3]);
   topleft = photon[0];
   topright = photon[1] - 0.20;
   bottomleft = photon[2];
   bottomright = photon[3];

   // YAW(Normal-axis) control routine
   if (((topleft - topright) >= threshold) || ((bottomleft - bottomright) >= threshold))  // left tilt
   {
      angle_yaw -= diff;    // -
      servo_yaw_control(angle_yaw);
      delay_us(1000000);//0.1s
   }


   else if ((topleft - topright < threshold) && (bottomleft - bottomright < threshold))
   {

      delay_us(1000000);//0.1s

   }
   if (((topright - topleft) >= threshold) || ((bottomright - bottomleft) >= threshold))      //right tilt
   {
      angle_yaw += diff;
      servo_yaw_control(angle_yaw);
      delay_us(1000000);//0.1s

   }
   else if ((topright - topleft < threshold) && (bottomright - bottomleft < threshold))
   {

      delay_us(1000000);//0.1s

   }

   else
   {

      printf("exception occured!y!");

   }

   // PITCH(Lateral-axis) control routine

   if (((topright - bottomright) >= threshold) || ((topleft - bottomleft) >= threshold))
   {
      angle_pitch -= diff;     //
      servo_pitch_control(angle_pitch);
      delay_us(1000000);//0.1s
      printf("go up???\n");
   }


   else if (((topright - bottomright) < threshold) && ((topleft - bottomleft) < threshold))
   {

      delay_us(1000000);//0.1s

   }
   if (((bottomright - topright) >= threshold) || ((bottomleft - topleft) >= threshold))
   {
      angle_pitch += diff;
      servo_pitch_control(angle_pitch);
      delay_us(1000000);//0.1s
      printf("go down???\n");
   }

   else if (((bottomright - topright) < threshold) && ((bottomleft - topleft) < threshold))
   {

      delay_us(1000000);//0.1s

   }

   else
   {

      printf("exception occured!p!");

   }
}
void VGA_display(void) {
   
   pixel_buffer_start = *(pixel_ctrl_ptr + 1);
   clear_screen(0x00, 0x00, 0x00);

   x = 20; y = 60;
   draw_vol_cur(x, y);      // voltage와 current VGA로 나타낸다

   if (check == -1) {
      x = 90; y = 160;
      draw_num(18, x, y); //번개
   }
   
   else if (check == 1) {   // LED
      x = 200; y = 160;
      draw_square(x, y, x + 30, y + 30, 0x1F, 0x3F, 0x1F);
      x = 209; y = 179;
      draw_square(x, y, x + 2, y + 35, 0x1F, 0x3F, 0x1F);
      x = 219; y = 179;
      draw_square(x, y, x + 2, y + 35, 0x1F, 0x3F, 0x1F);
      x = 198; y = 140;
      for (k = 0; k < 3; k++)
         for (j = 0; j < 3; j++)
            draw_line(x + k, y, x + 3 + j, y + 8, 0x1F, 0x3F, 0x1F);
      x = 214;
      draw_square(x, y, x + 2, y + 8, 0x1F, 0x3F, 0x1F);

      x = 230;
      for (k = 0; k < 3; k++)
         for (j = 0; j < 3; j++)
            draw_line(x + k, y, x - 3 + j, y + 8, 0x1F, 0x3F, 0x1F);
   }
      
   wait_for_vsync();
}
void pushbutton_ISR(void)
{
   int press = *(KEY_ptr + 3);    // disassert
   *(KEY_ptr + 3) = press;
/*
   if (press == 0b0001) {  //key0이면 온습도계
      dht22();
      update_dht22();
   }
   */
    if (press == 0b0010) {  //key1이면 전압계
      voltage_check();
      update_voltage();
      current_check();
      update_current();
   }

   else if (press == 0b0100) {
      check *= -1;
      relay();
   }
   
   return;
}
void delay_us(unsigned int counter)
{
   unsigned int i;
   for (i = 0; i < counter; i++)
   {
      asm("NOP");
      asm("NOP");
   }
}
void current_check(void) {   // ADC channel 5
   float raw;
   float temp;

   *ADC_ptr_ch0 = 0b01;
   delay_us(320); // 1.6us

   raw = *ADC_ptr_ch5;
   printf("current raw value : %f\n", raw);

   temp = raw * 5000.0 / 4096.0;
   temp = (temp - 2500) / 185;
   temp = temp * 100.0;
   current_mA = (int)temp;
}
void voltage_check(void) {   // ADC channel 4

   float raw;
   float temp;

   *ADC_ptr_ch0 = 0b01;
   delay_us(320);  //1.6 us

   raw = *ADC_ptr_ch4;
   printf("voltage raw value : %f\n", raw);

   temp = raw * 5.0 / 4096;
   temp = temp * 3.7;
   printf("voltage value : %f\n", temp);
   temp = temp * 100.0;
   voltage_100times = (int)temp;
}
void relay()
{

   *(GPIO_ptr + 1) = 0x0020;//direction bit servo output D5 OUTPUT
   if (check == 1)
      *(GPIO_ptr) = 0x0020;//D5 high
   else
      *(GPIO_ptr) = 0x0000;//D5 low
}
void photoresistor(float* ch_value) {

   float ch[4];
   int j;


   *ADC_ptr_ch0 = 0b01;
   delay_us(32);  // 1.6us
   ch[0] = *ADC_ptr_ch0;

   *ADC_ptr_ch0 = 0b01;
   delay_us(32);
   ch[1] = *ADC_ptr_ch1;

   *ADC_ptr_ch0 = 0b01;
   delay_us(32);
   ch[2] = *ADC_ptr_ch2;

   *ADC_ptr_ch0 = 0b01;
   delay_us(32);
   ch[3] = *ADC_ptr_ch3;

   for (j = 0; j < 4; j++)
   {
      ch_value[j] = ch[j] * (5.0 / 4096.0);
      //printf("ch%d Voltage = %f\n", j, ch_value[j]);

   }
   
}
void servo_yaw_control(double angle)
{
   //pwm pulse generate
   int i;
   if (flag_yaw)
   {
      for (i = 0; i < 10; i++)
      {
         *(GPIO_ptr) = 0x0004;//D2 high
         delay_us((int)(48000 / 180 * angle));
         *(GPIO_ptr) = 0x0000;//D2 low
         delay_us(400000 - ((int)(48000 / 180 * angle)));

      }
   }

   if (angle >= 180 || angle <= 0)
   {
      flag_yaw = 0;
   }
   else
   {
      flag_yaw = 1;
   }
}
void servo_pitch_control(double angle)
{
   //pwm pulse generate
   int i;
   if (flag_pitch)
   {
      for (i = 0; i < 10; i++)
      {
         *(GPIO_ptr) = 0x0008;//D3 high
         delay_us((int)(48000 / 180 * angle));
         *(GPIO_ptr) = 0x0000;//D3 low
         delay_us(400000 - ((int)(48000 / 180 * angle)));

      }
   }

   if (angle >= 180 || angle <= 0)
   {
      flag_pitch = 0;
   }
   else
   {
      flag_pitch = 1;
   }
}
void draw_num(int num, int x, int y) {
   int i, j;
   if (num == 0) {
      draw_square(x, y, x + 4, y + 40, 0, 0, 0x1F);
      draw_square(x, y, x + 20, y + 4, 0, 0, 0x1F);
      draw_square(x, y + 36, x + 20, y + 40, 0, 0, 0x1F);
      draw_square(x + 16, y, x + 20, y + 40, 0, 0, 0x1F);
   }
   else if (num == 1) {
      draw_square(x + 16, y, x + 20, y + 40, 0, 0, 0x1F);
   }
   else if (num == 2) {
      draw_square(x, y, x + 20, y + 4, 0, 0, 0x1F);
      draw_square(x + 16, y, x + 20, y + 22, 0, 0, 0x1F);
      draw_square(x, y + 18, x + 20, y + 22, 0, 0, 0x1F);
      draw_square(x, y + 18, x + 4, y + 40, 0, 0, 0x1F);
      draw_square(x, y + 36, x + 20, y + 40, 0, 0, 0x1F);
   }
   else if (num == 3) {
      draw_square(x, y, x + 20, y + 4, 0, 0, 0x1F);
      draw_square(x, y + 36, x + 20, y + 40, 0, 0, 0x1F);
      draw_square(x, y + 18, x + 20, y + 22, 0, 0, 0x1F);
      draw_square(x + 16, y, x + 20, y + 40, 0, 0, 0x1F);
   }
   else if (num == 4) {
      draw_square(x, y, x + 4, y + 22, 0, 0, 0x1F);
      draw_square(x + 16, y, x + 20, y + 40, 0, 0, 0x1F);
      draw_square(x, y + 18, x + 20, y + 22, 0, 0, 0x1F);
   }
   else if (num == 5) {
      draw_square(x, y, x + 4, y + 22, 0, 0, 0x1F);
      draw_square(x, y, x + 20, y + 4, 0, 0, 0x1F);
      draw_square(x, y + 18, x + 20, y + 22, 0, 0, 0x1F);
      draw_square(x, y + 36, x + 20, y + 40, 0, 0, 0x1F);
      draw_square(x + 16, y + 18, x + 20, y + 40, 0, 0, 0x1F);
   }
   else if (num == 6) {
      draw_square(x, y, x + 4, y + 40, 0, 0, 0x1F);
      draw_square(x, y, x + 20, y + 4, 0, 0, 0x1F);
      draw_square(x, y + 18, x + 20, y + 22, 0, 0, 0x1F);
      draw_square(x, y + 36, x + 20, y + 40, 0, 0, 0x1F);
      draw_square(x + 16, y + 18, x + 20, y + 40, 0, 0, 0x1F);
   }
   else if (num == 7) {
      draw_square(x, y, x + 4, y + 22, 0, 0, 0x1F);
      draw_square(x + 16, y, x + 20, y + 40, 0, 0, 0x1F);
      draw_square(x, y, x + 20, y + 4, 0, 0, 0x1F);
   }
   else if (num == 8) {
      draw_square(x, y, x + 4, y + 40, 0, 0, 0x1F);
      draw_square(x, y, x + 20, y + 4, 0, 0, 0x1F);
      draw_square(x, y + 18, x + 20, y + 22, 0, 0, 0x1F);
      draw_square(x, y + 36, x + 20, y + 40, 0, 0, 0x1F);
      draw_square(x + 16, y, x + 20, y + 40, 0, 0, 0x1F);
   }
   else if (num == 9) {
      draw_square(x, y, x + 4, y + 22, 0, 0, 0x1F);
      draw_square(x, y, x + 20, y + 4, 0, 0, 0x1F);
      draw_square(x, y + 18, x + 20, y + 22, 0, 0, 0x1F);
      draw_square(x, y + 36, x + 20, y + 40, 0, 0, 0x1F);
      draw_square(x + 16, y, x + 20, y + 40, 0, 0, 0x1F);
   }
   else if (num == 10) {  // dot
      draw_square(x, y + 36, x + 4, y + 40, 0, 0, 0x1F);
   }
   else if (num == 11) {   //도씨
      draw_square(x, y, x + 4, y + 4, 0, 0, 0x1F);
   }
   else if (num == 12) {   // V
      for (i = 0; i < 4; i++)
         for (j = 0; j < 4; j++)
            draw_line(x + i, y, x + 8 + j, y + 40, 0, 0, 0x1F);

      x += 16;
      for (i = 0; i < 4; i++)
         for (j = 0; j < 4; j++)
            draw_line(x + i, y, x - 8 + j, y + 40, 0, 0, 0x1F);
   }
   else if (num == 13) {   // A
      draw_square(x + 4, y + 22, x + 14, y + 26, 0, 0, 0x1F);

      x += 8;
      for (i = 0; i < 4; i++)
         for (j = 0; j < 4; j++)
            draw_line(x + i, y, x - 8 + j, y + 40, 0, 0, 0x1F);

      for (i = 0; i < 4; i++)
         for (j = 0; j < 4; j++)
            draw_line(x + i, y, x + 8 + j, y + 40, 0, 0, 0x1F);

   }
   else if (num == 14) {      // W
      for (i = 0; i < 4; i++)
         for (j = 0; j < 4; j++)
            draw_line(x + i, y, x + 8 + j, y + 40, 0, 0, 0x1F);

      x += 16;
      for (i = 0; i < 4; i++)
         for (j = 0; j < 4; j++)
            draw_line(x + i, y, x - 8 + j, y + 40, 0, 0, 0x1F);


      for (i = 0; i < 4; i++)
         for (j = 0; j < 4; j++)
            draw_line(x + i, y, x + 8 + j, y + 40, 0, 0, 0x1F);

      x += 16;;
      for (i = 0; i < 4; i++)
         for (j = 0; j < 4; j++)
            draw_line(x + i, y, x - 8 + j, y + 40, 0, 0, 0x1F);
   }
   else if (num == 15) {   // %
      draw_square(x + 2, y + 2, x + 6, y + 6, 0, 0, 0x1F);
      draw_square(x + 14, y + 34, x + 18, y + 38, 0, 0, 0x1F);
      x += 16;
      for (i = 0; i < 4; i++)
         for (j = 0; j < 4; j++)
            draw_line(x + i, y, x - 16 + j, y + 40, 0, 0, 0x1F);

   }
   else if (num == 16) {   // C
      draw_square(x, y, x + 20, y + 4, 0, 0, 0x1F);
      draw_square(x, y, x + 4, y + 40, 0, 0, 0x1F);
      draw_square(x, y + 36, x + 20, y + 40, 0, 0, 0x1F);
   }
   else if (num == 17) {   // m
      draw_square(x, y + 18, x + 20, y + 22, 0, 0, 0x1F);
      draw_square(x, y + 18, x + 4, y + 40, 0, 0, 0x1F);
      draw_square(x + 16, y + 18, x + 20, y + 40, 0, 0, 0x1F);
      draw_square(x + 8, y + 18, x + 12, y + 40, 0, 0, 0x1F);
   }
   else if (num == 18) {   // 번개
      for (i = 0; i < 10; i++)
         draw_line(x + 10 + i, y + 12, x + 5, y + 40, 0x1F, 0x3F, 0);
      x += 5;

      for (i = 0; i < 12; i++)
         for (j = 0; j < 12; j++)
            draw_line(x + i, y, x - 8 + j, y + 20, 0x1F, 0x3F, 0);
   }
}
void draw_temp_humid(int x, int y) {

   draw_num(temp_10, x, y);

   x += 25;
   draw_num(temp_1, x, y);

   x += 25;
   draw_num(10, x, y);  //dot

   x += 15;
   draw_num(temp_01, x, y);

   x += 25;
   draw_num(11, x, y);   // 도

   x += 10;
   draw_num(16, x, y);   // 씨 (C)

   x += 60;
   draw_num(humid_10, x, y);

   x += 25;
   draw_num(humid_1, x, y);

   x += 25;
   draw_num(10, x, y);

   x += 15;
   draw_num(humid_01, x, y);

   x += 25;
   draw_num(15, x, y);
}
void draw_vol_cur(int x, int y) {
   draw_num(vol_1, x, y);

   x += 25;
   draw_num(10, x, y);  // dot

   x += 10;
   draw_num(vol_01, x, y);

   x += 25;
   draw_num(vol_001, x, y);

   x += 30;
   draw_num(12, x, y);   // V

   x += 50;
   draw_num(cur_100, x, y);

   x += 25;
   draw_num(cur_10, x, y);

   x += 25;
   draw_num(10, x, y);  // dot

   x += 15;
   draw_num(cur_1, x, y);

   x += 25;
   draw_num(17, x, y);      // m

   x += 25;
   draw_num(13, x, y);      // A
}
void clear_screen(int r, int g, int b) {
   draw_square(0, 0, SCREEN_WIDTH - 1, SCREEN_HEIGHT - 1, r, g, b);
}
void draw_square(int x1, int y1, int x2, int y2, int r, int g, int b) {
   int x, y;
   for (x = x1; x <= x2; x++)
      for (y = y1; y <= y2; y++)
         FILL_PIXEL(x, y, r, g, b);
}
void swap(int *x1, int *x2) {
   int temp = *x1;
   *x1 = *x2;
   *x2 = temp;
}
void draw_line(int x1, int y1, int x2, int y2, int r, int g, int b) {
   int is_steep;

   if (abs(y1 - y2) > abs(x1 - x2))
      is_steep = 1;
   else
      is_steep = 0;

   if (is_steep) {
      swap(&x1, &y1);
      swap(&x2, &y2);
   }

   if (x1 > x2) {
      swap(&x1, &x2);
      swap(&y1, &y2);
   }

   int deltax = x2 - x1;
   int deltay = abs(y2 - y1);
   int error = -(deltax / 2);
   int y = y1;
   int y_step;

   if (y1 < y2) y_step = 1;
   else y_step = -1;
   int x;
   for (x = x1; x <= x2; x++) {
      if (is_steep)
         FILL_PIXEL(y, x, r, g, b);
      else
         FILL_PIXEL(x, y, r, g, b);

      error = error + deltay;

      if (error >= 0) {
         y = y + y_step;
         error = error - deltax;
      }
   }
}
void wait_for_vsync() {
   register int status;
   *pixel_ctrl_ptr = 1;

   status = *(pixel_ctrl_ptr + 3);
   while ((status & 0x01) != 0)
      status = *(pixel_ctrl_ptr + 3);
}
void update_voltage(void) {
   int a;
   vol_1 = voltage_100times / 100; //  일의 자리
   a = voltage_100times % 100;
   vol_01 = a / 10; // 소수점 첫째
   vol_001 = a % 10; // 소수점 둘째
}
void update_current(void) {
   int a;
   cur_100 = current_mA / 100; //  백의 자리
   a = current_mA % 100;
   cur_10 = a / 10; // 십의 자리
   cur_1 = a % 10; // 일의 자리
}
//온습도 센서 부분(구현 실패)
/*
void dht22(void)
{
   int hhttp, signal;
   int Temperature = 0;
   int Humidity = 0;
   checksum = 0;

   unsigned char buf[5] = { 0, };
   //신호 보드에서 센서로
   *(GPIO_ptr + 1) = 0x0010;//direction bit servo output D4 OUTPUT
   *(GPIO_ptr) = 0x0010;//D4 high
   *(GPIO_ptr) = 0x0000;//D4 low
   delay_us(1000 * 20);
   *(GPIO_ptr) = 0x0010;//D4 high
   //Tbe end

   delay_us(30 * 20);

   *(GPIO_ptr) = 0x0000;//D4 low
   //Tgo END
   *(GPIO_ptr + 1) = 0x0000;//direction bit servo output D4 INPUT
   if (*(GPIO_ptr) == 0x0010)
   {
      printf("error at Tbe\n");
      return;
   }
   delay_us(80 * 20);//rel END
   if (*(GPIO_ptr) == 0)
   {
      printf("error at Trel\n");
      return;
   }
   delay_us(80 * 20);//reh END
   if (*(GPIO_ptr) == 0x0010)
   {
      printf("error at Treh\n");
      return;
   }
   //신호 센서에서 보드로
   //8비트씩 5번 
   //각각 humidity high, humidity low, temperature high, temperature low, parity bit
   for (hhttp = 0; hhttp < 5; hhttp++)
   {
      for (signal = 0; signal < 8; signal++)
      {
         if (signal == 7)
            buf[hhttp] |= bit_discrimination();
         else
         {
            buf[hhttp] |= bit_discrimination();//or연산으로 넣어준다.
            buf[hhttp] = buf[hhttp] << 1;//한비트씩 shift시켜 넣을 공간확보
         }
      }
   }
  


   delay_us(50 * 20);
   //Ten END
 
   //parity check
   //buf[0],[1] humidity buf[2],[3] temperature, buf[4] check parity
   for (hhttp = 0; hhttp < 4; hhttp++)
   {
      checksum += buf[hhttp];
   }

   if (buf[4] == checksum)
   {
      Humidity |= buf[0] << 8;
      Humidity |= buf[1];
      Temperature |= buf[2] << 8;
      Temperature |= buf[3];
   }
   else
   {

      printf("parity check error!\n");
   }
   //TEMP_HUM_HEX5_0_DISPLAY(Temperature, Humidity);
   printf("CHECKSUM: %d\n", checksum);
   printf("TEMP: %d\n", Temperature);
   printf("HUMID: %d\n", Humidity);
   Humid = Humidity;
   Temp = Temperature;

   delay_us(2000000 * 20);//최소 2초의 텀을 두고 update
}
unsigned char bit_discrimination()
{
   //Tlow TH0,TH1 sequence
   unsigned char bitd = 0;
   while (*(GPIO_ptr) == 0)
      delay_us(20);//wait until Th0,Th1
      //Tlow end
   delay_us(960);//threshold value 48us
   if (*(GPIO_ptr) == 0)
      bitd |= 0;//decision 0
   else
   {
      bitd |= 1;//decision 1
      while (*(GPIO_ptr) == 1)
         delay_us(20);
   }
   return bitd;
} */