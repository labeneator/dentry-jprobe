#include <linux/module.h>
#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/kprobes.h>
#include <linux/limits.h>
#include <linux/slab.h>
#include <linux/dcache.h>

/* Interecept calls to d_alloc
 * http://lxr.free-electrons.com/source/fs/dcache.c?v=2.6.33#L917
 * */
int d_alloc_handler(struct dentry * parent, const struct qstr *name) {
    printk(KERN_INFO "d_alloc dirname: %s\n", name->name);
    dump_stack();
    jprobe_return();
    return 0;
}

static struct jprobe d_alloc_probe;

int jprobe_init(void) {
    printk("d_alloc jprobe module inserted\n");
    /* grep -w d_alloc /proc/kallsyms | awk '{print $1}' */
    d_alloc_probe.kp.addr = (kprobe_opcode_t *)0xffffffff8110afba;
    d_alloc_probe.entry = (kprobe_opcode_t *)d_alloc_handler;
    register_jprobe(&d_alloc_probe);
    return 0;
}

void jprobe_exit() {
    unregister_jprobe(&d_alloc_probe);
    printk("d_alloc jprobe module unloaded\n");
}

module_init(jprobe_init);
module_exit(jprobe_exit);
MODULE_AUTHOR("lmwangi");
MODULE_DESCRIPTION("jprobe dentry module");
MODULE_LICENSE("GPL");
