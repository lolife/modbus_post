//
//  main.c
//  modbus_post
//
//  Created by Michael Koppelman on 12/20/16.
//  Copyright © 2016 Badger Hill Brewing. All rights reserved.
//

#include "main.h"
#define READFILE 0
#define SELECT 0


// Main db connection

int main( int argc, char** argv ) {
    int             rtn = 0;
    emerson_data    *edata;
    char            buf[1024];
    int             n;
    FILE            *infile;
    
    edata = calloc( 1, sizeof(emerson_data) );

    if( READFILE ) {
        infile = fopen( "modbus_log.txt", "r" );
        assert( infile != NULL );
        
        n=-1;
        while( !feof( infile) ) {
            fgets( buf, 1024, infile );
            /* Comments begin with a '#' */
            if (buf[0] != '#') {
                n++;
                sscanf( buf, "%li, %f, %f, %f, %f, %f, %f\n", &edata->t, &edata->cltp, &edata->cltt, &edata->fvwet, &edata->fvf, &edata->fvt, &edata->bbtp );
                rtn = insert_emerson_data(edata);
                if( rtn !=0 ) {
                    fprintf( stderr, "Error when trying to insert emerson data.\n" );
                    return(rtn);
                }
            }
        }
        fclose( infile );
        printf( "Inserted %i lines from file\n", n );
    }
    else if (SELECT) {
        char *qry = "SELECT * FROM sensor_reports";
        printf( "rtn is %i\n", db_query( qry ) );
    }
    else {
        rtn = get_emerson_data( edata );
        if( rtn != 0 )
            return(rtn);
        rtn = insert_emerson_data(edata);
        if( rtn !=0 ) {
            fprintf( stderr, "Error when trying to insert emerson data.\n" );
            return(rtn);
        }
    }

    if(db)
       sqlite3_close(db);
    return(0);
}

void print_emerson_data ( emerson_data *edata ) {
    fprintf( stdout, "%li, %0.3f, %0.3f, %3.1f, %0.3f, %0.3f, %0.3f\n",
            edata->t, edata->cltp, edata->cltt,
            edata->fvwet, edata->fvf,
            edata->fvt, edata->bbtp
    );
}

int insert_emerson_data( emerson_data *edata ) {
    char            *qry;
    int             rtn;
    
    qry = calloc( 1, 1024 );
    
    print_emerson_data(edata);

    if( edata->cltp < 900000 ) {
        sprintf( qry, "INSERT INTO sensor_reports ( date_recorded, system_name, parameter_name, parameter_value, parameter_units, reported_by) VALUES ( %li, \"%s\", \"%s\", %8.3f, \"%s\", \"%s\" )", edata->t, "CLT", "Volume", edata->cltp, "InchesH2O", "Emerson Smart Gateway" );
        rtn = db_query( qry );
        if( rtn != 0 )
            return(rtn);
    }
    if( edata->cltp < 900000 ) {
        sprintf( qry, "INSERT INTO sensor_reports ( date_recorded, system_name, parameter_name, parameter_value, parameter_units, reported_by) VALUES ( %li, \"%s\", \"%s\", %8.3f, \"%s\", \"%s\" )", edata->t, "CLT", "Temperature", edata->cltt, "Degrees F", "Emerson Smart Gateway" );
        rtn = db_query(qry);
        if( rtn != 0 )
            return(rtn);
    }
    
    if ( edata->fvf < 900000 ) {
        sprintf( qry, "INSERT INTO sensor_reports ( date_recorded, system_name, parameter_name, parameter_value, parameter_units, reported_by) VALUES ( %li, \"%s\", \"%s\", %8.3f, \"%s\", \"%s\" )", edata->t, "FV", "Density", edata->fvf, "Hz", "Emerson Smart Gateway" );
        rtn = db_query(qry);
        if( rtn != 0 )
            return(rtn);
    }
    return(0);
}

int get_emerson_data( emerson_data *edata ) {
    uint16_t        tab_reg[32];
    modbus_t        *mb = modbus_new_tcp( "10.0.1.128", 502 );
    
    //    modbus_set_debug( mb, TRUE );
    modbus_connect( mb );
    int rc = modbus_read_input_registers( mb, 0, 12, tab_reg );
    if ( rc == -1 ) {
        fprintf( stderr, "Error in get_emerson_data(): %i, %s\n", errno, modbus_strerror(errno) );
        return(-1);
    }
    else
		parse_emerson_data( tab_reg, edata );

    modbus_close( mb );
    modbus_free( mb );
    return(0);
}

void parse_emerson_data( uint16_t data[32], emerson_data *edata ) {
    edata->t        = time(0);
    edata->cltp     = modbus_get_float_badc(&data[0]);
    edata->cltt     = modbus_get_float_badc(&data[2]);
    edata->fvwet    = modbus_get_float_badc(&data[4]);
    edata->fvf      = modbus_get_float_badc(&data[6]);
    edata->fvt      = modbus_get_float_badc(&data[8]);
    edata->bbtp     = modbus_get_float_badc(&data[10]);
}

void create_sqlite_table() {
    int     rtn;
    char    *qry = "CREATE table sensor_reports ( date_recorded int, system_name text, parameter_name text, parameter_value real, parameter_units text, reported_by text, date_received int DEFAULT CURRENT_TIMESTAMP)";
    
    rtn = db_query( qry );
    printf( "rtn is %i\n", rtn );
    return;
}



void get_brewhouse_data() {
//    uint16_t	tab_reg[32];
//    
//    modbus_t	*mb = modbus_new_tcp( "10.0.1.200", 11110 );
//    
//    modbus_set_debug( mb, TRUE );
//    modbus_connect( mb );
//    int rc = modbus_read_input_registers( mb, 0, 12, tab_reg );
//    if ( rc == -1 ) {
//        fprintf( stderr, "Error in get_brewhouse_data: %i, %s\n", errno, modbus_strerror(errno) );
//    }
//    else {
//        fprintf( stdout, "%li, %0.3f, %0.3f, %0.3f, %0.3f, %0.3f, %0.3f\n",
//                time(0), modbus_get_float_badc(&tab_reg[0]), modbus_get_float_badc(&tab_reg[2]),
//                modbus_get_float_badc( &tab_reg[4] ), modbus_get_float_badc( &tab_reg[6] ),
//                modbus_get_float_badc( &tab_reg[8] ), modbus_get_float_badc( &tab_reg[10] ) );
//    }
}


