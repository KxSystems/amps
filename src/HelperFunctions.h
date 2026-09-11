#ifndef __HELPER_FUNCTIONS_H__
#define __HELPER_FUNCTIONS_H__

#include <string>
#include <cstring>
#include <list>

#include <k.h>

#ifdef _WIN32
#define EXP __declspec(dllexport)
#else
#define EXP __attribute__((visibility("default")))
#endif // _WIN32


////////////////////////
// EXCEPTION HANDLING //
////////////////////////

#define KDB_EXCEPTION_TRY \
  static char error_msg[1024]; \
  *error_msg = '\0'; \
  try {

#define KDB_EXCEPTION_CATCH \
  } catch (std::exception& e) {  \
    std::strncpy(error_msg, e.what(), sizeof(error_msg));  \
    error_msg[sizeof(error_msg) - 1] = '\0';  \
    return krr(error_msg);  \
  }


/////////////////
// KDB STRINGS //
/////////////////

inline bool IsKdbString(K str)
{
  return str != NULL && (str->t == -KS || str->t == KC);
}

inline const std::string GetKdbString(K str)
{
  return str->t == -KS ? str->s : std::string((S)kG(str), str->n);
}

// Create a KC from a std::string
inline K kpn(const std::string& str)
{
  return kpn((S)str.c_str(), str.length());
}

// Create a KS from a list of strings
inline K ktns(const std::list<std::string> strings)
{
  K vals = ktn(KS, strings.size());
  size_t index = 0;
  for (auto i : strings)
    kS(vals)[index++] = sn((S)i.c_str(), (I)i.length());
  return vals;
}


#endif // __HELPER_FUNCTIONS_H__
