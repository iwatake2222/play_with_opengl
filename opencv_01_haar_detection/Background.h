#ifndef BACKGROUND_H
#define BACKGROUND_H

void BackgroundDrawer_init(int width, int height);
void BackgroundDrawer_finalize();
void BackgroundDrawer_draw(int width, int height, uint8_t *data);

#endif
