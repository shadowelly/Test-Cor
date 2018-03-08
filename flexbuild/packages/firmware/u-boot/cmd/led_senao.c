#include <common.h>
#include <command.h>
#include <mapmem.h>

#define GPIO_BASE		0x2300000
#define GPIO1			(GPIO_BASE+0x00000)
#define GPIO2			(GPIO_BASE+0x10000)
#define GPIO3			(GPIO_BASE+0x20000)
#define GPIO4			(GPIO_BASE+0x30000)
#define GPIO_DIRECTION		0x0
#define GPIO_DATA		0x8

#define GPIO1_DIRECTION		(GPIO1+GPIO_DIRECTION)
#define GPIO1_DATA		(GPIO1+GPIO_DATA)
#define GPIO2_DIRECTION		(GPIO2+GPIO_DIRECTION)
#define GPIO2_DATA		(GPIO2+GPIO_DATA)
#define GPIO3_DIRECTION		(GPIO3+GPIO_DIRECTION)
#define GPIO3_DATA		(GPIO3+GPIO_DATA)
#define GPIO4_DIRECTION         (GPIO4+GPIO_DIRECTION)
#define GPIO4_DATA              (GPIO4+GPIO_DATA)


//#define GPIO1_LED 		0xfea10000
//#define GPIO2_LED 		0x0000a407
//#define GPIO3_LED 		0x00e00100

#ifndef SENAO_FIX_GPIO
	#define GPIO1_LED 		0xfea10000
	#define GPIO2_LED 		0x0000b477
	#define GPIO3_LED 		0x00e00100
#else
	#define GPIO1_LED		0xfea10000
	#define GPIO2_LED		0x0000af07
	#define GPIO3_LED		0x10e00100
	#define GPIO4_LED		0x00000400
	#define ORANGE_LED		(1<<28)
#endif

#define OUTPUT			1
#define INPUT			0

#define LITTLE_ENDIAN(x)	((x / 8) * 16 + 7) - x
//#define DEBUG

int myatoi(char *str)
{
	int i = 0, result = 0;

	while(str[i] != '\0') {
		result = (result * 10) + (str[i] - '0');
		i++;
	}
	return result;
}



void gpio_read_value(ulong addr, uint32_t *value)
{
	ulong bytes = 1;
	const void *buf = map_sysmem(addr, bytes);
	*value = *(volatile uint32_t *)buf;
	unmap_sysmem(buf);
}


void gpio_write_value(ulong addr, uint32_t value, int ZO)
{
	ulong bytes = 1;
	uint32_t writeval=value;
#ifdef DEBUG
	uint32_t readval;
#endif

	const void *buf = map_sysmem(addr, bytes);
#ifdef DEBUG
	readval = *(volatile uint32_t *)buf;
	printf("0x%08lx - Before modify: 0x%08x\n", addr, readval);
#endif
	*(volatile uint32_t *)buf = (writeval) ;
#ifdef DEBUG
	readval = *(volatile uint32_t *)buf;
	printf("0x%08lx - After modify: 0x%08x\n", addr, readval);
#endif
	unmap_sysmem(buf);
}

void gpio_write_bit(ulong addr, int pos, int value)
{
	ulong bytes = 1;
	uint32_t writeval, readval;

	const void *buf = map_sysmem(addr, bytes);

	readval = *(volatile uint32_t *)buf;
#ifdef DEBUG
	printf("0x%08lx - Before modify: 0x%08x\n", addr, readval);
#endif
	if (value == 1)
		writeval = readval | (0x1 << (pos));
	else
		writeval = readval & ~(0x1 << (pos));
#ifdef DEBUG
	readval = *(volatile uint32_t *)buf;
	printf("0x%08lx - After modify: 0x%08x\n", addr, readval);
#endif
	*(volatile uint32_t *)buf = writeval;
	unmap_sysmem(buf);
}

int gpio_read_bit(ulong addr, int pos)
{
	ulong bytes = 1;
	uint32_t readvalue = 0;
	int result=0;

        const void *buf = map_sysmem(addr, bytes);

	readvalue = *(volatile uint32_t *)buf;
	unmap_sysmem(buf);
	result = (int)(readvalue & (0x1 << (pos)));
	if (result == 0)
		return 0;
	else
		return 1;
#if 0
	if (result == 0) {
		printf("0x%08lx - 0\n", addr, readvalue);
		return 0;
	}
	else {
                printf("0x%08lx,- 1\n", addr, readvalue);
		return 1;
	}
#endif
}

