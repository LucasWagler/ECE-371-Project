#include "queue.h"

int main()
{
    queue_t q1;
    init_q(&q1,10);
    for (int i = 1; i < 100; i++){
        put_q(&q1,-i);
    }
    int16_t msg;
    bool success_in_get = get_q(&q1, &msg);
    for (int i = 0; i<15; i++){
        printf("Value gotten = %d", msg);
        if (success_in_get) printf(" is Good.\n");
        else printf(" is BAD!!!!\n");
        success_in_get = get_q(&q1, &msg);
    }
    put_q(&q1, 321);
    success_in_get = get_q(&q1, &msg);
    printf("Value gotten = %d", msg);
    if (success_in_get) printf(" is Good.\n");
    else printf(" is BAD!!!!\n");
    return 0;
}
