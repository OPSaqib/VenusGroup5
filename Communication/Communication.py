# 1.6 is a lot better ducment than the newly release 2.0.0 that breaks backwards compatibilty
# This code is written for paho-mqtt==1.6.1 and will not work for paho 2.0!!!
import paho.mqtt.client as mqtt
import time
import sys

# Set up variables for connecting
broker_address = "mqtt.ics.ele.tue.nl"
topic_publish = "/pynqbridge/5/send"
topic_subscribe = "/pynqbridge/5/recv"
username = "Student09"  # Use Student09 or Student10
password = "Ok1Kasie"   # Use Ok1Kasie for Student09 of Ea6leof9 for Student10

# Select mode:
mode_0_or_1 = 1         # 1 for testing, 0 for running
if mode_0_or_1 == 1:
    topic_subscribe = topic_publish

# Callback when connecting to the MQTT broker
def on_connect(client, userdata, flags, rc):
    if rc == 0:
        print("Connected to MQTT Broker!")
        client.subscribe(topic_subscribe, qos=1)
    else:
        print(f"Failed to connect, return code {rc}\n")

# Callback when receiving a message from the MQTT broker            ===> what to do when message is recieved!
def on_message(client, userdata, message):
    print("Received message: " + str(message.payload.decode("utf-8")) + "| on topic " + message.topic)
    
# Setup MQTT client and callbacks
client = mqtt.Client("9", clean_session=True)                              # Use 9 for Student9 or 10 for Student10
client.on_connect = on_connect
client.on_message = on_message

# Set the username and password
client.username_pw_set(username, password)

# start the infinite loop
try:
    client.connect(broker_address, port=1883)                               # Connect to the broker
    client.loop_start()
    msg_count = 0
    while True:                                                             # Publish messages infinitelly
        if mode_0_or_1 == 1:                                                # Testting
            message = f"l_speed {msg_count}"
        else:                                                               # Actual message
            message = f"r_speed {msg_count}"
            print("Send message: %s" %message)                              # Check what message was sent
        client.publish(topic_publish, message)
        msg_count += 1
        time.sleep(3)                                                       # Wait b4 next message
except Exception as error:
    print(f"Failed to connect to MQTT broker at {broker_address}: {error}")
except KeyboardInterrupt:                                                   # Exit when CTRL+C pressed
    print("Exiting...")
finally:
    client.loop_stop
    print("Disconnecting from the MQTT broker")
    client.disconnect
