# 1.6 is a lot better ducment than the newly release 2.0.0 that breaks backwards compatibilty
# This code is written for paho-mqtt==1.6.1 and will not work for paho 2.0!!!
import paho.mqtt.client as mqtt
import time
import sys

# Set up variables for connecting
broker_address = "mqtt.ics.ele.tue.nl"
topic_publish_robot_A = "/pynqbridge/5/recv"        # switched send and recv because they are defined for the pynq and not the computer!!!
topic_subscribe_robot_A = "/pynqbridge/5/send"      # ---> see connectivity board section: https://pynq.tue.nl/5EID0/io/
topic_publish_robot_B = "/pynqbridge/41/recv"
topic_subscribe_robot_B = "/pynqbridge/41/send"
username_A = "Student09"  # use Student09 or Student10
password_A = "Ok1Kasie"   # use Ok1Kasie for Student09 or Ea6leof9 for Student10
username_B = "Student81"  # use Student81 or Student82
password_B = "uN0ooh0G"   # use uN0ooh0G for Student81 or uX5Ohzei for Student82

# Select mode:
mode_0_or_1 = 0         # 1 for testing, 0 for running
if mode_0_or_1 == 1:
    topic_subscribe_robot_A = topic_publish_robot_A
    topic_subscribe_robot_B = topic_publish_robot_B

# Callbacks when connecting to the MQTT broker
def on_connect_A(client_A, userdata, flags, rc):
    if rc == 0:
        print("Connected robot A!")
        client_A.subscribe(topic_subscribe_robot_A, qos=1)
    else:
        print(f"Failed to connect to A, return code {rc}\n")

def on_connect_B(client_B, userdata, flags, rc):
    if rc == 0:
        print("Connected to robot B!")
        client_B.subscribe(topic_subscribe_robot_B, qos=1)
    else:
        print(f"Failed to connect to B, return code {rc}\n")

# Callback when receiving a message from the MQTT broker            ===> what to do when message is recieved!
def on_message(client, userdata, message):
    print("Received message: " + str(message.payload.decode("utf-8")) + "| on topic " + message.topic)
    ##############
    # logic here #
    ##############
    
# Setup MQTT clients and callbacks
client_A = mqtt.Client("9", clean_session=True)                             # use 9 for Student9
client_A.on_connect = on_connect_A
client_A.on_message = on_message

client_B = mqtt.Client("81", clean_session=True)                            # use 81 for Student81
client_B.on_connect = on_connect_B  
client_B.on_message = on_message

# Set the username and password
client_A.username_pw_set(username_A, password_A)
client_B.username_pw_set(username_B, password_B)

# Start the infinite loop
try:
    client_A.connect(broker_address, port=1883)                             # connect to the broker
    client_B.connect(broker_address, port=1883)
    
    client_A.loop_start()
    client_B.loop_start()
    time.sleep(1)
    msg_count = 0 
    while True:                                                             # publish messages infinitelly!
        if mode_0_or_1 == 1:                                                # testting communication w/ server
            message = f"l_steps {msg_count}"
        else:                                                               # actual message ==> use this to send to robot
            message_A = f"r_speed {msg_count}"
            message_B = f"l_speed {msg_count}"
            print("Send message_A: %s" %message_A)                          # check what message was sent
            print("Send message_B: %s\n" %message_B)
        client_A.publish(topic_publish_robot_A, message_A)
        client_B.publish(topic_publish_robot_B, message_B)
        msg_count += 1
        time.sleep(3)                                                       # wait b4 next message
except Exception as error:
    print(f"Failed to connect to MQTT broker at {broker_address}: {error}")
except KeyboardInterrupt:                                                   # exit when CTRL+C pressed
    print("Exiting...")
finally:
    client_A.loop_stop
    client_B.loop_stop
    print("Disconnecting from the MQTT broker")
    client_A.disconnect
    client_B.disconnect
