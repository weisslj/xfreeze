/* 
 * auth_pam.c - ...
 *
 * Copyright 2004 Johannes Weißl
 *
 * This file is part of xfreeze.
 *
 * xfreeze is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * xfreeze is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with xfreeze; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "auth_pam.h"
#include "input.h" /* xgetstr */

#include <stdio.h> /* NULL */
#include <stdlib.h> /* calloc */
#include <unistd.h> /* getuid */
#include <pwd.h> /* getpwuid */
#include <sys/types.h> /* uid_t */
#include <security/pam_appl.h> /* pam_start, ... */

#if HAVE_CONFIG_H
#include <config.h>
#endif

static const char *get_user(uid_t uid);
static int xconv(int num_msg, const struct pam_message **msgm,
                 struct pam_response **response, void *display);

int auth_pam(Display *display)
{
    struct pam_conv pconv;
    pam_handle_t *pamh;
    const char *user;
    int retval;

    if (!display)
        return 0;

    user = get_user(getuid());
    if (!user)
        return 0;

    pconv.conv = xconv;
    pconv.appdata_ptr = display;
    pamh = NULL;

    retval = pam_start(PACKAGE, user, &pconv, &pamh);

    if (retval == PAM_SUCCESS)
        retval = pam_authenticate(pamh, 0);

    if (pam_end(pamh, retval) != PAM_SUCCESS)
        return 0;

    return (retval == PAM_SUCCESS) ? 1 : 0;
}

static const char *get_user(uid_t uid)
{
    struct passwd *pw;
    pw = getpwuid(uid);
    if (!pw)
        return NULL;
    return pw->pw_name;
}

static int xconv(int num_msg, const struct pam_message **msgm,
                 struct pam_response **response, void *display)
{
    int i;
    char *string;
    struct pam_response *reply;

    if (num_msg < 1)
        return PAM_CONV_ERR;

    reply = (struct pam_response *) calloc(num_msg,
            sizeof(struct pam_response));
    if (!reply)
        return PAM_CONV_ERR;

    for (i = 0; i < num_msg; i++) {
        string = NULL;
        switch (msgm[i]->msg_style) {
        case PAM_PROMPT_ECHO_ON:
            printf("%s\n", msgm[i]->msg);
        case PAM_PROMPT_ECHO_OFF:
            string = xgetstr((Display *) display, NULL, -1);
            break;
        case PAM_ERROR_MSG:
            fprintf(stderr, "%s\n", msgm[i]->msg);
            break;
        case PAM_TEXT_INFO:
            printf("%s\n", msgm[i]->msg);
            break;
        }
        if (string) {
            reply[i].resp_retcode = 0;
            reply[i].resp = string;
        }
    }

    *response = reply;

    return PAM_SUCCESS;
}
