#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/genetlink.h>
#include <net/genetlink.h>

#include "./genl_header.h"

/* attribute policy */
static struct nla_policy instance_genl_policy[INSTANCE_A_MAX + 1] = {
   [INSTANCE_A_MSG] = { .type = NLA_NUL_STRING },
};

/* handler */
int instance_echo(struct sk_buff *skb, struct genl_info *info);

/* operation definition */
static struct genl_ops instance_genl_ops[INSTANCE_C_MAX] = {
    {
        .cmd = INSTANCE_C_ECHO,
        .flags = 0,
        .policy = instance_genl_policy,
        .doit = instance_echo,
        .dumpit = NULL,
   },
};

/* family definition */
static struct genl_family instance_genl_family = {
   //.id = GENL_ID_GENERATE,
   .id = 0,
   .hdrsize = 0,
   .name = GENL_INSTANCE_NAME,
   .version = 1,
   .maxattr = INSTANCE_A_MAX,
   .netnsok = true,
   .parallel_ops = true,
   .ops = instance_genl_ops,
   .n_ops = ARRAY_SIZE(instance_genl_ops),
   .module = THIS_MODULE,
};

int instance_echo(struct sk_buff *skb, struct genl_info *info)
{
    struct nlattr *na;
    struct sk_buff *skb_echo = NULL;
    int rc;
    void *msg_head;
    char *mydata;
 
    if (info == NULL) {
        goto out;
    }
  
    /* For each attribute there is an index in info->attrs
     * which points to a nlattr structure in this structure
     * the data is given
     */
    na = info->attrs[INSTANCE_A_MSG];
    if (na) {
        mydata = (char *) nla_data(na);
        if (mydata == NULL) {
            printk(KERN_ERR "error while receiving data\n");
        } else {
            /* shuld be validate the contend of data before print */
            printk(KERN_INFO "received: %s\n", mydata);
        }
    } else {
        printk(KERN_INFO "no info->attrs %i\n", INSTANCE_A_MSG);
    }

    /* Send a message back */
    /* Allocate some memory, since the size is not
     * yet known use NLMSG_GOODSIZE
     */
    skb_echo = genlmsg_new(NLMSG_GOODSIZE, GFP_KERNEL);
    if (skb_echo == NULL) {
        goto out;
    }

    /* Create the message headers */
    /* arguments of genlmsg_put: 
     * struct sk_buff *, 
     * int (sending) pid, 
     * int sequence number, 
     * struct genl_family *, 
     * int flags, 
     * u8 command index (why do we need this?)
     */
    msg_head = genlmsg_put(skb_echo, 0, info->snd_seq + 1,
                           &instance_genl_family, 0, INSTANCE_C_ECHO);
    if (msg_head == NULL) {
        rc = -ENOMEM;
        goto out;
    }

    /* Add a INSTANCE_A_MSG attribute (actual value to be sent) */
    rc = nla_put_string(skb_echo, INSTANCE_A_MSG,
                        "Hello world from kernel space");
    if (rc != 0) {
        goto out;
    }
 
    /* Finalize the message */
    genlmsg_end(skb_echo, msg_head);

    /* Send the message back */
    rc = genlmsg_unicast(genl_info_net(info), skb_echo, info->snd_portid);
    if (rc != 0) {
        goto out;
    }

    return 0;

out:
    if (!skb_echo) {
        kfree_skb(skb_echo);
    }
    printk(KERN_ERR "an error occured in instance_echo\n");
    return 0;
}

static int __init genl_instance_init(void)
{
    int rc;

    rc = genl_register_family(&instance_genl_family);
    if (rc != 0) {
        printk(KERN_ERR "register instance genl family fail, err %d\n", rc);
        return rc;
    }

    printk(KERN_INFO "register %s genl family success\n", GENL_INSTANCE_NAME);

    return 0;
}

static void genl_instance_uninit(void)
{
    genl_unregister_family(&instance_genl_family);

    printk(KERN_INFO "unregister %s genl family success\n", GENL_INSTANCE_NAME);
}

module_init(genl_instance_init);
module_exit(genl_instance_uninit);

MODULE_DESCRIPTION("generic netlink instance");
MODULE_LICENSE("GPL");
MODULE_VERSION("0.1");
MODULE_ALIAS_GENL_FAMILY(GENL_INSTANCE_NAME);
