#include <stdio.h>
#include "interface.h"
#include "socket.h"
#include "tunnel.h"

extern struct tunnel *app_tunnel;
extern void (*app_login_handler)(int success);

struct cloudy_widget *txLogin;
struct cloudy_widget *txPassword;
struct cloudy_widget *btLogin;
struct cloudy_widget *lbWelcome;
struct cloudy_widget *lbLogin;
struct cloudy_widget *lbPassword;

void login_response(int success) {
    if (success) {
        cloudy_app_terminate();
    } else {
        printf("falha login\n");
    }
}

void btLogin_click(struct cloudy_widget *w, struct position *p) {
    char* login = ((struct cloudy_textbox *)txLogin)->text;
    char* password = ((struct cloudy_textbox *)txPassword)->text;
    cloudy_login(login, password);
}

void interface_resize(struct cloudy_widget *interface) {
    int width = interface->rectangle.dimension.width;
    int height = interface->rectangle.dimension.height;

#define LEFT_BASE ((width - 150) / 2)
#define TOP_BASE ((height - 130) / 2)
    
    cloudy_widget_resize(lbWelcome, LEFT_BASE, TOP_BASE, 150, 20);
    cloudy_widget_resize(lbLogin, LEFT_BASE, TOP_BASE + 20, 150, 20);
    cloudy_widget_resize(txLogin, LEFT_BASE, TOP_BASE + 40, 150, 20);
    cloudy_widget_resize(lbPassword, LEFT_BASE, TOP_BASE + 60, 150, 20);
    cloudy_widget_resize(txPassword, LEFT_BASE, TOP_BASE + 80, 150, 20);
    cloudy_widget_resize(btLogin, LEFT_BASE, TOP_BASE + 110, 33, 20);
}

int main() {
    printf("Início do app de login\n");
    cloudy_app_initialize();
    
    /* ESTA PARTE É USADA PRA SIMULAR O SERVIDOR /
    struct socket *socket = socket_create();
    socket_listen(socket, 30000);
    struct socket *client = socket_accept(socket);
    app_tunnel = tunnel_create_from_socket(client);
    /* ATÉ AQUI  */
    
    struct cloudy_widget *interface = cloudy_get_app_interface();
    interface->resize = interface_resize;
    
    txLogin = cloudy_textbox_create();
    cloudy_widget_add(interface, txLogin);
    
    txPassword = cloudy_textbox_create();
    ((struct cloudy_textbox *)txPassword)->password_mask = '*';
    cloudy_widget_add(interface, txPassword);
    
    btLogin = (struct cloudy_widget *)cloudy_button_create();
    ((struct cloudy_button *)btLogin)->text = "login";
    btLogin->click = btLogin_click;
    cloudy_widget_add(interface, btLogin);
    
    lbWelcome = cloudy_label_create("Welcome to Cloudy!");
    cloudy_widget_add(interface, lbWelcome);
    lbLogin = cloudy_label_create("user");
    cloudy_widget_add(interface, lbLogin);
    lbPassword = cloudy_label_create("password");
    cloudy_widget_add(interface, lbPassword);
    
    /* ISSO AQUI TAMBÉM /
    cloudy_widget_resize(interface, 0, 0, 320, 200);
    cloudy_widget_repaint(interface);
    /* ATÉ AQUI  */
    
    
    app_login_handler = login_response;
    
    cloudy_app_start();
    
    return 0;
}
