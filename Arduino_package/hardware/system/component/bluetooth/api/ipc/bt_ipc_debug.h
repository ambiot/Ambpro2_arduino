/**
*****************************************************************************************
*     Copyright(c) 2021, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file     bt_ipc_debug.h
  * @brief    head file for bt ipc debug.
  * @details  bt ipc debug api.
  * @author   sherman
  * @date     2021-10-20
  * @version  v1.0
  * *************************************************************************************
  */

#ifndef _BT_IPC_DEBUG_H_
#define _BT_IPC_DEBUG_H_

// #define CONFIG_BT_IPC_DEBUG

#ifdef CONFIG_BT_IPC_DEBUG
#define _dbgdump	printf("\n\r"); printf
#define PREFIX	"[BIPC]: "
#if	defined (_dbgdump)
#undef DBG_BT_IPC
#define DBG_BT_IPC(...)     do {\
		_dbgdump(PREFIX __VA_ARGS__);\
	}while(0)
#endif
#else
#define DBG_BT_IPC(x, ...) do {} while(0)
#endif /* CONFIG_BT_IPC_DEBUG */

#endif /* _BT_IPC_DEBUG_H_ */
