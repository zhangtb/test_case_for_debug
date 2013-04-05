#include <stdio.h>  
#include <pthread.h>  
#include <malloc.h>  
#include <unistd.h>
#include <vector>
#include <error.h>

#define likely(x) __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)

#define DEFAULT_NUM 5

class TestCase{
    public:
        TestCase(int32_t value) : m_vecScore(*t_vecScore) {
            m_vecScore.resize(value % 20);
        }
        int32_t size() {return m_vecScore.size();}
        void resize(int32_t count) { m_vecScore.resize(count); }
        static void init() { 
            if (unlikely(!t_vecScore)) {
                t_vecScore = new std::vector<int32_t>();
            }
        }
    public:
        std::vector<int32_t> &m_vecScore;
        
        static __thread std::vector<int32_t> *t_vecScore;
        static __thread uint64_t t_value;
};

__thread std::vector<int32_t> *TestCase::t_vecScore = NULL;
__thread uint64_t TestCase::t_value = 10;
  
void * thread_function( void * args )  
{  
    int id = *(int *)args;
    uint64_t tls_pointer;
    
    TestCase::t_value = id;
    TestCase::init();
    TestCase test(id);
    uint64_t tls, fs;
    asm volatile("movq %%fs:0, %0; \
            movq %%fs, %1;"
            :"=r"(tls) ,"=r"(fs)   
            );


    test.resize(id);
    fprintf( stderr, "thread%d : Tls TestCase pointor :(%p, %p [%d]) \
            fs : %ld tls segment : %p  id : %d\n",
                TestCase::t_value,
                &TestCase::t_value,&TestCase::t_vecScore,
                test.size(), fs, tls,
                ( int )pthread_self());
    
    sleep(3);
    fprintf( stderr, "thread%d : Tls pointor :(%p, %p [%d]) id : %d\n", TestCase::t_value, 
                &TestCase::t_value,&TestCase::t_vecScore,
                test.size(),
                ( int )pthread_self());  
 
    return NULL;  
}  
int main(int argc, const char *argv[])  
{  
    int i;  
    int thread_num = DEFAULT_NUM;
    if (argc >= 2 && argv[1] != '\0') {
        thread_num = atoi(argv[1]);
    }
    if (thread_num < 1) {
        thread_num = DEFAULT_NUM;
    }
    pthread_t threads[thread_num];  
    int args[thread_num];
    for ( i = 0; i < thread_num; ++i )  
    {  
        args[i] = i;
        pthread_create( &(threads[i]), NULL, thread_function, &args[i] );  
    }  
    for ( i = 0; i < thread_num; ++i )  
    {  
        pthread_join( threads[i], NULL );  
    }  
    return 0;  
} 
