//
//  sql.h
//  modbus_post
//
//  Created by Michael Koppelman on 2/12/17.
//  Copyright © 2017 Badger Hill Brewing. All rights reserved.
//

#ifndef sql_h
#define sql_h

#include <sqlite3.h>
#include <stdio.h>
#include <string.h>


sqlite3* db_connect( char* database_name );
int db_query ( char* qry );
static int callback( void *NotUsed, int num_cols, char **col_data, char **col_names );

#endif /* sql_h */
