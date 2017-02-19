#ifndef PROTOCOL_H
#define	PROTOCOL_H

#ifdef	__cplusplus
extern "C" {
#endif
   
    /*
     * Commands
     */
    #define WHO "WHO"
    #define INT "INT"
    #define SAV "SAV"
    #define STA "STA"
    #define STP "STP"
    #define SET "SET"
    #define GET "GET"

    /*
     * Functions
     */
    void PROTOCOL_Initialize(const char *device_id, void *start_handler, void *stop_hanldler, void *set_handler);
    void PROTOCOL_Read();
    
#ifdef	__cplusplus
}
#endif

#endif	/* PROTOCOL_H */

