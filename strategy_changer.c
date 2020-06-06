#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include "ouichefs.h"

MODULE_DESCRIPTION("Change the strategy of compare.");
MODULE_LICENSE("GPL");
int (*saved_strategy)(struct inode *a, struct inode *b) = NULL;

strategy_by_size(struct inode *a, struct inode *b)
{
	pr_info("compare_by_size\n");
	return a->i_size - b->i_size;
}
static int __init uname_init(void)
{
	saved_strategy = ouichefs_fblocks_strategy;
	ouichefs_fblocks_strategy = strategy_by_size;
	pr_info("New strategy used\n");


	return 0;
}
module_init(uname_init);

static void __exit uname_exit(void)
{

	ouichefs_fblocks_strategy = saved_strategy;
	
	pr_info("Restaured strat");

}
module_exit(uname_exit);
