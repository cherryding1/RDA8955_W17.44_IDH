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





#include "coder.h"

/* heap indexing, where heap[1] is root */
#define PARENT(i) ((i) >> 1)
#define LCHILD(i) ((i) << 1)
#define RCHILD(i) (LCHILD(i)+1)

static const int expbits_tab[8] = { 52, 47, 43, 37, 29, 22, 16, 0 };

/**************************************************************************************
 * Function:    CategorizeAndExpand
 *
 * Description: derive the quantizer setting for each region, based on decoded power
 *                envelope, number of bits available, and rateCode index
 *
 * Inputs:      pointer to initialized Gecko2Info struct
 *              number of bits remaining in bitstream for this frame
 *
 * Outputs:     catbuf[], which contains the correct quantizer index (cat) for each
 *                region, based on the rateCode read from bitstream
 *
 * Return:      none
 **************************************************************************************/
void CategorizeAndExpand(Gecko2Info *gi, int availbits)
{
    int r, n, k, val;
    int offset, delta, cat;
    int expbits, maxbits, minbits;
    int maxptr, minptr;
    int nminheap = 0, nmaxheap = 0;
    int *cp;
    int *maxcat = gi->db.maxcat;
    int *mincat = gi->db.mincat;
    int *changes = gi->db.changes;
    int *maxheap = gi->db.maxheap;
    int *minheap = gi->db.minheap;
    int *rmsIndex = gi->db.rmsIndex;
    int *catbuf = gi->db.catbuf;
    int rateCode = gi->rateCode;

    /* it's okay not to zero-init maxheap/minheap[1 ... MAXCREGN]
     * we don't read maxheap/minheap[1+] without putting something in them first
     */
    maxheap[0] = 0x7fffffff;    /* upheap sentinel */
    minheap[0] = 0x80000000;    /* upheap sentinel */

    /* Hack to compensate for different statistics at higher bits/sample */
    if (availbits > gi->nSamples)
        availbits = gi->nSamples + (((availbits - gi->nSamples) * 5)>>3);
    /*
     * Initial categorization.
     *
     * This attempts to assigns categories to regions using
     * a simple approximation of perceptual masking.
     * Offset is chosen via binary search for desired bits.
     */
    offset = -32;   /* start with minimum offset */
    for (delta = 32; delta > 0; delta >>= 1)
    {

        expbits = 0;
        val       = offset + delta;

        for (r = 0; r < gi->cRegions; r++)
        {
            cat = (val - rmsIndex[r]) >>1;

            if (cat <= 0)
                expbits += 52;
            else if(cat < 7)
                expbits += expbits_tab[cat];
        }

        if (expbits >= availbits-32)
            offset += delta;
    }

    /* Use the selected categorization */
    expbits = 0;
    for (r = 0; r < gi->cRegions; r++)
    {
        cat = (offset - rmsIndex[r]) >>1;
        if (cat < 0) cat = 0;   /* clip */
        if (cat > 7) cat = 7;   /* clip */
        expbits += expbits_tab[cat];
        mincat[r] = cat;    /* init */
        maxcat[r] = cat;    /* init */

        val = offset - rmsIndex[r] - 2*cat;
        val = (val << 16) | r;  /* max favors high r, min favors low r */

        if (cat < 7)
        {
            k = ++nmaxheap;
            maxheap[k] = val;
            while (val > maxheap[PARENT(k)])
            {
                maxheap[k] = maxheap[PARENT(k)];
                k = PARENT(k);
            }
            maxheap[k] = val;
        }

        if (cat > 0)
        {
            k = ++nminheap;
            minheap[k] = val;
            while (val < minheap[PARENT(k)])
            {
                minheap[k] = minheap[PARENT(k)];
                k = PARENT(k);
            }
            minheap[k] = val;
        }
    }

    /* init */
    minbits = expbits;
    maxbits = expbits;
    minptr = gi->nCatzns;   /* grows up, post-increment */
    maxptr = gi->nCatzns;   /* grows down, pre-decrement */

    for (n = 1; n < gi->nCatzns; n++)
    {

        if (maxbits + minbits <= 2*availbits)
        {
            if (!nminheap)
            {
                break;
            }
            r = minheap[1] & 0xffff;

            changes[--maxptr] = r;              /* add to change list */
            maxbits -= expbits_tab[maxcat[r]];  /* sub old bits */
            maxcat[r] -= 1;                     /* dec category */
            maxbits += expbits_tab[maxcat[r]];  /* add new bits */

            k = 1;
            if (maxcat[r] == 0)
                minheap[k] = minheap[nminheap--];   /* remove */
            else
                minheap[k] += (2 << 16);            /* update */

            val = minheap[k];
            while (k <= PARENT(nminheap))
            {
                int child = LCHILD(k);
                int right = RCHILD(k);
                if ((right <= nminheap) && (minheap[right] < minheap[child]))
                    child = right;
                if (val < minheap[child])
                    break;
                minheap[k] = minheap[child];
                k = child;
            }
            minheap[k] = val;

        }
        else
        {
            if (!nmaxheap)
            {
                break;
            }
            r = maxheap[1] & 0xffff;

            changes[minptr++] = r;              /* add to change list */
            minbits -= expbits_tab[mincat[r]];  /* sub old bits */
            mincat[r] += 1;                     /* inc category */
            minbits += expbits_tab[mincat[r]];  /* add new bits */

            k = 1;
            if (mincat[r] == 7)
                maxheap[k] = maxheap[nmaxheap--];   /* remove */
            else
                maxheap[k] -= (2 << 16);            /* update */

            val = maxheap[k];
            while (k <= PARENT(nmaxheap))
            {
                int child = LCHILD(k);
                int right = RCHILD(k);
                if ((right <= nmaxheap) && (maxheap[right] > maxheap[child]))
                    child = right;
                if (val > maxheap[child])
                    break;
                maxheap[k] = maxheap[child];
                k = child;
            }
            maxheap[k] = val;
        }
    }

    /* largest categorization */
    for (r = 0; r < gi->cRegions; r++)
        catbuf[r] = maxcat[r];

    /* make sure rateCode is not greater than number of changes in list */
    ASSERT(rateCode <= (minptr - maxptr));

    /* expand categories using change list, starting at max cat
     * we change one region at a time (cat++ = coarser quantizer)
     */
    cp = &changes[maxptr];
    while (rateCode--)
        catbuf[*cp++] += 1;

}
