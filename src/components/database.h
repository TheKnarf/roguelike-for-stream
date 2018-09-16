#ifndef DATABASE_H
#define DATABASE_H

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>

// Lookup table of predefined datatypes

struct Datatype
{
	char *name;
	size_t size;
};

static struct Datatype datatype_integer = {
	.name = "integer",
	.size = sizeof(int)
};

struct Datatype datatype_char = {
	.name = "char",
	.size = sizeof(char)
};

struct Datatype datatype_string = {
	.name = "string",
	.size = sizeof(char)
};

struct Column
{
	char *name;
	struct Datatype type;
	size_t count;

	// Only for column oriented layout
	void *data_begin; 
};

enum DataLayout
{
	DATALAYOUT_ROW_ORIENTED,
	DATALAYOUT_COLOUMN_ORIENTED
};

struct Table
{
	char *name;

	// TODO: rename to schema?
	struct Column columns[255];
	size_t number_of_columns;

	size_t number_of_rows;
	size_t rows_allocated; // Total space allocated

	enum DataLayout datalayout;
};

struct Tables
{
	struct Table tables[255]; // List of tables
	unsigned int number_of_tables;
};

struct Database_Handle
{
	struct Tables* tables;	
};

struct Query
{
	char *table_name;
	int query_schema;
	// list column names
	// filter method (WHERE)
};

struct Table* lookup_table(
	struct Database_Handle dbh, char* name) {

	for(int i=0; i<dbh.tables->number_of_tables; i++) {
		if(strcmp(dbh.tables->tables[i].name, name) == 0) {
			return &(dbh.tables->tables[i]);
		}
	}

	return NULL;
}

// QUERY / SUBSCRIBE
void query(struct Database_Handle dbh, struct Query query)
{
	if(strcmp(query.table_name, "tables") == 0) {
		printf("tables names\n");
		printf("------------\n");
		for(int i=0; i < dbh.tables->number_of_tables; i++) {
			printf("%s\n", dbh.tables->tables[i].name);
		}
		return;
	}

	struct Table* table = lookup_table(dbh, query.table_name);
	if(table == NULL) {
		fprintf(
			stderr,
			"Couldn't find table with name %s\n",
			query.table_name
		);
		return; 
	}

	if(query.query_schema) {
		// Print out all column names
		printf("column name   datatype name\n");
		printf("-----------   -------------\n");
		for(int i=0; i < table->number_of_columns; i++) {
			printf("%s       %s\n",
					table->columns[i].name,
					table->columns[i].type.name
					);

		}
	return;
	}

	for(int i=0; i < table->number_of_columns; i++) {
		printf("%s\t", table->columns[i].name);
	}
	printf("\n");

	for(size_t it_i = 0; it_i < table->number_of_rows; it_i++) {
		for(int i=0; i < table->number_of_columns; i++) {
			//	struct Iterator* it = create_itereator(
			//			dbh, table, table->columns[i]);

			char* type_name = table->columns[i].type.name;
			size_t total_size = table->columns[i].type.size *
				table->columns[i].count;

			void* it =
				table->columns[i].data_begin + it_i * total_size;
			if(strcmp(type_name, "integer") == 0) {
				printf("%d\t", *(int *)it);
			} else if(strcmp(type_name, "char") == 0) {
				printf("%c\t", *(char *)it);
			} else if(strcmp(type_name, "string") == 0) {
				printf("%s\t", (char *)it);
			}
		}
		printf("\n");
	}
}


struct Column* lookup_column(
	struct Database_Handle dbh, char* table_name, char* column_name)
{
	for(int i=0; i<dbh.tables->number_of_tables; i++) {
		if(strcmp(dbh.tables->tables[i].name, table_name) == 0) {
			
			for(
				int ic=0;
				ic<dbh.tables->tables[i].number_of_columns;
				ic++
			) {
				if(strcmp(
						dbh.tables->tables[i].columns[ic].name,
						column_name
				) == 0) {
					
					return &(dbh.tables->tables[i].columns[ic]);

				}
			}

		}
	}

	return NULL;
}

// INSERT
struct InsertData
{
	char *name;
	void *data;
};

void insert_into(
		struct Database_Handle dbh, char* table_name,
		int num, ...)
{
	va_list arg_list;
	va_start(arg_list, num);
	struct Table* table = lookup_table(dbh, table_name);
	for(int i=0; i<num; i++) {
		struct InsertData data = va_arg(arg_list, struct InsertData);
		struct Column* column =
			lookup_column(dbh, table_name, data.name);	

		assert(column != NULL);
		size_t column_size = column->type.size * column->count;

		// TOOD: rellaoc if we don't have enough space
		assert(
			table->number_of_rows < table->rows_allocated
		);

		void* data_current =
			column->data_begin + column_size * table->number_of_rows;
		memcpy(data_current, data.data, column_size);
	}

	table->number_of_rows++;
	va_end(arg_list);
}

// DELETE
// UPDATE

struct Database_Handle new_database()
{
	struct Database_Handle dbh = {};
	dbh.tables = calloc(sizeof(struct Tables), 1); 
	return dbh;
}

void create_table(
		struct Database_Handle dbh, char* name, int num, ...)
{
	assert(dbh.tables != NULL);
	assert(dbh.tables->number_of_tables < 255);

	struct Table new_table = {
		.name = name,
		.number_of_rows = 0,
		.rows_allocated = 255,
		.datalayout = DATALAYOUT_ROW_ORIENTED
	};

	dbh.tables->tables[dbh.tables->number_of_tables] = new_table;

	va_list arg_list;
	va_start(arg_list, num);
	for(int i=0; i<num; i++) {
		struct Column* current_column = &(dbh.tables
			->tables[dbh.tables->number_of_tables]
			.columns[i]);

		*current_column = va_arg(arg_list, struct Column);

		void *data = calloc(current_column->type.size, 255);

		current_column->data_begin = data;
	}
	dbh.tables
			->tables[dbh.tables->number_of_tables]
			.number_of_columns = num;
	va_end(arg_list);

	dbh.tables->number_of_tables++;
}

#endif
