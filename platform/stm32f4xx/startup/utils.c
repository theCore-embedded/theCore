/*
 * Definition of common utils
 */

/* Required for STM32 Peripherial library */
void assert_param(int exp)
{
	if (!exp)
		for(;;);
}
