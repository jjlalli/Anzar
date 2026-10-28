#ifndef ANZAR_APP_H
#define ANZAR_APP_H

/* Call anzar_app_init() once after the CubeMX peripheral init in main(),
   and anzar_app_loop() inside the main while(1) loop. */
void anzar_app_init(void);
void anzar_app_loop(void);

#endif
