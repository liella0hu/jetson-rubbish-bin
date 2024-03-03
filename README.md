jestson nanno和stm32平台上基于YOLOLV5的智能生活垃圾分类垃圾桶
==
上位机部分
---
使用pyqt5进行可交互界面的软件设计，详见pyqt5_cv2_yolov5_stm32_2.0.py的Open_Camera类。
继承生创基地的通信协议SimpleProtocol.py和ConvertCStruct.py在子线程中与stm32通信，老学长太给力了。
best.onnx包含YOLOV5模型权重和网络信息，使用onnx模型使得模型后处理速度更快。

下位机部分
---
使用两块stm32控制，一块负责控制舵机二维云台，一块负责控制直流电机传送带和光电红外传感器，都与jetson nano通信兼容。
