#ifndef AMIGAFFS_H
#define AMIGAFFS_H

#include <linux/types.h>
#include <endian.h>
#include <byteswap.h>

/* AmigaOS allows file names with up to 30 characters length.
 * Names longer than that will be silently truncated. If you
 * want to disallow this, comment out the following #define.
 * Creating filesystem objects with longer names will then
 * result in an error (ENAMETOOLONG).
 */
/*#define AFFS_NO_TRUNCATE */

/* Ugly macros make the code more pretty. */

#define GET_END_PTR(st,p,sz)		 ((st *)((char *)(p)+((sz)-sizeof(st))))
#define AFFS_GET_HASHENTRY(data,hashkey) be32_to_cpu(((struct dir_front *)data)->hashtable[hashkey])
#define AFFS_BLOCK(sb, bh, blk)		(AFFS_HEAD(bh)->table[AFFS_SB(sb)->s_hashsize-1-(blk)])



#ifdef __LITTLE_ENDIAN
#define BO_EXBITS	0x18UL
#elif defined(__BIG_ENDIAN)
#define BO_EXBITS	0x00UL
#else
#error Endianness must be known for affs to work.
#endif

#define FS_OFS		0x444F5300
#define FS_FFS		0x444F5301
#define FS_INTLOFS	0x444F5302
#define FS_INTLFFS	0x444F5303
#define FS_DCOFS	0x444F5304
#define FS_DCFFS	0x444F5305
#define MUFS_FS		0x6d754653   /* 'muFS' */
#define MUFS_OFS	0x6d754600   /* 'muF\0' */
#define MUFS_FFS	0x6d754601   /* 'muF\1' */
#define MUFS_INTLOFS	0x6d754602   /* 'muF\2' */
#define MUFS_INTLFFS	0x6d754603   /* 'muF\3' */
#define MUFS_DCOFS	0x6d754604   /* 'muF\4' */
#define MUFS_DCFFS	0x6d754605   /* 'muF\5' */

#define T_SHORT		2
#define T_LIST		16
#define T_DATA		8

#define ST_LINKFILE	-4
#define ST_FILE		-3
#define ST_ROOT		1
#define ST_USERDIR	2
#define ST_SOFTLINK	3
#define ST_LINKDIR	4

#define AFFS_ROOT_BMAPS		25

#define AFFS_HEAD(bh)		((struct affs_head *)(bh)->b_data)
#define AFFS_TAIL(sb, bh)	((struct affs_tail *)((bh)->b_data+(sb)->s_blocksize-sizeof(struct affs_tail)))
#define AFFS_ROOT_HEAD(bh)	((struct affs_root_head *)(bh)->b_data)
#define AFFS_ROOT_TAIL(sb, bh)	((struct affs_root_tail *)((bh)->b_data+(sb)->s_blocksize-sizeof(struct affs_root_tail)))
#define AFFS_DATA_HEAD(bh)	((struct affs_data_head *)(bh)->b_data)
#define AFFS_DATA(bh)		(((struct affs_data_head *)(bh)->b_data)->data)

struct affs_date {
	__u32 days;
	__u32 mins;
	__u32 ticks;
};

struct affs_short_date {
	__u16 days;
	__u16 mins;
	__u16 ticks;
};

struct affs_root_head {
	__u32 ptype;
	__u32 spare1;
	__u32 spare2;
	__u32 hash_size;
	__u32 spare3;
	__u32 checksum;
	__u32 hashtable[1];
};

struct affs_root_tail {
	__u32 bm_flag;
	__u32 bm_blk[AFFS_ROOT_BMAPS];
	__u32 bm_ext;
	struct affs_date root_change;
	__u8 disk_name[32];
	__u32 spare1;
	__u32 spare2;
	struct affs_date disk_change;
	struct affs_date disk_create;
	__u32 spare3;
	__u32 spare4;
	__u32 dcache;
	__u32 stype;
};

struct affs_head {
	__u32 ptype;
	__u32 key;
	__u32 block_count;
	__u32 spare1;
	__u32 first_data;
	__u32 checksum;
	__u32 table[1];
};

struct affs_tail {
	__u32 spare1;
	__u16 uid;
	__u16 gid;
	__u32 protect;
	__u32 size;
	__u8 comment[92];
	struct affs_date change;
	__u8 name[32];
	__u32 spare2;
	__u32 original;
	__u32 link_chain;
	__u32 spare[5];
	__u32 hash_chain;
	__u32 parent;
	__u32 extension;
	__u32 stype;
};

struct slink_front
{
	__u32 ptype;
	__u32 key;
	__u32 spare1[3];
	__u32 checksum;
	__u8 symname[1];	/* depends on block size */
};

struct affs_data_head
{
	__u32 ptype;
	__u32 key;
	__u32 sequence;
	__u32 size;
	__u32 next;
	__u32 checksum;
	__u8 data[1];	/* depends on block size */
};

/* Permission bits */

#define FIBF_OTR_READ		0x8000
#define FIBF_OTR_WRITE		0x4000
#define FIBF_OTR_EXECUTE	0x2000
#define FIBF_OTR_DELETE		0x1000
#define FIBF_GRP_READ		0x0800
#define FIBF_GRP_WRITE		0x0400
#define FIBF_GRP_EXECUTE	0x0200
#define FIBF_GRP_DELETE		0x0100

#define FIBF_HIDDEN		0x0080
#define FIBF_SCRIPT		0x0040
#define FIBF_PURE		0x0020		/* no use under linux */
#define FIBF_ARCHIVED		0x0010		/* never set, always cleared on write */
#define FIBF_NOREAD		0x0008		/* 0 means allowed */
#define FIBF_NOWRITE		0x0004		/* 0 means allowed */
#define FIBF_NOEXECUTE		0x0002		/* 0 means allowed, ignored under linux */
#define FIBF_NODELETE		0x0001		/* 0 means allowed */

#define FIBF_OWNER		0x000F		/* Bits pertaining to owner */
#define FIBF_MASK		0xEE0E		/* Bits modified by Linux */

#endif