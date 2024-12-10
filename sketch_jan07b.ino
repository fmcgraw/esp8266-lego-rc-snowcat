/**
  @file    wifi_car_esp8266_AP.ino
  @brief   Simple example of Wifi Car controlled by a web server in AP Mode. See also :
           https://github.com/lacour-vincent/wifi-car-esp8266

           Example of commands to control the car :
           
           MOVE:
           - http://<YourIP>:<YourPort>/move?dir=F (Forward)
           - http://<YourIP>:<YourPort>/move?dir=B (Bacward)
           
           ACTION:
           - http://<YourIP>:<YourPort>/action?type=1 (Action 1)
           - http://<YourIP>:<YourPort>/action?type=1 (Action 2)


  @author  LACOUR Vincent
  @date    2019-10

  Moving
  myservo1.write(180) - full speed forward
  myservo1.write(0) - full speed back
  myservo1.write(90) - stop

  servospeed max is 90 because we use myservo1.write(90 + servospeed) for moving forward and myservo1.write(90 - servospeed) for back
*/

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <Servo.h>

const char *ssid = "WIFI_CAR_ESP9266";
const char *password = "wificarpassword";

IPAddress ip(192, 168, 4, 1);
IPAddress netmask(255, 255, 255, 0);
const int port = 8080; // Port
ESP8266WebServer server(port);

//For moving
Servo myservo1;
Servo myservo2;
int servospeed;
int servospeed_max = 90;
int servospeed_min = 30;
bool detach_move = false;

//For front blade
Servo additional_servo1;
int additional_servo1_min = 0;
int additional_servo1_max = 90;
bool additional_servo1_detach = true;

void setup() {
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(ip, ip, netmask);
  WiFi.softAP(ssid, password);

  //For moving
  myservo1.attach(5); //D1
  myservo2.attach(4); //D2
  myservo1.write(90);
  myservo2.write(90);
  servospeed = servospeed_max;

  //For front blade
  additional_servo1.attach(2); //D4
  additional_servo1.write(additional_servo1_min);

  server.on("/move", HTTP_GET, handleMoveRequest);
  server.on("/action", HTTP_GET, handleActionRequest);
  server.onNotFound(handleNotFound);
  server.begin();
}

void loop() {
  server.handleClient();
}

void handleMoveRequest() {
  if (!server.hasArg("dir")) {
    server.send(404, "text / plain", "Move: undefined");
    return;
  }
  String direction = server.arg("dir");
  if (direction.equals("F")) {
    forward();
    server.send(200, "text / plain", "Move: forward");
  }
  else if (direction.equals("B")) {
    backward();
    server.send(200, "text / plain", "Move: backward");
  }
  else  if (direction.equals("S")) {
    stop_motors();
    server.send(200, "text / plain", "Move: stop");
  }
  else  if (direction.equals("L")) {
    turn_left();
    server.send(200, "text / plain", "Turn: Left");
  }
  else  if (direction.equals("R")) {
    turn_right();
    server.send(200, "text / plain", "Turn: Right");
  }
  else {
    server.send(404, "text / plain", "Move: undefined");
  }
}


void handleActionRequest() {
  if (!server.hasArg("type")) {
    server.send(404, "text / plain", "Action: undefined");
    return;
  }
  String type = server.arg("type");
  if (type.equals("1")) {
    // TODO : Action 1 - FRONT SERVO DOWN
    toggle_detach_add_down();
    server.send(200, "text / plain", "Action 1");
  }
  else if (type.equals("2")) {
    // TODO : Action 2 - CHANGE SPEED
    change_speed();
    server.send(200, "text / plain", "Action 2");
  }
  else if (type.equals("3")) {
    // TODO : Action 3
    server.send(200, "text / plain", "Action 3");
  }
  else if (type.equals("4")) {
    // TODO : Action 4 - Toggle Detach Move
    toggle_detach_move();
    server.send(200, "text / plain", "Action 4");
  }
  else if (type.equals("5")) {
    // TODO : Action 5 - FRONT SERVO UP
    toggle_detach_add_up();
    server.send(200, "text / plain", "Action 5");
  }
  else if (type.equals("6")) {
    // TODO : Action 6
    server.send(200, "text / plain", "Action 6");
  }
  else if (type.equals("7")) {
    // TODO : Action 7
    server.send(200, "text / plain", "Action 7");
  }
  else if (type.equals("8")) {
    // TODO : Action 8 - Toggle Detach for front blade
    toggle_detach_add_servo1();
    server.send(200, "text / plain", "Action 8");
  }
  else {
    server.send(404, "text / plain", "Action: undefined");
  }
}

void handleNotFound() {
  server.send(404, "text / plain", "404: Not found");
}


void stop_motors() {
  myservo1.write(90);
  myservo2.write(90);
  
  if(detach_move){
    myservo1.detach();
    myservo2.detach();  
  }
}

void backward() {
  if(detach_move){
    myservo1.attach(5);
    myservo2.attach(4);
  }
  
  myservo1.write(90 - servospeed);
  myservo2.write(90 + servospeed);
}

void forward() {
  if(detach_move){
    myservo1.attach(5);
    myservo2.attach(4);
  }
  
  myservo1.write(90 + servospeed);
  myservo2.write(90 - servospeed);
}

void turn_left() {
  if(detach_move){
    myservo1.attach(5);
    myservo2.attach(4);
  }
  
  myservo1.write(90 - servospeed);
  myservo2.write(90 - servospeed);
}

void turn_right() {
  if(detach_move){
    myservo1.attach(5);
    myservo2.attach(4);
  }
  
  myservo1.write(90 + servospeed);
  myservo2.write(90 + servospeed);
}

void toggle_detach_add_down(){
  if(additional_servo1_detach){
    additional_servo1.attach(2);
  }

  additional_servo1.write(additional_servo1_min);

  if(additional_servo1_detach){
    additional_servo1.detach();
  }
}

void toggle_detach_add_up(){
  if(additional_servo1_detach){
    additional_servo1.attach(2);
  }

  additional_servo1.write(additional_servo1_max);

  if(additional_servo1_detach){
    additional_servo1.detach();
  }
}

void change_speed() {
  if(servospeed == servospeed_max){
    servospeed = servospeed_min;
  } else {
    servospeed = servospeed_max;
  }
}

void toggle_detach_move() {
  detach_move = !detach_move;
  if(!detach_move){
    myservo1.attach(5);
    myservo2.attach(4);
  }
}

void toggle_detach_add_servo1() {
  additional_servo1_detach = !additional_servo1_detach;
  if(!additional_servo1_detach){
    additional_servo1.attach(2);
  }
}
