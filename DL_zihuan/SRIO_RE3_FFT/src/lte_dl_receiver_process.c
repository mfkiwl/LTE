/***********************************************/
/*main                                         */
/*功能：LTE PDSCH RECEIVER MAIN PROCESSER   */
/***********************************************/
#include "system_init.h"
#include "pl_comm.h"
#include "TSC.h"
#include "PHY_PHYADP_interfaceV3.h"
#include "SRIO_drv.h"
/******************vcp2 part cs 15/05/13*************************/
#include <ti/csl/csl_chip.h>
#include <ti/csl/csl_tsc.h>
#include <ti/csl/csl_cacheAux.h>
#include <ti/csl/csl_vcp2.h>
#include <ti/csl/csl_vcp2Aux.h>
#include <ti/csl/csl_chip.h>
//#include "stdio.h"

#include "vcp2_common.h"
#include "vcp2_testInit.h"
//#include "TSC.h"
#include "KeyStone_common.h"
#include "VCP2_Channel_Density.h"

#pragma DATA_SECTION(DerateData, ".VCP2Data")
#pragma DATA_ALIGN(DerateData, 128)
char DerateData[600]={0};  //cs 15/07/06
//volatile Uint32* powerOnFlagPtr = (volatile Uint32*)0x0C000000;
/******************vcp2 part cs 15/05/13*************************/
#define BUF_LEN 10
#define SENDOFFSET 10240
extern void Descrambling_data_pdsch_gen(unsigned char *restrict x1_sequence_table,
        unsigned int  x2_init_seq,
        unsigned int  bit_descrambler_length,
        unsigned char *descrambling_bit_ptr);

extern unsigned volatile int receivenumber;
extern unsigned volatile short  sfn;

extern volatile UEPHYADPtoPHYType2 phytomactype2;
extern volatile UE_DL_TYPE2_PBCH_D phytomac_pbchd;
extern volatile UE_DL_TYPE2_PHICH_D phytomac_phichd;
extern volatile UE_DL_TYPE2_PDCCH_C phytomac_pdcchc;
extern volatile UE_DL_TYPE2_PDCCH_D phytomac_pdcchd;
extern volatile UE_DL_TYPE2_PDSCH_C phytomac_pdschc;

extern SRIO_Trans_Config transParameter[];
extern void LTE_Test(char doorbellvalid,unsigned char event );

extern volatile UEPHYADPtoPHYType3 mactophytype3;


#define MAPPED_VIRTUAL_ADDRESS      0x80000000

/* MPAX segment 2 registers */
#define XMPAXL2                     0x08000010
#define XMPAXH2                     0x08000014
/*LTE PDSCH RECEIVER MAIN PROCESSER*/
#pragma DATA_SECTION(ethernet_packet_num, ".shareddata")
unsigned int ethernet_packet_num = 0;

/*#pragma DATA_SECTION(gxx_fft_complete_flag, ".shareddata")
unsigned int gxx_fft_complete_flag[4] = {0,0,0,0};*/
/*#pragma DATA_SECTION(gxx_decramble_complete_flag, ".shareddata")
unsigned int gxx_decramble_complete_flag[4] = {0,0,0,0};*/
/*#pragma DATA_SECTION(gxx_pdcch_complete_flag, ".shareddata")
unsigned int gxx_pdcch_complete_flag[4] = {0,0,0,0};*/

#pragma DATA_ALIGN (gxx_fft_core0_complete_flag, CACHE_L2_LINESIZE)
#pragma DATA_SECTION(gxx_fft_core0_complete_flag, ".shareddata")
unsigned int gxx_fft_core0_complete_flag = 0;
#pragma DATA_ALIGN (gxx_pdcch_core0_complete_flag, CACHE_L2_LINESIZE)
#pragma DATA_SECTION(gxx_pdcch_core0_complete_flag, ".shareddata")
unsigned int gxx_pdcch_core0_complete_flag = 0;
#pragma DATA_SECTION(gxx_decramble_core0_complete_flag, ".shareddata")
unsigned int gxx_decramble_core0_complete_flag = 0;

#pragma DATA_ALIGN (gxx_fft_core1_complete_flag, CACHE_L2_LINESIZE)
#pragma DATA_SECTION(gxx_fft_core1_complete_flag, ".shareddata")
unsigned int gxx_fft_core1_complete_flag = 0;
#pragma DATA_SECTION(gxx_pdcch_core1_complete_flag, ".shareddata")
unsigned int gxx_pdcch_core1_complete_flag = 0;
#pragma DATA_SECTION(gxx_decramble_core1_complete_flag, ".shareddata")
unsigned int gxx_decramble_core1_complete_flag = 0;

#pragma DATA_ALIGN (gxx_fft_core2_complete_flag, CACHE_L2_LINESIZE)
#pragma DATA_SECTION(gxx_fft_core2_complete_flag, ".shareddata")
unsigned int gxx_fft_core2_complete_flag = 0;
#pragma DATA_SECTION(gxx_pdcch_core2_complete_flag, ".shareddata")
unsigned int gxx_pdcch_core2_complete_flag = 0;
#pragma DATA_SECTION(gxx_decramble_core2_complete_flag, ".shareddata")
unsigned int gxx_decramble_core2_complete_flag = 0;

#pragma DATA_ALIGN (gxx_fft_core3_complete_flag, CACHE_L2_LINESIZE)
#pragma DATA_SECTION(gxx_fft_core3_complete_flag, ".shareddata")
unsigned int gxx_fft_core3_complete_flag = 0;
#pragma DATA_SECTION(gxx_pdcch_core3_complete_flag, ".shareddata")
unsigned int gxx_pdcch_core3_complete_flag = 0;
#pragma DATA_SECTION(gxx_decramble_core3_complete_flag, ".shareddata")
unsigned int gxx_decramble_core3_complete_flag = 0;

#pragma DATA_ALIGN (gxx_pbch_core0_core3_flag, CACHE_L2_LINESIZE)
#pragma DATA_SECTION(gxx_pbch_core0_core3_flag, ".shareddata")
unsigned int gxx_pbch_core0_core3_flag= 0;

#pragma DATA_ALIGN (gxx_pbch_process_flag, CACHE_L2_LINESIZE)
#pragma DATA_SECTION(gxx_pbch_process_flag, ".shareddata")
unsigned volatile int gxx_pbch_process_flag = 0;
#pragma DATA_ALIGN (gxx_pdcch_detect_flag, CACHE_L2_LINESIZE)
#pragma DATA_SECTION(gxx_pdcch_detect_flag, ".shareddata")
unsigned int gxx_pdcch_detect_flag = 0;
#pragma DATA_SECTION(gxx_pdcch_total_reg_num, ".shareddata")
unsigned int gxx_pdcch_total_reg_num = 0;
#pragma DATA_SECTION(gxx_slot_idx_c3, ".shareddata")
unsigned int   gxx_slot_idx_c3 = 0xff;
#pragma DATA_SECTION(gxx_slot_idx_c1, ".shareddata")
unsigned int   gxx_slot_idx_c1 = 0xff;
#pragma DATA_SECTION(gxx_slot_idx_c2, ".shareddata")
unsigned int   gxx_slot_idx_c2 = 0xff;

#pragma DATA_ALIGN (gxx_all_pdcch_complete_flag, CACHE_L2_LINESIZE)
#pragma DATA_SECTION(gxx_all_pdcch_complete_flag, ".shareddata")
unsigned int   gxx_all_pdcch_complete_flag = 0;

#pragma DATA_ALIGN (gxx_decoding_core3_core0_flag, CACHE_L2_LINESIZE)
#pragma DATA_SECTION(gxx_decoding_core3_core0_flag, ".shareddata")
unsigned int   gxx_decoding_core3_core0_flag = 0;

#pragma DATA_ALIGN (gxx_decoding_core3_core1_flag, CACHE_L2_LINESIZE)
#pragma DATA_SECTION(gxx_decoding_core3_core1_flag, ".shareddata")
unsigned int   gxx_decoding_core3_core1_flag = 0;

#pragma DATA_ALIGN (gxx_slot_idx_c0, CACHE_L2_LINESIZE)
#pragma DATA_SECTION(gxx_slot_idx_c0, ".shareddata")
unsigned int   gxx_slot_idx_c0 = 0xff;

#pragma DATA_SECTION(pdsch_pseudo_copy_flag, ".shareddata")
unsigned int pdsch_pseudo_copy_flag = 0;
char gxx_crc_check = 0;
unsigned int gxx_cycle_c0[10],gxx_cycle_c0_2[10],gxx_cycle_c1[10],gxx_cycle_c2[10],gxx_cycle_c3[10];

#pragma DATA_ALIGN (memory_start, CACHE_L2_LINESIZE)
#pragma DATA_SECTION(memory_start,".testData")
unsigned int memory_start;

#pragma DATA_ALIGN (used_index, CACHE_L2_LINESIZE)
#pragma DATA_SECTION(used_index, ".shareddata")
unsigned volatile int used_index = 0;

#pragma DATA_SECTION(send_index, ".shareddata")
unsigned volatile int send_index = 0;

#pragma DATA_SECTION(test_start, ".shareddata")
unsigned int test_start0,test_start1,test_start2,test_start3;
unsigned int total_time0,total_time1,total_time2,total_time3;

unsigned int dft_list[6 * NUM_ANTENNA_PORT_4] = {2048,2048,2048,2048,2048,2048,
                                                 2048,2048,2048,2048,2048,2048,
                                                 2048,2048,2048,2048,2048,2048,
                                                 2048,2048,2048,2048,2048,2048,};
unsigned int countnum = 0;
extern unsigned int fft_output_opt_data[];
extern volatile int symbol_flag;
extern volatile int slot_flag;
extern volatile int counter1;
extern unsigned int testdata_receive[10*61440];
unsigned int error_pdcchcrc = 0;
unsigned int right_pdcchcrc = 0;
unsigned int error_pdschcrc = 0;
unsigned int right_pdschcrc = 0;
unsigned int pbch_num=0;
unsigned int pbch_send=0;

unsigned int pbch_crc_right = 0;
unsigned int pbch_crc_error = 0;
unsigned int pdcchspecialfailed=0;
unsigned int pdcchcommonfailed=0;//lycan 2015-9-24
unsigned int disparity_num=0;
unsigned int accordance_num=0;
unsigned int decode_flag=0;
unsigned int received_num=0;
unsigned int a=0;
unsigned int b=0;
unsigned int c=0;
unsigned int d=0;
extern int   dbinfo;
unsigned int count_subf[10]={0};





void get_flag()
{
	gxx_pbch_process_flag=mactophytype3.DL_TYPE3_PDSCH_C.pbch_process_flag;
    CACHE_wbL2((void*)&gxx_pbch_process_flag,sizeof(gxx_pbch_process_flag),CACHE_WAIT);
}

void f_pdcch_monitor_proc_part2(
                      CELL_PARA *cell_para_ptr,
                      char subfn_index,
                      char rnti,
                      char common_rnti,
                      unsigned short pdcch_total_reg_num
                      );
unsigned short f_pdcch_monitor_proc_part1(
                      DL_resource_mapping_struct *dl_resource_mapping_ptr,
                      DL_cch_data_struct* cch_data_buffer_ptr,
                      CELL_PARA *cell_para_ptr,
                      char subfn_index,
                      char pdcch_symbol_num
                      );
