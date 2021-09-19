#ifndef DQN_CURL_H
#define DQN_CURL_H
// -----------------------------------------------------------------------------
// NOTE: Dqn_Curl
// -----------------------------------------------------------------------------
// Define DQN_CURL_IMPLEMENTATION in one and only one file to enable the
// implementation in translation unit.
//
// curl_global_init(CURL_GLOBAL_ALL) must return CURLE_OK before anything
// in this file is used. You may cleanup curl on exit via curl_global_cleanup()
// if desired.
//
// An easy way to generate the curl commands required to query a url is to use
// CURL itself and the option to dump the command to a C-compatible file, i.e.
//
// curl --libcurl RequestToCCode.c -X POST -H "Content-Type: application/json" --data-binary "{\"jsonrpc\": \"2.0\", \"id\": \"0\", \"method\": \"get_block_count\", \"params\": []}" oxen.observer:22023/json_rpc
//
// -----------------------------------------------------------------------------
// NOTE: Example
// -----------------------------------------------------------------------------
#if 0
struct CurlWriteFunctionUserData
{
    Dqn_ArenaAllocator *arena;
    Dqn_StringList      data;
};

size_t CurlWriteFunction(char *ptr, size_t size, size_t nmemb, void *userdata)
{
    auto *user_data = DQN_CAST(CurlWriteFunctionUserData *)userdata;
    Dqn_StringList_AppendStringCopy(&user_data->data, user_data->arena, Dqn_String_Init(ptr, nmemb));
    DQN_ASSERT(size == 1);
    return nmemb;
}

void main()
{
    // NOTE: Setup Curl handle
    CURL *handle = curl_easy_init();

    struct curl_slist *header_list = nullptr;
    header_list                    = curl_slist_append(header_list, "Content-Type: application/json");
    curl_easy_setopt(handle, CURLOPT_HTTPHEADER, header_list);

    Dqn_String post_data = DQN_STRING("{\"jsonrpc\": \"2.0\", \"id\": \"0\", \"method\": \"get_block_count\", \"params\": []}");
    Dqn_Curl_SetHTTPPost(handle, "oxen.observer:22023/json_rpc", post_data.str, DQN_CAST(int)post_data.size);

    // NOTE: Set write callback
    Dqn_ThreadScratch scratch           = Dqn_Thread_GetScratch();
    CurlWriteFunctionUserData user_data = {};
    user_data.arena                     = scratch.arena;
    Dqn_Curl_SetWriteCallback(handle, CurlWriteFunction, &user_data);

    // NOTE: Execute CURL query
    curl_easy_perform(handle);
    Dqn_String output = Dqn_StringList_Build(&user_data.string_list, scoped_arena.arena);
    DQN_LOG_I("%.*s", DQN_STRING_FMT(output));

    // NOTE: Cleanup
    curl_slist_free_all(header_list);
    curl_easy_cleanup(handle);
}
#endif

// NOTE: Warning this causes Windows.h to be included.
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
void          Dqn_Curl_SetURL(CURL *handle, char const *url);
void          Dqn_Curl_SetHTTPPost(CURL *handle, char const *url, char const *post_data, int post_data_size);
void          Dqn_Curl_SetWriteCallback(CURL *handle, size_t (*curl_write_callback)(char *ptr, size_t size, size_t nmemb, void *userdata), void *user_data);
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

void Dqn_Curl_SetURL(CURL *handle, char const *url)
{
    curl_easy_setopt(handle, CURLOPT_URL, url);
}

void Dqn_Curl_SetHTTPPost(CURL *handle, char const *url, char const *post_data, int post_data_size)
{
    curl_easy_setopt(handle, CURLOPT_BUFFERSIZE, 102400L);
    curl_easy_setopt(handle, CURLOPT_URL, url);
    curl_easy_setopt(handle, CURLOPT_NOPROGRESS, 1L);
    curl_easy_setopt(handle, CURLOPT_POSTFIELDS, post_data);
    curl_easy_setopt(handle, CURLOPT_POSTFIELDSIZE_LARGE, (curl_off_t)post_data_size);
    curl_easy_setopt(handle, CURLOPT_USERAGENT, "curl/7.55.1");
    curl_easy_setopt(handle, CURLOPT_MAXREDIRS, 50L);
    curl_easy_setopt(handle, CURLOPT_CUSTOMREQUEST, "POST");
    curl_easy_setopt(handle, CURLOPT_TCP_KEEPALIVE, 1L);
}

void Dqn_Curl_SetWriteCallback(CURL *handle,
                               size_t (*curl_write_callback)(char *ptr, size_t size, size_t nmemb, void *userdata),
                               void *user_data)
{
    curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, curl_write_callback);
    curl_easy_setopt(handle, CURLOPT_WRITEDATA, user_data);
}
#endif // DQN_CURL_IMPLEMENTATION
