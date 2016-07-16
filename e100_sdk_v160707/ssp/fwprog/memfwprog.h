
#ifndef xstr
#define xstr(s) str(s)
#endif

#ifndef str
#define str(s) #s
#endif

//#define AMB_VER_NUM(major,minor)        ((major << 16) | (minor))
//#define AMB_VER_DATE(year,month,day)    ((year << 16) | (month << 8) | day)

//#ifndef FIRM_VER_DATE
//#define FIRM_VER_DATE                   AMB_VER_DATE(_YEAR__, __MONTH__, __DAY__)
//#endif

