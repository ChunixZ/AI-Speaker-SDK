#include "httpclient.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <curl/curl.h>
#include <sys/stat.h>
#include <dirent.h>
#include "cJSON.h"
#include "wireless.h"
//#include "device_json.h"



extern int wifi_status;
extern bool is_httpclient_ok;
extern bool is_baidu_tts;

////////////////////////////////////////////////////////////////////

char *read_version()
{
    static char buffer[32];
    FILE *fp;
    
    if((fp = fopen("/etc/config/version", "r")) != NULL)
    {
        memset(buffer, 0, 32);
        fread(buffer, 1, 32, fp);
        debug(1, "%s", buffer);
    }
    fclose(fp);
    
    return buffer;
}

int write_version(char *version)
{
    FILE *fp;
    
    fp = fopen("/etc/config/version", "w");
    fwrite(version, strlen(version), 1, fp);
    fclose(fp);
    
    return 0;
}

char *read_baidu_time()
{
    static char buffer[256];
    FILE *fp;
    
    if((fp = fopen("/etc/config/baidu_time", "r")) != NULL)
    {
        memset(buffer, 0, 256);
        fread(buffer, 1, 256, fp);
        debug(1, "%s", buffer);
    }
    fclose(fp);
    
    return buffer;
}

int write_baidu_time(char *version)
{
    FILE *fp;
    
    fp = fopen("/etc/config/baidu_time", "w");
    fwrite(version, strlen(version), 1, fp);
    fclose(fp);
    
    return 0;
}

char *read_baidu_tts()
{
    static char buffer[1024];
    FILE *fp;
    
    if((fp = fopen("/etc/config/baidu_tts", "r")) != NULL)
    {
        memset(buffer, 0, 1024);
        fread(buffer, 1, 1024, fp);
        debug(1, "%s", buffer);
    }
    fclose(fp);
    
    return buffer;
}

int write_baidu_tts(char *version)
{
    FILE *fp;
    
    fp = fopen("/etc/config/baidu_tts", "w");
    fwrite(version, strlen(version), 1, fp);
    fclose(fp);
    
    return 0;
}

char *baidu_time;
char *baidu_tts;

size_t write_data_token(void *buffer, size_t size, size_t nmemb, void *userp) 
{
    //FILE *fptr = (FILE*)userp;
    size_t written = fwrite(buffer, size, nmemb, userp);
    return written;
}

int baidu_tts_token()
{
    char token_str[256] = "https://openapi.baidu.com/oauth/2.0/token?grant_type=client_credentials&client_id=&client_secret=";
    
    CURL *curl;
    CURLcode res;
    struct curl_slist *headers = NULL;
    FILE *fp;
    
    fp = fopen("/etc/config/baidu_tts", "w+");
    
    curl_global_init(CURL_GLOBAL_ALL);
    curl = curl_easy_init();
    
    headers = curl_slist_append(headers, "Content-Type: application/x-www-form-urlencoded");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    
    curl_easy_setopt(curl, CURLOPT_URL, token_str);
    curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, "");
    curl_easy_setopt(curl,CURLOPT_POST, 1);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data_token);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
    
    res = curl_easy_perform(curl);
    
    if(res != CURLE_OK)
        fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
    
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
    
    fclose(fp);
    
    return 0;
}


size_t write_tts_token(void *buffer, size_t size, size_t nmemb, void *userp) 
{
    //FILE *fptr = (FILE*)userp;
    size_t written = fwrite(buffer, size, nmemb, userp);
    is_baidu_tts = false;
    debug(1, "is_baidu_tts = %d", is_baidu_tts);
    
    return written;
}

