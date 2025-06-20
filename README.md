# PID_controlled_linear_actuator
A belt-driven linear actuator from recycled ink printer. Controlled by PID controller implemented on STM32 Nuleo Board. This project will also include a comparision of different position control algorithms.

The third goal is to compare different methods of displacement measurement, i'll try to compare ultrasonic sensor (HC-SR04) with encoder-based position feedback.

The linear encoder in this system is implemented using a slotted optocoupler and an encoder strip with markings. I’ve attached a photo that shows a close-up of the device.
![optocoupler](https://github.com/user-attachments/assets/5379347d-ad52-4ae9-aa2b-9654e58f6a97)

At the moment, I’m unable to proceed further due to a short circuit issue, but here’s a video showing how the controller works implemented on the Arduino R3 board.

https://github.com/user-attachments/assets/e9678b23-de05-4ef0-bce5-9acc992ed03b

![signal-2025-02-26-224814_002](https://github.com/user-attachments/assets/bf286654-9d7c-4796-8270-6f37583d6115)
