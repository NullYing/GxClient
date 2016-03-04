#define _Stringizing(v)    #v
#define _VerJoin(a, b, c, d)  _Stringizing(a.b.c.d)

#define VER_Major    1
#define VER_Minor    1
#define VER_Release  3323       
#define VER_Build    3323       
#define VER_Version    MAKELONG(MAKEWORD(VER_Major, VER_Minor), VER_Release)

#define STR_BuildDate    TEXT(__DATE__)
#define STR_BuildTime    TEXT(__TIME__)
#define STR_BuilDateTime  TEXT(__DATE__) TEXT(" ") TEXT(__TIME__)
#define STR_Version    TEXT(_VerJoin(VER_Major, VER_Minor, VER_Release, VER_Build))

#define MAX_STRING 2048
#define CONFIGNAME TEXT("gxgk.conf")

#ifdef _Debug
#define STR_AppName    TEXT("莞香客户端(内测版)")
#else
#define STR_AppName    TEXT("莞香客户端")
#endif

#define STR_Author    TEXT("YZX")
#define STR_Corporation    TEXT("莞香广科")
#define STR_Modify    TEXT("白月秋见心")

#define STR_WEB      TEXT("www.gxgk.cc")
#define STR_WEB_URL    TEXT("http://") STR_WEB
#define STR_AUTHOR_URL TEXT("http://bbs.gxgk.cc/space-uid-207.html")
#define STR_Modify_URL	TEXT("http://www.lastfighting.com")


#define STR_Description    TEXT("莞香广科")
#define STR_Copyright    TEXT("版权没有 (C) 2009-2015 ")
#define Copyright     TEXT(STR_Copyright STR_Description)