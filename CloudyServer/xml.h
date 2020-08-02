#ifndef XML_H
#define	XML_H

#ifdef	__cplusplus
extern "C" {
#endif

struct xml_attribute {
    char *name;
    char *value;
    struct xml_attribute *next;
}
    
struct xml_node {
    char *tag;
    struct xml_attribute *attributes;
    struct xml_node *child;
    struct xml_node *sibling;
};

#ifdef	__cplusplus
}
#endif

#endif	/* XML_H */

