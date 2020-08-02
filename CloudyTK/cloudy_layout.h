#ifndef CLOUDY_LAYOUT_H
#define	CLOUDY_LAYOUT_H

#ifdef	__cplusplus
extern "C" {
#endif
    
struct cloudy_layout_manager {
    void (*resize)(struct cloudy_layout_manager *manager, struct cloudy_widget *widget);
    union {
        
    } data;
};



#ifdef	__cplusplus
}
#endif

#endif	/* CLOUDY_LAYOUT_H */

