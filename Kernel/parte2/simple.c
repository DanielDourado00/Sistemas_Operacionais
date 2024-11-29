#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/list.h>
#include <linux/slab.h>

struct birthday {
    int dia;
    int mes;
    int ano;
    struct list_head lista;
};

static LIST_HEAD(lista_aniver);

/* Função chamada ao carregar o módulo */
int simple_init(void)
{
    struct birthday *novo_aniversario;
    int dias[] = {10, 1, 15, 20, 25};
    int meses[] = {10, 1, 7, 3, 12};
    int anos[] = {1910, 1911, 2000, 1995, 1987};

    printk(KERN_INFO "Carregando módulo: Gerenciador de aniversários\n");

    for (int i = 0; i < 5; i++) {
        novo_aniversario = kmalloc(sizeof(*novo_aniversario), GFP_KERNEL);
        if (!novo_aniversario) {
            printk(KERN_ERR "Falha ao alocar memória para aniversário %d\n", i + 1);
            return -ENOMEM;
        }
        novo_aniversario->dia = dias[i];
        novo_aniversario->mes = meses[i];
        novo_aniversario->ano = anos[i];
        INIT_LIST_HEAD(&novo_aniversario->lista);
        list_add_tail(&novo_aniversario->lista, &lista_aniver);
    }

    struct birthday *dourado;
    list_for_each_entry(dourado, &lista_aniver, lista) {
        printk(KERN_INFO "Aniversário: %02d/%02d/%d\n",
               dourado->dia, dourado->mes, dourado->ano);
    }

    return 0;
}

/* Função chamada ao remover o módulo */
void simple_exit(void)
{
    struct birthday *entrada, *temporario;

    printk(KERN_INFO "Removendo módulo: Limpando lista de aniversários\n");
    list_for_each_entry_safe(entrada, temporario, &lista_aniver, lista) {
        printk(KERN_INFO "Removendo aniversário: %02d/%02d/%d\n",
               entrada->dia, entrada->mes, entrada->ano);
        list_del(&entrada->lista);
        kfree(entrada);
    }
}

module_init(simple_init);
module_exit(simple_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Gerenciador de aniversários com lista encadeada");
MODULE_AUTHOR("Daniel Dourado");
