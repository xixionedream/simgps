#ifndef _REG_STR_H
#define _REG_STR_H

#include <string.h>
 /* Bits set in the FLAGS argument to `fnmatch'. copy from fnmatch.h(linux) */
#define    FNM_PATHNAME (1 << 0) /* No wildcard can ever match `/'.  */
#define    FNM_NOESCAPE (1 << 1) /* Backslashes don't quote special chars.  */
#define    FNM_PERIOD (1 << 2) /* Leading `.' is matched only explicitly.  */
#define    FNM_NOMATCH        1

static bool safe_fnmatch(const char* pattern, size_t patt_len, const char* str, size_t str_len)
{
     size_t p = 0;
     size_t s = 0;
     while (1)
     {
         if (p == patt_len  &&  s == str_len)
             return true;
         if (p == patt_len)
             return false;
         if (s == str_len)
             return p + 1 == patt_len  &&  pattern[p] == '*';
         if (pattern[p] == str[s] || pattern[p] == '?')
         {
             p += 1;
             s += 1;
             continue;
         }
         if (pattern[p] == '*')
         {
             if (p + 1 == patt_len) return true;
             do
             {
				 if (safe_fnmatch(pattern + (p + 1), patt_len - (p + 1), str + s, str_len - s))
                 {
                     return true;
                 }
                 s += 1;
             } while (s != str_len);
             return false;
         }
         return false;
     }
 }


static int fnmatch_str(const char *pattern, const char *name, int flags = 0)
{
	 if (safe_fnmatch(pattern, strlen(pattern), name, strlen(name)))
         return 0;
     else
         return FNM_NOMATCH;
}

#endif //_REG_STR_H