int baidu_wav(char *text, int person, int speed, int volume, int pit)
{
    char *tts_str;
    char baidu_wav[2048];
    char *token_str;
    //char *token_str = "24.63d5b3944a73db47cae99d8fd863726a.2592000.1515644692.282335-10513284";
    
    cJSON *root;
    cJSON *access_token;
    cJSON *session_key;
    cJSON *scope;
    cJSON *refresh_token;
    cJSON *session_secret;
    cJSON *expires_in;
    
    tts_str = read_baidu_tts();
    is_baidu_tts = true;
    debug(1, "is_baidu_tts = %d", is_baidu_tts);
    
    if(cJSON_Parse(tts_str))
    {
        root = cJSON_Parse(tts_str);
        expires_in = cJSON_GetObjectItem(root, "expires_in");
        access_token = cJSON_GetObjectItem(root, "access_token");
        token_str = access_token->valuestring;
        
        sprintf(baidu_wav, "http://tsn.baidu.com/text2audio?lan=zh&ctp=1&cuid=10513284&tok=%s&tex=\"%s\"&per=%d&spd=%d&vol=%d&pit=%d", token_str, text, person, speed, volume, pit);
    }
    
    debug(1, "%s", token_str);
    debug(1, "%s", baidu_wav);
    
    //sprintf(baidu_wav, "http://tsn.baidu.com/text2audio?lan=zh&ctp=1&cuid=10513284&tok=%s&tex=%s&per=%d&spd=%d&vol=%d&pit=%d", token_str, text, person, speed, volume, pit);

    CURL *curl;
    CURLcode res;
    struct curl_slist *headers = NULL;
    FILE *fp;
    
    fp = fopen("/tmp/imio_ai.mp3", "w+");
    
    curl_global_init(CURL_GLOBAL_ALL);
    curl = curl_easy_init();
    
    headers = curl_slist_append(headers, "Content-Type: application/x-www-form-urlencoded");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    
    curl_easy_setopt(curl, CURLOPT_URL, baidu_wav);
    curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, "");
    curl_easy_setopt(curl,CURLOPT_POST, 1);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_tts_token);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
    
    res = curl_easy_perform(curl);
    
    if(res != CURLE_OK)
        fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
    
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
    
    fclose(fp);
}

int baidu_tts_init()
{
    char *time_now;
    char *baidu_time;
    
    time_now = get_timestamp();
    debug(1, "%s", time_now);
    
    if(access("/etc/config/baidu_time",F_OK) == -1)
    {
        write_baidu_time(time_now);
        baidu_tts_token();
    }
    else
    {
        long int time_l;
        long int time_n;
    
        baidu_time = read_baidu_time();
        time_l = atol(baidu_time);
        time_n = atol(time_now);
        debug(1, "%ld  %ld", time_n, time_l);
        if((time_n - time_l) > 259200) //30*24*60*60
        {
            baidu_tts_token();
        }
    }
    
    baidu_tts = read_baidu_tts();
    
    return 0;
}


