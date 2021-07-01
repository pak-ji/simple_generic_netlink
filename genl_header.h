#define GENL_INSTANCE_NAME "gennl_ins"

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(_x)  ((sizeof(_x))/sizeof (_x)[0])
#endif

/* commands */
enum {
   INSTANCE_C_UNSPEC,
   INSTANCE_C_ECHO,
   __INSTANCE_C_MAX,
};

#define INSTANCE_C_MAX (__INSTANCE_C_MAX - 1)

/* attributes */
enum {
   INSTANCE_A_UNSPEC,
   INSTANCE_A_MSG,
   __INSTANCE_A_MAX,
};

#define INSTANCE_A_MAX (__INSTANCE_A_MAX - 1)
