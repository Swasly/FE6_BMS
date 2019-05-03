/*******************************************************************************
* File Name: Thermistor1.c
* Version 1.20
*
* Description:
*  This file provides the source code to the API for the ThermistorCalc
*  Component.
*
* Note:
*  None.
*
********************************************************************************
* Copyright 2013, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "Thermistor1.h"


/*******************************************************************************
* Function Name: Thermistor1_GetResistance
********************************************************************************
*
* Summary:
*  The digital values of the voltages across the reference resistor and the
*  thermistor are passed to this function as parameters. The function returns
*  the resistance, based on the voltage values.
*
* Parameters:
*  vReference: the voltage across the reference resistor;
*  vThermistor: the voltage across the thermistor.
*  The ratio of these two voltages is used by this function. Therefore, the
*  units for both parameters must be the same.
*
* Return:
*  The return value is the resistance across the thermistor. The value returned
*  is the resistance in Ohms.
*
*******************************************************************************/
uint32 Thermistor1_GetResistance(int16 vReference, int16 vThermistor)
                                      
{
    int32 resT;
    int16 temp;

    /* Calculate thermistor resistance from the voltages */
    resT = Thermistor1_REF_RESISTOR * ((int32)vThermistor);
    if (vReference < 0)
    {
        temp = -vReference;
        temp = (int16)((uint16)((uint16)temp >> 1u));
        temp = -temp;
    }
    else
    {
        temp = (int16)((uint16)((uint16)vReference >> 1u));
    }
    resT += temp;
    resT /= vReference;

    /* The ordering of Reference resistor value is specifically designed to keep the result from overflowing */
    return ((uint32)((uint32)resT << Thermistor1_REF_RES_SHIFT));
}


