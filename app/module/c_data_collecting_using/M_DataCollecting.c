#include "M_DataCollecting.h"

#include <stdlib.h>

#include "../../Mymain.h"
#include "hal_adc.h"
#include "hal_pwm.h"
#include "hal_gpio.h"
//#include "C_memory_pool.h"
#define IIR_STAGE	3000.0f /*Base on 10ms, 3000 means 30 seconds close feature.*/
#define IIR_STAGE_BL	25.0f /*Base on 10ms, 25 means 250m seconds close feature.*/

//vbatt table actually value is 10 times than
const tmtx_tbl_element_def mtVbatt[212] = {
{40,410},
{41,422},
{42,434},
{43,447},
{44,459},
{45,471},
{46,483},
{47,495},
{48,508},
{49,520},
{50,532},
{51,544},
{52,556},
{53,568},
{54,581},
{55,593},
{56,605},
{57,617},
{58,629},
{59,641},
{60,654},
{61,666},
{62,678},
{63,690},
{64,702},
{65,714},
{66,727},
{67,739},
{68,751},
{69,763},
{70,775},
{71,787},
{72,800},
{73,812},
{74,824},
{75,836},
{76,848},
{77,860},
{78,873},
{79,885},
{80,897},
{81,909},
{82,921},
{83,933},
{84,946},
{85,958},
{86,970},
{87,982},
{88,994},
{89,1006},
{90,1019},
{91,1031},
{92,1043},
{93,1055},
{94,1067},
{95,1080},
{96,1092},
{97,1104},
{98,1116},
{99,1128},
{100,1140},
{101,1153},
{102,1165},
{103,1177},
{104,1189},
{105,1201},
{106,1213},
{107,1226},
{108,1238},
{109,1250},
{110,1262},
{111,1274},
{112,1286},
{113,1299},
{114,1311},
{115,1323},
{116,1335},
{117,1347},
{118,1359},
{119,1372},
{120,1384},
{121,1396},
{122,1408},
{123,1420},
{124,1432},
{125,1445},
{126,1457},
{127,1469},
{128,1481},
{129,1493},
{130,1505},
{131,1518},
{132,1530},
{133,1542},
{134,1554},
{135,1566},
{136,1578},
{137,1591},
{138,1603},
{139,1615},
{140,1627},
{141,1639},
{142,1652},
{143,1664},
{144,1676},
{145,1688},
{146,1700},
{147,1712},
{148,1725},
{149,1737},
{150,1749},
{151,1761},
{152,1773},
{153,1785},
{154,1798},
{155,1810},
{156,1822},
{157,1834},
{158,1846},
{159,1858},
{160,1871},
{161,1883},
{162,1895},
{163,1907},
{164,1919},
{165,1931},
{166,1944},
{167,1956},
{168,1968},
{169,1980},
{170,1992},
{171,2004},
{172,2017},
{173,2029},
{174,2041},
{175,2053},
{176,2065},
{177,2077},
{178,2090},
{179,2102},
{180,2114},
{181,2126},
{182,2138},
{183,2151},
{184,2163},
{185,2175},
{186,2187},
{187,2199},
{188,2211},
{189,2224},
{190,2236},
{191,2248},
{192,2260},
{193,2272},
{194,2284},
{195,2297},
{196,2309},
{197,2321},
{198,2333},
{199,2345},
{200,2357},
{201,2370},
{202,2382},
{203,2394},
{204,2406},
{205,2418},
{206,2430},
{207,2443},
{208,2455},
{209,2467},
{210,2479},
{211,2491},
{212,2503},
{213,2516},
{214,2528},
{215,2540},
{216,2552},
{217,2564},
{218,2576},
{219,2589},
{220,2601},
{221,2613},
{222,2625},
{223,2637},
{224,2649},
{225,2662},
{226,2674},
{227,2686},
{228,2698},
{229,2710},
{230,2723},
{231,2735},
{232,2747},
{233,2759},
{234,2771},
{235,2783},
{236,2796},
{237,2808},
{238,2820},
{239,2832},
{240,2844},
{241,2856},
{242,2869},
{243,2881},
{244,2893},
{245,2905},
{246,2917},
{247,2929},
{248,2942},
{249,2954},
{250,2966},
{0x0000,0x0000}
};

