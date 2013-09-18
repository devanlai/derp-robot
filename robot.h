#ifndef __ROBOT_H__
#define __ROBOT_H__

#define SWAP_32(X) ((((X) & 0xff) << 24) | (((X) & 0xff00) << 8) | (((X) & 0xff0000) >> 8)  | (((X) & 0xff000000) >> 24))
#define SWAP_16(X) ((((X) & 0xff) << 8) | (((X) & 0xff00) >> 8))

#endif