/*******************************************************************************
* Function Name: Thermistor1_GetTemperature
********************************************************************************
*
* Summary:
*  The value of the thermistor resistance is passed to this function as a
*  parameter. The function returns the temperature, based on the resistance
*  value. The method used to calculate the temperature is dependent on whether
*  Equation or LUT was selected in the Customizer.
*
* Parameters:
*  resT: the resistance across the thermistor in Ohms.
*
* Return:
*  The return value is the temperature in 1/100ths of degrees C. For example,
*  the return value is 2345, when the actual temperature is 23.45 degrees C.
*
*******************************************************************************/
int16 Thermistor1_GetTemperature(uint32 resT) 
{
    int16 tempTR;
    static const uint32 CYCODE Thermistor1_LUT[] = { 20249u, 20199u, 20149u, 20099u, 20050u, 20000u, 19951u, 19901u, 19852u,
 19803u, 19755u, 19706u, 19657u, 19609u, 19561u, 19513u, 19465u, 19417u, 19369u, 19321u, 19274u, 19227u, 19179u, 19132u,
 19085u, 19039u, 18992u, 18945u, 18899u, 18853u, 18807u, 18761u, 18715u, 18669u, 18623u, 18578u, 18533u, 18487u, 18442u,
 18397u, 18352u, 18308u, 18263u, 18218u, 18174u, 18130u, 18086u, 18042u, 17998u, 17954u, 17910u, 17867u, 17823u, 17780u,
 17737u, 17694u, 17651u, 17608u, 17566u, 17523u, 17481u, 17438u, 17396u, 17354u, 17312u, 17270u, 17228u, 17187u, 17145u,
 17104u, 17063u, 17021u, 16980u, 16939u, 16899u, 16858u, 16817u, 16777u, 16736u, 16696u, 16656u, 16616u, 16576u, 16536u,
 16496u, 16457u, 16417u, 16378u, 16339u, 16299u, 16260u, 16221u, 16183u, 16144u, 16105u, 16067u, 16028u, 15990u, 15952u,
 15914u, 15876u, 15838u, 15800u, 15762u, 15725u, 15687u, 15650u, 15612u, 15575u, 15538u, 15501u, 15464u, 15427u, 15391u,
 15354u, 15318u, 15281u, 15245u, 15209u, 15173u, 15137u, 15101u, 15065u, 15029u, 14994u, 14958u, 14923u, 14888u, 14852u,
 14817u, 14782u, 14747u, 14712u, 14678u, 14643u, 14608u, 14574u, 14540u, 14505u, 14471u, 14437u, 14403u, 14369u, 14335u,
 14302u, 14268u, 14234u, 14201u, 14168u, 14134u, 14101u, 14068u, 14035u, 14002u, 13969u, 13937u, 13904u, 13871u, 13839u,
 13806u, 13774u, 13742u, 13710u, 13678u, 13646u, 13614u, 13582u, 13550u, 13519u, 13487u, 13456u, 13424u, 13393u, 13362u,
 13331u, 13300u, 13269u, 13238u, 13207u, 13177u, 13146u, 13115u, 13085u, 13055u, 13024u, 12994u, 12964u, 12934u, 12904u,
 12874u, 12844u, 12814u, 12785u, 12755u, 12726u, 12696u, 12667u, 12638u, 12608u, 12579u, 12550u, 12521u, 12492u, 12464u,
 12435u, 12406u, 12378u, 12349u, 12321u, 12292u, 12264u, 12236u, 12208u, 12180u, 12152u, 12124u, 12096u, 12068u, 12041u,
 12013u, 11985u, 11958u, 11931u, 11903u, 11876u, 11849u, 11822u, 11795u, 11768u, 11741u, 11714u, 11687u, 11661u, 11634u,
 11607u, 11581u, 11555u, 11528u, 11502u, 11476u, 11450u, 11424u, 11398u, 11372u, 11346u, 11320u, 11294u, 11269u, 11243u,
 11218u, 11192u, 11167u, 11141u, 11116u, 11091u, 11066u, 11041u, 11016u, 10991u, 10966u, 10941u, 10916u, 10892u, 10867u,
 10843u, 10818u, 10794u, 10769u, 10745u, 10721u, 10697u, 10673u, 10649u, 10625u, 10601u, 10577u, 10553u, 10529u, 10506u,
 10482u, 10458u, 10435u, 10411u, 10388u, 10365u, 10342u, 10318u, 10295u, 10272u, 10249u, 10226u, 10203u, 10181u, 10158u,
 10135u, 10112u, 10090u, 10067u, 10045u, 10022u, 10000u, 9978u, 9955u, 9933u, 9911u, 9889u, 9867u, 9845u, 9823u, 9801u,
 9779u, 9758u, 9736u, 9714u, 9693u, 9671u, 9650u, 9628u, 9607u, 9586u, 9565u, 9543u, 9522u, 9501u, 9480u, 9459u, 9438u,
 9417u, 9396u, 9376u, 9355u, 9334u, 9314u, 9293u, 9273u, 9252u, 9232u, 9211u, 9191u, 9171u, 9151u, 9131u, 9111u, 9090u,
 9070u, 9051u, 9031u, 9011u, 8991u, 8971u, 8952u, 8932u, 8912u, 8893u, 8873u, 8854u, 8835u, 8815u, 8796u, 8777u, 8757u,
 8738u, 8719u, 8700u, 8681u, 8662u, 8643u, 8624u, 8606u, 8587u, 8568u, 8549u, 8531u, 8512u, 8494u, 8475u, 8457u, 8438u,
 8420u, 8402u, 8383u, 8365u, 8347u, 8329u, 8311u, 8293u, 8275u, 8257u, 8239u, 8221u, 8203u, 8186u, 8168u, 8150u, 8133u,
 8115u, 8098u, 8080u, 8063u, 8045u, 8028u, 8010u, 7993u, 7976u, 7959u, 7942u, 7924u, 7907u, 7890u, 7873u, 7856u, 7840u,
 7823u, 7806u, 7789u, 7772u, 7756u, 7739u, 7722u, 7706u, 7689u, 7673u, 7656u, 7640u, 7624u, 7607u, 7591u, 7575u, 7559u,
 7542u, 7526u, 7510u, 7494u, 7478u, 7462u, 7446u, 7430u, 7415u, 7399u, 7383u, 7367u, 7352u, 7336u, 7320u, 7305u, 7289u,
 7274u, 7258u, 7243u, 7227u, 7212u, 7197u, 7182u, 7166u, 7151u, 7136u, 7121u, 7106u, 7091u, 7076u, 7061u, 7046u, 7031u,
 7016u, 7001u, 6986u, 6972u, 6957u, 6942u, 6928u, 6913u, 6898u, 6884u, 6869u, 6855u, 6840u, 6826u, 6812u, 6797u, 6783u,
 6769u, 6754u, 6740u, 6726u, 6712u, 6698u, 6684u, 6670u, 6656u, 6642u, 6628u, 6614u, 6600u, 6586u, 6573u, 6559u, 6545u,
 6531u, 6518u, 6504u, 6491u, 6477u, 6463u, 6450u, 6437u, 6423u, 6410u, 6396u, 6383u, 6370u, 6356u, 6343u, 6330u, 6317u,
 6304u, 6291u, 6278u, 6265u, 6251u, 6239u, 6226u, 6213u, 6200u, 6187u, 6174u, 6161u, 6149u, 6136u, 6123u, 6110u, 6098u,
 6085u, 6073u, 6060u, 6047u, 6035u, 6023u, 6010u, 5998u, 5985u, 5973u, 5961u, 5948u, 5936u, 5924u, 5912u, 5900u, 5887u,
 5875u, 5863u, 5851u, 5839u, 5827u, 5815u, 5803u, 5791u, 5779u, 5768u, 5756u, 5744u, 5732u, 5720u, 5709u, 5697u, 5685u,
 5674u, 5662u, 5651u, 5639u, 5627u, 5616u, 5605u, 5593u, 5582u, 5570u, 5559u, 5548u, 5536u, 5525u, 5514u, 5502u, 5491u,
 5480u, 5469u, 5458u, 5447u, 5436u, 5425u, 5414u, 5403u, 5392u, 5381u, 5370u, 5359u, 5348u, 5337u, 5326u, 5316u, 5305u,
 5294u, 5283u, 5273u, 5262u, 5251u, 5241u, 5230u, 5220u, 5209u, 5199u, 5188u, 5178u, 5167u, 5157u, 5146u, 5136u, 5126u,
 5115u, 5105u, 5095u, 5085u, 5074u, 5064u, 5054u, 5044u, 5034u, 5024u, 5014u, 5003u, 4993u, 4983u, 4973u, 4963u, 4954u,
 4944u, 4934u, 4924u, 4914u, 4904u, 4894u, 4885u, 4875u, 4865u, 4855u, 4846u, 4836u, 4826u, 4817u, 4807u, 4798u, 4788u,
 4779u, 4769u, 4760u, 4750u, 4741u, 4731u, 4722u, 4712u, 4703u, 4694u, 4684u, 4675u, 4666u, 4657u, 4647u, 4638u, 4629u,
 4620u, 4611u, 4601u, 4592u, 4583u, 4574u, 4565u, 4556u, 4547u, 4538u, 4529u, 4520u, 4511u, 4502u, 4494u, 4485u, 4476u,
 4467u, 4458u, 4449u, 4441u, 4432u, 4423u, 4415u, 4406u, 4397u, 4389u, 4380u, 4371u, 4363u, 4354u, 4346u, 4337u, 4329u,
 4320u, 4312u, 4303u, 4295u, 4286u, 4278u, 4270u, 4261u, 4253u, 4245u, 4236u, 4228u, 4220u, 4212u, 4203u, 4195u, 4187u,
 4179u, 4171u, 4163u, 4155u, 4146u, 4138u, 4130u, 4122u, 4114u, 4106u, 4098u, 4090u, 4082u, 4074u, 4067u, 4059u, 4051u,
 4043u, 4035u, 4027u, 4019u, 4012u, 4004u, 3996u, 3988u, 3981u, 3973u, 3965u, 3958u, 3950u, 3942u, 3935u, 3927u, 3920u,
 3912u, 3905u, 3897u, 3889u, 3882u, 3875u, 3867u, 3860u, 3852u, 3845u, 3837u, 3830u, 3823u, 3815u, 3808u, 3801u, 3793u,
 3786u, 3779u, 3772u, 3764u, 3757u, 3750u, 3743u, 3736u, 3728u, 3721u, 3714u, 3707u, 3700u, 3693u, 3686u, 3679u, 3672u,
 3665u, 3658u, 3651u, 3644u, 3637u, 3630u, 3623u, 3616u, 3609u, 3602u, 3596u, 3589u, 3582u, 3575u, 3568u, 3562u, 3555u,
 3548u, 3541u, 3535u, 3528u, 3521u, 3515u, 3508u, 3501u, 3495u, 3488u, 3481u, 3475u, 3468u, 3462u, 3455u, 3449u, 3442u,
 3436u, 3429u, 3423u, 3416u, 3410u, 3403u, 3397u, 3390u, 3384u, 3378u, 3371u, 3365u, 3359u, 3352u, 3346u, 3340u, 3333u,
 3327u, 3321u, 3315u, 3309u, 3302u, 3296u, 3290u, 3284u, 3278u, 3271u, 3265u, 3259u, 3253u, 3247u, 3241u, 3235u, 3229u,
 3223u, 3217u, 3211u, 3205u, 3199u, 3193u, 3187u, 3181u, 3175u, 3169u, 3163u, 3157u, 3151u, 3146u, 3140u, 3134u, 3128u,
 3122u, 3116u, 3111u, 3105u, 3099u, 3093u, 3088u, 3082u, 3076u, 3070u, 3065u, 3059u, 3053u, 3048u, 3042u, 3036u, 3031u,
 3025u, 3020u, 3014u, 3009u, 3003u, 2997u, 2992u, 2986u, 2981u, 2975u, 2970u, 2964u, 2959u, 2953u, 2948u, 2943u, 2937u,
 2932u, 2926u, 2921u, 2916u, 2910u, 2905u, 2900u, 2894u, 2889u, 2884u, 2878u, 2873u, 2868u, 2863u, 2857u, 2852u, 2847u,
 2842u, 2836u, 2831u, 2826u, 2821u, 2816u, 2811u, 2805u, 2800u, 2795u, 2790u, 2785u, 2780u, 2775u, 2770u, 2765u, 2760u,
 2755u, 2750u, 2745u, 2740u, 2735u, 2730u, 2725u, 2720u, 2715u, 2710u, 2705u, 2700u, 2695u, 2690u, 2686u, 2681u, 2676u,
 2671u, 2666u, 2661u, 2656u, 2652u, 2647u, 2642u, 2637u, 2633u, 2628u, 2623u, 2618u, 2614u, 2609u, 2604u, 2599u, 2595u,
 2590u, 2585u, 2581u, 2576u, 2571u, 2567u, 2562u, 2558u, 2553u, 2548u, 2544u, 2539u, 2535u, 2530u, 2526u, 2521u, 2517u,
 2512u, 2508u, 2503u, 2499u, 2494u, 2490u, 2485u, 2481u, 2476u, 2472u, 2467u, 2463u, 2459u, 2454u, 2450u, 2445u, 2441u,
 2437u, 2432u, 2428u, 2424u, 2419u, 2415u, 2411u, 2406u, 2402u, 2398u, 2394u, 2389u, 2385u, 2381u, 2377u, 2372u, 2368u,
 2364u, 2360u, 2356u, 2351u, 2347u, 2343u, 2339u, 2335u, 2331u, 2327u, 2322u, 2318u, 2314u, 2310u, 2306u, 2302u, 2298u,
 2294u, 2290u, 2286u, 2282u, 2278u, 2274u, 2270u, 2266u, 2262u, 2258u, 2254u, 2250u, 2246u, 2242u, 2238u, 2234u, 2230u,
 2226u, 2222u, 2218u, 2214u, 2211u, 2207u, 2203u, 2199u, 2195u, 2191u, 2187u, 2184u, 2180u, 2176u, 2172u, 2168u, 2165u,
 2161u, 2157u, 2153u, 2149u, 2146u, 2142u, 2138u, 2134u, 2131u, 2127u, 2123u, 2120u, 2116u, 2112u, 2109u, 2105u, 2101u,
 2098u, 2094u, 2090u, 2087u, 2083u, 2079u, 2076u, 2072u, 2069u, 2065u, 2061u, 2058u, 2054u, 2051u, 2047u, 2044u, 2040u,
 2037u, 2033u, 2030u, 2026u, 2023u, 2019u, 2016u, 2012u, 2009u, 2005u, 2002u, 1998u, 1995u, 1991u, 1988u, 1984u, 1981u,
 1978u, 1974u, 1971u, 1967u, 1964u, 1961u, 1957u, 1954u, 1951u, 1947u, 1944u, 1941u, 1937u, 1934u, 1931u, 1927u, 1924u,
 1921u, 1917u, 1914u, 1911u, 1908u, 1904u, 1901u, 1898u, 1895u, 1891u, 1888u, 1885u, 1882u, 1878u, 1875u, 1872u, 1869u,
 1866u, 1862u, 1859u, 1856u, 1853u, 1850u, 1847u, 1843u, 1840u, 1837u, 1834u, 1831u, 1828u, 1825u, 1822u, 1819u, 1815u,
 1812u, 1809u, 1806u, 1803u, 1800u, 1797u, 1794u, 1791u, 1788u, 1785u, 1782u, 1779u, 1776u, 1773u, 1770u, 1767u, 1764u,
 1761u, 1758u, 1755u, 1752u, 1749u, 1746u, 1743u, 1740u, 1737u, 1734u, 1731u, 1729u, 1726u, 1723u, 1720u, 1717u, 1714u,
 1711u, 1708u, 1705u, 1703u, 1700u, 1697u, 1694u, 1691u, 1688u, 1686u, 1683u, 1680u, 1677u, 1674u, 1671u, 1669u, 1666u,
 1663u, 1660u, 1658u, 1655u, 1652u, 1649u, 1647u, 1644u, 1641u, 1638u, 1636u, 1633u, 1630u, 1627u, 1625u, 1622u, 1619u,
 1617u, 1614u, 1611u, 1609u, 1606u, 1603u, 1601u, 1598u, 1595u, 1593u, 1590u, 1587u, 1585u, 1582u, 1579u, 1577u, 1574u,
 1572u, 1569u, 1566u, 1564u, 1561u, 1559u, 1556u, 1553u, 1551u, 1548u, 1546u, 1543u, 1541u, 1538u, 1536u, 1533u, 1531u,
 1528u, 1526u, 1523u, 1521u, 1518u, 1516u, 1513u, 1511u, 1508u, 1506u, 1503u, 1501u, 1498u, 1496u, 1493u, 1491u, 1488u,
 1486u, 1483u, 1481u, 1479u, 1476u, 1474u, 1471u, 1469u, 1467u, 1464u, 1462u, 1459u, 1457u, 1455u, 1452u, 1450u, 1448u,
 1445u, 1443u, 1440u, 1438u, 1436u, 1433u, 1431u, 1429u, 1426u, 1424u, 1422u, 1419u, 1417u, 1415u, 1413u, 1410u, 1408u,
 1406u, 1403u, 1401u, 1399u, 1397u, 1394u, 1392u, 1390u, 1388u, 1385u, 1383u, 1381u, 1379u, 1376u, 1374u, 1372u, 1370u,
 1367u, 1365u, 1363u, 1361u, 1359u, 1356u, 1354u, 1352u, 1350u, 1348u, 1346u, 1343u, 1341u, 1339u, 1337u, 1335u, 1333u,
 1331u, 1328u, 1326u, 1324u, 1322u, 1320u, 1318u, 1316u, 1314u, 1311u, 1309u, 1307u, 1305u, 1303u, 1301u, 1299u, 1297u,
 1295u, 1293u, 1291u, 1288u, 1286u, 1284u, 1282u, 1280u, 1278u, 1276u, 1274u, 1272u, 1270u, 1268u, 1266u, 1264u, 1262u,
 1260u, 1258u, 1256u };


    #if (Thermistor1_IMPLEMENTATION == Thermistor1_EQUATION_METHOD)

        float32 stEqn;
        float32 logrT;

        /* Calculate thermistor resistance from the voltages */
        #if(!CY_PSOC3)
            logrT = (float32)log((float64)resT);
        #else
            logrT = log((float32)resT);
        #endif  /* (!CY_PSOC3) */

        /* Calculate temperature from the resistance of thermistor using Steinhart-Hart Equation */
        #if(!CY_PSOC3)
            stEqn = (float32)(Thermistor1_THA + (Thermistor1_THB * logrT) + 
                             (Thermistor1_THC * pow((float64)logrT, (float32)3)));
        #else
            stEqn = (float32)(Thermistor1_THA + (Thermistor1_THB * logrT) + 
                             (Thermistor1_THC * pow(logrT, (float32)3)));
        #endif  /* (!CY_PSOC3) */

        tempTR = (int16)((float32)((((1.0 / stEqn) - Thermistor1_K2C) * (float32)Thermistor1_SCALE) + 0.5));

    #else /* Thermistor1_IMPLEMENTATION == Thermistor1_LUT_METHOD */

        uint16 mid;
        uint16 first = 0u;
        uint16 last = Thermistor1_LUT_SIZE - 1u;

        /* Calculate temperature from the resistance of thermistor by using the LUT */
        while (first < last)
        {
            mid = (first + last) >> 1u;

            if ((0u == mid) || ((Thermistor1_LUT_SIZE - 1u) == mid) || (resT == Thermistor1_LUT[mid]))
            {
                last = mid;
                break;
            }
            else if (Thermistor1_LUT[mid] > resT)
            {
                first = mid + 1u;
            }
            else /* (Thermistor1_LUT[mid] < resT) */
            {
                last = mid - 1u;
            }
        }

        /* Calculation the closest entry in the LUT */
        if ((Thermistor1_LUT[last] > resT) && (last < (Thermistor1_LUT_SIZE - 1u)) &&
           ((Thermistor1_LUT[last] - resT) > (resT - Thermistor1_LUT[last + 1u])))
        {
            last++;
        }
        else if ((Thermistor1_LUT[last] < resT) && (last > 0u) &&
                ((Thermistor1_LUT[last - 1u] - resT) < (resT - Thermistor1_LUT[last])))
        {
            last--;
        }
        else
        {
            /* Closest entry in the LUT already found */
        }

        tempTR = Thermistor1_MIN_TEMP + (int16)((uint16)(last * Thermistor1_ACCURACY));

    #endif /* (Thermistor1_IMPLEMENTATION == Thermistor1_EQUATION_METHOD) */

    return (tempTR);
}


/* [] END OF FILE */
