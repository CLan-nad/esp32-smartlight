#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <ESP32Servo.h>  // 使用 ESP32 专用的舵机库

#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"
#define SERVO_PIN 13  // 舵机信号线接 GPIO13

Servo myservo;  // 使用 ESP32Servo 库创建舵机对象
bool deviceConnected = false;
BLEServer* pServer = nullptr;
BLECharacteristic* pChar = nullptr;

class MyCallbacks : public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
        std::string value = pCharacteristic->getValue();
        if (value == "OFF") {           
            myservo.write(100);          
            delay(1000);                
            myservo.write(50);
            delay(1000);
            myservo.write(10);
            delay(1000);
            myservo.write(50);
        } 
    }
};


class MyServerCallbacks : public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
        deviceConnected = true;
    }

    void onDisconnect(BLEServer* pServer) {
        deviceConnected = false;
    }
};

void setup() {
    Serial.begin(115200);  // 初始化串行通信
    myservo.attach(SERVO_PIN);  // 将舵机绑定到 GPIO13
    myservo.write(50);
    BLEDevice::init("SmartLight_BLE"); // 设备名称
    pServer = BLEDevice::createServer();
    pServer->setCallbacks(new MyServerCallbacks());
    BLEService *pService = pServer->createService(SERVICE_UUID);
    pChar = pService->createCharacteristic(
        CHARACTERISTIC_UUID,
        BLECharacteristic::PROPERTY_WRITE
    );
    pChar->setCallbacks(new MyCallbacks());
    pService->start();
    pServer->getAdvertising()->start();
    Serial.println("BLE 广告开始，等待连接...");
}

void loop() {
    if (!deviceConnected) {
        // 如果设备断开连接，重新启动广告
        pServer->getAdvertising()->start();
    }
    delay(2000);
    
}
