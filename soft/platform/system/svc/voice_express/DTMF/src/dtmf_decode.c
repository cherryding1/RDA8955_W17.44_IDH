#include "dtmf_decode.h"

#define  UNQUALIFY		0
#define  QUALIFIED		1
#define  LINE    0
#define  ROW    1 

#define CONST_PI	3.1415926
//#define CONST_FS	8000
#define CONST_N		128
#define CONST_PI	3.1415926

#define CONST_N		128



#define MiniPow 1000*1000
#define Magnitude_thr DTMF_FRAME_SIZE*MiniPow


short dtmfValid = 128;
double	row_threshold = 0.8;
double	col_threshold = 10.2;
typedef double   dtmf_t;


dtmf_t  __attribute__((section(".sramdata"))) coeff[8];
dtmf_t  __attribute__((section(".sramdata"))) magnitude[8];
short dtmf_dt_flag = 1;

/* --------------------------------------------------------------------------------
    DTMF - Coding Frequencies Digit to be Encoded Low Frequency Values (Hz) 
    Digit to be Encoded 
    1       2        3     A     697 
    4       5        6     B     770 
    7       8        9     C     852 
    *       0        #     D     941 
    1209   1336     1477   1633  
   --------------------------------------------------------------------------------*/
const char dtmf_value[4][4] = {
	{'1','2','3','A'},
	{'4','5','6','B'},
	{'7','8','9','C'},
	{'*','0','#','D'}
};




 int __attribute__((section(".sramtext")))dtmf_calc_magnitude(signed short *x, int len, int line_row)
{
//	double Q0=0, Q1=0, Q2=0; 
	dtmf_t Q0=0, Q1=0, Q2=0; 
	int  i, j,temp;
       double m_coeff=0;
    
	if (len < CONST_N)
		return 0;

	switch(line_row)
	{
		case 0:
			temp = 0;
			break;
		case 1:
			temp = 4;
			break;
		default:
			return 0;
	}
	
	for (j=temp; j<temp+4; j++) 
	{
	       m_coeff = (double)coeff[j];
           
		for(i=0; i<len; i++)
		{	
			Q0 = m_coeff*Q1 - Q2 + x[i];
//			Q0 = (dtmf_t)coeff[j]*Q1/100000 - Q2 + (dtmf_t)x[i];
			Q2 = Q1;
			Q1 = Q0;
		}
		/**************** 计算8个magnitude ***********************/
		magnitude[j]=Q2*Q2 + Q1*(Q1 - m_coeff*Q2);
		Q0 = Q1 = Q2 = 0;
	}
	return 1;
}
 

int testCheckDbCmp(double dVal)
{
	if (dVal > 3.1415926)
		return 0;
	return 1;
}
int testCheckDbCalc(double dVal)
{
	dVal = dVal * 1.99;
	if (dVal > 3.1415926)
		return 0;
	return 1;
}

/********计算8个2*cos(2*pi*K/N)*********/
void dtmf_init_coeff(void)
{
	coeff[0] = 1.7077378190696;
	coeff[1] = 1.6452810477723;
	coeff[2] = 1.5686869980284;
	coeff[3] = 1.4782045852242;
	coeff[4] = 1.1641040493694;
	coeff[5] = 0.99637024171836;
	coeff[6] = 0.79861842551062;
	coeff[7] = 0.56853274872613;

}




/*---------------------------------------------------------------------------*
 * 门限判决函数 dtmf_check_power
   门限1：DTMF信号强度门限
 *  要求:行、列频率能量最大值之和占据8个频点总能量的百分比是否大于ENERGY_THRESHOLD
 *（默认0.9）。不是则认为不是DTMF信号                                      *
 *---------------------------------------------------------------------------*/
static int  dtmf_check_power(dtmf_t a[], int x, int y)
{   
	#define THRESHOLD_SUM	0.9	
	int i;

	double max_sum=0;
	double total_sum=0;
	for (i=0; i<8; i++)
	{
		total_sum+=a[i];
	}
	max_sum = a[x] + a[y];
	//yeshunzhou modify	
	if (max_sum <= THRESHOLD_SUM*total_sum)
	//if (max_sum/total_sum <= THRESHOLD_SUM)
//	if (max_sum/total_sum <= row_threshold)
	{
		return UNQUALIFY;
	}
/*
	dtmf_t max_sum=0;
	dtmf_t total_sum=0;
	for (i=0; i<8; i++)
	{
		total_sum+=a[i];
	}
	max_sum = (a[x] + a[y])*10;
	if (max_sum/total_sum <= 9)
	{
		return UNQUALIFY;
	}
*/
	return QUALIFIED;
}

/*---------------------------------------------------------------------------*
 * 门限判决函数 dtmf_check_peak
   门限2：行、列峰值门限
 *  要求:行、列能量最大值/行列能量次大值> PEAK_VALUE_THRESHOLD(默认14.2)。
 *  不是则认为不是DTMF信号,本函数为了简便,判断了所有其他三个值而没有寻找次大值*                                    *
 *---------------------------------------------------------------------------*/