const tmtx_tbl_element_def mtTpcb[162] = {
{-40,1285},
{-39,1290},
{-38,1295},
{-37,1300},
{-36,1304},
{-35,1309},
{-34,1314},
{-33,1319},
{-32,1324},
{-31,1330},
{-30,1335},
{-29,1340},
{-28,1345},
{-27,1350},
{-26,1355},
{-25,1360},
{-24,1365},
{-23,1371},
{-22,1376},
{-21,1381},
{-20,1386},
{-19,1391},
{-18,1397},
{-17,1402},
{-16,1407},
{-15,1412},
{-14,1418},
{-13,1423},
{-12,1428},
{-11,1433},
{-10,1439},
{-9,1444},
{-8,1449},
{-7,1455},
{-6,1460},
{-5,1465},
{-4,1470},
{-3,1476},
{-2,1481},
{-1,1486},
{0,1492},
{1,1497},
{2,1502},
{3,1507},
{4,1513},
{5,1518},
{6,1523},
{7,1528},
{8,1534},
{9,1539},
{10,1544},
{11,1549},
{12,1555},
{13,1560},
{14,1565},
{15,1570},
{16,1576},
{17,1581},
{18,1586},
{19,1591},
{20,1596},
{21,1601},
{22,1607},
{23,1612},
{24,1617},
{25,1622},
{26,1627},
{27,1632},
{28,1637},
{29,1643},
{30,1648},
{31,1653},
{32,1658},
{33,1663},
{34,1668},
{35,1673},
{36,1678},
{37,1683},
{38,1688},
{39,1693},
{40,1698},
{41,1703},
{42,1708},
{43,1713},
{44,1718},
{45,1723},
{46,1728},
{47,1733},
{48,1738},
{49,1743},
{50,1747},
{51,1752},
{52,1757},
{53,1762},
{54,1767},
{55,1772},
{56,1777},
{57,1781},
{58,1786},
{59,1791},
{60,1796},
{61,1800},
{62,1805},
{63,1810},
{64,1815},
{65,1819},
{66,1824},
{67,1829},
{68,1833},
{69,1838},
{70,1843},
{71,1847},
{72,1852},
{73,1857},
{74,1861},
{75,1866},
{76,1871},
{77,1875},
{78,1880},
{79,1884},
{80,1889},
{81,1893},
{82,1898},
{83,1902},
{84,1907},
{85,1911},
{86,1916},
{87,1920},
{88,1925},
{89,1929},
{90,1934},
{91,1938},
{92,1943},
{93,1947},
{94,1951},
{95,1956},
{96,1960},
{97,1965},
{98,1969},
{99,1973},
{100,1978},
{101,1982},
{102,1986},
{103,1991},
{104,1995},
{105,1999},
{106,2004},
{107,2008},
{108,2012},
{109,2016},
{110,2021},
{111,2025},
{112,2029},
{113,2033},
{114,2038},
{115,2042},
{116,2046},
{117,2050},
{118,2054},
{119,2059},
{120,2063},
{0x0000,0x0000}
};

