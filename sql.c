//
//  sql.c
//  modbus_post
//
//  Created by Michael Koppelman on 2/12/17.
//  Copyright © 2017 Badger Hill Brewing. All rights reserved.
//

#include "sql.h"

sqlite3 *db;
char    *db_name = "/Users/lolife/Documents/Beer/BadgerHill/code/c/bh_sensor_data.db";

sqlite3* db_connect( char* database_name ) {
    int     rc;
    
    if( strlen(database_name) < 1  ){
        fprintf(stderr, "No database given.\n" );
        return(NULL);
    }
    
    rc = sqlite3_open( database_name, &db );
    if( rc ){
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db) );
        sqlite3_close(db);
        return(NULL);
    }
    return(db);
}

int db_query ( char* qry ) {
    char    *zErrMsg = 0;
    int     rc;
    
    if( ! db )
        db = db_connect(db_name);
    
    rc = sqlite3_exec( db, qry, callback, 0, &zErrMsg );
    if( rc!=SQLITE_OK ){
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    }
    return 0;
}

static int callback( void *NotUsed, int num_cols, char **col_data, char **col_names ) {
    int i;
    for( i=0; i<num_cols; i++ ){
        printf("%s = %s\n", col_names[i], col_data[i] ? col_data[i] : "NULL");
    }
    printf("\n");
    return 0;
}
