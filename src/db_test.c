#include "database/query.h"
#include "database/vm.h"
#include "database/core.h"
#include "database/repl.h"

int main()
{
	struct Database_Handle dbh = new_database();

	struct Column column1 = {
		.name = "Column1",
		.type = datatype_uint,
		.count = 1
	};
	struct Column column2 = {
		.name = "Column2",
		.type = datatype_float,
		.count = 1
	};

	create_table(
		dbh,
		"table_with_columns",
		2, column1, column2
	);


	printf("\n");

	// Inserting data into the database
	{
		int column1_data_data = 20;
		float column2_data_data = 1.5;
		struct InsertData column1_data = {
			.name = "Column1",
			.data = &column1_data_data
		};
		struct InsertData column2_data = {
			.name = "Column2",
			.data = &column2_data_data
		};
		insert_into(dbh, "table_with_columns", 2,
				column1_data, column2_data);
	}
	
	// Inserting data into the database
	{
		int column1_data_data = 50;
		float column2_data_data = 2.5;
		struct InsertData column1_data = {
			.name = "Column1",
			.data = &column1_data_data
		};
		struct InsertData column2_data = {
			.name = "Column2",
			.data = &column2_data_data
		};
		insert_into(dbh, "table_with_columns", 2,
				column1_data, column2_data);
	}

	// Query data back
	{
		printf("\n");
		struct Query test_query3 = {
			.table_name = "table_with_columns"
		};
		struct Iterator it = query(dbh, test_query3);

		print_column_headers(it);
		while(iterate(&it) != ITERATE_END)
			print_column_data(it);
	}
	struct Column column_string = {
		.name = "StringStuff",
		.type = datatype_string,
		.count = 255
	};
	create_table(dbh, "table_with_char", 1, column_string);

	char * test_string_data = "test_strign";
	struct InsertData column_StringStuff = {
		.name = "StringStuff",
		.data = test_string_data
	};
	insert_into(dbh, "table_with_char", 1, column_StringStuff);

	printf("\n query(\"tables\"); \n");

	{
		struct Query test_query = {
			.table_name = "tables"
		};
		struct Iterator it = query(dbh, test_query);

		print_column_headers(it);
		while(iterate(&it) != ITERATE_END)
			print_column_data(it);
	}

	while(repl(dbh));

	return 0;
}