int read_register(char **dev_str)
{
    FILE_RESULT ret = FILE_ERROR;
    
    //1.1 mkdir if not exist
    DIR *fdir;
    int ret_mk;
    
    fdir = opendir(FILE_PATH);
    if(NULL == fdir)
    {
        ret_mk = mkdir(FILE_PATH, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
        if(0 == ret_mk)
        {
            printf("mkdir success\n");
        }
    }
    else
    {
        closedir(fdir);
    }
    
    //1.2. mkdir if not exist
    DIR *f_dir;
    int ret_m;
    
    f_dir = opendir(FILE_UPDATE_PATH);
    if(NULL == f_dir)
    {
        ret_m = mkdir(FILE_UPDATE_PATH, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
        if(0 == ret_m)
        {
            printf("mkdir success\n");
        }
    }
    else
    {
        closedir(f_dir);
    }
    
    //2. open file
    if((access(FILE_DEVICE_ID, F_OK)) != -1)
    {
        FILE *fp;
        char buffer[BUFF_SIZE];
        static char *dev_key_str;
        cJSON *root_reg;
        cJSON *signup_rep;
        cJSON *dev_key;
        cJSON *error_code;
        cJSON *error_info;
        
        fp = fopen(FILE_DEVICE_ID, "r");
        if(fp == NULL)
        {
            ret = FILE_NOT_OPEN;
            return ret;
        }
        
        memset(buffer, 0, BUFF_SIZE);
        fread(buffer, 1, BUFF_SIZE, fp);
        debug(1, "%s", buffer);
        if(cJSON_Parse(buffer))
        {
            debug(1, "JSON format is OK!");
        }
        else
        {
            debug(1, "JSON format is error!");
            ret = FILE_RELOAD;
        }
        
        root_reg = cJSON_Parse(buffer);
        if(NULL != root_reg)
        {
            signup_rep = cJSON_GetObjectItem(root_reg, "signup_rep");
            if(NULL != signup_rep)
            {
                dev_key = cJSON_GetObjectItem(signup_rep, "dev_key");
                error_code = cJSON_GetObjectItem(signup_rep, "error_code");
                error_info = cJSON_GetObjectItem(signup_rep, "error_info");
                debug(1, "dev_key = %d %s", strlen(dev_key->valuestring), dev_key->valuestring);
                debug(1, "error_code = %d", error_code->valueint);
                debug(1, "error_info = %s", error_info->valuestring);
                
                if(0 == error_code->valueint)
                {
                    debug(1, "dev_key = %s", dev_key->valuestring);
                    if((NULL != dev_key) && (strlen(dev_key->valuestring) > 5))
                    {
                        dev_key_str = dev_key->valuestring;
                        *dev_str = dev_key_str;
                        ret = FILE_OK;
                    }
                    else
                    {
                        debug(1, "%d %s", error_code->valueint, error_info->valuestring);
                        *dev_str = NULL;
                        ret = FILE_RELOAD;
                    }
                }
                else
                {
                    *dev_str = NULL;
                    ret = FILE_RELOAD;
                }
            }
        }
        fclose(fp);
    }
    else
    {
        ret = FILE_NOT_EXIST;
        *dev_str = NULL;
    }
    
    return ret;
}


char* read_userid()
{
    static char buffer[BUFF_SIZE];
    FILE *fp;
    
    if((fp = fopen(FILE_USER_ID, "r")) != NULL)
    {
        memset(buffer, 0, BUFF_SIZE);
        fread(buffer, 1, BUFF_SIZE, fp);
        //debug(1, "%s", buffer);
    }
    else
    {
        return NULL;
    }
    fclose(fp);
    
    return buffer;
}

void write_user_id(char *user_id)
{
    FILE *fp;
    
    fp = fopen(FILE_USER_ID, "w+");
    fwrite(user_id, strlen(user_id), 1, fp);
    fclose(fp);
}

void write_file(char *file_name, char *file_str)
{
    FILE *fp;
    
    fp = fopen(file_name, "w+");
    fwrite(file_str, strlen(file_str), 1, fp);
    fclose(fp);
}

char* read_device_id_file(char *file_name)
{
    FILE *fp;
    static char buffer[BUFF_SIZE];
    
    if((fp = fopen(file_name, "r")) != NULL)
    {
        memset(buffer, 0, BUFF_SIZE);
        fread(buffer, 1, BUFF_SIZE, fp);
        debug(1, "%s", buffer);
    }
    fclose(fp);
    
    return buffer;
}

char* read_produce_id_file(char *file_name)
{
    FILE *fp;
    static char buffer[BUFF_SIZE];
    
    if((fp = fopen(file_name, "r")) != NULL)
    {
        memset(buffer, 0, BUFF_SIZE);
        fread(buffer, 1, BUFF_SIZE, fp);
        debug(1, "%s", buffer);
    }
    fclose(fp);
    
    return buffer;
}


size_t write_data_reg(void *buffer, size_t size, size_t nmemb, void *userp) 
{
    //FILE *fptr = (FILE*)userp;
    size_t written = fwrite(buffer, size, nmemb, userp);
    return written;
}  

void httpclient_relaod()
{
    char dev_id[BUFF_SIZE];
    
    system("rm /etc/config/dev_id");
    sprintf(dev_id, "%s%s%s", DEV_ID, get_timestamp(), get_random_string(128));//142
    write_file(FILE_DEV_ID, dev_id);
    debug(1, "DEV_ID %s", dev_id);
}

HTTPCLIENT_RESULT httpclient_register()
{
    //1. Ready the post data
    HTTPCLIENT_RESULT ret = HTTPCLIENT_ERROR_CONN;
    char *post_url = HTTPS_R_POST_REGISTER_URL;
    
    //2. set json param
    char dev_id[BUFF_SIZE];
    char *type = DEV_TYPE;
    char *user_id;
    char *out;
    
    write_file(FILE_PRODUCT_ID, PRODUCT_ID);
    write_file(FILE_PRODUCT_SECRET, PRODUCT_SECRET);//len 40
    sprintf(dev_id, "%s%s%s", DEV_ID, get_timestamp(), get_random_string(128));//142
    write_file(FILE_DEV_ID, dev_id);
    debug(1, "PRODUCT_ID %s", PRODUCT_ID);
    debug(1, "PRODUCT_SECRET %s", PRODUCT_SECRET);
    debug(1, "DEV_ID %s", dev_id);
    
    //write_user_id(USER_ID);//<------------------------------------------------>//
    if(NULL != read_userid())
    {
        user_id = read_userid();
        debug(1, "%s len = %d", user_id, strlen(user_id));
        if(strlen(user_id) <= 8)
        {
            is_httpclient_ok = true;
            sleep(2);
            wifi_status = WIFI_CONNECTFAIL;
            return -1;
        }
    }
    else
    {
        debug(1, "<------------------------------------------------>");
        debug(1, "--  The user_id is NULL!  Please reload the network! --");
        debug(1, "<------------------------------------------------>");
        is_httpclient_ok = true;
        sleep(2);
        wifi_status = WIFI_CONNECTFAIL;
    }
    
    cJSON *root_reg;
    cJSON *signup_req;
    
    root_reg = cJSON_CreateObject();
    cJSON_AddItemToObject(root_reg, "signup_req", signup_req = cJSON_CreateObject());
    cJSON_AddStringToObject(signup_req, "product_id", PRODUCT_ID);
    cJSON_AddStringToObject(signup_req, "product_secret", PRODUCT_SECRET);
    cJSON_AddStringToObject(signup_req, "dev_id", dev_id);
    cJSON_AddStringToObject(signup_req, "type", type);
    if(NULL != user_id)
    {
        cJSON_AddStringToObject(signup_req, "user_id", user_id);
        //Note:  There is an error to wifi_connect
    }
    out = cJSON_Print(root_reg); /* Print to text */
    cJSON_Delete(root_reg);      /* Delete the cJSON object */
    debug(1, "[out] %s", out);

    //3. curl httpclient
    CURL *curl;
    CURLcode res;
    struct curl_slist *headers = NULL;
    
    FILE *fptr;
    if ((fptr = fopen(FILE_DEVICE_ID, "w+")) == NULL) 
    {  
        fprintf(stderr, "fopen file error: %s\n", FILE_DEVICE_ID);  
        return ret;
    }

    res = curl_global_init(CURL_GLOBAL_ALL);    //    CURL_GLOBAL_SSL
    if (CURLE_OK != res)
    {  
        printf("init libcurl failed.");
        curl_global_cleanup();
        return -1;
    }
    
    curl = curl_easy_init();
    if(curl) 
    {
        curl_easy_setopt(curl, CURLOPT_URL, post_url);
        if (res != CURLE_OK)
        {
            curl_easy_cleanup(curl);
            return -1;  
        }
        
        headers = curl_slist_append(headers, "Content-Type:application/json");
        //headers = curl_slist_append(headers, "charset:utf-8");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        
        // Now specify the POST data
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, out);
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
        curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);
        curl_easy_setopt(curl,CURLOPT_POST, 1);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data_reg);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, fptr);

        // Perform the request, res will get the return code
        res = curl_easy_perform(curl);
        curl_slist_free_all(headers);
        
        // Check for errors 
        if(res != CURLE_OK)
            fprintf(stderr, "curl_easy_perform() failed: %s\n",curl_easy_strerror(res));

        fclose(fptr);
        // always cleanup 
        curl_easy_cleanup(curl);
        ret = HTTPCLIENT_OK;
    }
    curl_global_cleanup();
    
    return ret;
}

