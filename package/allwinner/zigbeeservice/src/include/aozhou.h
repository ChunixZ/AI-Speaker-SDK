

#if __GNUC__ > 2 || (__GNUC__ == 2 && __GNUC_MINOR__ >= 95)
#define error(...) do {\
        fprintf(stderr, " %s:%d: ",  __FUNCTION__, __LINE__); \
        fprintf(stderr, __VA_ARGS__); \
        putc('\n', stderr); \
} while (0)
#else
#define error(args...) do {\
        fprintf(stderr, " %s:%d: ",  __FUNCTION__, __LINE__); \
        fprintf(stderr, ##args); \
        putc('\n', stderr); \
} while (0)
#endif  

#define MAX_GRAMMARID_LEN   (32)
#define MAX_PARAMS_LEN      (1024)

int text_to_speech(char* src_text,char *des_path);
int service_f(char *cmpbuf,const char *buf,char *rebuf,char *endbuf);
u_char *change_buf_channel(u_char * data,int* switchcmd,size_t *len);
