import mysql.connector
import paho.mqtt.client as mqtt
import json

# Define callback functions
def on_connect(client, userdata, flags, rc):
    print("Connected with result code "+str(rc))
    # Subscribe to the topic
    client.subscribe("/quantanics/industry/ultrasonic4")

# Define callback functions
def on_message(client, userdata, msg):
    print(msg.topic+" "+str(msg.payload))
    # Parse the JSON payload
    try:
        json_data = json.loads(msg.payload)
        distance = json_data.get('distance')
        # Convert the distance to integer
        distance = int(distance)
    except (json.JSONDecodeError, ValueError) as e:
        print("Invalid JSON data received:", msg.payload)
        return
    # Insert received data into the database0
    insert_into_database(distance)

# Function to insert data into the database
def insert_into_database(distance):
    mydb = mysql.connector.connect(
        host="localhost",
        user="root",
        password="",
        database="data"
    )
    mycursor = mydb.cursor()
    sql = "INSERT INTO data(distance) VALUES(%s)"
    val = (distance,)
    mycursor.execute(sql, val)
    mydb.commit()

    mycursor.close()
    mydb.close()
    print("Data inserted into database:", distance)

# Create a MQTT client instance
client = mqtt.Client()

# Assign callback functions
client.on_connect = on_connect
client.on_message = on_message

# Connect to MQTT broker
client.connect("broker.emqx.io", 1883, 60)

# Start the loop
client.loop_forever()
