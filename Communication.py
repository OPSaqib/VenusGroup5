# 1.6 is a lot better ducment than the newly release 2.0.0 that breaks backwards compatibilty
# This code is written for paho-mqtt==1.6.1 and will not work for paho 2.0!!!
import paho.mqtt.client as mqtt
import time
import sys

# Set up variables for connecting
broker_address = "mqtt.ics.ele.tue.nl"
topic_publish = "/pynqbridge/5/send"
topic_subscribe = "/pynqbridge/5/recv"
username = "Student09"  #use Student09 or Student10
password = "Ok1Kasie"   #use Ok1Kasie for Student09 of Ea6leof9 for Student10

# Select mode:
mode_0_or_1 = 1         #1 for testing, 0 for running
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
client = mqtt.Client("Example", clean_session=False)
client.on_connect = on_connect
client.on_message = on_message

# Set the username and password
client.username_pw_set(username, password)

# start the infinite loop
try:
    client.connect(broker_address, port=1883)                               #connect to the broker
    client.loop_start()
    msg_count = 0
    while True:                                                             #publish messages infinitelly
        if mode_0_or_1 == 1:                                                #testting
            message = f"Hello world, this is msg {msg_count}"
        else:                                                               #actual message
            message = f"Goodbye world, this is msg {msg_count}"
            print("Send message: %s" %message)                              #check what message was sent
        client.publish(topic_publish, message)
        msg_count += 1
        time.sleep(3)                                                       #wait b4 next message
except Exception as error:
    print(f"Failed to connect to MQTT broker at {broker_address}: {error}")
except KeyboardInterrupt:                                                   #exit when CTRL+C pressed
    print("Exiting...")
finally:
    client.loop_stop
    print("Disconnecting from the MQTT broker")
    client.disconnect