static void gpio_show(int gpio_num)
{
	ulong direction = 0, data = 0;
	const void *buf;
	uint32_t readvalue;

	if (gpio_num == 1) {
                direction=GPIO1_DIRECTION;
                data=GPIO1_DATA;
        }
        else if (gpio_num == 2) {
                direction=GPIO2_DIRECTION;
                data=GPIO2_DATA;
        }
        else if (gpio_num == 3) {
                direction=GPIO3_DIRECTION;
                data=GPIO3_DATA;
        }
        else if (gpio_num == 4) {
                direction=GPIO4_DIRECTION;
                data=GPIO4_DATA;
        }

	buf = map_sysmem(direction, 1);
	readvalue = *(volatile uint32_t *)buf;
	printf("GPIO%d: Direction:%08x ", gpio_num, readvalue);
        unmap_sysmem(buf);

	buf = map_sysmem(data, 1);
        readvalue = *(volatile uint32_t *)buf;
        printf("Data:%08x\n ", readvalue);
        unmap_sysmem(buf);

}

static int do_gpio_wbl(cmd_tbl_t *cmdtp, int flag, int argc, char *const argv[])
{
	int pos=0;
        if (argc < 4 || argc > 5)
                return CMD_RET_USAGE;

	pos = LITTLE_ENDIAN(myatoi(argv[2]));

        gpio_write_bit(simple_strtoul(argv[1], NULL, 16), pos, myatoi(argv[3]));

        return 0;
}


static int do_gpio_wb(cmd_tbl_t *cmdtp, int flag, int argc, char *const argv[])
{
	if (argc < 4 || argc > 5)
		return CMD_RET_USAGE;

	gpio_write_bit(simple_strtoul(argv[1], NULL, 16), myatoi(argv[2]), myatoi(argv[3]));

	return 0;
}

static int do_gpio_w(cmd_tbl_t *cmdtp, int flag, int argc, char *const argv[])
{
        int pos = 0;
	ulong direction = 0, data = 0;
        if (argc < 4 || argc > 5)
                return CMD_RET_USAGE;

	if (!strcmp(argv[1],"1")) {
		direction=GPIO1_DIRECTION;
		data=GPIO1_DATA;
	}
	else if (!strcmp(argv[1],"2")) {
                direction=GPIO2_DIRECTION;
                data=GPIO2_DATA;
        }
	else if (!strcmp(argv[1],"3")) {
                direction=GPIO3_DIRECTION;
                data=GPIO3_DATA;
        }
	else if (!strcmp(argv[1],"4")) {
                direction=GPIO4_DIRECTION;
                data=GPIO4_DATA;
        }
	pos = LITTLE_ENDIAN(myatoi(argv[2]));

	gpio_write_bit(data, pos, myatoi(argv[3]));
	gpio_write_bit(direction, pos, 1);
        printf("After modified:\n");
        gpio_show(myatoi(argv[1]));

        return 0;
}


static int do_gpio_rbl(cmd_tbl_t *cmdtp, int flag, int argc, char *const argv[])
{
        int value, pos;
        if (argc < 3 || argc > 4)
                return CMD_RET_USAGE;

	pos = LITTLE_ENDIAN(myatoi(argv[2]));

        value = gpio_read_bit(simple_strtoul(argv[1], NULL, 16), pos);

	if (value != 0 && value != 1)
		return 1;
        return 0;
}

static int do_gpio_rb(cmd_tbl_t *cmdtp, int flag, int argc, char *const argv[])
{
	int value;
	if (argc < 3 || argc > 4)
		return CMD_RET_USAGE;

	value = gpio_read_bit(simple_strtoul(argv[1], NULL, 16), myatoi(argv[2]));

        if (value != 0 && value != 1)
                return 1;
        return 0;
}

