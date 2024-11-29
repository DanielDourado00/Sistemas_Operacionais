#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>

/* Função chamada ao carregar o módulo */
int simple_init(void)
{
    printk(KERN_INFO "Loading Module: Hello Kernel!\n");
    return 0;
}

/* Função chamada ao remover o módulo */
void simple_exit(void)
{
    printk(KERN_INFO "Removing Module: Goodbye Kernel!\n");
}

/* Registrando os pontos de entrada e saída do módulo */
module_init(simple_init);
module_exit(simple_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Simple Linux Kernel Module");
MODULE_AUTHOR("Daniel Dourado");