static int  dtmf_check_peak(dtmf_t a[], int x, int line_row)
{   
	#define THRESHOLD_ROW	14.2
	#define THRESHOLD_LINE	14.2
	int i;

	switch(line_row)
		{
			case 0:
				{
					//行门限判决
					for (i=0; i<4; i++)	
					if ((x!=i) && (a[x] < col_threshold*a[i]))	
				 	{
				 		return UNQUALIFY;
					}			
				}
				break;
			case 1:
				{
					//列门限判决
					for (i=4; i<8; i++)	
					if ((x!=i) && (a[x] < col_threshold*a[i]))
					{
						return UNQUALIFY;
				 	}			
				}
				break;
			default:
				return UNQUALIFY;
		}

	return QUALIFIED;
}







static int  __attribute__((section(".sramtext")))dtmf_check_second_harmonic(dtmf_t magnitude, int row, signed short *x, int len)
{
	dtmf_t second_harmonic_thr = 0.01;
	dtmf_t second_harmonic_coeff[4] = {-0.644862,-1.0072464,-1.3622087,-1.67677056,};
	dtmf_t second_harmonic_magnitude;
	dtmf_t Q0=0, Q1=0, Q2=0; 
	int  i;
       double m_coeff;

      m_coeff = (double)second_harmonic_coeff[row];
      
	for(i=0; i<len; i++)
	{	
		Q0 = m_coeff*Q1 - Q2 + x[i];
		Q2 = Q1;
		Q1 = Q0;
	}
	
	second_harmonic_magnitude = Q2*Q2 + Q1*(Q1 - m_coeff*Q2);


	if(second_harmonic_magnitude >= magnitude * second_harmonic_thr)
	{
		return UNQUALIFY;
	}

	return QUALIFIED;
	
}

char dtmf_check_character(signed short *x, int len)
{
	int		i, ret, row, line,temp;
	dtmf_t	maxm;	
	int		nvalid_pos = 0;

	for(i = 0;i < len;i++)
	{
		temp = x[i];
		if(temp < 0)
		{
			temp = -temp;
		}
		if(temp < dtmfValid)
		{
			nvalid_pos++;
		}
	}

	if(nvalid_pos >= (len >> 1))
	{
		dtmf_dt_flag = 1;
		return 0;
	}
	if(dtmf_dt_flag == 0)
	{
		return 0;
	}
	
	ret = dtmf_calc_magnitude(x, len,LINE);
	if (ret == 0)
		return 0xf0;

	//printf( " magnitude line: %f, %f, %f, %f\n",magnitude[0],magnitude[1],magnitude[2],magnitude[3]);
	//printf( " magnitude row: %f, %f, %f, %f\n",magnitude[4],magnitude[5],magnitude[6],magnitude[7]);
	
	line = -1;
	/**************比较得出4个行频中Xk的最大值**************/
	//yeshunzhou modify
	//maxm = 0;
	maxm= Magnitude_thr;
	for (i=0; i<4; i++)
	{
		if (magnitude[i] > maxm)
		{
			maxm = magnitude[i];
			line = i;
		}
	}
	if(-1 == line)
		return 0xf1;
	 ret = dtmf_check_peak(magnitude, line, LINE);
	if (UNQUALIFY == ret)	
	{
		return 0xf2;
	}
	
	/**************进行列频计算与绝对能量判决**************/		

	ret = dtmf_calc_magnitude(x, len,ROW);

	if (ret == 0)
		return 0xf3;

	row = -1;
	maxm= Magnitude_thr;
	for(i=4; i<8; i++)
	{
		if (magnitude[i] > maxm)
		{
			maxm = magnitude[i];
			row  = i;
		}
	}
	
	if (-1 == row)
		return 0xf4;

	ret = dtmf_check_peak(magnitude, row, ROW);
	if (UNQUALIFY == ret)	
	{
		return 0xf5;
	}
	
/*---------------------------------------------------------------------------*
 * 门限判决  
   门限1：DTMF信号强度门限
 *  要求:行、列频率能量最大值之和占据8个频点总能量的百分比是否大于ENERGY_THRESHOLD
 *（默认0.9）。不是则认为不是DTMF信号                                      *
 *---------------------------------------------------------------------------*/
	ret = dtmf_check_power(magnitude, line, row);
	if (UNQUALIFY == ret)	
	{
		return 0xf6;
	}

 /*---------------------------------------------------------------------------*
 * 门限判决函数 
   门限2：行、列峰值门限
 *  要求:行、列能量最大值/行列能量次大值> PEAK_VALUE_THRESHOLD(默认14.2)。
 *  不是则认为不是DTMF信号,本函数为了简便,判断了所有其他三个值而没有寻找次大值*                                    *
 *---------------------------------------------------------------------------*/
#if 0
 	ret = dtmf_check_peak(magnitude, line, row);
	if (UNQUALIFY == ret)	
	{
		return 0;
	}
#endif


	ret = dtmf_check_second_harmonic(magnitude[row], row - 4, x, len);
	if (UNQUALIFY == ret)	
	{
		return 0xf7;
	}

	return dtmf_value[line][row-4];
}

/*---------------------------------------------------------------------------*
 * 门限判决函数 dtmf_compare
   门限3：稳定信号门限
    实际的DTMF信号应持续超过一定长度的时间。
   所以应在连续检测到两次或两次以上同一信号时再认为信号存在
*                                    *
 *---------------------------------------------------------------------------*/
int  dtmf_compare(int a, int b, int x, int y)
{
	if ((a==x) && (b==y))
		return 0;

	return 1;
}