static int do_gpio_r(cmd_tbl_t *cmdtp, int flag, int argc, char *const argv[])
{
        int value, pos = 0;
	ulong data = 0;
        if (argc < 3 || argc > 4)
                return CMD_RET_USAGE;

        if (!strcmp(argv[1],"1")) {
                data=GPIO1_DATA;
        }
        else if (!strcmp(argv[1],"2")) {
                data=GPIO2_DATA;
        }
        else if (!strcmp(argv[1],"3")) {
                data=GPIO3_DATA;
        }
        else if (!strcmp(argv[1],"4")) {
                data=GPIO4_DATA;
        }

        pos = LITTLE_ENDIAN(myatoi(argv[2]));
        value = gpio_read_bit(data, pos);

        if (value != 0 && value != 1)
                return 1;

	printf("GPIO%s-%d - %d\n", argv[1], myatoi(argv[2]), value);
        return 0;
}


void led_all_on(void)
{
	//STATUS_LED_R
	gpio_write_bitl(GPIO1_DIRECTION, 20, 1);	
	gpio_write_bitl(GPIO1_DATA,      20, 1);
	//STATUS_LED_G
	gpio_write_bitl(GPIO1_DIRECTION, 22, 1);
	gpio_write_bitl(GPIO1_DATA,      22, 1);
	//RADIO_LED_G
	gpio_write_bitl(GPIO1_DIRECTION, 29, 1);
	gpio_write_bitl(GPIO1_DATA,      29, 1);
	//RADIO_LED_R
	gpio_write_bitl(GPIO1_DIRECTION, 30, 1);
	gpio_write_bitl(GPIO1_DATA,      30, 1);
	//POWER_LED_G
	gpio_write_bitl(GPIO4_DIRECTION, 2, 1);
	gpio_write_bitl(GPIO4_DATA,      2, 1);
	//POWER_LED_R
	gpio_write_bitl(GPIO4_DIRECTION, 3, 1);
	gpio_write_bitl(GPIO4_DATA,      3, 1);
    
}

void led_all_off(void)
{
	//STATUS_LED_R
	gpio_write_bitl(GPIO1_DIRECTION, 20, 1);	
	gpio_write_bitl(GPIO1_DATA,      20, 0);
	//STATUS_LED_G
	gpio_write_bitl(GPIO1_DIRECTION, 22, 1);
	gpio_write_bitl(GPIO1_DATA,      22, 0);
	//RADIO_LED_G
	gpio_write_bitl(GPIO1_DIRECTION, 29, 1);
	gpio_write_bitl(GPIO1_DATA,      29, 0);
	//RADIO_LED_R
	gpio_write_bitl(GPIO1_DIRECTION, 30, 1);
	gpio_write_bitl(GPIO1_DATA,      30, 0);
	//POWER_LED_G
	gpio_write_bitl(GPIO4_DIRECTION, 2, 1);
	gpio_write_bitl(GPIO4_DATA,      2, 0);
	//POWER_LED_R
	gpio_write_bitl(GPIO4_DIRECTION, 3, 1);
	gpio_write_bitl(GPIO4_DATA,      3, 0);

}

static int do_led(cmd_tbl_t *cmdtp, int flag, int argc, char *const argv[])
{
	if (argc != 2)
		return CMD_RET_USAGE;

	if (!strcmp(argv[1],"on"))
		led_all_on();
	else if (!strcmp(argv[1],"off") || !strcmp(argv[1],"reset"))
		led_all_off();
	else
		return CMD_RET_USAGE;
	return 0;
}

U_BOOT_CMD(led, 2, 1, do_led, "led control",
	   "\tled on \n"
	   "\tled off");
U_BOOT_CMD(gpiow, 4, 1, do_gpio_w, "write a bit to set GPIO direction and data", "ex: gpiow <GPIO_NUM> <pos> <0/1>");
U_BOOT_CMD(gpiowrite, 4, 1, do_gpio_wb, "write a bit to gpio", "ex: gpiowrite 0x2300000 <pos> <0/1>");
U_BOOT_CMD(gpiowritel, 4, 1, do_gpio_wbl, "write a bit to gpio by little-endian", "ex: gpiowrite 0x2300000 <pos> <0/1>");
U_BOOT_CMD(gpior, 3, 1, do_gpio_r, "read a bit to get GPIO data", "ex: gpior <GPIO_NUM> <pos>");
U_BOOT_CMD(gpioread, 3, 1, do_gpio_rb, "read a bit of gpio", "ex: gpioread 0x2300000 <pos>");
U_BOOT_CMD(gpioreadl, 3, 1, do_gpio_rbl, "read a bit of gpio by little-endian", "ex: gpioread 0x2300000 <pos>");