const tmtx_tbl_element_def mtTbl[162] = {
{120,183},
{119,187},
{118,191},
{117,195},
{116,200},
{115,204},
{114,209},
{113,214},
{112,219},
{111,224},
{110,229},
{109,234},
{108,240},
{107,245},
{106,251},
{105,257},
{104,263},
{103,269},
{102,275},
{101,282},
{100,288},
{99,295},
{98,302},
{97,309},
{96,317},
{95,324},
{94,332},
{93,340},
{92,349},
{91,357},
{90,366},
{89,374},
{88,383},
{87,393},
{86,402},
{85,412},
{84,422},
{83,432},
{82,443},
{81,454},
{80,465},
{79,476},
{78,488},
{77,500},
{76,512},
{75,524},
{74,537},
{73,551},
{72,564},
{71,578},
{70,592},
{69,606},
{68,621},
{67,636},
{66,652},
{65,668},
{64,684},
{63,701},
{62,718},
{61,735},
{60,753},
{59,771},
{58,790},
{57,809},
{56,829},
{55,849},
{54,869},
{53,890},
{52,911},
{51,933},
{50,955},
{49,977},
{48,1000},
{47,1024},
{46,1047},
{45,1071},
{44,1096},
{43,1121},
{42,1146},
{41,1171},
{40,1197},
{39,1224},
{38,1250},
{37,1277},
{36,1305},
{35,1332},
{34,1360},
{33,1389},
{32,1417},
{31,1446},
{30,1476},
{29,1505},
{28,1535},
{27,1565},
{26,1595},
{25,1625},
{24,1656},
{23,1686},
{22,1717},
{21,1747},
{20,1778},
{19,1809},
{18,1840},
{17,1871},
{16,1902},
{15,1933},
{14,1964},
{13,1995},
{12,2025},
{11,2056},
{10,2086},
{9,2116},
{8,2146},
{7,2176},
{6,2206},
{5,2235},
{4,2264},
{3,2293},
{2,2321},
{1,2349},
{0,2377},
{-1,2404},
{-2,2431},
{-3,2458},
{-4,2484},
{-5,2509},
{-6,2535},
{-7,2559},
{-8,2584},
{-9,2608},
{-10,2631},
{-11,2653},
{-12,2676},
{-13,2697},
{-14,2718},
{-15,2739},
{-16,2759},
{-17,2779},
{-18,2798},
{-19,2816},
{-20,2834},
{-21,2851},
{-22,2868},
{-23,2885},
{-24,2900},
{-25,2916},
{-26,2931},
{-27,2945},
{-28,2959},
{-29,2972},
{-30,2985},
{-31,2997},
{-32,3009},
{-33,3021},
{-34,3032},
{-35,3042},
{-36,3052},
{-37,3062},
{-38,3071},
{-39,3080},
{-40,3089},
{0x0000,0x0000}
};

const tmtx_tbl_element_def mtVsync[201] = {
{1,15},
{2,30},
{3,45},
{4,61},
{5,76},
{6,91},
{7,106},
{8,121},
{9,136},
{10,152},
{11,167},
{12,182},
{13,197},
{14,212},
{15,227},
{16,242},
{17,258},
{18,273},
{19,288},
{20,303},
{21,318},
{22,333},
{23,348},
{24,364},
{25,379},
{26,394},
{27,409},
{28,424},
{29,439},
{30,455},
{31,470},
{32,485},
{33,500},
{34,515},
{35,530},
{36,545},
{37,561},
{38,576},
{39,591},
{40,606},
{41,621},
{42,636},
{43,652},
{44,667},
{45,682},
{46,697},
{47,712},
{48,727},
{49,742},
{50,758},
{51,773},
{52,788},
{53,803},
{54,818},
{55,833},
{56,848},
{57,864},
{58,879},
{59,894},
{60,909},
{61,924},
{62,939},
{63,955},
{64,970},
{65,985},
{66,1000},
{67,1015},
{68,1030},
{69,1045},
{70,1061},
{71,1076},
{72,1091},
{73,1106},
{74,1121},
{75,1136},
{76,1152},
{77,1167},
{78,1182},
{79,1197},
{80,1212},
{81,1227},
{82,1242},
{83,1258},
{84,1273},
{85,1288},
{86,1303},
{87,1318},
{88,1333},
{89,1348},
{90,1364},
{91,1379},
{92,1394},
{93,1409},
{94,1424},
{95,1439},
{96,1455},
{97,1470},
{98,1485},
{99,1500},
{100,1515},
{101,1530},
{102,1545},
{103,1561},
{104,1576},
{105,1591},
{106,1606},
{107,1621},
{108,1636},
{109,1652},
{110,1667},
{111,1682},
{112,1697},
{113,1712},
{114,1727},
{115,1742},
{116,1758},
{117,1773},
{118,1788},
{119,1803},
{120,1818},
{121,1833},
{122,1848},
{123,1864},
{124,1879},
{125,1894},
{126,1909},
{127,1924},
{128,1939},
{129,1955},
{130,1970},
{131,1985},
{132,2000},
{133,2015},
{134,2030},
{135,2045},
{136,2061},
{137,2076},
{138,2091},
{139,2106},
{140,2121},
{141,2136},
{142,2152},
{143,2167},
{144,2182},
{145,2197},
{146,2212},
{147,2227},
{148,2242},
{149,2258},
{150,2273},
{151,2288},
{152,2303},
{153,2318},
{154,2333},
{155,2348},
{156,2364},
{157,2379},
{158,2394},
{159,2409},
{160,2424},
{161,2439},
{162,2455},
{163,2470},
{164,2485},
{165,2500},
{166,2515},
{167,2530},
{168,2545},
{169,2561},
{170,2576},
{171,2591},
{172,2606},
{173,2621},
{175,2652},
{174,2636},
{176,2667},
{177,2682},
{178,2697},
{179,2712},
{180,2727},
{181,2742},
{182,2758},
{183,2773},
{184,2788},
{185,2803},
{186,2818},
{187,2833},
{188,2848},
{189,2864},
{190,2879},
{191,2894},
{192,2909},
{193,2924},
{194,2939},
{195,2955},
{196,2970},
{197,2985},
{198,3000},
{199,3015},
{200,3030},
{0x0000,0x0000}
};

