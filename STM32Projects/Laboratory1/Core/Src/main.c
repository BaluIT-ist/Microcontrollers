int main(void)
{
RCC->AHBENR |=(1UL<<18U)|(1UL<<17U);
GPIOB->MODER=0X00055000;
GPIOB->ODR=0X00000200;

while(1);
return 0;
}
