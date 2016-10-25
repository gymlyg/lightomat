#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"

#define STOP_SIGNAL GPIO_ODR_ODR7
#define YELLOW_SIGNAL GPIO_ODR_ODR8
#define GO_SIGNAL GPIO_ODR_ODR9


enum proc_state {JUST_STOP, STOP_AND_READY, PLEASE_GO, GO_BLINKED, PSEVDO_STOP};


#define ONE_SEC 2000000

void setup_mf() {
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB , ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;

	GPIO_Init( GPIOB , &GPIO_InitStructure);

}

void delay(unsigned long p)
{
	unsigned long i;
	for(i=0;i<p ;i++);
}

void setPinOn(uint16_t pin) {
	GPIOB->ODR |= pin;//GPIO_ODR_ODR9;
}

void setPinOff(uint16_t pin) {
	GPIOB->ODR &= ~pin;//GPIO_ODR_ODR9;
}

void setPinBlinked(uint16_t pin, unsigned int blink_amount, unsigned int blink_period) {
	unsigned int i = 0;
	for( i = 0; i<blink_amount; i++) {
		setPinOn(pin);
		delay(blink_period);
		setPinOff(pin);
		delay(blink_period);
	}
}

void procState(unsigned int state) {
	switch(state) {
	case JUST_STOP:
		setPinOn(STOP_SIGNAL);
		delay(10 * ONE_SEC);
		setPinOff(STOP_SIGNAL);
		break;
	case STOP_AND_READY:
		setPinOn(STOP_SIGNAL);
		setPinOn( YELLOW_SIGNAL);
		delay(3 * ONE_SEC);
		setPinOff(STOP_SIGNAL);
		setPinOff( YELLOW_SIGNAL);
		break;
	case PLEASE_GO:
		setPinOn(GO_SIGNAL);
		delay(10 * ONE_SEC);
		setPinOff(GO_SIGNAL);
		break;
	case GO_BLINKED:
		setPinBlinked(GO_SIGNAL, 3, ONE_SEC);
		setPinOff(GO_SIGNAL);
		break;
	case PSEVDO_STOP:
		setPinOn(YELLOW_SIGNAL);
		delay(3 * ONE_SEC);
		setPinOff(YELLOW_SIGNAL);
	default:
		break;
	}

}

int main(void)
{
	enum proc_state state = JUST_STOP;
	setup_mf();

	while (1) {
		procState(state);
		state++;
		if(state > PSEVDO_STOP ) {
			state = JUST_STOP;
		}
	}

	return 0;
}
