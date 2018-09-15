#include "components/database.h"

int main()
{
	struct Database_Handle dbh = new_database();

	create_table(dbh, "test", 0);	
	create_table(dbh, "test2", 0);

	struct Datatype datatype_integer = {
		.name = "integer",
		.size = sizeof(int)
	};
	struct Column column1 = {
		.name = "Column1",
		.type = datatype_integer
	};
	struct Column column2 = {
		.name = "Column2",
		.type = datatype_integer	
	};

	create_table(
		dbh,
		"table_with_columns",
		2, column1, column2
	);

	struct Query test_query = {
		.table_name = "tables"
	};
	query(dbh, test_query);

	printf("\n");

	struct Query test_query2 = {
		.table_name = "table_with_columns",
		.query_schema = 1
	};
	query(dbh, test_query2);

	// Inserting data into the database
	{
		int column1_data_data = 20;
		int column2_data_data = 30;
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
		int column2_data_data = 60;
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
	struct Query test_query3 = {
		.table_name = "table_with_columns"
	};
	query(dbh, test_query3);

	return 0;
}
