#ifndef DQN_CURL_H
#define DQN_CURL_H
// -----------------------------------------------------------------------------
// NOTE: Dqn_Curl
// -----------------------------------------------------------------------------
// A wrapper over CURL that is primarily used for hot-code reloading by
// packaging the CURL api into a struct that can be passed across DLL
// boundaries.
//
// curl_global_init(CURL_GLOBAL_ALL) must return CURLE_OK before anything
// in this file is used. You may cleanup curl on exit via curl_global_cleanup()
// if desired.
//
// An easy way to generate the curl commands required to query a url is to use
// CURL itself and the option to dump the command to a C-compatible file, i.e.
//
// curl --libcurl RequestToCCode.c -X POST -H "Content-Type: application/json" --data-binary "{\"jsonrpc\": \"2.0\", \"id\": \"0\", \"method\": \"get_service_nodes\", \"params\": []}" oxen.observer:22023/json_rpc
//
// -----------------------------------------------------------------------------
// NOTE: Configuration
// -----------------------------------------------------------------------------
// #define DQN_CURL_IMPLEMENTATION
//     Define this in one and only one C++ file to enable the implementation
//     code of the header file.

#define CURL_STATICLIB
#define NOMINMAX
#include <curl-7.72.0/include/curl/curl.h>

struct Dqn_CurlProcs
{
    CURL *(*curl_easy_init)(void);
    CURLcode (*curl_easy_getinfo)(CURL *curl, CURLINFO info, ...);
    CURLcode (*curl_easy_setopt)(CURL *handle, CURLoption option, ...);
    const char *(*curl_easy_strerror)(CURLcode);
    void (*curl_easy_cleanup)(CURL *curl);

    CURLM *(*curl_multi_init)(void);
    CURLMcode (*curl_multi_perform)(CURLM *multi_handle, int *running_handles);
    CURLMsg *(*curl_multi_info_read)(CURLM *multi_handle, int *msgs_in_queue);
    CURLMcode (*curl_multi_remove_handle)(CURLM *multi_handle, CURL *curl_handle);
    CURLMcode (*curl_multi_add_handle)(CURLM *multi_handle, CURL *curl_handle);
    const char *(*curl_multi_strerror)(CURLMcode);

    struct curl_slist *(*curl_slist_append)(struct curl_slist *, const char *);
    void (*curl_slist_free_all)(struct curl_slist *);
};

Dqn_CurlProcs Dqn_CurlProcs_Init();
#endif // DQN_CURL_H

#if defined(DQN_CURL_IMPLEMENTATION)
#pragma comment (lib, "Advapi32")
#pragma comment (lib, "Crypt32")
#pragma comment (lib, "Ws2_32")
#pragma comment (lib, "Wldap32")
#pragma comment (lib, "Normaliz")

Dqn_CurlProcs Dqn_CurlProcs_Init()
{
    Dqn_CurlProcs result      = {};
    result.curl_easy_init     = curl_easy_init;
    result.curl_easy_getinfo  = curl_easy_getinfo;
    result.curl_easy_setopt   = curl_easy_setopt;
    result.curl_easy_strerror = curl_easy_strerror;
    result.curl_easy_cleanup  = curl_easy_cleanup;

    result.curl_multi_init          = curl_multi_init;
    result.curl_multi_perform       = curl_multi_perform;
    result.curl_multi_info_read     = curl_multi_info_read;
    result.curl_multi_remove_handle = curl_multi_remove_handle;
    result.curl_multi_add_handle    = curl_multi_add_handle;
    result.curl_multi_strerror      = curl_multi_strerror;

    result.curl_slist_append   = curl_slist_append;
    result.curl_slist_free_all = curl_slist_free_all;

    return result;
}
#endif // DQN_CURL_IMPLEMENTATION