HTTPCLIENT_JSON httpclient_login_prepare(char *send_str)
{
    HTTPCLIENT_JSON ret = HTTPCLIENT_JSON_UNKNOW;
    cJSON *root_reg;
    
    if(cJSON_Parse(send_str))
    {
        cJSON *signup_rep;
        cJSON *error_code;
        cJSON *error_info;
        
        root_reg = cJSON_Parse(send_str);
        if((cJSON_GetObjectItem(root_reg, "signup_rep")))
        {
            signup_rep = cJSON_GetObjectItem(root_reg, "signup_rep");
            if(cJSON_GetObjectItem(signup_rep, "error_code"))
            {
                error_code = cJSON_GetObjectItem(signup_rep, "error_code");
                if(0 == (error_code->valueint))
                {
                    ret = HTTPCLIENT_JSON_OK;
                    debug(1, "dev_key %s", (cJSON_GetObjectItem(signup_rep, "dev_key"))->valuestring);
                }
                
                if(cJSON_GetObjectItem(signup_rep, "error_info"))
                {
                    error_info = cJSON_GetObjectItem(signup_rep, "error_info");
                    if(NULL == (error_info->valuestring))
                    {
                        debug(1, "error_info %s", error_info->valuestring);
                    }
                }
                
            }
        }
    }
    else
    {
        ret = HTTPCLIENT_JSON_ERROR;
        debug(1, "cJSON format is error!");
    }
    
    return ret;
}

