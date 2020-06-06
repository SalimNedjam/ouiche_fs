#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include "ouichefs.h"

MODULE_DESCRIPTION("Change the strategy of compare.");
MODULE_LICENSE("GPL");

int (*default_strategy)(struct inode *a, struct inode *b) = NULL;

int ouichefs_strategy_size(struct inode *a, struct inode *b)
{
	return b->i_size - a->i_size;
}

static int __init ouichefs_strategy_changer_init(void)
{
	/* Sauvegarde la stratégie actuelle */
	default_strategy = ouichefs_fblocks_strategy;
	/* Attribue notre stratégie */
	ouichefs_fblocks_strategy = ouichefs_strategy_size;
	pr_info("New ouichefs free blocks strategy applied\n");

	return 0;
}
module_init(ouichefs_strategy_changer_init);

static void __exit ouichefs_strategy_changer_exit(void)
{
	/* Restaure la stratégie par défaut */
	ouichefs_fblocks_strategy = default_strategy;
	pr_info("ouichefs free blocks strategy restored");

}
module_exit(ouichefs_strategy_changer_exit);
