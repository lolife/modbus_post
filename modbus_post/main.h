//
//  main.h
//  modbus_post
//
//  Created by Michael Koppelman on 2/12/17.
//  Copyright © 2017 Badger Hill Brewing. All rights reserved.
//

#ifndef main_h
#define main_h

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <modbus/modbus.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include "mreadfile.h"
#include "sql.h"

extern sqlite3 *db;
extern char    *db_name;

typedef struct emerson_data {
    long    t;      // time in seconds since the epoch
    float   cltp;   // volume of cold liquor tank in InchesOfH2O
    float   cltt;   // temp of cold liquor tank in ˚F
    float   fvwet;  // Whether FV probe is wet or not (1=YES, 0=NO)
    float   fvf;    // frequency from fork probe
    float   fvt;    // temp of fork probe
    float   bbtp;   // volume of bright beer tank
} emerson_data;

void get_brewhouse_data();
int get_emerson_data( emerson_data *edata );
void parse_emerson_data( uint16_t data[32], emerson_data *edata );
void create_sqlite_table();
int insert_emerson_data( emerson_data *edata );
void print_emerson_data ( emerson_data *edata );

#endif /* main_h */
