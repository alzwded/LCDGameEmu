#ifndef VIEWER_H
#define VIEWER_H

typedef struct viewer_s {
    void* _data;
    void (*nudge)(struct viewer_s*, void*);
} viewer_t;

#endif
