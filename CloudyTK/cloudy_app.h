#ifndef CLOUDY_APP_H
#define	CLOUDY_APP_H

#ifdef	__cplusplus
extern "C" {
#endif

struct tunnel *cloudy_create_tunnel();
void cloudy_app_terminate();
struct cloudy_widget *cloudy_get_app_interface();
void cloudy_login(char* login, char* password);
void cloudy_app_exec(char* app_path);
void cloudy_app_show(int pid);

#ifdef	__cplusplus
}
#endif

#endif	/* CLOUDY_APP_H */

