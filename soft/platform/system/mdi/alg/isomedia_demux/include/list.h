/* Copyright (C) 2016 RDA Technologies Limited and/or its affiliates("RDA").
* All rights reserved.
*
* This software is supplied "AS IS" without any warranties.
* RDA assumes no responsibility or liability for the use of the software,
* conveys no license or title under any patent, copyright, or mask work
* right to the product. RDA reserves the right to make changes in the
* software without notification.  RDA also make no representation or
* warranty that such application will be suitable for the specified use
* without further testing or modification.
*/

#ifndef SHEEN_VC_DEBUG
#include "global.h"
#endif

#ifdef MP4_3GP_SUPPORT

#ifndef _LIST_H_
#define _LIST_H_

#include "tools.h"

struct _tag_array
{
    void **slots;
    uint32 entryCount;
};

typedef struct _tag_array GF_List;


/*!
 *  \brief list constructor
 *
 *  Constructs a new list object
 *  \return new list object
 */
GF_List *gf_list_new();
/*!
 *  \brief list destructor
 *
 *  Destructs a list object
 *  \param ptr list object to destruct
 *  \note It is the caller responsability to destroy the content of the list if needed
 */
void gf_list_del(GF_List *ptr);

uint32 gf_list_count(GF_List *ptr);
/*!
 *  \brief add item
 *GF_Err
 *  Adds an item at the end of the list
 *  \param ptr target list object
 *  \param item item to add
 */
GF_Err gf_list_add(GF_List *ptr,void* item);
GF_Err gf_list_add2(GF_List *ptr, void *item);//add shenh
GF_Err gf_list_add3(GF_List *ptr, void *item);

GF_Err gf_list_insert(GF_List *ptr, void *item, uint32 position);
/*!
 *  \brief removes item
 *
 *  Removes an item from the list given its position
 *  \param ptr target list object
 *  \param position position of the item to remove. It is expressed between 0 and gf_list_count-1.
 *  \note It is the caller responsability to destroy the content of the list if needed
 */
GF_Err gf_list_rem(GF_List *ptr, uint32 position);
/*!
 *  \brief gets item
 *
 *  Gets an item from the list given its position
 *  \param ptr target list object
 *  \param position position of the item to get. It is expressed between 0 and gf_list_count-1.
 */
void *gf_list_get(GF_List *ptr, uint32 position);
/*!
 *  \brief finds item
 *
 *  Finds an item in the list
 *  \param ptr target list object.
 *  \param item the item to find.
 *  \return 0-based item position in the list, or -1 if the item could not be found.
 */
s32 gf_list_find(GF_List *ptr, void *item);
/*!
 *  \brief deletes item
 *
 *  Deletes an item from the list
 *  \param ptr target list object.
 *  \param item the item to find.
 *  \return 0-based item position in the list before removal, or -1 if the item could not be found.
 */
s32 gf_list_del_item(GF_List *ptr, void *item);
/*!
 *  \brief resets list
 *
 *  Resets the content of the list
 *  \param ptr target list object.
 *  \note It is the caller responsability to destroy the content of the list if needed
 */
void gf_list_reset(GF_List *ptr);

#endif

#endif


