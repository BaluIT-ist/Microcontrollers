#include "stm32f3xx.h"

int main(void){



	RCC->AHBENR |=(1UL<<18U)|(1UL<<17U);
	GPIOB->MODER=0X00055000;
	//0x40-red
	//0x80-blue
	//0x100-orange
	//0x200-green
	//0x000 all off
	//0x3C0 all on

int state = 0;
int previous = 0;

    while (1) {
        int now = GPIOA->IDR & 0x1;
        if (now && !previous) {
            state = (state + 1) % 5;
        }
        previous = now;

        switch (state) {
            case 0: GPIOB->ODR = 0x40; break;
            case 1: GPIOB->ODR = 0x140; break;
            case 2: GPIOB->ODR = 0x1C0; break;
            case 3: GPIOB->ODR = 0x3C0; break;
            case 4: GPIOB->ODR = 0x000; break;
        }
    }
}