data_collecting_info_def gtDCInfo;

/**
 * @brief <b>Stable count for each temperature.</b>
 * 
 * @details Stable count for each temperature.\n
 * 
 */
static uint8_t gu8BootCount = 0U;

/******************************************************************************
;       Function Name			:	uint8_t M_DC_Status_Get(void)
;       Function Description	:	This state for error condition
;       Parameters				:	void
;       Return Values			:	uint8_t
;		Source ID				:
******************************************************************************/
uint8_t M_DC_Status_Get(void)
{
	return gtDCInfo.u8Status;
}
/******************************************************************************
;       Function Name			:	void M_DC_Status_Set(void)
;       Function Description	:	This state for error condition
;       Parameters				:	void
;       Return Values			:	void
;		Source ID				:
******************************************************************************/
void M_DC_Status_Set(uint8_t u8Action)
{
	gtDCInfo.u8Status = u8Action;
}

/******************************************************************************
;       Function Name			:	void M_DC_Init(void)
;       Function Description	:	
;       Parameters				:	void
;       Return Values			:	void
;		Source ID				:
******************************************************************************/
void M_DC_Init(void)
{
	HAL_ADC_Init();
	HAL_ADC_Register_Event_Callback(M_DC_Callback);
}

/******************************************************************************
;       Function Name			:	void M_DC_MoveAvgCal(void)
;       Function Description	:	
;       Parameters				:	void
;       Return Values			:	void
;		Source ID				:
******************************************************************************/
int16_t M_DC_MoveAvgCal(int16_t *pData, uint8_t u8Length)
{
	uint8_t u8i;
	int16_t i16InitValue = 0;

	for(u8i = 0; u8i < u8Length ;u8i++)
	{
		i16InitValue += *(pData + u8i);
	}

	return (i16InitValue / u8Length);
}

/******************************************************************************
;       Function Name			:	void M_DC_IIRFIlter(void)
;       Function Description	:	
;       Parameters				:	void
;       Return Values			:	void
;		Source ID				:
******************************************************************************/
float M_DC_IIRFIlter(float f16BaseTemperature, float fIIRRange, int16_t i16LatestTemperature)
{
	float fReturnValue=0.0f;

	fReturnValue=(f16BaseTemperature*(fIIRRange-1.0f)+(float)i16LatestTemperature)/fIIRRange;

	return fReturnValue;
}

/******************************************************************************
;       Function Name			:	void M_DC_CheckTable(void)
;       Function Description	:	This state for error condition
;       Parameters				:	void
;       Return Values			:	void
;		Source ID				:
******************************************************************************/
static uint16_t M_DC_CheckTable(const tmtx_tbl_element_def *pTable , uint16_t u16Input)
{
	uint16_t u16Idx;

	for(;(u16Idx = pTable->u16ADCVol) != 0; pTable++)
	{
		if(u16Input <= u16Idx)
		{
			break;
		}
	}
	return pTable->i16ActulValue;
}

