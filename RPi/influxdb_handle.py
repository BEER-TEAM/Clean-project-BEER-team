from influxdb import InfluxDBClient
from time import sleep

class InfluxDBHandler:
    def __init__(self, host, port, database, username=None, password=None):
        """
        Initialize the InfluxDBHandler class for InfluxDB v1.x.

        :param host: Hostname or IP address of the InfluxDB server.
        :param port: Port number of the InfluxDB server.
        :param database: Database name where the data will be stored.
        """
        self.client = InfluxDBClient(host=host, port=port, username=username, password=password, database=database)
        self.database = database
        # Create the database if it does not exist
        self.client.create_database(database)
        self.client.switch_database(database)

    def write_data(self, measurement, fields):
        """
        Write data to InfluxDB.

        :param measurement: Measurement name.
        :param fields: A dictionary of field key-value pairs to be stored.
        """
        if not isinstance(fields, dict):
            raise ValueError("Fields must be provided as a dictionary.")

        json_body = [
            {
                "measurement": measurement,
                "fields": fields
            }
        ]
        try:
            self.client.write_points(json_body) # commented for testing
            print(f"Successfully wrote to InfluxDB: {measurement} -> {fields}")
            # sleep(10) # delay for testing
        except Exception as e:
            print(f"Failed to write to InfluxDB: {e}")

    def close(self):
        """Close the InfluxDB client connection."""
        self.client.close()
