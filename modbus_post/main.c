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


int main( int argc, char** argv ) {
    int             rtn = 0;
    emerson_data    *edata;
    
    edata = calloc( 1, sizeof(emerson_data) );

//    read_from_serial();
    
    if( READFILE ) {
        int num_records = read_and_insert_file();
        printf( "Inserted %i lines from file\n", num_records );

    }
    else if (SELECT) {
        char *qry = "SELECT * FROM sensor_reports";
        printf( "rtn is %i\n", db_query( qry ) );
    }
    else {
        rtn = get_emerson_data( edata );
        if( rtn != 0 )
            return(rtn);
//        rtn = insert_emerson_data(edata);
//        if( rtn !=0 ) {
//            fprintf( stderr, "Error when trying to insert emerson data.\n" );
//            return(rtn);
//        }
    }

    free(edata);
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
    free(qry);
    return(0);
}

void get_gateway_time( modbus_t *mb, char* date_string ) {
    uint16_t        data[32];

    int rc = modbus_read_registers( mb, 9000, 16, data );
    if ( rc == -1 ) {
        fprintf( stderr, "Error in get_gateway_time(): %i, %s\n", errno, modbus_strerror(errno) );
        //        return(-1);
    }
    else {
        sprintf( date_string, "%02i/%02i/%i, %02i:%02i:%02i",
                data[1],
                data[2],
                data[0],
                data[3],
                data[4],
                data[5]
        );
    }
}

int get_emerson_data( emerson_data *edata ) {
    uint16_t        tab_reg[32];
    int             rc = 0;
    modbus_t        *mb = modbus_new_tcp( "192.168.1.36", 502 );
//  modbus_t        *mb = modbus_new_tcp( "10.0.0.25", 502 );
    char            date_string[1024];
    FILE            *log_file;
 
    log_file = fopen( "/Users/lolife/modbus_post-log.txt", "a" );
    modbus_connect( mb );
//  modbus_set_debug( mb, TRUE );
    
    while ( rc >= 0 ) {
        rc = modbus_read_input_registers( mb, 0, 16, tab_reg );
        if ( rc == -1 ) {
            fprintf( stderr, "Error in get_emerson_data(): %i, %s\n", errno, modbus_strerror(errno) );
        }
        else {
            get_gateway_time( mb, date_string );
            fprintf( stdout, "%s", "." );
            fprintf( log_file, "%s, %li, %7.2f, %5.2f\n", date_string, time(0), modbus_get_float_badc(&tab_reg[2]), modbus_get_float_badc(&tab_reg[4]) );
            fflush( log_file );
//          print_modbus_data( tab_reg, 8 );
//          parse_emerson_data( tab_reg, edata );
        }
        sleep(60);
//        memset( &tab_reg, '\0', 32 );
    }
    modbus_close( mb );
    modbus_free( mb );
    return(0);
}

void print_modbus_data( uint16_t data[32], int nnum ) {
    for( int n = 0; n < nnum; n+=2 ) {
        fprintf( stdout, "data[%02i] = %7.2f (%i)\n", n, modbus_get_float_badc(&data[n]), data[n] );
    }
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
//    int     rtn;
//    char    *qry = "CREATE table sensor_reports (
//        date_recorded int,
//        system_name text,
//        parameter_name text,
//        parameter_value real,
//        parameter_units text, reported_by text, date_received int DEFAULT CURRENT_TIMESTAMP)";"
//    
//    rtn = db_query( qry );
//    printf( "rtn is %i\n", rtn );
    return;
}

int read_and_insert_file() {
    char            buf[1024];
    int             n;
    FILE            *infile;
    int             rtn = 0;
    emerson_data    *edata;
    
    edata = calloc( 1, sizeof(emerson_data) );

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
    free(edata);
    return(n);
}

//int read_from_serial() {
//    int         rc;
//    modbus_t    *mb;
//    uint16_t    tab_reg[64];
//    uint32_t     t1 = 10, t2=10000;
//
//    mb = modbus_new_rtu("/dev/tty.KeySerial1", 9600, 'N', 8, 1);
//    if (mb == NULL) {
//        fprintf(stderr, "Unable to create the libmodbus context\n");
//        return -1;
//    }
//    
//    modbus_set_debug( mb, TRUE );
//    rc = modbus_set_slave( mb, 1);
//    if (rc == -1) {
//        fprintf(stderr, "Invalid slave ID\n");
//        modbus_free(mb);
//        return -1;
//    }
//    
//    if (modbus_connect(mb) == -1) {
//        fprintf(stderr, "Connection failed: %s\n", modbus_strerror(errno));
//        modbus_free(mb);
//        return -1;
//    }

//    modbus_set_response_timeout( mb, t1, t2 );
    
//    uint8_t tab_bytes[MODBUS_MAX_PDU_LENGTH];
//    rc = modbus_report_slave_id( mb, MODBUS_MAX_PDU_LENGTH, tab_bytes);
//    modbus_set_response_timeout( mb, t1, t2 );
//    if (rc > 1) {
//        printf("Slave ID: %s\n", &tab_bytes[0] );
//    }
//    rc = modbus_read_input_registers( mb, 1000, 2, tab_reg );
//   rc = modbus_read_registers( mb, 1000, 2, tab_reg );
//    modbus_flush(mb);
//  rc = modbus_read_bits( mb, 800, 3,  tab_reg );
//    if ( rc == -1 ) {
//        fprintf( stderr, "Error in read_from_serial(): %i, %s\n", errno, modbus_strerror(errno) );
//        //        return(-1);
//    }
//    else {
//        fprintf( stdout, "It seemed to work!\n" );
//        fprintf( stdout, "%f, %f, %f, %f\n",
//                modbus_get_float_abcd(&tab_reg[0]),
//                modbus_get_float_badc(&tab_reg[0]),
//                modbus_get_float_cdab(&tab_reg[0]),
//                modbus_get_float_dcba(&tab_reg[0]) );
//        for ( int i=0; i < rc; i++) {
//            printf("reg[%d]=%d (0x%X)\n", i, tab_reg[i], tab_reg[i]);
//        }
//    }
    
//    modbus_close( mb );
//    modbus_free( mb );

//    return(0);
//}