//lycan time test
int cyclenum = 0;
void lte_dl_receiver (unsigned char slot_idx)
{
    unsigned int ok_flag;
    unsigned int i,j,k,cInit;

    unsigned int cp_len;
    unsigned int cycle_start;
    unsigned int m_sybol,core_m_symbol[4];
    unsigned int core_start_numBitsG[4][NUM_CODEWORD_2];
    unsigned char total_symbol;
    unsigned int fftc_output_data_address[NUM_RX_DESC];
    signed short symbol_start[1];
    unsigned int*  input_data_ptr1;
    unsigned int*  input_data_ptr2;
    unsigned int*  input_data_ptr3;


    signed short rho_a_b[2];
    unsigned char four_ant_transmit_div_flag = 0;
    signed char power_offset;
    signed int temp[1];
    unsigned char n_id_1;
    unsigned char n_id_2;
    unsigned char *PBCHCarrierIdx;
    int crc_out;
    char pdcch_symbol_num = 0;
    unsigned char core0_eq_symbol,core1_eq_symbol,core2_eq_symbol,core3_eq_symbol;
    int phich_demond_data[3];
    int copy_flag[3]={0,0,0};
    int test_a=0;
    unsigned int core0_time1,core0_time2;
    int fftc_start,fftc_cycle;
    int test_start,test_cycle;
    char pbch_ok_flag;
    int iiii=1;

    FILE *p=NULL;
        
    if(0 == glbCoreId)
    {
    	 // while(iiii);
#if 1
          if(UL == vxx_cell_para.subframe_direction[slot_idx >> 1])
          {
              used_index=used_index+1;

              CACHE_wbL2((void *)&used_index, sizeof(used_index), CACHE_WAIT);
              CACHE_invL2((void *)&used_index, sizeof(used_index), CACHE_WAIT);

              return;
          }

    	  //printf("frame %d processing starts\n",slot_idx>>1);
		  cycle_start = TSCL;
          CACHE_invL2((void *)&gxx_slot_idx_c0, sizeof(gxx_slot_idx_c0), CACHE_WAIT);
          gxx_slot_idx_c0 = slot_idx;//必须传参进来给gxx_slot_idx_c0赋值，否则会编译告警导致无法对gxx_slot_idx_c0操作
          CACHE_wbL2((void *)&gxx_slot_idx_c0, sizeof(gxx_slot_idx_c0), CACHE_WAIT);// Wait for the writeback operation to complete.

          //printf("gxx_slot_idx_c0 is %d\n",gxx_slot_idx_c0);

          if((slot_idx == 2) || (slot_idx == 12))/*特殊子帧*/
          {
              total_symbol = vxx_cell_para.dwpts_symbol;
          }
          else
          {
              total_symbol = 2 * N_SYM_PER_SLOT;
          }
#endif
          /*计算数据和导频之间的幅度比 rho_a_b[0]=rho_a非导频 符号所用，rho_a_b[1]=rho_b导频符号*/
          if (((vxx_cell_para.num_antenna_port == NUM_ANTENNA_PORT_2) || (vxx_cell_para.num_antenna_port == NUM_ANTENNA_PORT_4)) && (vxx_cell_para.P_B == 1))
          {
               rho_a_b[0] = 8192; /* 1 Q(16,3) */
               rho_a_b[1] = 8192; /* 1 Q(16,3) */
          }
          /*PA/PB=1*/
          else if ((vxx_cell_para.num_antenna_port == NUM_ANTENNA_PORT_1) && (vxx_cell_para.P_B == 0))
          {
              rho_a_b[0] = 8192; /* 1 Q(16,3) */
              rho_a_b[1] = 8192; /* 1 Q(16,3) */
          }
          else
          {
              if ((vxx_cell_para.num_antenna_port == NUM_ANTENNA_PORT_4) && (vxx_user_para[0].ueTransMode == TRANSMIT_DIVERSITY))
              {
                  four_ant_transmit_div_flag = 1;

              }
              power_offset = 0;
              /*PA/PB~=1时需要乘以P_B对应的倍数 */
              rho_a_b[0] = rho_a_lut[power_offset][four_ant_transmit_div_flag][vxx_user_para[0].P_A];
              rho_a_b[1] = (rho_a_b[0] * ratio_b_lut[vxx_cell_para.num_antenna_port - 1][vxx_cell_para.P_B]) >> 15;/*Q(16,3)*/
              rho_a_b[0] = rho_a_b[0] >> 1; /*Q(16,3)*/
          }
          CACHE_invL2((void *)&gxx_pbch_process_flag, sizeof(gxx_pbch_process_flag), CACHE_WAIT);
          if(0 == gxx_pbch_process_flag)
          {
        	  pdsch_pseudo_copy_flag = 1;
        	  CACHE_wbL2((void *)&pdsch_pseudo_copy_flag, sizeof(pdsch_pseudo_copy_flag), CACHE_WAIT);// Wait for the writeback operation to complete
          }

/*
          if(slot_idx<=2)
          {
        	  used_index = (slot_idx>>1);
          }
          else if((slot_idx>=6)&&(slot_idx<=12))
          {
        	  used_index = (slot_idx>>1)-1;
          }
          else if((slot_idx>=16)&&(slot_idx<=18))
          {
        	  used_index = (slot_idx>>1)-2;
          }
          */

          /*10.22 mjt 设置接收端数据存放大小：
           * BUF_LEN*2<=4:		BUF_LEN
           * 6=<BUF_LEN*2<=12:	BUF_LEN-1
           * 16<=BUF_LEN*2<=18: BUF_LEN-2
           * */
          CACHE_invL2((void *)&used_index, sizeof(used_index), CACHE_WAIT);
          used_index=used_index%BUF_LEN;

          CACHE_wbL2((void *)&used_index, sizeof(used_index), CACHE_WAIT);
          CACHE_invL2((void *)&used_index, sizeof(used_index), CACHE_WAIT);


          //10.15 mjt修改，这里执行前需要等待上个子帧译码前的数据搬移过程结束。
          if(0 == gxx_pbch_process_flag)
          {
              while(gxx_all_pdcch_complete_flag)
              {
          	      CACHE_invL2((void *)&gxx_all_pdcch_complete_flag, sizeof(gxx_all_pdcch_complete_flag), CACHE_WAIT);
              }
              gxx_all_pdcch_complete_flag=1;


              CACHE_wbL2((void *)&gxx_all_pdcch_complete_flag, sizeof(gxx_all_pdcch_complete_flag), CACHE_WAIT);
          }

          //input_data_ptr1 = &ifft_subframe[used_index * SUBFRAME_LEN * NUM_MAX_ANTENNA];
          //input_data_ptr2 = &ifft_subframe[used_index * SUBFRAME_LEN * NUM_MAX_ANTENNA + SUBFRAME_LEN];

          //input_data_ptr1 = ifft_subframe;
          //input_data_ptr2 = &ifft_subframe[SUBFRAME_LEN];


#if 1
          /******************************处理符号0,便于获取scale因子*****************************************/
          while(1)
          {
              //CACHE_invL2(&symbol_flag, sizeof(symbol_flag), CACHE_WAIT);
        	  //printf("Symbol %d receive!\n",symbol_flag-1);
              //if(1 == gxx_symbol0_flag)//说明符号0到了
        	  //2015.3.23添加
        	  if(symbol_flag>=139)
        		  symbol_flag = symbol_flag-140;


        	  if(symbol_flag>=(slot_idx*7))//符号0到了
              {
        		  if(countnum==0)
        			  a=dbinfo;
        		  if(countnum==1)
        			  b=dbinfo;
        		  if(countnum==2)
        			  c=dbinfo;
        		  if(countnum==3)
        			  d=dbinfo;

        		  countnum++;

        		  /*  int iii;
        		  for(iii=0;iii<2048+160;iii++)
        			  testdata_receive[iii]=0;*/
        		  fftc_start=TSCL;

        		  input_data_ptr1 = &testdata_receive[used_index * SUBFRAME_LEN * NUM_MAX_ANTENNA];

                  if(* input_data_ptr1!=0)
                  {
                	  received_num=received_num+1;
                  }
        		  //input_data_ptr1 = &ifft_subframe[used_index * SUBFRAME_LEN * NUM_MAX_ANTENNA];

        		  CACHE_invL2(input_data_ptr1,(2048+160)*4*2,CACHE_WAIT);

        		  CSL_XMC_invalidatePrefetchBuffer();
        		  		    _mfence();
        		  		    _mfence();


                gxx_symbol0_flag = 0;
                ok_flag = fft_config(dft_list,/*输入的DFT/IDFTsize，仅支持DFT_sz_list里面的各种size*/
                                 1 - 0,/*输入的DFT/IDFTsize个数,目前仅支持每次1种DFTSIZE配置*/
                                 FFT_SIZE,/*输入数据长度必须是4的倍数*/
                                 input_data_ptr1,/*输入数据*/
                                 DFT_MODE,/*DFT/IDFT*/
                                 fftc_output_data_address,/*输出数据地址,使用时注意输出数据要偏移4个int才是真正的结果*/
                                 CSL_FFTC_A,/*agc因子，负数表明要右移，整数位左移*/
                                 cp_len,
                                 0,
                                 1);
                break;
              }
          }
          fftc_cycle = TSC_delay_cycle(fftc_start);

          
          /**************************时隙0导频到了后先启动PDCCH盲检****************************/
          /*符号0导频解资源映射*/
          crs_prb_demapping((signed int*)fft_data,//fft_data,   //In, 频域数据首地址
                          NUM_MAX_ANTENNA,       //Para
                          vxx_cell_para.cellID,        //Para,小区ID（0-503）
                          0,        //Para,本次demapping的导频的端口号，目前支持0,1
                          (signed int*)prbdemapping_crs_data,      //Out, 导频数据首地址
                          0,//表示符号0的导频
                          1//表示一个crs
                         );
          /*时隙0导频解资源映射*/
          crs_prb_demapping((signed int*)fft_data,   //In, 频域数据首地址
                          NUM_MAX_ANTENNA,       //Para
                          vxx_cell_para.cellID,        //Para,小区ID（0-503）
                          1,        //Para,本次demapping的导频的端口号，目前支持0,1
                          (signed int*)prbdemapping_crs_data,      //Out, 导频数据首地址
                          0,//表示符号0的导频
                          1//表示一个crs
                         );
          /*时隙0端口0信道估计*/
          chan_est((signed int*)prbdemapping_crs_data,   //In, 导频接收数据首地址
                  &rsgen_output[slot_idx][0][0],   //In, 本地导频序列首地址，排列方式同e_freq_crs_ptr；为了方便实现e/d，此处直接存d的共轭
                  NUM_MAX_ANTENNA,         //Para
                  0,            //Para,***目前仅适用于连续分配的prb***
                  N_DL_RB,              //Para
                  0,          //Para,本次demapping的导频的端口号，目前支持0,1
                  (signed int)(rho_a_b[0] << 16),      //Para,由导频功率/数据功率折算出的二者幅度比；为了方便实现e/ratio，此处直接存数据/导频幅度比；以复数形式存储，高16bit有效（实部），低16bit为0
                  (signed int*)crs_che_data,        //Out, 频域信道估计首地址（插值前），排列方式同e_freq_crs_ptr
                  0,//表示符号0的导频
                  1//表示一个crs
                  );
          /*时隙0端口1信道估计*/
          chan_est((signed int*)prbdemapping_crs_data,   //In, 导频接收数据首地址
                  &rsgen_output[slot_idx][0][0],   //In, 本地导频序列首地址，排列方式同e_freq_crs_ptr；为了方便实现e/d，此处直接存d的共轭
                  NUM_MAX_ANTENNA,         //Para
                  0,            //Para,***目前仅适用于连续分配的prb***
                  N_DL_RB,              //Para
                  1,          //Para,本次demapping的导频的端口号，目前支持0,1
                  (signed int)(rho_a_b[0] << 16),      //Para,由导频功率/数据功率折算出的二者幅度比；为了方便实现e/ratio，此处直接存数据/导频幅度比；以复数形式存储，高16bit有效（实部），低16bit为0
                  (signed int*)crs_che_data,        //Out, 频域信道估计首地址（插值前），排列方式同e_freq_crs_ptr
                  0,//表示符号0的导频
                  1//表示一个crs
                  );
          /*端口0信道估计插值*/
          chan_interp_freq((signed int*)crs_che_data,        //In, 频域信道估计首地址（插值前），排列方式同e_freq_crs_ptr
                     NUM_MAX_ANTENNA,          //Para
                     0,             //Para,***目前仅适用于连续分配的prb***
                     N_DL_RB,               //Para
                     vxx_cell_para.cellID,         //Para,小区ID（0-503）
                     0,         //Para,本次demapping的导频的端口号，目前支持0,1
                     (signed int*)che_interp_data,      //Out, 频域信道估计首地址，排列方式同e_freq_data_ptr，kaV在最外层
                     0,//符号0导频
                     1//crs长度只有一个，符号0
                     );
          /*端口1信道估计插值*/
          chan_interp_freq((signed int*)crs_che_data,        //In, 频域信道估计首地址（插值前），排列方式同e_freq_crs_ptr
                     NUM_MAX_ANTENNA,          //Para
                     0,             //Para,***目前仅适用于连续分配的prb***
                     N_DL_RB,               //Para
                     vxx_cell_para.cellID,         //Para,小区ID（0-503）
                     1,         //Para,本次demapping的导频的端口号，目前支持0,1
                     (signed int*)che_interp_data,      //Out, 频域信道估计首地址，排列方式同e_freq_data_ptr，kaV在最外层
                     0,//符号0导频
                     1//crs长度只有一个，符号0
                     );
          
          if(0 == gxx_pbch_process_flag)//pbch已经成功该检测后才进入该流程
          {
#if 1
               /*********************************PCFICH************************************/
               //提取PCFICH频域数据
               f_pcfich_freq_data_recover(
                              (int *)fft_data[0],
                                  g_cch_data_buffer.pcfich_freq_data[0],
                                  &g_dl_resource_mapping,
                                  &vxx_cell_para
                                 );
               //提取PCFICH信道估计
               f_pcfich_freq_data_recover(
                                  (int *)che_interp_data[0][0],
                                  g_cch_data_buffer.pcfich_H_data[0][0],
                                  &g_dl_resource_mapping,
                                  &vxx_cell_para
                                 );    
               f_pcfich_freq_data_recover(
                              (int *)che_interp_data[1][0],
                                  g_cch_data_buffer.pcfich_H_data[1][0],
                                  &g_dl_resource_mapping,
                                  &vxx_cell_para
                                 );    
               temp[0] = 16;
               symbol_start[0] = 0;
               /*单码字*/
               chan_eq((signed int*)g_cch_data_buffer.pcfich_freq_data,     //In, 接收天线的数据
                       (signed int*)g_cch_data_buffer.pcfich_H_data,   //In, 第1根接收天线的第1个流的第1个子载波，a=h_1_1_c1
                        symbol_start,
                        g_cch_data_buffer.pcfich_data_after_decoding,  //Out, d1
                        TRANSMIT_DIVERSITY, /*下行传输模式*/
                        temp,
                        0,
                        1,
                        16,
                        PCFICH,
                        pbch_pcfich_pdcch_llr_data     //Out,LLr结果首地址
                       );
               
               demodulation((signed short*)g_cch_data_buffer.pcfich_data_after_decoding,//Q(16,3)
                            pbch_pcfich_pdcch_llr_data,//Q(16,1)
                            g_cch_data_buffer.pcfich_data_after_dem,//Q(8,6)
                            TRANSMIT_DIVERSITY,
                            QPSK,
                            16
                            );
               
               //扰码生成
               cInit = ((slot_idx >> 1) + 1) * ((2 * vxx_cell_para.cellID + 1) << 9) + vxx_cell_para.cellID;
               BitDescrambling(x1_sequence_table,
                              g_cch_data_buffer.pcfich_data_after_dem,
                              g_cch_data_buffer.pcfich_data_after_descram,//vxx_user_para[user_index].descramble_data_ptr[i],
                              cInit,
                               32);
               vxx_cell_para.CFI = f_pcfich_decode((int *)g_cch_data_buffer.pcfich_data_after_descram,
                                            (int *)&g_const_cfi_code_table[0][0]);
               
               /*********************************PHICH************************************/
               if(0 != g_N_group_phich[slot_idx>>1])
               {
                   //提取PHICH频域数据
                   f_phich_freq_data_recover(
                                      (int *)fft_data[0],
                                      g_cch_data_buffer.pdcch_freq_data[0],//复用pdcch的buffer
                                      &g_dl_resource_mapping,
                                      &vxx_cell_para,
                                      slot_idx
                                     );
                   //提取PHICH信道估计
                   f_phich_freq_data_recover(
                                      (int *)che_interp_data[0][0],
                                      g_cch_data_buffer.H_freq_data[0][0],//复用pdcch的buffer
                                      &g_dl_resource_mapping,
                                      &vxx_cell_para,
                                      slot_idx
                                     );    
                   f_phich_freq_data_recover(
                                      (int *)che_interp_data[1][0],
                                      g_cch_data_buffer.H_freq_data[1][0],//复用pdcch的buffer
                                      &g_dl_resource_mapping,
                                      &vxx_cell_para,
                                      slot_idx
                                     );    
                   temp[0] = 12 * g_N_group_phich[slot_idx>>1];
                   symbol_start[0] = 0;
                   /*单码字*/
                   chan_eq((signed int*)g_cch_data_buffer.pdcch_freq_data,     //In, 接收天线的数据
                           (signed int*)g_cch_data_buffer.H_freq_data,   //In, 第1根接收天线的第1个流的第1个子载波，a=h_1_1_c1
                            symbol_start,
                            g_phich_freq_data,  //Out, d1
                            TRANSMIT_DIVERSITY, /*下行传输模式*/
                            temp,
                            0,
                            1,
                            12 * g_N_group_phich[slot_idx>>1],
                            PHICH,
                            pbch_pcfich_pdcch_llr_data     //Out,LLr结果首地址
                           );
                   temp[0] = 0;
                   Descrambling_data_pbch_gen(x1_sequence_table,
                                              cInit,
                                              12,
                                              temp);//temp中存储的是生成的phich扰码
                   f_phich_det(
                              vxx_phich_para.n_group,
                              vxx_phich_para.n_seq,
                              temp,
                              (short *)g_phich_freq_data,
                              pbch_pcfich_pdcch_llr_data,
                              phich_demond_data
                              );
                   vxx_phich_para.ACK_NACK =  f_harq_ind_decode(phich_demond_data);
                   phytomac_phichd.HARQIndex = vxx_phich_para.ACK_NACK;
               }
#endif
               vxx_cell_para.CFI = 1;
               /*********************************PDCCH************************************/
               if (vxx_cell_para.num_PRB_per_bw_config < 10)
               {
                   pdcch_symbol_num = vxx_cell_para.CFI + 1;
               }
               else
               {
                   pdcch_symbol_num = vxx_cell_para.CFI;
               }
               gxx_pdcch_total_reg_num = f_pdcch_monitor_proc_part1(
                                    &g_dl_resource_mapping,
                                    &g_cch_data_buffer,
                                    &vxx_cell_para,
                                    slot_idx >> 1,
                                    pdcch_symbol_num
                                    );
               while(pdsch_pseudo_copy_flag)
               {
            	   CACHE_invL2((void *)&pdsch_pseudo_copy_flag, sizeof(pdsch_pseudo_copy_flag), CACHE_WAIT);
               }
               gxx_pdcch_detect_flag = 1;

               test_start = TSCL;

               CACHE_invL2(vxx_user_para, sizeof(vxx_user_para), CACHE_WAIT);
               CACHE_wbL2((void *)pdcch_data_after_descram, sizeof(pdcch_data_after_descram), CACHE_WAIT);
               CACHE_wbL2((void *)&gxx_pdcch_total_reg_num, sizeof(gxx_pdcch_total_reg_num), CACHE_WAIT);
               CACHE_wbL2((void *)&gxx_pdcch_detect_flag, sizeof(gxx_pdcch_detect_flag), CACHE_WAIT);
          }


          //input_data_ptr1 = &ifft_subframe[used_index * SUBFRAME_LEN * NUM_MAX_ANTENNA + 160 + FFT_SIZE];
          //input_data_ptr1 = &testdata_receive[used_index * SUBFRAME_LEN * NUM_MAX_ANTENNA + 160 + FFT_SIZE];
          //puts("core0 symbol0 processing completed");
          /******************************处理符号1~6*****************************************/
          while(1)
          {
              //CACHE_invL2(&gxx_symbol6_flag, sizeof(gxx_symbol6_flag), CACHE_WAIT);
              //if(1 == gxx_symbol6_flag)//说明符号6到了
        	  if(symbol_flag>=6+(slot_idx*7))
              {
        		  input_data_ptr1 = &testdata_receive[used_index * SUBFRAME_LEN * NUM_MAX_ANTENNA + (160 + FFT_SIZE)*2];
        		  //input_data_ptr1 = &ifft_subframe[used_index * SUBFRAME_LEN * NUM_MAX_ANTENNA + 160 + FFT_SIZE];

        		  CACHE_invL2(input_data_ptr1,(2048+144)*6*4*2,CACHE_WAIT);
        		  //CACHE_invL2(input_data_ptr1+30720,(2048+144)*6*4,CACHE_WAIT);

        		  gxx_symbol6_flag = 0;
                ok_flag = fft_config(dft_list,/*输入的DFT/IDFTsize，仅支持DFT_sz_list里面的各种size*/
                                 N_SYM_PER_SLOT - 1,/*输入的DFT/IDFTsize个数,目前仅支持每次1种DFTSIZE配置*/
                                 FFT_SIZE,/*输入数据长度必须是4的倍数*/
                                 input_data_ptr1,/*输入数据*/
                                 DFT_MODE,/*DFT/IDFT*/
                                 fftc_output_data_address,/*输出数据地址,使用时注意输出数据要偏移4个int才是真正的结果*/
                                 CSL_FFTC_A,/*agc因子，负数表明要右移，整数位左移*/
                                 cp_len,
                                 1,
                                 N_SYM_PER_SLOT);
                break;
              }
          }
          /******************************处理符号7~13*****************************************/
          
          //input_data_ptr1 = &ifft_subframe[used_index * SUBFRAME_LEN * NUM_MAX_ANTENNA + (SUBFRAME_LEN >> 1)];
          //input_data_ptr1 = &testdata_receive[used_index * SUBFRAME_LEN * NUM_MAX_ANTENNA + (SUBFRAME_LEN >> 1)];
          if((slot_idx == 2) || (slot_idx == 12))/*特殊子帧*/
          {  
               /******************************处理符号7-dwpts*****************************************/
               //不支持DWPTS的配置0，其他配置最多会有11个dwpts符号
               while(1)
               {
               
                   //CACHE_invL2(&gxx_symbol11_flag, sizeof(gxx_symbol11_flag), CACHE_WAIT);
                   //if(1 == gxx_symbol11_flag)//说明符号11到了
            	   if(symbol_flag>=9+(slot_idx*7))//10.13修改，子帧配置2下特殊子帧有10个符号
                   {
            		   //input_data_ptr1 = &ifft_subframe[used_index * SUBFRAME_LEN * NUM_MAX_ANTENNA + (SUBFRAME_LEN >> 1)];
            		   input_data_ptr1 = &testdata_receive[used_index * SUBFRAME_LEN * NUM_MAX_ANTENNA + SUBFRAME_LEN];

            		   CACHE_invL2(input_data_ptr1,((2048+144)*(total_symbol-8)+2048+160)*4*2,CACHE_WAIT);
            		   //CACHE_invL2(input_data_ptr1+30720,((2048+144)*(total_symbol-8)+2048+160)*4,CACHE_WAIT);

            		   gxx_symbol11_flag = 0;
                       ok_flag = fft_config(dft_list,/*输入的DFT/IDFTsize，仅支持DFT_sz_list里面的各种size*/
                                        (total_symbol - N_SYM_PER_SLOT),/*输入的DFT/IDFTsize个数,目前仅支持每次1种DFTSIZE配置*/
                                        FFT_SIZE,/*输入数据长度必须是4的倍数*/
                                        input_data_ptr1,/*输入数据*/
                                        DFT_MODE,/*DFT/IDFT*/
                                        fftc_output_data_address,/*输出数据地址,使用时注意输出数据要偏移4个int才是真正的结果*/
                                        CSL_FFTC_A,/*agc因子，负数表明要右移，整数位左移*/
                                        cp_len,
                                        N_SYM_PER_SLOT,
                                        total_symbol);
                       break;
                   }
               }
          }
          else
          {
             
               /******************************处理符号7~11*****************************************/
               while(1)
               {
               
                   //CACHE_invL2(&gxx_symbol11_flag, sizeof(gxx_symbol11_flag), CACHE_WAIT);
                   //if(1 == gxx_symbol11_flag)//说明符号11到了
            	   if(symbol_flag>=11+(slot_idx*7))
                   {
            		   //input_data_ptr1 = &ifft_subframe[used_index * SUBFRAME_LEN * NUM_MAX_ANTENNA + (SUBFRAME_LEN >> 1)];
            		 input_data_ptr1 = &testdata_receive[used_index * SUBFRAME_LEN * NUM_MAX_ANTENNA + SUBFRAME_LEN];

            		 CACHE_invL2(input_data_ptr1,((2048+144)*4+2048+160)*4*2,CACHE_WAIT);
            		 //CACHE_invL2(input_data_ptr1+30720,((2048+144)*4+2048+160)*4,CACHE_WAIT);

            		 gxx_symbol11_flag = 0;
                     ok_flag = fft_config(dft_list,/*输入的DFT/IDFTsize，仅支持DFT_sz_list里面的各种size*/
                                      12 - N_SYM_PER_SLOT,/*输入的DFT/IDFTsize个数,目前仅支持每次1种DFTSIZE配置*/
                                      FFT_SIZE,/*输入数据长度必须是4的倍数*/
                                      input_data_ptr1,/*输入数据*/
                                      DFT_MODE,/*DFT/IDFT*/
                                      fftc_output_data_address,/*输出数据地址,使用时注意输出数据要偏移4个int才是真正的结果*/
                                      CSL_FFTC_A,/*agc因子，负数表明要右移，整数位左移*/
                                      cp_len,
                                      N_SYM_PER_SLOT,
                                      12);
                     break;
                   }
               } 
               //input_data_ptr1 = &ifft_subframe[used_index * SUBFRAME_LEN * NUM_MAX_ANTENNA + SUBFRAME_LEN  - 2 * (144 + FFT_SIZE)];
               //input_data_ptr1 = &testdata_receive[used_index * SUBFRAME_LEN * NUM_MAX_ANTENNA + SUBFRAME_LEN  - 2 * (144 + FFT_SIZE)];
               /******************************处理符号12~13*****************************************/
               while(1)
               {
               
                   //CACHE_invL2(&gxx_symbol13_flag, sizeof(gxx_symbol13_flag), CACHE_WAIT);
                   //if(1 == gxx_symbol13_flag)//说明符号13到了
                   if(symbol_flag>=13+(slot_idx*7))
                   {
                     gxx_symbol13_flag = 0;
                     input_data_ptr1 = &testdata_receive[used_index * SUBFRAME_LEN * NUM_MAX_ANTENNA + SUBFRAME_LEN*2  - 4 * (144 + FFT_SIZE)];

                     CACHE_invL2(input_data_ptr1,(2048+144)*2*4*2,CACHE_WAIT);
                     //CACHE_invL2(input_data_ptr1+30720,(2048+144)*2*4,CACHE_WAIT);
                     //input_data_ptr1 = &ifft_subframe[used_index * SUBFRAME_LEN * NUM_MAX_ANTENNA + SUBFRAME_LEN  - 2 * (144 + FFT_SIZE)];
                     ok_flag = fft_config(dft_list,/*输入的DFT/IDFTsize，仅支持DFT_sz_list里面的各种size*/
                                      2 * N_SYM_PER_SLOT - 12,/*输入的DFT/IDFTsize个数,目前仅支持每次1种DFTSIZE配置*/
                                      FFT_SIZE,/*输入数据长度必须是4的倍数*/
                                      input_data_ptr1,/*输入数据*/
                                      DFT_MODE,/*DFT/IDFT*/
                                      fftc_output_data_address,/*输出数据地址,使用时注意输出数据要偏移4个int才是真正的结果*/
                                      CSL_FFTC_A,/*agc因子，负数表明要右移，整数位左移*/
                                      cp_len,
                                      12,
                                      2 * N_SYM_PER_SLOT);
                     break;
                   }
               }
           }
          if(0 == gxx_pbch_process_flag)
          {
     	      while(gxx_decoding_core3_core0_flag)
     	      {
     		      CACHE_invL2((void *)&gxx_decoding_core3_core0_flag, sizeof(gxx_decoding_core3_core0_flag), CACHE_WAIT);
     	      }
              gxx_decoding_core3_core0_flag = 1;
    	      CACHE_wbL2((void *)&gxx_decoding_core3_core0_flag, sizeof(gxx_decoding_core3_core0_flag), CACHE_WAIT);
          }


           /*符号4,7,11导频解资源映射*/
           crs_prb_demapping((signed int*)fft_data,   //In, 频域数据首地址
                           NUM_MAX_ANTENNA,       //Para
                           vxx_cell_para.cellID,        //Para,小区ID（0-503）
                           0,        //Para,本次demapping的导频的端口号，目前支持0,1
                           (signed int*)prbdemapping_crs_data,      //Out, 导频数据首地址
                           1,//offset=1表示符号4的导频
                           3//表示3个crs
                          );
           /*符号4,7,11导频解资源映射*/
           crs_prb_demapping((signed int*)fft_data,   //In, 频域数据首地址
                           NUM_MAX_ANTENNA,       //Para
                           vxx_cell_para.cellID,        //Para,小区ID（0-503）
                           1,        //Para,本次demapping的导频的端口号，目前支持0,1
                           (signed int*)prbdemapping_crs_data,      //Out, 导频数据首地址
                           1,//offset=1表示符号4的导频
                           3//表示3个crs
                          );
           /*时隙1端口0信道估计*/
           chan_est((signed int*)prbdemapping_crs_data,   //In, 导频接收数据首地址
                   &rsgen_output[slot_idx][0][0],   //In, 本地导频序列首地址，排列方式同e_freq_crs_ptr；为了方便实现e/d，此处直接存d的共轭
                   NUM_MAX_ANTENNA,         //Para
                   0,            //Para,***目前仅适用于连续分配的prb***
                   N_DL_RB,              //Para
                   0,          //Para,本次demapping的导频的端口号，目前支持0,1
                   (signed int)(rho_a_b[0] << 16),      //Para,由导频功率/数据功率折算出的二者幅度比；为了方便实现e/ratio，此处直接存数据/导频幅度比；以复数形式存储，高16bit有效（实部），低16bit为0
                   (signed int*)crs_che_data,        //Out, 频域信道估计首地址（插值前），排列方式同e_freq_crs_ptr
                   1,//offset=1表示符号4的导频
                   3//表示3个crs
                   );
           /*时隙1端口1信道估计*/
           chan_est((signed int*)prbdemapping_crs_data,   //In, 导频接收数据首地址
                   &rsgen_output[slot_idx][0][0],   //In, 本地导频序列首地址，排列方式同e_freq_crs_ptr；为了方便实现e/d，此处直接存d的共轭
                   NUM_MAX_ANTENNA,         //Para
                   0,            //Para,***目前仅适用于连续分配的prb***
                   N_DL_RB,              //Para
                   1,          //Para,本次demapping的导频的端口号，目前支持0,1
                   (signed int)(rho_a_b[0] << 16),      //Para,由导频功率/数据功率折算出的二者幅度比；为了方便实现e/ratio，此处直接存数据/导频幅度比；以复数形式存储，高16bit有效（实部），低16bit为0
                   (signed int*)crs_che_data,        //Out, 频域信道估计首地址（插值前），排列方式同e_freq_crs_ptr
                   1,//offset=1表示符号4的导频
                   3//表示3个crs
                   );
                   
           /*端口0信道估计插值*/
           chan_interp_freq((signed int*)crs_che_data,        //In, 频域信道估计首地址（插值前），排列方式同e_freq_crs_ptr
                      NUM_MAX_ANTENNA,          //Para
                      0,             //Para,***目前仅适用于连续分配的prb***
                      N_DL_RB,               //Para
                      vxx_cell_para.cellID,         //Para,小区ID（0-503）
                      0,         //Para,本次demapping的导频的端口号，目前支持0,1
                      (signed int*)che_interp_data,      //Out, 频域信道估计首地址，排列方式同e_freq_data_ptr，kaV在最外层
                      1,//offset=1表示符号4的导频
                      3//表示3个crs
                      );
           /*端口1信道估计插值*/
           chan_interp_freq((signed int*)crs_che_data,        //In, 频域信道估计首地址（插值前），排列方式同e_freq_crs_ptr
                      NUM_MAX_ANTENNA,          //Para
                      0,             //Para,***目前仅适用于连续分配的prb***
                      N_DL_RB,               //Para
                      vxx_cell_para.cellID,         //Para,小区ID（0-503）
                      1,         //Para,本次demapping的导频的端口号，目前支持0,1
                      (signed int*)che_interp_data,      //Out, 频域信道估计首地址，排列方式同e_freq_data_ptr，kaV在最外层
                      1,//offset=1表示符号4的导频
                      3//表示3个crs
                      );

           CACHE_wbL2((void *)fft_data, sizeof(fft_data), CACHE_WAIT);
           CACHE_wbL2((void *)che_interp_data, sizeof(che_interp_data), CACHE_WAIT);
           if(0 == gxx_pbch_process_flag)
           {
               gxx_fft_core0_complete_flag = 1;
               CACHE_wbL2((void *)&gxx_fft_core0_complete_flag, sizeof(gxx_fft_core0_complete_flag), CACHE_WAIT);
               gxx_fft_core1_complete_flag = 1;
               CACHE_wbL2((void *)&gxx_fft_core1_complete_flag, sizeof(gxx_fft_core1_complete_flag), CACHE_WAIT);
               gxx_fft_core2_complete_flag = 1;
               CACHE_wbL2((void *)&gxx_fft_core2_complete_flag, sizeof(gxx_fft_core2_complete_flag), CACHE_WAIT);
               gxx_fft_core3_complete_flag = 1;
               CACHE_wbL2((void *)&gxx_fft_core3_complete_flag, sizeof(gxx_fft_core3_complete_flag), CACHE_WAIT);
           }




           /*           for(k = 0;k < 4;k++)
           {
             gxx_fft_complete_flag[k] = 1;//置上完成flag，通知core1/core2/core3可以启动后面计算
             CACHE_wbL2((void *)&gxx_fft_complete_flag[k], sizeof(gxx_fft_complete_flag[k]), CACHE_WAIT);// Wait for the writeback operation to complete.
           }*/
           //puts("core0 fft completed");
           //pbch已经成功该检测后才进入该流程
           if((0 == gxx_pbch_process_flag) && (1 == gxx_fft_core0_complete_flag))//确保信道估计和频域数据已经准备好了
           {
               while(1)
               {


                   CACHE_invL2(&gxx_pdcch_core0_complete_flag, sizeof(gxx_pdcch_core0_complete_flag), CACHE_WAIT);
                   //PDCCH盲检成功后
                   if(1== gxx_pdcch_core0_complete_flag)//表示pdcch译码成功，用户pdsch配置正确获得
                   {
                	   test_cycle = TSC_delay_cycle(test_start);

                	   gxx_pdcch_core0_complete_flag = 0;
                	   gxx_fft_core0_complete_flag = 0;
                	   //puts("core0 flag clear");
                       CACHE_wbL2((void *)&gxx_pdcch_core0_complete_flag, sizeof(gxx_pdcch_core0_complete_flag), CACHE_WAIT);
                       CACHE_wbL2((void *)&gxx_fft_core0_complete_flag, sizeof(gxx_fft_core0_complete_flag), CACHE_WAIT);
                       CACHE_invL2(vxx_user_para, sizeof(vxx_user_para), CACHE_WAIT);
                       if((slot_idx == 2) || (slot_idx == 12))/*特殊子帧*/
                       {
                            if(1 == vxx_cell_para.CFI)
                            {
                                core0_eq_symbol = 3;//特殊子帧最多考虑10个符号每个core的eq要处理的ofdm结束符号，各core按照2,2,3,2个符号分配处理
                                core1_eq_symbol = 5;
                                core2_eq_symbol = 8;
                                core3_eq_symbol = total_symbol;
                            }
                            if(2 == vxx_cell_para.CFI)
                            {
                                core0_eq_symbol = 4;//特殊子帧最多考虑10个符号每个core的eq要处理的ofdm结束符号，各core按照2,2,2,2个符号分配处理
                                core1_eq_symbol = 6;
                                core2_eq_symbol = 8;
                                core3_eq_symbol = total_symbol;
                            }
                            if(3 == vxx_cell_para.CFI)
                            {
                                core0_eq_symbol = 5;//特殊子帧最多考虑10个符号每个core的eq要处理的ofdm结束符号，各core按照2,2,2,1个符号分配处理
                                core1_eq_symbol = 7;
                                core2_eq_symbol = 9;
                                core3_eq_symbol = total_symbol;
                            }
                       }
                       else
                       {
                            if(1 == vxx_cell_para.CFI)
                            {
                                core0_eq_symbol = 4;//每个core的eq要处理的ofdm结束符号，各core按照3,3,4,3个符号分配处理
                                core1_eq_symbol = 7;
                                core2_eq_symbol = 11;
                                core3_eq_symbol = total_symbol;
                            }
                            if(2 == vxx_cell_para.CFI)
                            {
                                core0_eq_symbol = 5;//每个core的eq要处理的ofdm结束符号，各core按照3,3,3,3个符号分配处理
                                core1_eq_symbol = 8;
                                core2_eq_symbol = 11;
                                core3_eq_symbol = total_symbol;
                            }
                            if(3 == vxx_cell_para.CFI)
                            {
                                core0_eq_symbol = 5;//每个core的eq要处理的ofdm结束符号，各core按照2,3,3,3个符号分配处理
                                core1_eq_symbol = 8;
                                core2_eq_symbol = 11;
                                core3_eq_symbol = total_symbol;
                            }
                       }
                       //分核进行时域插值
                       chan_interp_time(
                                        total_symbol,
                                        0,         //Para,本次demapping的导频的端口号，目前支持0,1
                                        vxx_cell_para.CFI,
                                        core0_eq_symbol,
                                        (signed int*)che_interp_data      //in/Out, 频域信道估计首地址（插值后），排列方式同e_freq_data_ptr，kaV在最外层
                                        );
                       chan_interp_time(
                                        total_symbol,
                                        1,         //Para,本次demapping的导频的端口号，目前支持0,1
                                        vxx_cell_para.CFI,
                                        core0_eq_symbol,
                                        (signed int*)che_interp_data      //in/Out, 频域信道估计首地址（插值后），排列方式同e_freq_data_ptr，kaV在最外层
                                        );

                       /*插值后的信道估计解映射*/
                       for(k = vxx_cell_para.CFI; k < core0_eq_symbol;k++)
                       {
                           if(slot_idx==2)
                        	   test_a=1;
                    	   prb_demapping_pdsch((signed int*)che_interp_data[0],   //In, 频域数据首地址
                                                  NUM_MAX_ANTENNA,       //Para           //Para
                                                  vxx_cell_para.cellID,        //Para,小区ID（0-503）
                                                  k,        //Para,本次demapping的符号，这样可以较灵活的匹配时序调整（第几个符号开始处理...）
                                                  slot_idx >> 1,             //Para,本次处理的子帧号（可考虑与ns合并）
                                                  slot_idx,           //Para,本次处理的时隙号（port2/3时需要）
                                                  (signed int*)vxx_fft_lay_buffer.prbdemapping_freq_che[0][0][k - vxx_cell_para.CFI],    //In/Out, demapping后的接收天线0的频域数据首地址（每次传入本符号该存放的首地址，传出下个符号该存放的首地址）
                                                  (signed int*)vxx_fft_lay_buffer.prbdemapping_freq_che[0][1][k - vxx_cell_para.CFI]    //In/Out, demapping后的接收天线0的频域数据首地址（每次传入本符号该存放的首地址，传出下个符号该存放的首地址）
                                                  );
                            prb_demapping_pdsch((signed int*)che_interp_data[1],   //In, 频域数据首地址
                                                  NUM_MAX_ANTENNA,       //Para        //Para
                                                  vxx_cell_para.cellID,        //Para,小区ID（0-503）
                                                  k,        //Para,本次demapping的符号，这样可以较灵活的匹配时序调整（第几个符号开始处理...）
                                                  slot_idx >> 1,             //Para,本次处理的子帧号（可考虑与ns合并）
                                                  slot_idx,           //Para,本次处理的时隙号（port2/3时需要）
                                                  (signed int*)vxx_fft_lay_buffer.prbdemapping_freq_che[1][0][k - vxx_cell_para.CFI],    //In/Out, demapping后的接收天线0的频域数据首地址（每次传入本符号该存放的首地址，传出下个符号该存放的首地址）
                                                  (signed int*)vxx_fft_lay_buffer.prbdemapping_freq_che[1][1][k - vxx_cell_para.CFI]    //In/Out, demapping后的接收天线0的频域数据首地址（每次传入本符号该存放的首地址，传出下个符号该存放的首地址）
                                                  );
                            prb_demapping_pdsch((signed int*)fft_data,   //In, 频域数据首地址
                                                         NUM_MAX_ANTENNA,       //Para          //Para
                                                         vxx_cell_para.cellID,        //Para,小区ID（0-503）
                                                         k,        //Para,本次demapping的符号，这样可以较灵活的匹配时序调整（第几个符号开始处理...）
                                                         slot_idx >> 1,             //Para,本次处理的子帧号（可考虑与ns合并）
                                                         slot_idx,           //Para,本次处理的时隙号（port2/3时需要）
                                                         (signed int*)vxx_desramble_tb_buffer.prbdemapping_freq_data[0][k - vxx_cell_para.CFI],    //In/Out, demapping后的接收天线0的频域数据首地址（每次传入本符号该存放的首地址，传出下个符号该存放的首地址）
                                                         (signed int*)vxx_desramble_tb_buffer.prbdemapping_freq_data[1][k - vxx_cell_para.CFI]    //In/Out, demapping后的接收天线0的频域数据首地址（每次传入本符号该存放的首地址，传出下个符号该存放的首地址）
                                                         );
                       } 

                       /*************************计算每个core要均衡的符号个数***************************/
                       for(k = 0; k < 4;k++)
                       {
                           core_m_symbol[k] = 0;
                       }
                       for(k = vxx_cell_para.CFI; k < core0_eq_symbol;k++)
                       {
                           core_m_symbol[0] += vxx_user_para[0].subcarrier_demap_num[k];//eq处理完后的紧凑排列的调制符号个数
                       }
                       for(k = core0_eq_symbol; k < core1_eq_symbol;k++)
                       {
                           core_m_symbol[1] += vxx_user_para[0].subcarrier_demap_num[k];//eq处理完后的紧凑排列的调制符号个数
                       }
                       for(k = core1_eq_symbol; k < core2_eq_symbol;k++)
                       {
                           core_m_symbol[2] += vxx_user_para[0].subcarrier_demap_num[k];//eq处理完后的紧凑排列的调制符号个数
                       }
                       for(k = core2_eq_symbol; k < core3_eq_symbol;k++)
                       {
                           core_m_symbol[3] += vxx_user_para[0].subcarrier_demap_num[k];//eq处理完后的紧凑排列的调制符号个数
                       }
                       /*************************计算每个core要解调的符号个数***************************/
                       for(i = 0; i < vxx_user_para[0].num_code_word;i++)
                       {
                         for(k = 0; k < 4;k++)
                         {
                             core_start_numBitsG[k][i] = 0;
                         }
                           for(k = 0; k < 1;k++)
                           {
                               core_start_numBitsG[1][i] += core_m_symbol[k] * vxx_user_para[0].modulation[i];
                           }
                           for(k = 0; k < 2;k++)
                           {
                               core_start_numBitsG[2][i] += core_m_symbol[k] * vxx_user_para[0].modulation[i];
                           }
                           for(k = 0; k < 3;k++)
                           {
                               core_start_numBitsG[3][i] += core_m_symbol[k] * vxx_user_para[0].modulation[i];
                           }
                       }
                       /*************************16.3.25测试用***********************************/
                      // memset(&vxx_fft_lay_buffer.prbdemapping_freq_che[0][1][0],0,4800*4);
                      // memset(&vxx_fft_lay_buffer.prbdemapping_freq_che[1][0][0],0,4800*4);
                      // CACHE_wbL2(&vxx_fft_lay_buffer.prbdemapping_freq_che[0][1][0],14400*4, CACHE_NOWAIT);

                       /*********************************分核均衡和解调*************************************/
                       chan_eq((signed int*)vxx_desramble_tb_buffer.prbdemapping_freq_data,     //In, 接收天线的数据
                                   (signed int*)vxx_fft_lay_buffer.prbdemapping_freq_che,   //In, 第1根接收天线的第1个流的第1个子载波，a=h_1_1_c1
                                    vxx_user_para[0].symbol_start,
                                    (signed int*)vxx_demodu_rm_buffer.predecoding_data[0],  //Out, d1
                                    vxx_user_para[0].ueTransMode, /*下行传输模式*/
                                    vxx_user_para[0].subcarrier_demap_num,
                                    vxx_cell_para.CFI,//本core处理的符号开始
                                    core0_eq_symbol,//本core处理的符号结束
                                    core_m_symbol[glbCoreId],
                                    PDSCH,
                                    (signed short*)vxx_che_crs_buffer.llr_data     //Out,LLr结果首地址
                                   );
                       //空分复用且双流 或者发射分集且单流 不用拷贝，节省cycle
                       CACHE_invL2(pdsch_pseudo, sizeof(pdsch_pseudo), CACHE_WAIT);
                       for(k = 0; k < vxx_user_para[0].num_code_word;k++)
                       {
                         demodulation_descramble
                             ((signed short*)&vxx_demodu_rm_buffer.predecoding_data[k],//Q(16,3)
                              vxx_che_crs_buffer.llr_data[k],//Q(16,1)
                             (signed char*)demodulation_data[glbCoreId][k],//Q(8,6)
                              vxx_user_para[0].ueTransMode,
                              vxx_user_para[0].modulation[k],
                              core_m_symbol[glbCoreId],
                              &pdsch_pseudo[k][core_start_numBitsG[glbCoreId][k]]
                             );
                        }
                        gxx_decramble_core0_complete_flag = 1;
                        //puts("core0 processing completed");
                        CACHE_wbL2((void *)&gxx_decramble_core0_complete_flag,sizeof(gxx_decramble_core0_complete_flag), CACHE_NOWAIT);
                        CACHE_wbL2((void *)demodulation_data[glbCoreId],NUM_CODEWORD_2*N_SC_PER_RB * N_DL_RB * 4 * QAM64 , CACHE_NOWAIT);// Wait for the writeback operation to complete.
                        break;

                   }
                   
               }
           }
           else if ((1 == gxx_pbch_process_flag) && (0 == slot_idx))//否则先进入PBCH检测流程
           {

               chan_interp_time(
                              total_symbol,
                              0,         //Para,本次demapping的导频的端口号，目前支持0,1
                              7,
                              11,
                              (signed int*)che_interp_data      //in/Out, 频域信道估计首地址（插值后），排列方式同e_freq_data_ptr，kaV在最外层
                              );
               chan_interp_time(
                            total_symbol,
                              1,         //Para,本次demapping的导频的端口号，目前支持0,1
                              7,
                              11,
                              (signed int*)che_interp_data      //in/Out, 频域信道估计首地址（插值后），排列方式同e_freq_data_ptr，kaV在最外层
                               );
               /*************参数更新********************/
               /*大部分小区参数在小区同步时候已经获取了，PBCH主要是获取帧号信息和带宽信息。带宽不影响PBCH位置计算，
               因为肯定是1200数据的中间。不过影响fftdata数据读取的开始位置，本链路仅考虑100prb场景*/                                       
               n_id_2 = vxx_cell_para.cellID % 3;
               PBCHCarrierIdx = (unsigned char*)(PBCH_index[n_id_2]);
               //提取PBCH频域数据
               prb_demapping_pbch(
                                  &fft_data[0][N_SYM_PER_SLOT * N_DL_RB * N_SC_PER_RB + vxx_cell_para.PBCHSyncSCStart],
                                  &fft_data[1][N_SYM_PER_SLOT * N_DL_RB * N_SC_PER_RB + vxx_cell_para.PBCHSyncSCStart],
                                  vxx_pbch_freq_lay_data.pbch_freq_data[0],
                                  vxx_pbch_freq_lay_data.pbch_freq_data[1],
                                  PBCHCarrierIdx
                                 );
               //提取PBCH信道估计
               prb_demapping_pbch(
                                  &che_interp_data[0][0][N_SYM_PER_SLOT][vxx_cell_para.PBCHSyncSCStart],
                                  &che_interp_data[0][1][N_SYM_PER_SLOT][vxx_cell_para.PBCHSyncSCStart],
                                  &vxx_pbch_che_llr_data.pbch_che_data[0][0],
                                  &vxx_pbch_che_llr_data.pbch_che_data[0][1],
                                  PBCHCarrierIdx
                                 );    
               prb_demapping_pbch(
                                  &che_interp_data[1][0][N_SYM_PER_SLOT][vxx_cell_para.PBCHSyncSCStart],
                                  &che_interp_data[1][1][N_SYM_PER_SLOT][vxx_cell_para.PBCHSyncSCStart],
                                  &vxx_pbch_che_llr_data.pbch_che_data[1][0],
                                  &vxx_pbch_che_llr_data.pbch_che_data[1][1],
                                  PBCHCarrierIdx
                                 );    
               temp[0] = 240;
               symbol_start[0] = 0;
               chan_eq((signed int*)vxx_pbch_freq_lay_data.pbch_freq_data,     //In, 接收天线的数据
                       (signed int*)vxx_pbch_che_llr_data.pbch_che_data,   //In, 第1根接收天线的第1个流的第1个子载波，a=h_1_1_c1
                        symbol_start,
                        vxx_precoding_demod_data.pbch_precoding_data,  //Out, d1
                        TRANSMIT_DIVERSITY, /*下行传输模式*/
                        temp,
                        0,
                        1,
                        N_PBCH_SYMBOL,
                        PBCH,
                        pbch_pcfich_pdcch_llr_data     //Out,LLr结果首地址
                       );
               
               demodulation((signed short*)vxx_precoding_demod_data.pbch_precoding_data,//Q(16,3)
                            pbch_pcfich_pdcch_llr_data,//Q(16,1)
                            vxx_pbch_freq_lay_data.pbch_demodulation_data,//Q(8,6)
                            TRANSMIT_DIVERSITY,
                            QPSK,
                            N_PBCH_SYMBOL
                            );
               
               //扰码生成
               cInit = vxx_cell_para.cellID;
               Descrambling_data_pbch_gen(x1_sequence_table,
                                    cInit,
                                    N_PBCH_SYMBOL * 4 * 2,
                                    vxx_pbch_che_llr_data.descrambling_bits);
               //4次解扰、解速率匹配、译码
               for(i = 0; i < 4;i++)
               {
                   BitDescrambling_pbch(vxx_pbch_freq_lay_data.pbch_demodulation_data,
                             vxx_precoding_demod_data.descramble_data,//vxx_user_para[user_index].descramble_data_ptr[i],
                             &vxx_pbch_che_llr_data.descrambling_bits[i * (N_PBCH_SYMBOL*4*2/32/4)],
                                 N_PBCH_SYMBOL*2);
                   cc_conv_derate_matching(vxx_precoding_demod_data.descramble_data,
                                         PBCH_TBSIZE + 16,
                                         N_PBCH_SYMBOL * 2,
                                         (char*)srcBuf[0],
                                         (char*)&srcBuf[0] + PBCH_TBSIZE + 16,
                                         (char*)&srcBuf[0] + 2 * (PBCH_TBSIZE + 16));
               
                   crc_out = cc_conv_decoder(PBCH_TBSIZE + 16,
                                        N_PBCH_SYMBOL * 2,
                                       (char*)srcBuf[0],
                                       (char*)&srcBuf[0] + PBCH_TBSIZE + 16,
                                       (char*)&srcBuf[0] + 2 * (PBCH_TBSIZE + 16),
                                       (short*)srcBuf[1],
                                       (char*)srcBuf[2],
                                       &vxx_pbch_freq_lay_data.decoded_bits,
                                       16);
                   pbch_ok_flag = crc_decoder(&vxx_pbch_freq_lay_data.decoded_bits,
                                                       LTE_CRC16,
                                                       crc_out,
                                                       PBCH_TBSIZE,
                                                       &vxx_cell_para,
                                                       PBCH,
                                                       0);
                   vxx_pbch_freq_lay_data.decoded_bits = vxx_pbch_freq_lay_data.decoded_bits >> 8;
                   phytomac_pbchd.SourcePBCH = vxx_pbch_freq_lay_data.decoded_bits;//2015.3.30添加
                   if(0 == pbch_ok_flag)
                   {
                	   pbch_crc_right++;
                	   break;
                   }

#if 0
                   if(0 == pbch_ok_flag)              //0表示PBCH CRC校验成功
                   {
                       vxx_cell_para.num_PRB_per_bw_config = vxx_dl_Bandwidth[(vxx_pbch_freq_lay_data.decoded_bits >> 21) & 0x7];
                       vxx_cell_para.PBCHSyncSCStart = ((vxx_cell_para.num_PRB_per_bw_config*N_SC_PER_RB)/2) - 36;
                       vxx_cell_para.PBCHSyncSCEnd = ((vxx_cell_para.num_PRB_per_bw_config*N_SC_PER_RB)/2) + 35;
                       vxx_cell_para.phich_Durtion = (vxx_pbch_freq_lay_data.decoded_bits >> 20) & 0x1;
                       vxx_cell_para.g_phich_Ng = (vxx_pbch_freq_lay_data.decoded_bits >> 18) & 0x3;
                       f_dl_database_updata(&vxx_cell_para);
                       for(i = 0;i < 20;i+=2)//遍历生成该用户所有时隙的所有码字的扰码
                       {
                           f_dl_phich_mapping(i >> 1,
                                              g_N_group_phich[i >> 1],
                                              g_dl_resource_mapping.phich_mapping[i >> 1]);

                       }
                       CACHE_wbL2((void *)&vxx_cell_para, sizeof(vxx_cell_para), CACHE_WAIT);// Wait for the writeback operation to complete.
                       //CACHE_wbL2((void *)&gxx_pbch_process_flag, sizeof(gxx_pbch_process_flag), CACHE_WAIT);// Wait for the writeback operation to complete.
                       break;
               
                   }
#endif

               }
               /*********************PBCH数据构造与发送**************************/
               UEphyto719_pbch[0]=1;
               UEphyto719_pbch[1]=1-pbch_ok_flag;
               UEphyto719_pbch[2]= phytomac_pbchd.SourcePBCH;
               UEphyto719_pbch[3]=0;
               CACHE_wbL2((void *) UEphyto719_pbch, sizeof( UEphyto719_pbch), CACHE_WAIT);
              // LTE_Test(0,1);
               gxx_pbch_core0_core3_flag=1;
               CACHE_wbL2((void *)gxx_pbch_core0_core3_flag, sizeof( gxx_pbch_core0_core3_flag), CACHE_WAIT);
               pbch_num++;
#endif
           }

            used_index++;
            CACHE_wbL2((void *)&used_index, sizeof(used_index), CACHE_WAIT);

           //gxx_cycle_c0[slot_idx>>1] = TSC_delay_cycle(fftc_start);
            gxx_cycle_c0[slot_idx>>1] = TSC_delay_cycle(fftc_start);
            gxx_cycle_c0_2[slot_idx>>1] = TSC_delay_cycle(cycle_start);//这里测出来数据上行子帧后的子帧时延较久，因为这个时间包括了上行子帧处理时等待下一子帧的时间




    }
    if(1 == glbCoreId)
    {

        while(1)
        {
        	while(1)
        	{
                CACHE_invL2(&gxx_pdcch_detect_flag, sizeof(gxx_pdcch_detect_flag), CACHE_WAIT);
                CACHE_invL2((void *)&gxx_slot_idx_c0, sizeof(gxx_slot_idx_c0), CACHE_WAIT);
                gxx_slot_idx_c1 = gxx_slot_idx_c0;//拷贝到core1的gxx_slot_idx上，便于core2使用，否则core0早就更新
                if(1 == gxx_pdcch_detect_flag)//启动PDCCH的盲检
                {
                	//puts("core1 pdcch detecting started");
                    cycle_start = TSCL;
                    slot_idx = gxx_slot_idx_c1;
                    if(slot_idx==0)
                    {
          			  test_start1 = TSCL;
          			  //CACHE_wbL2((void *)&test_start, sizeof(test_start), CACHE_WAIT);
                    }
                    /*************参数更新********************/
                    gxx_pdcch_detect_flag = 0;
                    CACHE_wbL2((void *)&gxx_pdcch_detect_flag, sizeof(gxx_pdcch_detect_flag), CACHE_WAIT);

                    CSL_XMC_invalidatePrefetchBuffer();
          		  		    _mfence();
          		  		    _mfence();
                    CACHE_invL2(&vxx_cell_para, sizeof(vxx_cell_para), CACHE_WAIT);
                    CACHE_invL2(vxx_user_para, sizeof(vxx_user_para), CACHE_WAIT);
                    CACHE_invL2((void *)pdcch_data_after_descram, sizeof(pdcch_data_after_descram), CACHE_WAIT);
                    f_pdcch_monitor_proc_part2(
                                              &vxx_cell_para,
                                              slot_idx >> 1,
                                              vxx_user_para[0].rnti,
                                              vxx_user_para[0].common_rnti,
                                              gxx_pdcch_total_reg_num
                                              );
//                    vxx_user_para[0] = vxx_user_para_buffer[0];
//                    CACHE_wbL2(vxx_user_para, sizeof(vxx_user_para), CACHE_WAIT);
                    //puts("core1 pdcch detecting completed");
                    break;
                }
        	}

 
              while(1)
              {
                  CACHE_invL2(&gxx_pdcch_core1_complete_flag, sizeof(gxx_pdcch_core1_complete_flag), CACHE_WAIT);
                  CACHE_invL2(&gxx_fft_core1_complete_flag, sizeof(gxx_fft_core1_complete_flag), CACHE_WAIT);
                  //PDCCH盲检成功后
                  if((1 == gxx_pdcch_core1_complete_flag) && (1 == gxx_fft_core1_complete_flag))//表示pdcch译码成功，用户pdsch配置正确获得
                  {
                	  gxx_pdcch_core1_complete_flag = 0;
                	  gxx_fft_core1_complete_flag = 0;
                      //puts("core1 flag clear");
                      CACHE_wbL2((void *)&gxx_pdcch_core1_complete_flag, sizeof(gxx_pdcch_core1_complete_flag), CACHE_WAIT);
                      CACHE_wbL2((void *)&gxx_fft_core1_complete_flag, sizeof(gxx_fft_core1_complete_flag), CACHE_WAIT);

            		  CSL_XMC_invalidatePrefetchBuffer();
            		  		    _mfence();
            		  		    _mfence();
            		  CACHE_invL2((void*)che_interp_data, sizeof(che_interp_data), CACHE_WAIT);
            		  CACHE_invL2((void*)fft_data, sizeof(fft_data), CACHE_WAIT);
                      if((slot_idx == 2) || (slot_idx == 12))/*特殊子帧*/
                      {
                           total_symbol = vxx_cell_para.dwpts_symbol;
                           if(1 == vxx_cell_para.CFI)
                           {
                               core0_eq_symbol = 3;//特殊子帧最多考虑10个符号每个core的eq要处理的ofdm结束符号，各core按照2,2,3,2个符号分配处理
                               core1_eq_symbol = 5;
                               core2_eq_symbol = 8;
                               core3_eq_symbol = total_symbol;
                           }
                           if(2 == vxx_cell_para.CFI)
                           {
                               core0_eq_symbol = 4;//特殊子帧最多考虑10个符号每个core的eq要处理的ofdm结束符号，各core按照2,2,2,2个符号分配处理
                               core1_eq_symbol = 6;
                               core2_eq_symbol = 8;
                               core3_eq_symbol = total_symbol;
                           }
                           if(3 == vxx_cell_para.CFI)
                           {
                               core0_eq_symbol = 5;//特殊子帧最多考虑10个符号每个core的eq要处理的ofdm结束符号，各core按照2,2,2,1个符号分配处理
                               core1_eq_symbol = 7;
                               core2_eq_symbol = 9;
                               core3_eq_symbol = total_symbol;
                           }
                      }
                      else
                      {
                           total_symbol = 2 * N_SYM_PER_SLOT;
                           if(1 == vxx_cell_para.CFI)
                           {
                               core0_eq_symbol = 4;//每个core的eq要处理的ofdm结束符号，各core按照3,3,4,3个符号分配处理
                               core1_eq_symbol = 7;
                               core2_eq_symbol = 11;
                               core3_eq_symbol = total_symbol;
                           }
                           if(2 == vxx_cell_para.CFI)
                           {
                               core0_eq_symbol = 5;//每个core的eq要处理的ofdm结束符号，各core按照3,3,3,3个符号分配处理
                               core1_eq_symbol = 8;
                               core2_eq_symbol = 11;
                               core3_eq_symbol = total_symbol;
                           }
                           if(3 == vxx_cell_para.CFI)
                           {
                               core0_eq_symbol = 5;//每个core的eq要处理的ofdm结束符号，各core按照2,3,3,3个符号分配处理
                               core1_eq_symbol = 8;
                               core2_eq_symbol = 11;
                               core3_eq_symbol = total_symbol;
                           }
                      }
                      //分核进行时域插值
                      chan_interp_time(
                                       total_symbol,
                                       0,         //Para,本次demapping的导频的端口号，目前支持0,1
                                       core0_eq_symbol,
                                       core1_eq_symbol,
                                       (signed int*)che_interp_data      //in/Out, 频域信道估计首地址（插值后），排列方式同e_freq_data_ptr，kaV在最外层
                                       );
                      chan_interp_time(
                                       total_symbol,
                                       1,         //Para,本次demapping的导频的端口号，目前支持0,1
                                       core0_eq_symbol,
                                       core1_eq_symbol,
                                       (signed int*)che_interp_data      //in/Out, 频域信道估计首地址（插值后），排列方式同e_freq_data_ptr，kaV在最外层
                                       );

                      /*插值后的信道估计解映射*/
                      for(k = core0_eq_symbol; k < core1_eq_symbol;k++)
                      {
                           prb_demapping_pdsch((signed int*)che_interp_data[0],   //In, 频域数据首地址
                                                 NUM_MAX_ANTENNA,       //Para           //Para
                                                 vxx_cell_para.cellID,        //Para,小区ID（0-503）
                                                 k,        //Para,本次demapping的符号，这样可以较灵活的匹配时序调整（第几个符号开始处理...）
                                                 slot_idx >> 1,             //Para,本次处理的子帧号（可考虑与ns合并）
                                                 slot_idx,           //Para,本次处理的时隙号（port2/3时需要）
                                                 (signed int*)vxx_fft_lay_buffer.prbdemapping_freq_che[0][0][k - core0_eq_symbol],    //In/Out, demapping后的接收天线0的频域数据首地址（每次传入本符号该存放的首地址，传出下个符号该存放的首地址）
                                                 (signed int*)vxx_fft_lay_buffer.prbdemapping_freq_che[0][1][k - core0_eq_symbol]    //In/Out, demapping后的接收天线0的频域数据首地址（每次传入本符号该存放的首地址，传出下个符号该存放的首地址）
                                                 );
                           prb_demapping_pdsch((signed int*)che_interp_data[1],   //In, 频域数据首地址
                                                 NUM_MAX_ANTENNA,       //Para           //Para
                                                 vxx_cell_para.cellID,        //Para,小区ID（0-503）
                                                 k,        //Para,本次demapping的符号，这样可以较灵活的匹配时序调整（第几个符号开始处理...）
                                                 slot_idx >> 1,             //Para,本次处理的子帧号（可考虑与ns合并）
                                                 slot_idx,           //Para,本次处理的时隙号（port2/3时需要）
                                                 (signed int*)vxx_fft_lay_buffer.prbdemapping_freq_che[1][0][k - core0_eq_symbol],    //In/Out, demapping后的接收天线0的频域数据首地址（每次传入本符号该存放的首地址，传出下个符号该存放的首地址）
                                                 (signed int*)vxx_fft_lay_buffer.prbdemapping_freq_che[1][1][k - core0_eq_symbol]    //In/Out, demapping后的接收天线0的频域数据首地址（每次传入本符号该存放的首地址，传出下个符号该存放的首地址）
                                                 );
                           prb_demapping_pdsch((signed int*)fft_data,   //In, 频域数据首地址
                                                        NUM_MAX_ANTENNA,       //Para           //Para
                                                        vxx_cell_para.cellID,        //Para,小区ID（0-503）
                                                        k,        //Para,本次demapping的符号，这样可以较灵活的匹配时序调整（第几个符号开始处理...）
                                                        slot_idx >> 1,             //Para,本次处理的子帧号（可考虑与ns合并）
                                                        slot_idx,           //Para,本次处理的时隙号（port2/3时需要）
                                                        (signed int*)vxx_desramble_tb_buffer.prbdemapping_freq_data[0][k - core0_eq_symbol],    //In/Out, demapping后的接收天线0的频域数据首地址（每次传入本符号该存放的首地址，传出下个符号该存放的首地址）
                                                        (signed int*)vxx_desramble_tb_buffer.prbdemapping_freq_data[1][k - core0_eq_symbol]    //In/Out, demapping后的接收天线0的频域数据首地址（每次传入本符号该存放的首地址，传出下个符号该存放的首地址）
                                                        );
                      } 

                      /*************************计算每个core要均衡的符号个数***************************/
                      for(k = 0; k < 4;k++)
                      {
                          core_m_symbol[k] = 0;
                      }
                      for(k = vxx_cell_para.CFI; k < core0_eq_symbol;k++)
                      {
                          core_m_symbol[0] += vxx_user_para[0].subcarrier_demap_num[k];//eq处理完后的紧凑排列的调制符号个数
                      }
                      for(k = core0_eq_symbol; k < core1_eq_symbol;k++)
                      {
                          core_m_symbol[1] += vxx_user_para[0].subcarrier_demap_num[k];//eq处理完后的紧凑排列的调制符号个数
                      }
                      for(k = core1_eq_symbol; k < core2_eq_symbol;k++)
                      {
                          core_m_symbol[2] += vxx_user_para[0].subcarrier_demap_num[k];//eq处理完后的紧凑排列的调制符号个数
                      }
                      for(k = core2_eq_symbol; k < core3_eq_symbol;k++)
                      {
                          core_m_symbol[3] += vxx_user_para[0].subcarrier_demap_num[k];//eq处理完后的紧凑排列的调制符号个数
                      }
                      /*************************计算每个core要解调的符号个数***************************/
                      for(i = 0; i < vxx_user_para[0].num_code_word;i++)
                      {
                         for(k = 0; k < 4;k++)
                         {
                            core_start_numBitsG[k][i] = 0;
                         }
                           for(k = 0; k < 1;k++)
                           {
                                core_start_numBitsG[1][i] += core_m_symbol[k] * vxx_user_para[0].modulation[i];
                           }
                           for(k = 0; k < 2;k++)
                           {
                                core_start_numBitsG[2][i] += core_m_symbol[k] * vxx_user_para[0].modulation[i];
                           }
                           for(k = 0; k < 3;k++)
                           {
                                core_start_numBitsG[3][i] += core_m_symbol[k] * vxx_user_para[0].modulation[i];
                           }
                      }
                      /*********************************分核均衡和解调*************************************/
                      chan_eq((signed int*)vxx_desramble_tb_buffer.prbdemapping_freq_data,     //In, 接收天线的数据
                                  (signed int*)vxx_fft_lay_buffer.prbdemapping_freq_che,   //In, 第1根接收天线的第1个流的第1个子载波，a=h_1_1_c1
                                   vxx_user_para[0].symbol_start,
                                   (signed int*)vxx_demodu_rm_buffer.predecoding_data,  //Out, d1
                                   vxx_user_para[0].ueTransMode, /*下行传输模式*/
                                   vxx_user_para[0].subcarrier_demap_num,
                                   core0_eq_symbol,//本core处理的符号开始
                                   core1_eq_symbol,//本core处理的符号结束
                                   core_m_symbol[glbCoreId],
                                   PDSCH,
                                   (signed short*)vxx_che_crs_buffer.llr_data     //Out,LLr结果首地址
                                  );
                      //空分复用且双流 或者发射分集且单流 不用拷贝，节省cycle
                      CACHE_invL2(pdsch_pseudo, sizeof(pdsch_pseudo), CACHE_WAIT);
                      for(k = 0; k < vxx_user_para[0].num_code_word;k++)
                      {
                           demodulation_descramble
                             ((signed short*)&vxx_demodu_rm_buffer.predecoding_data[k],//Q(16,3)
                              vxx_che_crs_buffer.llr_data[k],//Q(16,1)
                             (signed char*)demodulation_data[glbCoreId][k],//Q(8,6)
                              vxx_user_para[0].ueTransMode,
                              vxx_user_para[0].modulation[k],
                              core_m_symbol[glbCoreId],
                              &pdsch_pseudo[k][core_start_numBitsG[glbCoreId][k]]
                             );
                      }
                      gxx_decramble_core1_complete_flag = 1;

                      //puts("core1 processing completed");
                      CACHE_wbL2((void *)&gxx_decramble_core1_complete_flag,sizeof(gxx_decramble_core1_complete_flag), CACHE_NOWAIT);
                      CACHE_wbL2((void *)demodulation_data[glbCoreId],NUM_CODEWORD_2*N_SC_PER_RB * N_DL_RB * 4 * QAM64 , CACHE_NOWAIT);// Wait for the writeback operation to complete.
                      gxx_cycle_c1[slot_idx>>1] = TSC_delay_cycle(cycle_start);
                      break;

                  }
                  
              }
        }
    }
    if(2 == glbCoreId)
    {
        while(1)
        {
            CACHE_invL2(&pdsch_pseudo_copy_flag, sizeof(pdsch_pseudo_copy_flag), CACHE_WAIT);
            if(1 == pdsch_pseudo_copy_flag)
            {
                //puts("core2 copy started");
            	CACHE_invL2(vxx_user_para, sizeof(vxx_user_para), CACHE_WAIT);
            	CACHE_invL2(pdsch_pseudo_init, sizeof(pdsch_pseudo_init), CACHE_WAIT);
                memcpy(pdsch_pseudo[0],pdsch_pseudo_init[vxx_user_para[0].rnti-61][(gxx_slot_idx_c0/2)%10][0],PDSCH_PSEUDO_LENGTH);
                memcpy(pdsch_pseudo[1],pdsch_pseudo_init[vxx_user_para[0].rnti-61][(gxx_slot_idx_c0/2)%10][1],PDSCH_PSEUDO_LENGTH);
                CACHE_wbL2((void *)pdsch_pseudo, sizeof(pdsch_pseudo), CACHE_WAIT);
                //printf("core2 copy %d completed",gxx_slot_idx_c0);
                pdsch_pseudo_copy_flag = 0;
                CACHE_wbL2((void *)&pdsch_pseudo_copy_flag, sizeof(pdsch_pseudo_copy_flag), CACHE_WAIT);
            }
            CACHE_invL2(&gxx_pdcch_core2_complete_flag, sizeof(gxx_pdcch_core2_complete_flag), CACHE_WAIT);
            CACHE_invL2(&gxx_fft_core2_complete_flag, sizeof(gxx_fft_core2_complete_flag), CACHE_WAIT);

            CACHE_invL2((void *)&gxx_slot_idx_c0, sizeof(gxx_slot_idx_c0), CACHE_WAIT);
            gxx_slot_idx_c2 = gxx_slot_idx_c0;
            //PDCCH盲检成功后
            if((1 == gxx_pdcch_core2_complete_flag) && (1 == gxx_fft_core2_complete_flag))//表示pdcch译码成功，用户pdsch配置正确获得
            {
                cycle_start = TSCL;
                gxx_pdcch_core2_complete_flag = 0;
                gxx_fft_core2_complete_flag = 0;
                //puts("core2 flag clear");
                CACHE_wbL2((void *)&gxx_pdcch_core2_complete_flag, sizeof(gxx_pdcch_core2_complete_flag), CACHE_WAIT);
                CACHE_wbL2((void *)&gxx_fft_core2_complete_flag, sizeof(gxx_fft_core2_complete_flag), CACHE_WAIT);
                CSL_XMC_invalidatePrefetchBuffer();
      		  		    _mfence();
      		  		    _mfence();
                CACHE_invL2(vxx_user_para, sizeof(vxx_user_para), CACHE_WAIT);
                CACHE_invL2(&vxx_cell_para, sizeof(vxx_cell_para), CACHE_WAIT);
      		    CACHE_invL2((void*)che_interp_data, sizeof(che_interp_data), CACHE_WAIT);
      		    CACHE_invL2((void*)fft_data, sizeof(fft_data), CACHE_WAIT);
                slot_idx = gxx_slot_idx_c2;

                if(slot_idx==0)
                {
      			  test_start2 = TSCL;
      			  //CACHE_wbL2((void *)&test_start, sizeof(test_start), CACHE_WAIT);
                }




                if((slot_idx == 2) || (slot_idx == 12))//特殊子帧
                {
                     total_symbol = vxx_cell_para.dwpts_symbol;
                     if(1 == vxx_cell_para.CFI)
                     {
                         core0_eq_symbol = 3;//特殊子帧最多考虑10个符号每个core的eq要处理的ofdm结束符号，各core按照2,2,3,2个符号分配处理
                         core1_eq_symbol = 5;
                         core2_eq_symbol = 8;
                         core3_eq_symbol = total_symbol;
                     }
                     if(2 == vxx_cell_para.CFI)
                     {
                         core0_eq_symbol = 4;//特殊子帧最多考虑10个符号每个core的eq要处理的ofdm结束符号，各core按照2,2,2,2个符号分配处理
                         core1_eq_symbol = 6;
                         core2_eq_symbol = 8;
                         core3_eq_symbol = total_symbol;
                     }
                     if(3 == vxx_cell_para.CFI)
                     {
                         core0_eq_symbol = 5;//特殊子帧最多考虑10个符号每个core的eq要处理的ofdm结束符号，各core按照2,2,2,1个符号分配处理
                         core1_eq_symbol = 7;
                         core2_eq_symbol = 9;
                         core3_eq_symbol = total_symbol;
                     }
                }
                else
                {
                     total_symbol = 2 * N_SYM_PER_SLOT;
                     if(1 == vxx_cell_para.CFI)
                     {
                         core0_eq_symbol = 4;//每个core的eq要处理的ofdm结束符号，各core按照3,3,4,3个符号分配处理
                         core1_eq_symbol = 7;
                         core2_eq_symbol = 11;
                         core3_eq_symbol = total_symbol;
                     }
                     if(2 == vxx_cell_para.CFI)
                     {
                         core0_eq_symbol = 5;//每个core的eq要处理的ofdm结束符号，各core按照3,3,3,3个符号分配处理
                         core1_eq_symbol = 8;
                         core2_eq_symbol = 11;
                         core3_eq_symbol = total_symbol;
                     }
                     if(3 == vxx_cell_para.CFI)
                     {
                         core0_eq_symbol = 5;//每个core的eq要处理的ofdm结束符号，各core按照2,3,3,3个符号分配处理
                         core1_eq_symbol = 8;
                         core2_eq_symbol = 11;
                         core3_eq_symbol = total_symbol;
                     }
                }
                //分核进行时域插值
                chan_interp_time(
                                 total_symbol,
                                 0,         //Para,本次demapping的导频的端口号，目前支持0,1
                                 core1_eq_symbol,
                                 core2_eq_symbol,
                                 (signed int*)che_interp_data      //in/Out, 频域信道估计首地址（插值后），排列方式同e_freq_data_ptr，kaV在最外层
                                 );
                chan_interp_time(
                                 total_symbol,
                                 1,         //Para,本次demapping的导频的端口号，目前支持0,1
                                 core1_eq_symbol,
                                 core2_eq_symbol,
                                 (signed int*)che_interp_data      //in/Out, 频域信道估计首地址（插值后），排列方式同e_freq_data_ptr，kaV在最外层
                                 );
                //插值后的信道估计解映射
                for(k = core1_eq_symbol; k < core2_eq_symbol;k++)
                {
                     prb_demapping_pdsch((signed int*)che_interp_data[0],   //In, 频域数据首地址
                                           NUM_MAX_ANTENNA,       //Para           //Para
                                           vxx_cell_para.cellID,        //Para,小区ID（0-503）
                                           k,        //Para,本次demapping的符号，这样可以较灵活的匹配时序调整（第几个符号开始处理...）
                                           slot_idx >> 1,             //Para,本次处理的子帧号（可考虑与ns合并）
                                           slot_idx,           //Para,本次处理的时隙号（port2/3时需要）
                                           (signed int*)vxx_fft_lay_buffer.prbdemapping_freq_che[0][0][k - core1_eq_symbol],    //In/Out, demapping后的接收天线0的频域数据首地址（每次传入本符号该存放的首地址，传出下个符号该存放的首地址）
                                           (signed int*)vxx_fft_lay_buffer.prbdemapping_freq_che[0][1][k - core1_eq_symbol]    //In/Out, demapping后的接收天线0的频域数据首地址（每次传入本符号该存放的首地址，传出下个符号该存放的首地址）
                                           );
                     prb_demapping_pdsch((signed int*)che_interp_data[1],   //In, 频域数据首地址
                                           NUM_MAX_ANTENNA,       //Para         //Para
                                           vxx_cell_para.cellID,        //Para,小区ID（0-503）
                                           k,        //Para,本次demapping的符号，这样可以较灵活的匹配时序调整（第几个符号开始处理...）
                                           slot_idx >> 1,             //Para,本次处理的子帧号（可考虑与ns合并）
                                           slot_idx,           //Para,本次处理的时隙号（port2/3时需要）
                                           (signed int*)vxx_fft_lay_buffer.prbdemapping_freq_che[1][0][k - core1_eq_symbol],    //In/Out, demapping后的接收天线0的频域数据首地址（每次传入本符号该存放的首地址，传出下个符号该存放的首地址）
                                           (signed int*)vxx_fft_lay_buffer.prbdemapping_freq_che[1][1][k - core1_eq_symbol]    //In/Out, demapping后的接收天线0的频域数据首地址（每次传入本符号该存放的首地址，传出下个符号该存放的首地址）
                                           );
                     prb_demapping_pdsch((signed int*)fft_data,   //In, 频域数据首地址
                                                  NUM_MAX_ANTENNA,       //Para           //Para
                                                  vxx_cell_para.cellID,        //Para,小区ID（0-503）
                                                  k,        //Para,本次demapping的符号，这样可以较灵活的匹配时序调整（第几个符号开始处理...）
                                                  slot_idx >> 1,             //Para,本次处理的子帧号（可考虑与ns合并）
                                                  slot_idx,           //Para,本次处理的时隙号（port2/3时需要）
                                                  (signed int*)vxx_desramble_tb_buffer.prbdemapping_freq_data[0][k - core1_eq_symbol],    //In/Out, demapping后的接收天线0的频域数据首地址（每次传入本符号该存放的首地址，传出下个符号该存放的首地址）
                                                  (signed int*)vxx_desramble_tb_buffer.prbdemapping_freq_data[1][k - core1_eq_symbol]    //In/Out, demapping后的接收天线0的频域数据首地址（每次传入本符号该存放的首地址，传出下个符号该存放的首地址）
                                                  );
                } 

                /***********************计算每个core要均衡的符号个数*************************/
                for(k = 0; k < 4;k++)
                {
                    core_m_symbol[k] = 0;
                }
                for(k = vxx_cell_para.CFI; k < core0_eq_symbol;k++)
                {
                    core_m_symbol[0] += vxx_user_para[0].subcarrier_demap_num[k];//eq处理完后的紧凑排列的调制符号个数
                }
                for(k = core0_eq_symbol; k < core1_eq_symbol;k++)
                {
                    core_m_symbol[1] += vxx_user_para[0].subcarrier_demap_num[k];//eq处理完后的紧凑排列的调制符号个数
                }
                for(k = core1_eq_symbol; k < core2_eq_symbol;k++)
                {
                    core_m_symbol[2] += vxx_user_para[0].subcarrier_demap_num[k];//eq处理完后的紧凑排列的调制符号个数
                }
                for(k = core2_eq_symbol; k < core3_eq_symbol;k++)
                {
                    core_m_symbol[3] += vxx_user_para[0].subcarrier_demap_num[k];//eq处理完后的紧凑排列的调制符号个数
                }
                /***********************计算每个core要解调的符号个数*************************/
                for(i = 0; i < vxx_user_para[0].num_code_word;i++)
                {
                   for(k = 0; k < 4;k++)
                   {
                       core_start_numBitsG[k][i] = 0;
                   }
                     for(k = 0; k < 1;k++)
                     {
                         core_start_numBitsG[1][i] += core_m_symbol[k] * vxx_user_para[0].modulation[i];
                     }
                     for(k = 0; k < 2;k++)
                     {
                         core_start_numBitsG[2][i] += core_m_symbol[k] * vxx_user_para[0].modulation[i];
                     }
                     for(k = 0; k < 3;k++)
                     {
                         core_start_numBitsG[3][i] += core_m_symbol[k] * vxx_user_para[0].modulation[i];
                     }
                }
                /*******************************分核均衡和解调***********************************/
                chan_eq((signed int*)vxx_desramble_tb_buffer.prbdemapping_freq_data,     //In, 接收天线的数据
                            (signed int*)vxx_fft_lay_buffer.prbdemapping_freq_che,   //In, 第1根接收天线的第1个流的第1个子载波，a=h_1_1_c1
                             vxx_user_para[0].symbol_start,
                             (signed int*)vxx_demodu_rm_buffer.predecoding_data,  //Out, d1
                             vxx_user_para[0].ueTransMode, //下行传输模式
                             vxx_user_para[0].subcarrier_demap_num,
                             core1_eq_symbol,//本core处理的符号开始
                             core2_eq_symbol,//本core处理的符号结束
                             core_m_symbol[glbCoreId],
                             PDSCH,
                             (signed short*)vxx_che_crs_buffer.llr_data     //Out,LLr结果首地址
                            );
                //空分复用且双流 或者发射分集且单流 不用拷贝，节省cycle
                CACHE_invL2(pdsch_pseudo, sizeof(pdsch_pseudo), CACHE_WAIT);
                for(k = 0; k < vxx_user_para[0].num_code_word;k++)
                {
                    demodulation_descramble
                             ((signed short*)&vxx_demodu_rm_buffer.predecoding_data[k],//Q(16,3)
                              vxx_che_crs_buffer.llr_data[k],//Q(16,1)
                             (signed char*)demodulation_data[glbCoreId][k],//Q(8,6)
                              vxx_user_para[0].ueTransMode,
                              vxx_user_para[0].modulation[k],
                              core_m_symbol[glbCoreId],
                              &pdsch_pseudo[k][core_start_numBitsG[glbCoreId][k]]
                             );
                }
                gxx_decramble_core2_complete_flag = 1;
                //puts("core2 processing completed");
                //printf("core2 flag is %d and %d\n",gxx_pdcch_core2_complete_flag,gxx_fft_core2_complete_flag);
                CACHE_wbL2((void *)&gxx_decramble_core2_complete_flag,sizeof(gxx_decramble_core2_complete_flag), CACHE_NOWAIT);
                CACHE_wbL2((void *)demodulation_data[glbCoreId],NUM_CODEWORD_2*N_SC_PER_RB * N_DL_RB * 4 * QAM64 , CACHE_NOWAIT);// Wait for the writeback operation to complete.
                
                gxx_cycle_c2[slot_idx>>1] = TSC_delay_cycle(cycle_start);

            }
            
        }


    }

    if(3 == glbCoreId)
    {
      while(1)
      {
        while(1)
        {
            CACHE_invL2((void *)&gxx_pbch_process_flag, sizeof(gxx_pbch_process_flag), CACHE_WAIT);
            if(1 == gxx_pbch_process_flag)
        	{
        		CACHE_invL2((void *)&gxx_pbch_core0_core3_flag, sizeof(gxx_pbch_core0_core3_flag), CACHE_WAIT);
        		if(1==gxx_pbch_core0_core3_flag)
        		{
        		    LTE_Test(0,1);
        		    gxx_pbch_core0_core3_flag=0;
        		    CACHE_wbL2((void *)&gxx_pbch_core0_core3_flag, sizeof(gxx_pbch_core0_core3_flag), CACHE_WAIT);
        		    pbch_send++;
                }
        	}



            CACHE_invL2(&gxx_pdcch_core3_complete_flag, sizeof(gxx_pdcch_core3_complete_flag), CACHE_WAIT);
            CACHE_invL2(&gxx_fft_core3_complete_flag, sizeof(gxx_fft_core3_complete_flag), CACHE_WAIT);
            //PDCCH盲检成功后
            CACHE_invL2((void *)&gxx_slot_idx_c0, sizeof(gxx_slot_idx_c0), CACHE_WAIT);
            gxx_slot_idx_c3 = gxx_slot_idx_c0;
            if((1 == gxx_pdcch_core3_complete_flag) && (1 == gxx_fft_core3_complete_flag))//表示pdcch译码成功，用户pdsch配置正确获得
            {
                cycle_start = TSCL;
                slot_idx = gxx_slot_idx_c3;

                if(slot_idx==0)
                {
      			  test_start3 = TSCL;
      			  //CACHE_wbL2((void *)&test_start, sizeof(test_start), CACHE_WAIT);
                }

                gxx_pdcch_core3_complete_flag = 0;
                gxx_fft_core3_complete_flag = 0;
                CACHE_wbL2((void *)&gxx_pdcch_core3_complete_flag, sizeof(gxx_pdcch_core3_complete_flag), CACHE_WAIT);
                CACHE_wbL2((void *)&gxx_fft_core3_complete_flag, sizeof(gxx_fft_core3_complete_flag), CACHE_WAIT);
                //puts("core3 flag clear");
      		  CSL_XMC_invalidatePrefetchBuffer();
      		  		    _mfence();
      		  		    _mfence();
                CACHE_invL2(vxx_user_para, sizeof(vxx_user_para), CACHE_WAIT);
                CACHE_invL2(&vxx_cell_para, sizeof(vxx_cell_para), CACHE_WAIT);
      		    CACHE_invL2((void*)che_interp_data, sizeof(che_interp_data), CACHE_WAIT);
      		    CACHE_invL2((void*)fft_data, sizeof(fft_data), CACHE_WAIT);
                if((slot_idx == 2) || (slot_idx == 12))//特殊子帧
                {
                     total_symbol = vxx_cell_para.dwpts_symbol;
                     if(1 == vxx_cell_para.CFI)
                     {
                         core0_eq_symbol = 3;//特殊子帧最多考虑10个符号每个core的eq要处理的ofdm结束符号，各core按照2,2,3,2个符号分配处理
                         core1_eq_symbol = 5;
                         core2_eq_symbol = 8;
                         core3_eq_symbol = total_symbol;
                     }
                     if(2 == vxx_cell_para.CFI)
                     {
                         core0_eq_symbol = 4;//特殊子帧最多考虑10个符号每个core的eq要处理的ofdm结束符号，各core按照2,2,2,2个符号分配处理
                         core1_eq_symbol = 6;
                         core2_eq_symbol = 8;
                         core3_eq_symbol = total_symbol;
                     }
                     if(3 == vxx_cell_para.CFI)
                     {
                         core0_eq_symbol = 5;//特殊子帧最多考虑10个符号每个core的eq要处理的ofdm结束符号，各core按照2,2,2,1个符号分配处理
                         core1_eq_symbol = 7;
                         core2_eq_symbol = 9;
                         core3_eq_symbol = total_symbol;
                     }
                }
                else
                {
                     total_symbol = 2 * N_SYM_PER_SLOT;
                     if(1 == vxx_cell_para.CFI)
                     {
                         core0_eq_symbol = 4;//每个core的eq要处理的ofdm结束符号，各core按照3,3,4,3个符号分配处理
                         core1_eq_symbol = 7;
                         core2_eq_symbol = 11;
                         core3_eq_symbol = total_symbol;
                     }
                     if(2 == vxx_cell_para.CFI)
                     {
                         core0_eq_symbol = 5;//每个core的eq要处理的ofdm结束符号，各core按照3,3,3,3个符号分配处理
                         core1_eq_symbol = 8;
                         core2_eq_symbol = 11;
                         core3_eq_symbol = total_symbol;
                     }
                     if(3 == vxx_cell_para.CFI)
                     {
                         core0_eq_symbol = 5;//每个core的eq要处理的ofdm结束符号，各core按照2,3,3,3个符号分配处理
                         core1_eq_symbol = 8;
                         core2_eq_symbol = 11;
                         core3_eq_symbol = total_symbol;
                     }
                }
                
                //分核进行时域插值
                chan_interp_time(
                                 total_symbol,
                                 0,         //Para,本次demapping的导频的端口号，目前支持0,1
                                 core2_eq_symbol,
                                 core3_eq_symbol,
                                 (signed int*)che_interp_data      //in/Out, 频域信道估计首地址（插值后），排列方式同e_freq_data_ptr，kaV在最外层
                                 );
                chan_interp_time(
                                 total_symbol,
                                 1,         //Para,本次demapping的导频的端口号，目前支持0,1
                                 core2_eq_symbol,
                                 core3_eq_symbol,
                                 (signed int*)che_interp_data      //in/Out, 频域信道估计首地址（插值后），排列方式同e_freq_data_ptr，kaV在最外层
                                 );
                /*插值后的信道估计解映射*/
                for(k = core2_eq_symbol; k < core3_eq_symbol;k++)
                {
                     prb_demapping_pdsch((signed int*)che_interp_data[0],   //In, 频域数据首地址
                                           NUM_MAX_ANTENNA,       //Para         //Para
                                           vxx_cell_para.cellID,        //Para,小区ID（0-503）
                                           k,        //Para,本次demapping的符号，这样可以较灵活的匹配时序调整（第几个符号开始处理...）
                                           slot_idx >> 1,             //Para,本次处理的子帧号（可考虑与ns合并）
                                           slot_idx,           //Para,本次处理的时隙号（port2/3时需要）
                                           (signed int*)vxx_fft_lay_buffer.prbdemapping_freq_che[0][0][k - core2_eq_symbol],    //In/Out, demapping后的接收天线0的频域数据首地址（每次传入本符号该存放的首地址，传出下个符号该存放的首地址）
                                           (signed int*)vxx_fft_lay_buffer.prbdemapping_freq_che[0][1][k - core2_eq_symbol]    //In/Out, demapping后的接收天线0的频域数据首地址（每次传入本符号该存放的首地址，传出下个符号该存放的首地址）
                                           );
                     prb_demapping_pdsch((signed int*)che_interp_data[1],   //In, 频域数据首地址
                                           NUM_MAX_ANTENNA,       //Para        //Para
                                           vxx_cell_para.cellID,        //Para,小区ID（0-503）
                                           k,        //Para,本次demapping的符号，这样可以较灵活的匹配时序调整（第几个符号开始处理...）
                                           slot_idx >> 1,             //Para,本次处理的子帧号（可考虑与ns合并）
                                           slot_idx,           //Para,本次处理的时隙号（port2/3时需要）
                                           (signed int*)vxx_fft_lay_buffer.prbdemapping_freq_che[1][0][k - core2_eq_symbol],    //In/Out, demapping后的接收天线0的频域数据首地址（每次传入本符号该存放的首地址，传出下个符号该存放的首地址）
                                           (signed int*)vxx_fft_lay_buffer.prbdemapping_freq_che[1][1][k - core2_eq_symbol]    //In/Out, demapping后的接收天线0的频域数据首地址（每次传入本符号该存放的首地址，传出下个符号该存放的首地址）
                                           );
                     prb_demapping_pdsch((signed int*)fft_data,   //In, 频域数据首地址
                                                  NUM_MAX_ANTENNA,       //Para           //Para
                                                  vxx_cell_para.cellID,        //Para,小区ID（0-503）
                                                  k,        //Para,本次demapping的符号，这样可以较灵活的匹配时序调整（第几个符号开始处理...）
                                                  slot_idx >> 1,             //Para,本次处理的子帧号（可考虑与ns合并）
                                                  slot_idx,           //Para,本次处理的时隙号（port2/3时需要）
                                                  (signed int*)vxx_desramble_tb_buffer.prbdemapping_freq_data[0][k - core2_eq_symbol],    //In/Out, demapping后的接收天线0的频域数据首地址（每次传入本符号该存放的首地址，传出下个符号该存放的首地址）
                                                  (signed int*)vxx_desramble_tb_buffer.prbdemapping_freq_data[1][k - core2_eq_symbol]    //In/Out, demapping后的接收天线0的频域数据首地址（每次传入本符号该存放的首地址，传出下个符号该存放的首地址）
                                                  );
                } 
        
                vxx_user_para[0].m_sybol = 0;
                for(k = vxx_cell_para.CFI; k < total_symbol;k++)
                {
                  vxx_user_para[0].m_sybol += vxx_user_para[0].subcarrier_demap_num[k];
                }
                for(k = 0;k < vxx_user_para[0].num_code_word;k++)
                {
                    vxx_user_para[0].numBitsG[k] = vxx_user_para[0].m_sybol * vxx_user_para[0].modulation[k];
                }
                /************************计算每个core要均衡的符号个数**************************/
                for(k = 0; k < 4;k++)
                {
                    core_m_symbol[k] = 0;
                }
                for(k = vxx_cell_para.CFI; k < core0_eq_symbol;k++)
                {
                    core_m_symbol[0] += vxx_user_para[0].subcarrier_demap_num[k];//eq处理完后的紧凑排列的调制符号个数
                }
                for(k = core0_eq_symbol; k < core1_eq_symbol;k++)
                {
                    core_m_symbol[1] += vxx_user_para[0].subcarrier_demap_num[k];//eq处理完后的紧凑排列的调制符号个数
                }
                for(k = core1_eq_symbol; k < core2_eq_symbol;k++)
                {
                    core_m_symbol[2] += vxx_user_para[0].subcarrier_demap_num[k];//eq处理完后的紧凑排列的调制符号个数
                }
                for(k = core2_eq_symbol; k < core3_eq_symbol;k++)
                {
                    core_m_symbol[3] += vxx_user_para[0].subcarrier_demap_num[k];//eq处理完后的紧凑排列的调制符号个数
                }
                /************************计算每个core要解调的符号个数**************************/
                for(i = 0; i < vxx_user_para[0].num_code_word;i++)
                {
                   for(k = 0; k < 4;k++)
                   {
                       core_start_numBitsG[k][i] = 0;
                   }
                     for(k = 0; k < 1;k++)
                     {
                         core_start_numBitsG[1][i] += core_m_symbol[k] * vxx_user_para[0].modulation[i];
                     }
                     for(k = 0; k < 2;k++)
                     {
                         core_start_numBitsG[2][i] += core_m_symbol[k] * vxx_user_para[0].modulation[i];
                     }
                     for(k = 0; k < 3;k++)
                     {
                         core_start_numBitsG[3][i] += core_m_symbol[k] * vxx_user_para[0].modulation[i];
                     }
                }
                /********************************分核均衡和解调************************************/
                chan_eq((signed int*)vxx_desramble_tb_buffer.prbdemapping_freq_data,     //In, 接收天线的数据
                            (signed int*)vxx_fft_lay_buffer.prbdemapping_freq_che,   //In, 第1根接收天线的第1个流的第1个子载波，a=h_1_1_c1
                             vxx_user_para[0].symbol_start,
                             (signed int*)vxx_demodu_rm_buffer.predecoding_data,  //Out, d1
                             vxx_user_para[0].ueTransMode, //下行传输模式
                             vxx_user_para[0].subcarrier_demap_num,
                             core2_eq_symbol,//本core处理的符号开始
                             core3_eq_symbol,//本core处理的符号结束
                             core_m_symbol[glbCoreId],
                             PDSCH,
                             (signed short*)vxx_che_crs_buffer.llr_data     //Out,LLr结果首地址
                            );
                //空分复用且双流 或者发射分集且单流 不用拷贝，节省cycle
                CACHE_invL2(pdsch_pseudo, sizeof(pdsch_pseudo), CACHE_WAIT);
                for(k = 0; k < vxx_user_para[0].num_code_word;k++)
                {
                    demodulation_descramble
                             ((signed short*)&vxx_demodu_rm_buffer.predecoding_data[k],//Q(16,3)
                              vxx_che_crs_buffer.llr_data[k],//Q(16,1)
                             (signed char*)demodulation_data[glbCoreId][k],//Q(8,6)
                              vxx_user_para[0].ueTransMode,
                              vxx_user_para[0].modulation[k],
                              core_m_symbol[glbCoreId],
                              &pdsch_pseudo[k][core_start_numBitsG[glbCoreId][k]]
                             );
                }
                gxx_decramble_core3_complete_flag = 1;
                CACHE_wbL2((void *)&gxx_decramble_core3_complete_flag,sizeof(gxx_decramble_core3_complete_flag), CACHE_NOWAIT);
                CACHE_wbL2((void *)demodulation_data[glbCoreId],NUM_CODEWORD_2*N_SC_PER_RB * N_DL_RB * 4 * QAM64 , CACHE_NOWAIT);// Wait for the writeback operation to complete.
                for(i = 0; i < vxx_user_para[0].num_code_word;i++)
                {
                    memcpy(&srcBuf[i][BCP_HEAD_LENGTH + core_start_numBitsG[glbCoreId][i]/4],demodulation_data[glbCoreId][i],(core_m_symbol[glbCoreId] * vxx_user_para[0].modulation[i]));
                }
                //gxx_cycle_c3[slot_idx>>1] = TSC_delay_cycle(cycle_start);
                break;
                
            }
            
        }
        while(1)
        {
            //访问core0的fft完成标志，core1进行非硬件加速器的计算
            while(1)
            {
      		  CSL_XMC_invalidatePrefetchBuffer();
      		  		    _mfence();
      		  		    _mfence();

                CACHE_invL2(&gxx_decramble_core0_complete_flag, sizeof(gxx_decramble_core0_complete_flag), CACHE_WAIT);
                if(1 == gxx_decramble_core0_complete_flag)//轮询可能优先处理完的数据，节省拷贝时间
                {
                	gxx_decramble_core0_complete_flag = 0;
                      CACHE_wbL2((void *)&gxx_decramble_core0_complete_flag, sizeof(gxx_decramble_core0_complete_flag), CACHE_NOWAIT);// Wait for the writeback operation to complete.
                      CACHE_invL2(demodulation_data[0],NUM_CODEWORD_2 * N_SC_PER_RB * N_DL_RB * 4 * QAM64, CACHE_WAIT);// Wait for the writeback operation to complete.

                      for(i = 0; i < vxx_user_para[0].num_code_word;i++)
                      {
                          memcpy(&srcBuf[i][BCP_HEAD_LENGTH + core_start_numBitsG[0][i]/4],demodulation_data[0][i],(core_m_symbol[0] * vxx_user_para[0].modulation[i]));
                      }
                      copy_flag[0]=1;
                }
                CACHE_invL2(&gxx_decramble_core1_complete_flag, sizeof(gxx_decramble_core1_complete_flag), CACHE_WAIT);
                if(1 == gxx_decramble_core1_complete_flag)//轮询可能优先处理完的数据，节省拷贝时间
                {
                	gxx_decramble_core1_complete_flag = 0;
                      CACHE_wbL2((void *)&gxx_decramble_core1_complete_flag, sizeof(gxx_decramble_core1_complete_flag), CACHE_NOWAIT);// Wait for the writeback operation to complete.
                      CACHE_invL2(demodulation_data[1],NUM_CODEWORD_2 * N_SC_PER_RB * N_DL_RB * 4 * QAM64, CACHE_WAIT);// Wait for the writeback operation to complete.

                      for(i = 0; i < vxx_user_para[0].num_code_word;i++)
                      {
                          memcpy(&srcBuf[i][BCP_HEAD_LENGTH + core_start_numBitsG[1][i]/4],demodulation_data[1][i],(core_m_symbol[1] * vxx_user_para[0].modulation[i]));
                      }
                      copy_flag[1]=1;
                }
                CACHE_invL2(&gxx_decramble_core2_complete_flag, sizeof(gxx_decramble_core2_complete_flag), CACHE_WAIT);
                if(1 == gxx_decramble_core2_complete_flag)//core2处理符号最多
                {
                      gxx_crc_check = 0;

                      gxx_decramble_core2_complete_flag = 0;
                      CACHE_wbL2((void *)&gxx_decramble_core2_complete_flag, sizeof(gxx_decramble_core2_complete_flag), CACHE_NOWAIT);// Wait for the writeback operation to complete.

                      CACHE_invL2(demodulation_data[2],NUM_CODEWORD_2 * N_SC_PER_RB * N_DL_RB * 4 * QAM64, CACHE_WAIT);// Wait for the writeback operation to complete.

                      for(i = 0; i < vxx_user_para[0].num_code_word;i++)
                      {
                          memcpy(&srcBuf[i][BCP_HEAD_LENGTH + core_start_numBitsG[2][i]/4],demodulation_data[2][i],(core_m_symbol[2] * vxx_user_para[0].modulation[i]));
                      }
                      copy_flag[2]=1;
                }
                if((1==copy_flag[0])&&(1==copy_flag[1])&&(1==copy_flag[2]))
                {

                	gxx_all_pdcch_complete_flag = 0;
                	CACHE_wbL2((void *)&gxx_all_pdcch_complete_flag, sizeof(gxx_all_pdcch_complete_flag), CACHE_WAIT);
                	break;
                }

            }

                  
                 decode_flag= bcp_lte_pdsch_receive_part1(&vxx_user_para[0],
                                                  &vxx_cell_para,
                                                  slot_idx,
                                                  core_start_numBitsG[0],
                                                  core_m_symbol);
                // if(decode_flag==1)
				// {
					// memcpy(&tb_buffer_total[slot_idx>>1],tb_buffer,sizeof(tb_buffer));
				// }

                 if(decode_flag==0)
                 {
                	 count_subf[slot_idx>>1]++;
                 }
#if 1
                 if(decode_flag==1)
                 {
               	     memcpy(&tb_buffer_total[slot_idx>>1],tb_buffer,sizeof(tb_buffer));
                     phytomactype2.GHead.NewFlag = 1;
                     phytomactype2.GHead.SFN = sfn;
                     phytomactype2.GHead.SubframeN = slot_idx>>1;
                     //PBCH、PHICH数据在解析后更新
                     phytomac_pdcchc.HARQprocessID = 0;//在哪里获取？
                     phytomac_pdcchc.RV = vxx_user_para[0].rvIdx[0];
                     phytomac_pdcchc.NDI = vxx_user_para[0].ndi;
                     CACHE_wbL2((void *)&phytomac_pdcchc, sizeof(phytomac_pdcchc), CACHE_WAIT);
                     //PDCCH数据在盲检后更新
                     phytomac_pdschc.CRC = gxx_crc_check;

                     phytomactype2.DL_TYPE2_PUBLIC_C.PDSCHLEN = (vxx_user_para[0].tbSize[0]+7)/8;

                     //CACHE_invL2((void *)tb_buffer, sizeof(tb_buffer), CACHE_WAIT);
                     int sendsize = sizeof(phytomactype2)+sizeof(phytomac_pbchd)
                   		  +sizeof(phytomac_phichd)+sizeof(phytomac_pdcchc)
                   		  +sizeof(phytomac_pdcchd)+sizeof(phytomac_pdschc)
                   		  +phytomactype2.DL_TYPE2_PUBLIC_C.PDSCHLEN;


                     CACHE_wbL2((void *)&phytomactype2,sendsize , CACHE_WAIT);
                     //CACHE_wbL2((void *)&phytomac_pdcchc, sizeof(phytomac_pdcchc), CACHE_WAIT);

                     transParameter[0].local_addr = (Uint32)(&phytomactype2);

                     if(send_index%5 == 0)
                     {
                   	  transParameter[0].remote_addrL = 0xfc100800;
                   	  //transParameter[0].remote_addrL = mactophytype3.DL_TYPE3_PDCCH_C.MemStart;
                     }
                     else
                     {
                   	  transParameter[0].remote_addrL += SENDOFFSET;
                     }


                     transParameter[0].byte_count = ((sendsize+63)/64)*64;
                     LTE_Test(0,0);
                     send_index++;
                     CACHE_wbL2((void *)&send_index, sizeof(send_index), CACHE_WAIT);
                 }
                  /***********************Type2数据发送结束********************************/
#endif

                  //puts("core3 decoding completed");
                  for(i=0;i<3;i++)
                  {
                	  copy_flag[i]=0;
                  }

                  //memset(tb_buffer,0,sizeof(tb_buffer));
        		  gxx_cycle_c3[slot_idx>>1] = TSC_delay_cycle(cycle_start);


#if 0
                  ("core3 flag is %d and %d\n",gxx_pdcch_core3_complete_flag,gxx_fft_core3_complete_flag);


                  p=fopen("tb_buffer_total.dat","a");
                  fwrite(&tb_buffer_total[slot_idx>>1],4,72,p);
                  fclose(p);
#endif

                  //CACHE_invL2((void *)&gxx_decoding_core3_core0_flag, sizeof(gxx_decoding_core3_core0_flag), CACHE_WAIT);

                  gxx_decoding_core3_core0_flag = 0;
                  CACHE_wbL2((void *)&gxx_decoding_core3_core0_flag, sizeof(gxx_decoding_core3_core0_flag), CACHE_WAIT);

                  //CACHE_invL2((void *)&gxx_decoding_core3_core1_flag, sizeof(gxx_decoding_core3_core1_flag), CACHE_WAIT);
                  gxx_decoding_core3_core1_flag = 0;
                  CACHE_wbL2((void *)&gxx_decoding_core3_core1_flag, sizeof(gxx_decoding_core3_core1_flag), CACHE_WAIT);
                  break;


        }    
      }
    }
     /* gxx_all_pdcch_complete_flag = 1;
	  CACHE_wbL2((void *)&gxx_all_pdcch_complete_flag, sizeof(gxx_all_pdcch_complete_flag), CACHE_WAIT);
 	  while(gxx_all_pdcch_complete_flag)
 	  {
 		  CACHE_invL2((void *)&gxx_all_pdcch_complete_flag, sizeof(gxx_all_pdcch_complete_flag), CACHE_WAIT);
 	  }*/
    //10.15 by mjt 这里默认核0处理比核3快，但是如果存在中断，核0会多次被打断，可能处理上会比核3慢一些，将这段代码修改后加到核0的数据读取之前。

}

