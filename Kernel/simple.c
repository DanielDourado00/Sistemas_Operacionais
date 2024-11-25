#include <linux/module.h>
#include <linux/kernel.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Seu Nome");
MODULE_DESCRIPTION("Um módulo simples do kernel");
MODULE_VERSION("1.0");

static int __init simple_module_init(void) {
    printk(KERN_INFO "Módulo carregado: Olá, kernel!\n");
    return 0;
}

static void __exit simple_module_exit(void) {
    printk(KERN_INFO "Módulo descarregado: Adeus, kernel!\n");
}

module_init(simple_module_init);
module_exit(simple_module_exit);
