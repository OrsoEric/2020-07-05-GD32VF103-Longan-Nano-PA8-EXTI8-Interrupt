#include <gd32vf103.h>
#define EVER (;;)

void init()
{
	//Clock the GPIO banks
	rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_GPIOC);
    //Setup the R, G and B LEDs
    gpio_init(GPIOA, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ,GPIO_PIN_1);
    gpio_init(GPIOA, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ,GPIO_PIN_2);
	gpio_init(GPIOA, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ,GPIO_PIN_8);
	//Setup the boot button
	gpio_init(GPIOC, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ,GPIO_PIN_13);
    //Initialize the LEDs to: OFF
    gpio_bit_set(GPIOC,GPIO_PIN_13);
    gpio_bit_set(GPIOA,GPIO_PIN_1);
    gpio_bit_set(GPIOA,GPIO_PIN_2);
	
	//Clock the alternate functions
	rcu_periph_clock_enable(RCU_AF);
	
	//Initialize the ECLIC IRQ lines
	eclic_priority_group_set(ECLIC_PRIGROUP_LEVEL3_PRIO1);
	eclic_irq_enable(EXTI5_9_IRQn, 1, 1);

	//Initialize the EXTI. IRQ can be generated from GPIO edge detectors
	gpio_exti_source_select(GPIO_PORT_SOURCE_GPIOA, GPIO_PIN_SOURCE_8);
	exti_init(EXTI_8, EXTI_INTERRUPT, EXTI_TRIG_BOTH);
	
	//Clear interrupt flag. Ensure no spurious execution at start
	exti_interrupt_flag_clear(EXTI_8);
	
	//Enable the interrupts. From now on interrupt handlers can be executed
	eclic_global_interrupt_enable();	

	return;
}

extern "C"
void EXTI5_9_IRQHandler()
{
	//If: interrupt from PA8 boot button
	if (exti_interrupt_flag_get(EXTI_8) != RESET)
	{
		//Clear the interrupt from PA8 boot button
		exti_interrupt_flag_clear(EXTI_8);
		//Toggle the blue led
		gpio_bit_write(GPIOA, GPIO_PIN_2, (bit_status)(1-gpio_input_bit_get(GPIOA, GPIO_PIN_2)));
	}
	//Default: interrupt from an unhandled GPIO
	else
	{
		//Do nothing (should clear the interrupt flags)
	}
}

void delay_us(unsigned int us)
{
    uint64_t start_mtime, delta_mtime;
    
	uint64_t tmp = get_timer_value();

    do
    {
        start_mtime = get_timer_value();
    }
    while (start_mtime == tmp);

    do
    {
        delta_mtime = get_timer_value() - start_mtime;
    }
    while(delta_mtime <(SystemCoreClock/4000000.0 *us ));

    return;
}

int main()
{
	init();

	for EVER
	{
		//Toggle the RED LED
		gpio_bit_write(GPIOC, GPIO_PIN_13, (bit_status)(1-gpio_input_bit_get(GPIOC, GPIO_PIN_13)));
		//2Hz blink
		delay_us(250000);
	}

	return 0;
}