////////////////////////////////////////////////////////////////////
int read_online(char **dev_str, char **host_str)
{
    FILE_RESULT ret = FILE_ERROR;
    
    if((access(FILE_ONLINE_ID, F_OK)) != -1)
    {
        FILE *fp;
        char buffer[BUFF_SIZE];
        static char *dev_key_str;
        static char *host_key_str;
        cJSON *root_reg = cJSON_CreateObject();
        cJSON *signin_rep;
        cJSON *channel_no;
        cJSON *connect_backup;
        cJSON *connect_main;
        cJSON *error_code;
        cJSON *error_info;
        
        fp = fopen(FILE_ONLINE_ID, "r");
        if(fp == NULL)
        {
            ret = FILE_NOT_OPEN;
            return ret;
        }
        
        memset(buffer, 0, BUFF_SIZE);
        fread(buffer, 1, BUFF_SIZE, fp);
        //debug(1, "%s", buffer);
        
        root_reg = cJSON_Parse(buffer);
        if(NULL != root_reg)
        {
            signin_rep = cJSON_GetObjectItem(root_reg, "signin_rep");
            if(NULL != signin_rep)
            {
                channel_no = cJSON_GetObjectItem(signin_rep, "channel_no");
                connect_backup = cJSON_GetObjectItem(signin_rep, "connect_backup");
                connect_main = cJSON_GetObjectItem(signin_rep, "connect_main");
                error_code = cJSON_GetObjectItem(signin_rep, "error_code");
                error_info = cJSON_GetObjectItem(signin_rep, "error_info");
                if(0 == error_code->valueint)
                {
                    dev_key_str = channel_no->valuestring;
                    host_key_str = connect_main->valuestring;
                    *dev_str = dev_key_str;
                    *host_str = host_key_str;
                    ret = FILE_OK;
                }
                else
                {
                    //debug(1, "%d %s", error_code->valueint, error_info->valuestring);
                    *dev_str = NULL;
                    ret = FILE_NOT_REG;
                }
            }
        }
        
        fclose(fp);
    }
    else
    {
        ret = FILE_NOT_EXIST;
    }
    
    return ret;
}