unsigned short f_pdcch_monitor_proc_part1(
                      DL_resource_mapping_struct *dl_resource_mapping_ptr,
                      DL_cch_data_struct* cch_data_buffer_ptr,
                      CELL_PARA *cell_para_ptr,
                      char subfn_index,
                      char pdcch_symbol_num
                      )
{
    //计算搜索空间
    unsigned char dl_port_num = 0;
    unsigned char dl_rb_num = 0;
    unsigned short length = 0;
    unsigned int cell_id;
    int c_init = 0;
    unsigned short pdcch_total_reg_num;
    unsigned short *pdcch_data_table_ptr;
    unsigned short *reg_k_ptr;
    unsigned short *interleave_pattern_ptr;
    unsigned short *buffer_ptr;
    unsigned char *reg_unused_flag_ptr;
    unsigned short *symbol0_reg_k_ptr,*symbol1_reg_k_ptr,*symbol2_reg_k_ptr;
    int temp[1];
    signed short symbol_start[1];

    dl_port_num = cell_para_ptr->num_antenna_port;
    dl_rb_num = cell_para_ptr->num_PRB_per_bw_config;
    cell_id = cell_para_ptr->cellID;
    pdcch_data_table_ptr = (unsigned short *)(cch_data_buffer_ptr->pdcch_data_k);
    reg_k_ptr = (unsigned short *)(cch_data_buffer_ptr->buffer1);
    interleave_pattern_ptr = (unsigned short *)(cch_data_buffer_ptr->buffer2);
    buffer_ptr = (unsigned short *)&(cch_data_buffer_ptr->buffer2[1200]);
    reg_unused_flag_ptr = (unsigned char *)&(dl_resource_mapping_ptr->reg_unused_flag[subfn_index][0]);
    symbol0_reg_k_ptr = &dl_resource_mapping_ptr->symbol0_reg_k[0][0];
    symbol1_reg_k_ptr = &dl_resource_mapping_ptr->symbol1_reg_k[0][0];
    symbol2_reg_k_ptr = &dl_resource_mapping_ptr->symbol2_reg_k[0][0];

    //计算总CCE数
    pdcch_total_reg_num =  f_dl_pdcch_mapping_k(symbol0_reg_k_ptr,
                                            symbol1_reg_k_ptr,
                                            symbol2_reg_k_ptr,
                                            pdcch_symbol_num,
                                            subfn_index,
                                            dl_port_num,
                                            dl_rb_num,
                                            reg_k_ptr,
                                            reg_unused_flag_ptr);
    //计算交织图样
    f_gen_interleave_pattern(interleave_pattern_ptr,
                         buffer_ptr,
                         pdcch_total_reg_num,
                         cell_id,
                         pdcch_data_table_ptr,
                         reg_k_ptr);

    //取出频域数据
    length = pdcch_total_reg_num * 4;
    f_pdcch_freq_data_recover(fft_data[0],
                        g_cch_data_buffer.pdcch_freq_data,
                        &pdcch_data_table_ptr[0],
                        length,
                        cell_para_ptr->num_antenna_port
                        );
    //取出信道估计
    f_pdcch_freq_data_recover((int *)che_interp_data[0],
                        g_cch_data_buffer.H_freq_data[0],
                        &pdcch_data_table_ptr[0],
                        length,
                        cell_para_ptr->num_antenna_port
                        );
    f_pdcch_freq_data_recover((int *)che_interp_data[1],
                        g_cch_data_buffer.H_freq_data[1],
                        &pdcch_data_table_ptr[0],
                        length,
                        cell_para_ptr->num_antenna_port
                        );

    temp[0] = pdcch_total_reg_num * 4;
    symbol_start[0] = 0;
    /*单码字*/
    chan_eq((signed int*)g_cch_data_buffer.pdcch_freq_data,     //In, 接收天线的数据
            (signed int*)g_cch_data_buffer.H_freq_data,   //In, 第1根接收天线的第1个流的第1个子载波，a=h_1_1_c1
             symbol_start,
             g_cch_data_buffer.pdcch_data_after_decoding,  //Out, d1
             TRANSMIT_DIVERSITY, /*下行传输模式*/
             temp,
             0,
             1,
             pdcch_total_reg_num * 4,
             PDCCH,
             pbch_pcfich_pdcch_llr_data     //Out,LLr结果首地址
            );

    demodulation((signed short*)g_cch_data_buffer.pdcch_data_after_decoding,//Q(16,3)
                 pbch_pcfich_pdcch_llr_data,//Q(16,1)
                 g_cch_data_buffer.pdcch_data_after_demod,//Q(8,6)
                 TRANSMIT_DIVERSITY,
                 QPSK,
                 pdcch_total_reg_num * 8
                 );

    //扰码生成
    c_init = (subfn_index << 9) + cell_id;//c_init = (0 << 9) + cell_id;// c_init = (subfn_index << 9) + cell_id;
    /*Descrambling_data_pbch_gen(x1_sequence_table,
                         c_init,
                         pdcch_total_reg_num * 8,
                         g_cch_data_buffer.pdcch_pseudo);

    BitDescrambling_pbch(g_cch_data_buffer.pdcch_data_after_demod,
                         g_cch_data_buffer.pdcch_data_after_descram,//vxx_user_para[user_index].descramble_data_ptr[i],
                         g_cch_data_buffer.pdcch_pseudo,
                         pdcch_total_reg_num * 8);*/
    BitDescrambling(x1_sequence_table,
                g_cch_data_buffer.pdcch_data_after_demod,
                pdcch_data_after_descram,//vxx_user_para[user_index].descramble_data_ptr[i],
                c_init,
                pdcch_total_reg_num * 8);
    return(pdcch_total_reg_num);

}


