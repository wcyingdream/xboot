/*
 * kernel/command/cmd-test.c
 */

#include <command/command.h>
#include <mmc_op.h>
#include "mmc_def.h"
#include "mmc.h"

static void usage(void)
{
	printf("usage:\r\n");
	printf("    test [args ...]\r\n");
}

#define CCU_Addr 0x01C20000

u8_t Read_In[1024];

static int do_test(int argc, char ** argv)
{
	s32_t card_no=0;
	u32_t old_data;
	char buf[64];

	//to xboot, if init these code, sdio can not be run
	#if(0)
	static const struct mmap_t mach_map[] = {
		{"ram",  0x40000000, 0x40000000, SZ_16M, MAP_TYPE_CB},
		{"dma",  0x41000000, 0x41000000, SZ_16M, MAP_TYPE_NCNB},
		{"heap", 0x42000000, 0x42000000, SZ_32M, MAP_TYPE_CB},
		{ 0 },
		};
	mmu_setup(mach_map);
	#endif

	old_data = 0;
	old_data = readl(CCU_Addr + 0x60);
	old_data = (old_data|(1<<6)|(1<<8));
	writel(old_data, CCU_Addr + 0x60);

	old_data = 0;
	old_data = readl(CCU_Addr + 0x02C0);
	old_data = (old_data|(1<<6)|(1<<8));
	writel(old_data, CCU_Addr + 0x02C0);

	if( SDMMC_PhyInit( card_no, 4 ) == -1)   //高速卡，4线配置
	{
		printf("Fail in Init sdmmc.\r\n");
	}


	printf("sdcard %d init ok\r\n", card_no);
	/* load 1k boot1 head */
	if( SDMMC_PhyRead( 0, 1024/512, (void *)Read_In, card_no ) != (1024/512))
	{
		printf("Fail in reading boot1 head.\r\n");
	}
	else
	{
		int i;
		for(i = 0; i < 1024; i++)
		{
			if((i & 0xf) == 0)
				printf("\r\n0x%08x:", i);
			printf(" %02x", Read_In[i]);
		}
	}
	return 0;
}

static struct command_t cmd_test = {
	.name	= "test",
	.desc	= "debug command for programmer",
	.usage	= usage,
	.exec	= do_test,
};

static __init void test_cmd_init(void)
{
	register_command(&cmd_test);
}

static __exit void test_cmd_exit(void)
{
	unregister_command(&cmd_test);
}

command_initcall(test_cmd_init);
command_exitcall(test_cmd_exit);