size_t write_data_online(void *buffer, size_t size, size_t nmemb, void *userp) 
{
    //FILE *fptr = (FILE*)userp;  
    size_t written = fwrite(buffer, size, nmemb, userp);
    //printf("[receive] %s\n", (char *)buffer);
    return written;
}  

HTTPCLIENT_RESULT httpclient_login(char *dev_str)
{
    //1. Ready the post data
    HTTPCLIENT_RESULT ret = HTTPCLIENT_ERROR_CONN;
    char *post_url = HTTPS_R_POST_LOGIN_URL;
    
    //2. set json param
    char *product_id = read_produce_id_file(FILE_PRODUCT_ID);
    char *dev_id = read_device_id_file(FILE_DEV_ID);
    char *dev_key = dev_str;
    char *type = DEV_TYPE;
    char *user_id;    // = "494cefd5d4bacce6241173fedf77a38d960c6f6b3ceba3403bcab520ae82fd09";
    char *out;

    user_id = read_userid();
    debug(1, "%s", user_id);
    
    cJSON *root_reg;
    cJSON *signup_req;
    root_reg = cJSON_CreateObject();
    cJSON_AddItemToObject(root_reg, "signin_req", signup_req = cJSON_CreateObject());
    cJSON_AddStringToObject(signup_req, "product_id", product_id);
    cJSON_AddStringToObject(signup_req, "dev_id", dev_id);
    cJSON_AddStringToObject(signup_req, "dev_key", dev_key);
    cJSON_AddStringToObject(signup_req, "type", type);
    if(NULL != user_id)
    {
        cJSON_AddStringToObject(signup_req, "user_id", user_id);
    }
    out = cJSON_Print(root_reg); /* Print to text */
    cJSON_Delete(root_reg);      /* Delete the cJSON object */
    debug(1, "[out] %s", out);

    //3. curl httpclient
    CURL *curl;
    CURLcode res;
    struct curl_slist *headers = NULL;
    FILE *fptr;
 
     if ((fptr = fopen(FILE_ONLINE_ID, "w+")) == NULL) 
    {  
        fprintf(stderr, "fopen file error: %s\n", FILE_ONLINE_ID);  
        return ret;
    }  
    
    res = curl_global_init(CURL_GLOBAL_ALL);
    if (CURLE_OK != res)  
    {  
        printf("init libcurl failed.");  
        curl_global_cleanup();  
        return -1;  
    }
    
    /* get a curl handle */ 
    curl = curl_easy_init();
    if(curl) 
    {
        curl_easy_setopt(curl, CURLOPT_URL, post_url);
        headers = curl_slist_append(headers, "Content-Type:application/json");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        
        // Now specify the POST data
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, out);
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
        curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);
        curl_easy_setopt(curl,CURLOPT_POST,1);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data_online);  
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, fptr);  
        
        // Perform the request, res will get the return code
        res = curl_easy_perform(curl);
        curl_slist_free_all(headers);
        
        // Check for errors 
        if(res != CURLE_OK)
            fprintf(stderr, "curl_easy_perform() failed: %s\n",curl_easy_strerror(res));

        fclose(fptr);
        // always cleanup 
        curl_easy_cleanup(curl);
        ret = HTTPCLIENT_OK;
    }
    curl_global_cleanup();
    
    return ret;
}


//OTA
int i = 0;