void f_pdcch_monitor_proc_part2(
                      CELL_PARA *cell_para_ptr,
                      char subfn_index,
                      char rnti,
                      char common_rnti,
                      unsigned short pdcch_total_reg_num
                      )
{
    //计算搜索空间
    unsigned short A = 39827;
    unsigned int D = 65537;
    unsigned int Y[10];
    unsigned short Y_1,Yk;
    unsigned short N_cce = 0;
    unsigned int index = 0;
    unsigned char L_index = 0;
    unsigned char pdcch_crc_flag = 1;
    unsigned int bit_satrt_index;
    unsigned short L,M,cce_satrt_index,m,format_index;
    int crc_out;
    unsigned char K = 0;
    unsigned char dci_buffer_index = 0;
    char trans_mode = 0;
    char dci_format = 0;
    int time;
    trans_mode = cell_para_ptr->TM_mode;
    time = TSCL;

    N_cce = pdcch_total_reg_num / 9;//9个REG为一个CCE
    Y_1 =  rnti;//协议中Y(-1)
    Y[0] = (A * Y_1) % D;

    for(index = 1;index < (subfn_index + 2);index++)//这个计算应该可以放入参数计算
    {
      Y[index] = (A * Y[index - 1]) % D;
    }

    //搜索专用搜索空间
    Yk = Y[subfn_index + 1];
    for(L_index = 0;L_index < 4;L_index++)
    {
        L = g_const_pdcch_user_serch_space[L_index].L;//本次尝试的聚合等级
        M = g_const_pdcch_user_serch_space[L_index].M;//本次需要搜索的次数
        for(m = 0;m < M;m++)
        {
            cce_satrt_index = L * (Yk + m) % (N_cce/L);
            bit_satrt_index = cce_satrt_index * 9 * 8;
            for(format_index = 0;format_index < 2;format_index++)
            {
                //判断本次盲检的DCI格式
                dci_format = g_const_pdcch_DCI_UE_search[trans_mode][format_index];
                if(100 == cell_para_ptr -> num_PRB_per_bw_config)
                {
                	K = VCC_DCI_LEN_20M[dci_format];// 31
                }
                else if(50 == cell_para_ptr -> num_PRB_per_bw_config)
                {
                	K = VCC_DCI_LEN_10M[dci_format];
                }
                else if(25 == cell_para_ptr -> num_PRB_per_bw_config)
                {
                	K = VCC_DCI_LEN_5M[dci_format];
                }

#if 0
                //解速率匹配
                cc_conv_derate_matching(pdcch_data_after_descram + bit_satrt_index,
                                        K + 16,
                                        L * 72,//72,
                                        g_cch_data_buffer.buffer3[0],
                                        g_cch_data_buffer.buffer3[0] + K + 16,
                                        g_cch_data_buffer.buffer3[0] + 2 * (K + 16));
                //卷积译码

                crc_out = cc_conv_decoder(K + 16,
                                      L * 72,//72,
                                          g_cch_data_buffer.buffer3[0],
                                          g_cch_data_buffer.buffer3[0] + K + 16,
                                          g_cch_data_buffer.buffer3[0] + 2 * (K + 16),
                                          (short*)g_cch_data_buffer.buffer3[1],
                                          g_cch_data_buffer.buffer3[2],
                                          g_cch_data_buffer.pdcch_data_decode_bits,
                                          16);

#endif
                cc_conv_pdcch_table_derate_matching(pdcch_data_after_descram + bit_satrt_index,
                                        K + 16,
                                        L * 72,
                                        g_cch_data_buffer.buffer3[0],
                                        g_cch_data_buffer.buffer3[0] + K + 16,
                                        g_cch_data_buffer.buffer3[0] + 2 * (K + 16),
                                        &DerateData[0]);

                crc_out =VCP2_Channel_Density_Test(K + 16,DerateData,g_cch_data_buffer.pdcch_data_decode_bits);
                //crc校验
                pdcch_crc_flag = crc_decoder(g_cch_data_buffer.pdcch_data_decode_bits,
                                             LTE_CRC16,
                                             crc_out,
                                             K,
                                             &vxx_cell_para,
                                             PDCCH,
                                             rnti);
                if(0!= pdcch_crc_flag)//如果译码失败
                {
                    error_pdcchcrc++;
                }
#if 0
                /*2015.6.4添加帮助检查*/
                if(0!= pdcch_crc_flag)//如果译码失败
                {
                	error_pdcchcrc++;
                	pdcch_crc_flag = 0;
                	g_cch_data_buffer.pdcch_data_decode_bits[0] = 0x8ED8F080;

                }
                else
                	right_pdcchcrc++;
#endif

                if(0== pdcch_crc_flag)//译码正确
                {
                	right_pdcchcrc++;
                	phytomac_pdcchd.DCIData1 = g_cch_data_buffer.pdcch_data_decode_bits[0];
                	phytomac_pdcchd.DCIData2 = g_cch_data_buffer.pdcch_data_decode_bits[1];
                	if(g_cch_data_buffer.pdcch_data_decode_bits[0]!=0x8F28F080)       //61  0x8ED8F080    63   0x8F28F080
                	{
                		disparity_num+=1;
                		g_cch_data_buffer.pdcch_data_decode_bits[0] = 0x8F28F080;
                		if(dci_format!=g_const_pdcch_DCI_UE_search[3][0])
                		{
                			dci_format = g_const_pdcch_DCI_UE_search[3][0];
                		}
                	}
                	else
                	{
                		accordance_num+=1;
                	}

                    fcc_dci_data_distill(g_cch_data_buffer.pdcch_data_decode_bits,
                                         (unsigned int *)&g_dci_rel_data_buffer[dci_buffer_index],
                                          dci_format,
                                          trans_mode,
                                          subfn_index,
                                          &vxx_cell_para,
                                          &vxx_user_para[0]);
                    g_cch_data_buffer.dci_format[dci_buffer_index] = dci_format;//DCI格式
                    dci_buffer_index++;
//                    vxx_user_para[0] = vxx_user_para_buffer[0];
                    CACHE_wbL2(vxx_user_para, sizeof(vxx_user_para), CACHE_WAIT);// Wait for the writeback operation to complete.
                    //CACHE_wbL2((void *)&gxx_slot_idx_c1, sizeof(gxx_slot_idx_c1), CACHE_WAIT);// Wait for the writeback operation to complete.
                    CACHE_wbL2(&vxx_user_para_buffer[0], sizeof(vxx_user_para_buffer[0]), CACHE_WAIT);

                    while(gxx_decoding_core3_core1_flag)
                    {
                	    CACHE_invL2((void *)&gxx_decoding_core3_core1_flag, sizeof(gxx_decoding_core3_core1_flag), CACHE_WAIT);
                    }
                    gxx_decoding_core3_core1_flag = 1;
                    CACHE_wbL2((void *)&gxx_decoding_core3_core1_flag, sizeof(gxx_decoding_core3_core1_flag), CACHE_WAIT);

                    for(index = 0;index < 1;index++)
                    {
                    gxx_pdcch_core0_complete_flag = 1;
                    CACHE_wbL2((void *)&gxx_pdcch_core0_complete_flag, sizeof(gxx_pdcch_core0_complete_flag), CACHE_WAIT);
                    gxx_pdcch_core1_complete_flag = 1;
                    CACHE_wbL2((void *)&gxx_pdcch_core1_complete_flag, sizeof(gxx_pdcch_core1_complete_flag), CACHE_WAIT);
                    gxx_pdcch_core2_complete_flag = 1;
                    CACHE_wbL2((void *)&gxx_pdcch_core2_complete_flag, sizeof(gxx_pdcch_core2_complete_flag), CACHE_WAIT);
                    gxx_pdcch_core3_complete_flag = 1;
                    CACHE_wbL2((void *)&gxx_pdcch_core3_complete_flag, sizeof(gxx_pdcch_core3_complete_flag), CACHE_WAIT);
                    }

                    break;
                }

            }
            if(2 != format_index)
            {
              break;
            }
        }
        if(M != m)
        {
            break;
        }
    }
    if((M == m)&&(2 == format_index)&&(4 == L_index))
    {
    	pdcchspecialfailed++;
    	dci_format=g_const_pdcch_DCI_UE_search[3][0];
    	g_cch_data_buffer.pdcch_data_decode_bits[0] = 0x8ED8F080;
        fcc_dci_data_distill(g_cch_data_buffer.pdcch_data_decode_bits,
                             (unsigned int *)&g_dci_rel_data_buffer[dci_buffer_index],
                             dci_format,
                             trans_mode,
                             subfn_index,
                             &vxx_cell_para,
                             &vxx_user_para[0]);
        CACHE_wbL2(vxx_user_para, sizeof(vxx_user_para), CACHE_WAIT);// Wait for the writeback operation to complete.
        //CACHE_wbL2((void *)&gxx_slot_idx_c1, sizeof(gxx_slot_idx_c1), CACHE_WAIT);// Wait for the writeback operation to complete.
        CACHE_wbL2(&vxx_user_para_buffer[0], sizeof(vxx_user_para_buffer[0]), CACHE_WAIT);
        while(gxx_decoding_core3_core1_flag)
        {
    	    CACHE_invL2((void *)&gxx_decoding_core3_core1_flag, sizeof(gxx_decoding_core3_core1_flag), CACHE_WAIT);
        }
        gxx_decoding_core3_core1_flag = 1;
        CACHE_wbL2((void *)&gxx_decoding_core3_core1_flag, sizeof(gxx_decoding_core3_core1_flag), CACHE_WAIT);

        for(index = 0;index < 1;index++)
        {
        gxx_pdcch_core0_complete_flag = 1;
        CACHE_wbL2((void *)&gxx_pdcch_core0_complete_flag, sizeof(gxx_pdcch_core0_complete_flag), CACHE_WAIT);
        gxx_pdcch_core1_complete_flag = 1;
        CACHE_wbL2((void *)&gxx_pdcch_core1_complete_flag, sizeof(gxx_pdcch_core1_complete_flag), CACHE_WAIT);
        gxx_pdcch_core2_complete_flag = 1;
        CACHE_wbL2((void *)&gxx_pdcch_core2_complete_flag, sizeof(gxx_pdcch_core2_complete_flag), CACHE_WAIT);
        gxx_pdcch_core3_complete_flag = 1;
        CACHE_wbL2((void *)&gxx_pdcch_core3_complete_flag, sizeof(gxx_pdcch_core3_complete_flag), CACHE_WAIT);
        }
    }
 /*   else
    {
    	pdcchspecialfailed = 0;
    }   */
    g_cch_data_buffer.pdcch_num = dci_buffer_index;
    return;
    //搜索公共搜索空间
    if(common_rnti !=0 )
    {
    Yk = 0;
    dci_buffer_index = 0;
    for(L_index = 0;L_index < 2;L_index++)
    {
        L = g_const_pdcch_common_serch_space[L_index].L;//本次尝试的聚合等级
        M = g_const_pdcch_common_serch_space[L_index].M;//本次需要搜索的次数
        for(m = 0;m < M;m++)
        {
            cce_satrt_index = L * (Yk + m) % (N_cce/L);
            bit_satrt_index = cce_satrt_index * 9 * 8;
            for(format_index = 0;format_index < 2;format_index++)
            {
              //判断本次盲检的DCI格式
              dci_format = g_const_pdcch_DCI_common_search[format_index];
              if(100 == cell_para_ptr -> num_PRB_per_bw_config)
              {
              	K = VCC_DCI_LEN_20M[dci_format];// 31
              }
              else if(50 == cell_para_ptr -> num_PRB_per_bw_config)
              {
              	K = VCC_DCI_LEN_10M[dci_format];
              }
              else if(25 == cell_para_ptr -> num_PRB_per_bw_config)
              {
              	K = VCC_DCI_LEN_5M[dci_format];
              }

#if 0
              //原来的解速率匹配
              cc_conv_derate_matching(pdcch_data_after_descram + bit_satrt_index,
                                      K + 16,
                                      L * 72,
                                       g_cch_data_buffer.buffer3[0],
                                       g_cch_data_buffer.buffer3[0]+ K + 16,
                                       g_cch_data_buffer.buffer3[0] + 2 * (K + 16));





              //程序卷积译码
              crc_out = cc_conv_decoder(K + 16,
                                   L * 72,
                                       g_cch_data_buffer.buffer3[0],
                                       g_cch_data_buffer.buffer3[0] + K + 16,
                                       g_cch_data_buffer.buffer3[0] + 2 * (K + 16),
                                      (short*)g_cch_data_buffer.buffer3[1],
                                       g_cch_data_buffer.buffer3[2],
                                       g_cch_data_buffer.pdcch_data_decode_bits,
                                       16);
#endif
              //查表速率匹配
              cc_conv_pdcch_table_derate_matching(pdcch_data_after_descram + bit_satrt_index,
                                                   K + 16,
                                                   L * 72,
                                                   g_cch_data_buffer.buffer3[0],
                                                   g_cch_data_buffer.buffer3[0] + K + 16,
                                                   g_cch_data_buffer.buffer3[0] + 2 * (K + 16),
                                                   &DerateData[300]);
              //vcp译码
              crc_out =VCP2_Channel_Density_Test(K + 16,&DerateData[300],g_cch_data_buffer.pdcch_data_decode_bits);
              //crc校验
              pdcch_crc_flag = crc_decoder(g_cch_data_buffer.pdcch_data_decode_bits,
                                           LTE_CRC16,
                                           crc_out,
                                           K,
                                           &vxx_cell_para,
                                           PDCCH,
                                           vxx_user_para[0].common_rnti);
              if(0 == pdcch_crc_flag)
              {
              	phytomac_pdcchd.DCIData1 = g_cch_data_buffer.pdcch_data_decode_bits[0];
              	phytomac_pdcchd.DCIData2 = g_cch_data_buffer.pdcch_data_decode_bits[1];
                //对解出的DCI进行解析
                   fcc_dci_data_distill(g_cch_data_buffer.pdcch_data_decode_bits,
                       (unsigned int *)&g_dci_rel_data_buffer[dci_buffer_index],
                       dci_format,
                       trans_mode,
                       subfn_index,
                       &vxx_cell_para,
                       &vxx_user_para_buffer[1]);
                   CACHE_wbL2(vxx_user_para, sizeof(vxx_user_para), CACHE_WAIT);
                   CACHE_wbL2(&vxx_user_para_buffer[1], sizeof(vxx_user_para_buffer[1]), CACHE_WAIT);
                   g_cch_data_buffer.dci_format[dci_buffer_index] = dci_format;//DCI格式
                   dci_buffer_index++;
                   break;
              }


            }
            if(2 != format_index)
            {
                break;
            }

        }
        if(M != m)
        {
            break;
        }
    }
    if((M == m)&&(2 == format_index)&&(4 == L_index))
    {
    	pdcchcommonfailed = 1;


    }
    else
    	pdcchcommonfailed = 0;


    }

    while(gxx_decoding_core3_core1_flag)
    {
	    CACHE_invL2((void *)&gxx_decoding_core3_core1_flag, sizeof(gxx_decoding_core3_core1_flag), CACHE_WAIT);
    }
    gxx_decoding_core3_core1_flag = 1;
    CACHE_wbL2((void *)&gxx_decoding_core3_core1_flag, sizeof(gxx_decoding_core3_core1_flag), CACHE_WAIT);

    for(index = 0;index < 1;index++)
    {
    gxx_pdcch_core0_complete_flag = 1;
    CACHE_wbL2((void *)&gxx_pdcch_core0_complete_flag, sizeof(gxx_pdcch_core0_complete_flag), CACHE_WAIT);
    gxx_pdcch_core1_complete_flag = 1;
    CACHE_wbL2((void *)&gxx_pdcch_core1_complete_flag, sizeof(gxx_pdcch_core1_complete_flag), CACHE_WAIT);
    gxx_pdcch_core2_complete_flag = 1;
    CACHE_wbL2((void *)&gxx_pdcch_core2_complete_flag, sizeof(gxx_pdcch_core2_complete_flag), CACHE_WAIT);
    gxx_pdcch_core3_complete_flag = 1;
    CACHE_wbL2((void *)&gxx_pdcch_core3_complete_flag, sizeof(gxx_pdcch_core3_complete_flag), CACHE_WAIT);
    }
    cyclenum = TSC_delay_cycle(time);

}
