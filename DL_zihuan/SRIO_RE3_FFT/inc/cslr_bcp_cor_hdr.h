
#ifndef _CSLR_COR_HDR_H_
#define _CSLR_COR_HDR_H_

#include "cslr.h"




/* Word 0 */
#define CSL_COR_HDR_WORD0_LOCAL_HDR_LEN_MASK (0x000000FFu)
#define CSL_COR_HDR_WORD0_LOCAL_HDR_LEN_SHIFT (0x00000000u)
#define CSL_COR_HDR_WORD0_LOCAL_HDR_LEN_RESETVAL (0x00000000u)

#define CSL_COR_HDR_WORD0_MOD_ID_MASK    (0x00000F00u)
#define CSL_COR_HDR_WORD0_MOD_ID_SHIFT   (0x00000008u)
#define CSL_COR_HDR_WORD0_MOD_ID_RESETVAL (0x00000000u)

#define CSL_COR_HDR_WORD0_PUCCH_DESPREAD_SEL_MASK (0x00010000u)
#define CSL_COR_HDR_WORD0_PUCCH_DESPREAD_SEL_SHIFT (0x00000010u)
#define CSL_COR_HDR_WORD0_PUCCH_DESPREAD_SEL_RESETVAL (0x00000000u)

#define CSL_COR_HDR_WORD0_DESPREAD_FLAG_CPLX_MASK (0x00020000u)
#define CSL_COR_HDR_WORD0_DESPREAD_FLAG_CPLX_SHIFT (0x00000011u)
#define CSL_COR_HDR_WORD0_DESPREAD_FLAG_CPLX_RESETVAL (0x00000000u)

#define CSL_COR_HDR_WORD0_RESETVAL       (0x00000000u)

/* Word 1 */
#define CSL_COR_HDR_WORD1_PUCCH_A_MASK  (0x0000000Fu)
#define CSL_COR_HDR_WORD1_PUCCH_A_SHIFT  (0x00000000u)
#define CSL_COR_HDR_WORD1_PUCCH_A_RESETVAL (0x00000000u)

#define CSL_COR_HDR_WORD1_PUCCH_SCR_SEQ_MASK   (0x00FFFFF0u)
#define CSL_COR_HDR_WORD1_PUCCH_SCR_SEQ_SHIFT   (0x00000004u)
#define CSL_COR_HDR_WORD1_PUCCH_SCR_SEQ_RESETVAL (0x00000000u)

#define CSL_COR_HDR_WORD1_PUCCH_NR_MASK  (0x0F000000u)
#define CSL_COR_HDR_WORD1_PUCCH_NR_SHIFT  (0x00000018u)
#define CSL_COR_HDR_WORD1_PUCCH_NR_RESETVAL (0x00000000u)

#define CSL_COR_HDR_WORD1_PUCCH_MODE_MASK (0x10000000u)
#define CSL_COR_HDR_WORD1_PUCCH_MODE_SHIFT (0x0000001Cu)
#define CSL_COR_HDR_WORD1_PUCCH_MODE_RESETVAL (0x00000000u)

#define CSL_COR_HDR_WORD1_RESETVAL       (0x00000000u)

/* Words 2 to 7 */
#define CSL_COR_HDR_WORD2_DESPREAD_LENGTH_MASK (0x0000FFFFu)
#define CSL_COR_HDR_WORD2_DESPREAD_LENGTH_SHIFT (0x00000000u)
#define CSL_COR_HDR_WORD2_DESPREAD_LENGTH_RESETVAL (0x00000000u)

#define CSL_COR_HDR_WORD2_SF_RATIO_MASK  (0x00070000u)
#define CSL_COR_HDR_WORD2_SF_RATIO_SHIFT (0x00000010u)
#define CSL_COR_HDR_WORD2_SF_RATIO_RESETVAL (0x00000000u)

#define CSL_COR_HDR_WORD2_RESETVAL       (0x00000000u)

#endif