size_t getcontentlengthfunc(void *ptr, size_t size, size_t nmemb, void *stream)
{
    int r;
    long len = 0;

    r = sscanf(ptr, "Content-Length: %ld\n", &len);
    if (r)
        *((long *) stream) = len;

    return size * nmemb;
}

static size_t write_data(void *ptr, size_t size, size_t nmemb, void *stream)  
{  
    size_t written = fwrite(ptr, size, nmemb, (FILE *)stream);
    printf("--------Downloading %d--------\n", i++);
    return written;  
} 

HTTPCLIENT_RESULT httpclient_ota(char *url, char *md5, char *version)
{
    HTTPCLIENT_RESULT ret = HTTPCLIENT_ERROR_CONN;
    CURL *curl_handle;
    CURLcode res;
    curl_off_t local_file_len = -1;
    struct stat file_info;
    int use_resume = 0;
    long filesize =0 ;
    static const char *pagefile_path = "/mnt/UDISK/misc-upgrade";
    FILE *pagefile;
    
    res = curl_global_init(CURL_GLOBAL_ALL);
    if (CURLE_OK != res)
    {  
        printf("init libcurl failed.");
        curl_global_cleanup();  
        return -1;
    }
    
    /* init the curl session */
    curl_handle = curl_easy_init();
    if(curl_handle)
    {
        /* set URL to get here */
        res = curl_easy_setopt(curl_handle, CURLOPT_URL, url);
        if (res != CURLE_OK)
        {
            curl_easy_cleanup(curl_handle);
            return -1;
        }
        
        /* Switch on full protocol/debug output while testing */
        curl_easy_setopt(curl_handle, CURLOPT_VERBOSE, 1L);
       
        /* disable progress meter, set to 0L to enable and disable debug output */
        curl_easy_setopt(curl_handle, CURLOPT_NOPROGRESS, 1L);
       
        /* send all data to this function  */   
        curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_data);
        
        /* open the file */   
        if(stat(pagefile_path, &file_info) == 0) 
        {
            local_file_len =  file_info.st_size;
            use_resume  = 1;
        }
        
        pagefile = fopen(pagefile_path, "ab+");
        if (pagefile)
        {
            curl_easy_setopt(curl_handle, CURLOPT_HEADERFUNCTION, getcontentlengthfunc);
            curl_easy_setopt(curl_handle, CURLOPT_HEADERDATA, &filesize);
            curl_easy_setopt(curl_handle, CURLOPT_RESUME_FROM_LARGE, use_resume?local_file_len:0);
            
            /* write the page body to this file handle */
            curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, pagefile);
            
            /* get it! */
            res = curl_easy_perform(curl_handle);
            /* Check for errors */
            if(res != CURLE_OK)
            {  
                fprintf(stderr, "curl_easy_perform() failed: %s\n",curl_easy_strerror(res));
                curl_easy_cleanup(curl_handle);
                return -1;
            }
            else
            {
                printf("--------Download is Ok --------\n");
                printf("--------Download is Ok --------\n");
                printf("--------Download is Ok --------\n");
                /*
                system("mv /mnt/UDISK/misc-upgrade/*.tar.gz /mnt/UDISK/misc-upgrade/update.tar.gz");
                system("tar -zxvf /mnt/UDISK/misc-upgrade/update.tar.gz -C /tmp");
                //system("mv /mnt/UDISK/misc-upgrade/ramdisk_sys.tar.gz /tmp");
                //system("mv /mnt/UDISK/misc-upgrade/target_sys.tar.gz /tmp");
                system("rm /mnt/UDISK/misc-upgrade/update.tar.gz");
                system("aw_upgrade_normal.sh -f -l /mnt/UDISK/misc-upgrade");
                */
            }
            
            /* close the header file */
            fclose(pagefile);
        }
        /* cleanup curl stuff */
        curl_easy_cleanup(curl_handle);
        
    }
   
    curl_global_cleanup();
    
    return ret;
}


