#ifndef TIMEDATE_H_
#define TIMEDATE_H_

    void mainMenu_setTime_job(void);

    struct _taskman_setTime
    {
        int8_t sm0;
        int8_t sign;
        int8_t reInitJob;

    };
    extern struct _taskman_setTime taskman_setTime;
#endif // TIMEDATE_H_
