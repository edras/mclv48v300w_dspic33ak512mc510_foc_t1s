/* ************************************************************************** */
/** Descriptive File Name

  @Company
    Company Name

  @File Name
    filename.h

  @Summary
    Brief description of the file.

  @Description
    Describe the purpose of this file.
 */
/* ************************************************************************** */

#ifndef _T1S_LWIP_H    /* Guard against multiple inclusion */
#define _T1S_LWIP_H

#include <stdbool.h>

bool  T1S_init(void);
bool  T1S_available(void);
void  T1S_execute(void);
void  T1S_set_bridge(bool state);
void  T1S_set_debug(bool state);
void  T1S_print_menu(void);
void  T1S_send_cmd(char* buffer);

#endif /* _T1S_LWIP_H */

/* *****************************************************************************
 End of File
 */