/******************************************************************************
;       Function Name			:	void M_DC_Function_Execute(void)
;       Function Description	:	This state for error condition
;       Parameters				:	void
;       Return Values			:	void
;		Source ID				:
******************************************************************************/
void M_DC_Function_Execute(uint8_t u8Action)
{
	switch(u8Action)
	{
		case DC_INIT_ADCVALUE:
			memset((int16_t *)(gtDCInfo.mi16BacklightTempADCData),(int)0,(size_t)(sizeof(int16_t)*SAMPLE_ELEMENT_NUM));
			memset((int16_t *)(gtDCInfo.mi16PCBTempADCData),(int)0,(size_t)(sizeof(int16_t)*SAMPLE_ELEMENT_NUM));
			memset((int16_t *)(gtDCInfo.mi16VbattADCData),(int)0,(size_t)(sizeof(int16_t)*SAMPLE_ELEMENT_NUM));
			memset((int16_t *)(gtDCInfo.mi16VSyncADCData),(int)0,(size_t)(sizeof(int16_t)*SAMPLE_ELEMENT_NUM));
			memset((int16_t *)(gtDCInfo.mi16FPCTxOutADCData),(int)0,(size_t)(sizeof(int16_t)*SAMPLE_ELEMENT_NUM));
			memset((int16_t *)(gtDCInfo.mi16FPCRxOutADCData),(int)0,(size_t)(sizeof(int16_t)*SAMPLE_ELEMENT_NUM));
			gtDCInfo.i16PresentBacklightTempADC = 0;
			gtDCInfo.fPresentBacklightTempADC = 0.0f;
			gtDCInfo.i16PresentPCBTempADC = 0;
			gtDCInfo.fPresentPCBTempADC = 0.0f;
			gtDCInfo.i16PresentVbattADC = 0;
			gtDCInfo.i16PresentVSyncADC = 0;
			gtDCInfo.i16PresentFPCTxOutADC = 0;
			gtDCInfo.i16PresentFPCRxOutADC = 0;			
			gtDCInfo.u8Cnt = 0U;
			gtDCInfo.u8Index = 0U;
			gtDCInfo.u8FIRInit = 0U;
			gtDCInfo.u8IIRInit = 0U;
			gu8BootCount = 0U;
			/*Analog Voltage*/
			gtDCInfo.i16PresentTPCBAnaVol = 0;
			gtDCInfo.i16PresentTBacklightAnaVol = 0;
			gtDCInfo.i16PresentVbattAnaVol = 0;
			gtDCInfo.i16PresentVSyncAnaVol = 0;
			gtDCInfo.i16PresentFPCTxAnaVol = 0;
			gtDCInfo.i16PresentFPCRxAnaVol = 0;
			/*Actual Value*/
			gtDCInfo.i16PresentTPCB = THERMAL_DEFAULT;
			gtDCInfo.i16PresentTBacklight = THERMAL_DEFAULT;
			gtDCInfo.i16PresentVbatt = 0;
			gtDCInfo.i16PresentVSync = 0;
			gtDCInfo.i16PresentFPCTxOut = 0;
			gtDCInfo.i16PresentFPCRxOut = 0;			
		break;

		case DC_TURNON_VBATT_MEASURE:
			HAL_GPIO_High(EN_VBATT_SENSE_PORT,EN_VBATT_SENSE_PIN);
		break;

		case DC_START_MEASURE:
			HAL_ADC_Start_Convert();
		break;

		case DC_GETVALUE:
			/*When initialize, fill in all data as first time detected ADC.*/
			if(gtDCInfo.u8FIRInit == 0U)
			{
				gtDCInfo.u8FIRInit=1U;
				for(uint8_t i=0U; i<SAMPLE_ELEMENT_NUM; i++)
				{
					gtDCInfo.mi16VbattADCData[i]=gtDCInfo.mi16VbattADCData[gtDCInfo.u8Index];
					gtDCInfo.mi16VSyncADCData[i]=gtDCInfo.mi16VSyncADCData[gtDCInfo.u8Index];
					gtDCInfo.mi16FPCTxOutADCData[i]=gtDCInfo.mi16FPCTxOutADCData[gtDCInfo.u8Index];
					gtDCInfo.mi16FPCRxOutADCData[i]=gtDCInfo.mi16FPCRxOutADCData[gtDCInfo.u8Index];
				}
				gtDCInfo.i16PresentVbattADC=gtDCInfo.mi16VbattADCData[gtDCInfo.u8Index];
				gtDCInfo.i16PresentVSyncADC=gtDCInfo.mi16VSyncADCData[gtDCInfo.u8Index];
				gtDCInfo.i16PresentFPCTxOutADC=gtDCInfo.mi16FPCTxOutADCData[gtDCInfo.u8Index];
				gtDCInfo.i16PresentFPCRxOutADC=gtDCInfo.mi16FPCRxOutADCData[gtDCInfo.u8Index];
			}
			/*Use moving average for related data.*/
			else
			{
				gtDCInfo.i16PresentVbattADC=M_DC_MoveAvgCal(&(gtDCInfo.mi16VbattADCData[0]),SAMPLE_ELEMENT_NUM);
				gtDCInfo.i16PresentVSyncADC=M_DC_MoveAvgCal(&(gtDCInfo.mi16VSyncADCData[0]),SAMPLE_ELEMENT_NUM);
				gtDCInfo.i16PresentFPCTxOutADC=M_DC_MoveAvgCal(&(gtDCInfo.mi16FPCTxOutADCData[0]),SAMPLE_ELEMENT_NUM);
				gtDCInfo.i16PresentFPCRxOutADC=M_DC_MoveAvgCal(&(gtDCInfo.mi16FPCRxOutADCData[0]),SAMPLE_ELEMENT_NUM);
			}
			
			gu8BootCount++;
			if(gu8BootCount >= THERMAL_SIGNALSTABLETIME)
			{
				gu8BootCount=THERMAL_SIGNALSTABLETIME;
				gtDCInfo.u8IIRInit = 1U;
			}
			else
			{ /* Nothing */ }			

			/*When initialize, fill in all data as first time detected ADC.*/
			if(gtDCInfo.u8IIRInit == 0U)
			{
				gtDCInfo.fPresentPCBTempADC=(float)gtDCInfo.mi16PCBTempADCData[gtDCInfo.u8Index];
				gtDCInfo.i16PresentPCBTempADC=gtDCInfo.mi16PCBTempADCData[gtDCInfo.u8Index];
				gtDCInfo.fPresentBacklightTempADC=(float)gtDCInfo.mi16BacklightTempADCData[gtDCInfo.u8Index];
				gtDCInfo.i16PresentBacklightTempADC=gtDCInfo.mi16BacklightTempADCData[gtDCInfo.u8Index];

			}
			/*Use IIR filter for related data.*/
			else
			{
				gtDCInfo.fPresentPCBTempADC=M_DC_IIRFIlter(gtDCInfo.fPresentPCBTempADC, IIR_STAGE, gtDCInfo.mi16PCBTempADCData[gtDCInfo.u8Index]);
				gtDCInfo.i16PresentPCBTempADC=(int16_t)gtDCInfo.fPresentPCBTempADC;
				gtDCInfo.fPresentBacklightTempADC=M_DC_IIRFIlter(gtDCInfo.fPresentBacklightTempADC, IIR_STAGE_BL, gtDCInfo.mi16BacklightTempADCData[gtDCInfo.u8Index]);
				gtDCInfo.i16PresentBacklightTempADC=(int16_t)gtDCInfo.fPresentBacklightTempADC;
			}

			gtDCInfo.u8Index++;
			gtDCInfo.u8Index%=SAMPLE_ELEMENT_NUM;
		break;

		case DC_QUERY_VBATT_TABLE:
			gtDCInfo.i16PresentVbattAnaVol=HAL_ADC0_Get_Channel_mVolt(HAL_ADC_VBATT_VOLT,gtDCInfo.i16PresentVbattADC);
			gtDCInfo.i16PresentVbatt=M_DC_CheckTable(&mtVbatt[0],gtDCInfo.i16PresentVbattAnaVol);
		break;

		case DC_QUERY_VSYNC_TABLE:
			gtDCInfo.i16PresentVSyncAnaVol=HAL_ADC0_Get_Channel_mVolt(HAL_ADC_SYNC_VOLT,gtDCInfo.i16PresentVSyncADC);
			gtDCInfo.i16PresentVSync=M_DC_CheckTable(&mtVsync[0],gtDCInfo.i16PresentVSyncAnaVol);
		break;

		case DC_QUERY_PCBT_TABLE:
			gtDCInfo.i16PresentTPCBAnaVol=HAL_ADC0_Get_Channel_mVolt(HAL_ADC_PCB_TEMP,gtDCInfo.i16PresentPCBTempADC);
			gtDCInfo.i16PresentTPCB=M_DC_CheckTable(&mtTpcb[0],gtDCInfo.i16PresentTPCBAnaVol);
		break;

		case DC_QUERY_BLT_TABLE:
			gtDCInfo.i16PresentTBacklightAnaVol=HAL_ADC0_Get_Channel_mVolt(HAL_ADC_BACKLIGHT_TEMP,gtDCInfo.i16PresentBacklightTempADC);
			gtDCInfo.i16PresentTBacklight=M_DC_CheckTable(&mtTbl[0],gtDCInfo.i16PresentTBacklightAnaVol);
		break;

		case DC_QUERY_FPCTXOUT_TABLE:
			gtDCInfo.i16PresentFPCTxAnaVol=HAL_ADC0_Get_Channel_mVolt(HAL_ADC_FPCTXOUT_VOLT,gtDCInfo.i16PresentFPCTxOutADC);
			gtDCInfo.i16PresentFPCTxOut=gtDCInfo.i16PresentFPCTxAnaVol;
		break;

		case DC_QUERY_FPCRXOUT_TABLE:
			gtDCInfo.i16PresentFPCRxAnaVol=HAL_ADC0_Get_Channel_mVolt(HAL_ADC_FPCRXOUT_VOLT,gtDCInfo.i16PresentFPCRxOutADC);
			gtDCInfo.i16PresentFPCRxOut=gtDCInfo.i16PresentFPCRxAnaVol;
		break;

		default:
			/* Nothing */
		break;
	}
	M_DC_Status_Set(u8Action);
}

