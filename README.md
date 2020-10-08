# Solar-Tracker

최종목표 -  Solar tracker & Power system 구현

1. Solar Tracker
   - 태양 전지가 항상 태양을 바라보도록 한다
   ① 서보모터 
- PWM 방식
- YAW, PITCH 제어용 서보모터 2개
   ② 광센서
- 총 4개를 이용해서 YAW 상하, PITCH 좌우 4방향 결정
- 아날로그 출력
- De1-SoC의 내장 ADC 이용
2. Power System 
- 태양전지를 이용해서 Power system 구성한다. 부하를 구동한다. 
① 6V 태양전지
② DC to DC converter 
 - 배터리에 충전전압에 맞춰서 전압 변동
③ 전압계
   - 아날로그 출력
   - 태양전지 출력 전압 측정
   - 부하가 걸리는 전압 측정
④ 전류계
   - 아날로그 출력
⑤ Li-ion 배터리
   - 3.7V, 2500mAh
