#pragma once

#ifdef _MSC_VER

#ifdef HYT_BUILD_SHARED

#ifdef HYT_EXPORT
#define DLL_API __declspec(dllexport)
#else
#define DLL_API __declspec(dllimport)
#endif
#else
#define DLL_API
#endif
#else
#define DLL_API
#endif