/******************************************************************************
;       Function Name			:	void M_DC_Callback(void)
;       Function Description	:	Registe ADC user interrupt callback APIfunction
;       Parameters				:	void
;       Return Values			:	Return ADC values.
;		Source ID				:	U625_ADC-005
******************************************************************************/
void M_DC_Callback(void)
{
	uint32_t intrMask = HAL_ADC0_GetIntStatusMask();

	if((HAL_ADC_EOS_MASK & intrMask) != 0U)
	{
		gtDCInfo.mi16PCBTempADCData[gtDCInfo.u8Index] = HAL_ADC0_Get_ADCRead(HAL_ADC_PCB_TEMP);
		gtDCInfo.mi16BacklightTempADCData[gtDCInfo.u8Index] = HAL_ADC0_Get_ADCRead(HAL_ADC_BACKLIGHT_TEMP);
		gtDCInfo.mi16VbattADCData[gtDCInfo.u8Index] = HAL_ADC0_Get_ADCRead(HAL_ADC_VBATT_VOLT);
		gtDCInfo.mi16VSyncADCData[gtDCInfo.u8Index] = HAL_ADC0_Get_ADCRead(HAL_ADC_SYNC_VOLT);
#if(CX430_TDDI_NT51926 || U717_TDDI_NT51926 || BX726_TDDI_NT51926)
		gtDCInfo.mi16FPCTxOutADCData[gtDCInfo.u8Index] = HAL_ADC0_Get_ADCRead(HAL_ADC_FPCTXOUT_VOLT);
		gtDCInfo.mi16FPCRxOutADCData[gtDCInfo.u8Index] = HAL_ADC0_Get_ADCRead(HAL_ADC_FPCRXOUT_VOLT);
#endif
		M_DC_Function_Execute(DC_GETVALUE);
	}
	else
	{/*Nothing*/}

	HAL_ADC0_ClearIntFlag(intrMask);
}

