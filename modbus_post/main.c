//
//  main.c
//  modbus_post
//
//  Created by Michael Koppelman on 12/20/16.
//  Copyright © 2016 Badger Hill Brewing. All rights reserved.
//

#include <stdio.h>
#include <modbus/modbus.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>

void get_emerson_data();
void get_brewhouse_data();

int main( int argc, char** argv ) {
//    while( 1 ) {
        get_emerson_data();
        // get_brewhouse_data();
        fflush(0);
//        sleep(11);
//    }
    return(0);
}

void get_brewhouse_data() {
    uint16_t	tab_reg[32];
    
    modbus_t	*mb = modbus_new_tcp( "10.0.1.200", 11110 );
    
    modbus_set_debug( mb, TRUE );
    modbus_connect( mb );
    int rc = modbus_read_input_registers( mb, 0, 12, tab_reg );
    if ( rc == -1 ) {
        fprintf( stderr, "Error in get_brewhouse_data: %i, %s\n", errno, modbus_strerror(errno) );
    }
    else {
        fprintf( stdout, "%li, %0.3f, %0.3f, %0.3f, %0.3f, %0.3f, %0.3f\n",
                time(0), modbus_get_float_badc(&tab_reg[0]), modbus_get_float_badc(&tab_reg[2]),
                modbus_get_float_badc( &tab_reg[4] ), modbus_get_float_badc( &tab_reg[6] ),
                modbus_get_float_badc( &tab_reg[8] ), modbus_get_float_badc( &tab_reg[10] ) );
    }
}

void get_emerson_data() {
    uint16_t	tab_reg[32];
    modbus_t	*mb = modbus_new_tcp( "10.0.1.128", 502 );
    
    //    modbus_set_debug( mb, TRUE );
    modbus_connect( mb );
    int rc = modbus_read_input_registers( mb, 0, 12, tab_reg );
    if ( rc == -1 ) {
        fprintf( stderr, "Error in get_emerson_data(): %i, %s\n", errno, modbus_strerror(errno) );
    }
    else {
        fprintf( stdout, "%li, %0.3f, %0.3f, %0.3f, %0.3f, %0.3f, %0.3f\n",
                time(0), modbus_get_float_badc(&tab_reg[0]), modbus_get_float_badc(&tab_reg[2]),
                modbus_get_float_badc( &tab_reg[4] ), modbus_get_float_badc( &tab_reg[6] ),
                modbus_get_float_badc( &tab_reg[8] ), modbus_get_float_badc( &tab_reg[10] ) );
    }
    modbus_close( mb );
    modbus_free( mb );
}
