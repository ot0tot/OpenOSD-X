VPD TABLE (BREAKOUTBOARD)

| ADDR       |  TYPE          |       | DEFAULT-VAL         |
| ----       | ----           | ----- | ----------  | 
| 0x0801F800 | char[4]        | MAGIC |56 50 44 54 (VPDT)|
| 0x0801F804 | uint16_t[9]    | freq  |e0 15 12 16 44 16 76 16 a8 16 da 16 0c 17 3e 1770 17 |
| 0x0801F816 | uint8_t[2]     | power |0e 14 | 
| 0x0801F818 | uint16_t[2][9] | VPD   |14 05 32 05 41 05 78 05 c8 05 36 06 86 06 ae 06 e0 06 76 07 b2 07 bc 07 48 08 de 08 7e 09 ec 09 3c 0a be 0a |


```
#define CAL_FREQ_SIZE 9
#define CAL_DBM_SIZE 2
typedef struct vpd_table_def {
    char magic[4];
    uint16_t calFreqs[CAL_FREQ_SIZE];
    uint8_t calDBm[CAL_DBM_SIZE];
    uint16_t calVpd[CAL_DBM_SIZE][CAL_FREQ_SIZE];
} vpd_table_t;

// OpenOSD-X BreakoutBoard
const vpd_table_t vpd_table = {
    .calFreqs = {5600,	5650,	5700,	5750,	5800,	5850,	5900, 5950, 6000},
    .calDBm = {14, 20},
    .calVpd = {
        {1300,1330,1345,1400,1480,1590,1670,1710,1760},
        {1910,1970,1980,2120,2270,2430,2540,2620,2750}
    }
};

```
```
:020000040801F1
:10F8000056504454E015121644167616A816DA1609
:10F810000C173E1770170E141405320541057805B4
:10F82000C80536068606AE06E0067607B207BC07B0
:0CF830004808DE087E09EC093C0ABE0A0C
:00000001FF
```
