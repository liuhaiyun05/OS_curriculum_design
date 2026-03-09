#ifndef MOUSE_H
#define MOUSE_H

typedef struct {
    int x;
    int y;
    int left;
    int right;
    int middle;
    int enabled;
} MouseState;

void mouse_init(void);
void mouse_poll(void);
MouseState mouse_get_state(void);
void mouse_set_enabled(int enabled);

#endif