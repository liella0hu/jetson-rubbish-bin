# here put the import lib
# here for jetson nano
import sys
from PyQt5 import QtCore, QtGui, QtWidgets
from PyQt5.QtWidgets import QApplication, QMainWindow
from PyQt5.QtCore import QTimer
from PyQt5.QtGui import QImage, QPixmap
from PyQt5.QtWidgets import QMessageBox
from rubish_qt_show import Ui_MainWindow
import numpy as np
import cv2
import time
from random import uniform
from PyQt5.Qt import *
from SimpleProtocol import SimTypes, SimpleProtocol
import serial
import re
from config import Common
import onnxruntime
import queue


CLASSES = ['0', '1', '2', '3', '4', '5', '6', '7', '8', '9']

# net = models.resnet50()

sum_fg = 0
result_info = queue.Queue()
record_info = queue.Queue()


class YOLOV5():
    def __init__(self, onnxpath):
        print(onnxpath)
        opt = onnxruntime.SessionOptions()
        opt.intra_op_num_threads = 3 
        opt.inter_op_num_threads = 3
        self.onnx_session = onnxruntime.InferenceSession(onnxpath, opt, providers='CPUExecutionProvider')
        # self.onnx_session = onnxruntime.InferenceSession(onnxpath)
        print(self.onnx_session)
        self.input_name = self.get_input_name()
        self.output_name = self.get_output_name()

    # -------------------------------------------------------
    #   获取输入输出的名字
    # -------------------------------------------------------
    def get_input_name(self):
        input_name = []
        for node in self.onnx_session.get_inputs():
            input_name.append(node.name)
        return input_name

    def get_output_name(self):
        output_name = []
        for node in self.onnx_session.get_outputs():
            output_name.append(node.name)
        return output_name

    # -------------------------------------------------------
    #   输入图像
    # -------------------------------------------------------
    def get_input_feed(self, img_tensor):
        input_feed = {}
        for name in self.input_name:
            input_feed[name] = img_tensor
        return input_feed

    # -------------------------------------------------------
    #   1.cv2读取图像并resize
    #	2.图像转BGR2RGB和HWC2CHW
    #	3.图像归一化
    #	4.图像增加维度
    #	5.onnx_session 推理
    # -------------------------------------------------------
    def inference(self, img_path):
        # img = cv2.imread(img_path)
        or_img = cv2.resize(img_path, (640, 640))
        img = or_img[:, :, ::-1].transpose(2, 0, 1)  # BGR2RGB和HWC2CHW
        img = img.astype(dtype=np.float32)
        img /= 255.0
        img = np.expand_dims(img, axis=0)
        input_feed = self.get_input_feed(img)
        pred = self.onnx_session.run(None, input_feed)[0]
        return pred, or_img

# dets:  array [x,6] 6个值分别为x1,y1,x2,y2,score,class
# thresh: 阈值
def nms(dets, thresh):
    x1 = dets[:, 0]
    y1 = dets[:, 1]
    x2 = dets[:, 2]
    y2 = dets[:, 3]
    # -------------------------------------------------------
    #   计算框的面积
    #	置信度从大到小排序
    # -------------------------------------------------------
    areas = (y2 - y1 + 1) * (x2 - x1 + 1)
    scores = dets[:, 4]
    keep = []
    index = scores.argsort()[::-1]

    while index.size > 0:
        i = index[0]
        keep.append(i)
        # -------------------------------------------------------
        #   计算相交面积
        #	1.相交
        #	2.不相交
        # -------------------------------------------------------
        x11 = np.maximum(x1[i], x1[index[1:]])
        y11 = np.maximum(y1[i], y1[index[1:]])
        x22 = np.minimum(x2[i], x2[index[1:]])
        y22 = np.minimum(y2[i], y2[index[1:]])

        w = np.maximum(0, x22 - x11 + 1)
        h = np.maximum(0, y22 - y11 + 1)

        overlaps = w * h
        # -------------------------------------------------------
        #   计算该框与其它框的IOU，去除掉重复的框，即IOU值大的框
        #	IOU小于thresh的框保留下来
        # -------------------------------------------------------
        ious = overlaps / (areas[i] + areas[index[1:]] - overlaps)
        idx = np.where(ious <= thresh)[0]
        index = index[idx + 1]
    return keep


def xywh2xyxy(x):
    # [x, y, w, h] to [x1, y1, x2, y2]
    y = np.copy(x)
    y[:, 0] = x[:, 0] - x[:, 2] / 2
    y[:, 1] = x[:, 1] - x[:, 3] / 2
    y[:, 2] = x[:, 0] + x[:, 2] / 2
    y[:, 3] = x[:, 1] + x[:, 3] / 2
    return y


def filter_box(org_box, conf_thres, iou_thres):  # 过滤掉无用的框
    # -------------------------------------------------------
    #   删除为1的维度
    #	删除置信度小于conf_thres的BOX
    # -------------------------------------------------------
    org_box = np.squeeze(org_box)
    conf = org_box[..., 4] > conf_thres
    box = org_box[conf == True]
    # -------------------------------------------------------
    #	通过argmax获取置信度最大的类别
    # -------------------------------------------------------
    cls_cinf = box[..., 5:]
    cls = []
    for i in range(len(cls_cinf)):
        cls.append(int(np.argmax(cls_cinf[i])))
    all_cls = list(set(cls))
    # -------------------------------------------------------
    #   分别对每个类别进行过滤
    #	1.将第6列元素替换为类别下标
    #	2.xywh2xyxy 坐标转换
    #	3.经过非极大抑制后输出的BOX下标
    #	4.利用下标取出非极大抑制后的BOX
    # -------------------------------------------------------
    output = []


    for i in range(len(all_cls)):
        curr_cls = all_cls[i]
        curr_cls_box = []
        curr_out_box = []
        for j in range(len(cls)):
            if cls[j] == curr_cls:
                box[j][5] = curr_cls
                curr_cls_box.append(box[j][:6])
        curr_cls_box = np.array(curr_cls_box)
        # curr_cls_box_old = np.copy(curr_cls_box)
        curr_cls_box = xywh2xyxy(curr_cls_box)
        curr_out_box = nms(curr_cls_box, iou_thres)
        for k in curr_out_box:
            output.append(curr_cls_box[k])
    output = np.array(output)

    return output


def draw(image, box_data):
    # -------------------------------------------------------
    #	取整，方便画框
    # -------------------------------------------------------
    #  image = cv2.flip(image, -1)
    boxes = box_data[..., :4].astype(np.int32)
    scores = box_data[..., 4]
    classes = box_data[..., 5].astype(np.int32)

    for box, score, cl in zip(boxes, scores, classes):
        top, left, right, bottom = box
        # print('class: {}, score: {}'.format(CLASSES[cl], score))
        # print('box coordinate left,top,right,down: [{}, {}, {}, {}]'.format(top, left, right, bottom))

        cv2.rectangle(image, (top, left), (right, bottom), (255, 0, 0), 2)
        cv2.putText(image, '{0} {1:.2f}'.format(CLASSES[cl], score),
                    (top, left),
                    cv2.FONT_HERSHEY_SIMPLEX,
                    0.6, (0, 0, 255), 2)



UserStruct=[
    [SimTypes.c_int8, 'vari82', 1],
    [SimTypes.c_int8, 'vari8', 1]
]
# 创建协议类
SIMPRO1 = SimpleProtocol()
# 创建用户数据结构体
user_data = SimTypes(UserStruct)
send_data = SimTypes(UserStruct)
ser = serial.Serial('/dev/ttyTHS1', 115200, timeout=1)  # 波特率为 115200 jetson nano
# ser = serial.Serial('COM36', 115200, timeout=1)  # 波特率为 115200 PC
s = []  # 创建一个空列表
listdata = []
rubish = ['电池', '水瓶', '砖头', '易拉罐', '萝卜', '白萝卜', '药品', '瓷片', '土豆', '鹅卵石', 'none']
# rubish_index = 10
con = 1


class loadmoudle(QThread):
    def __init__(self, my_signal, img, model, record_signal):
        super().__init__()
        self.my_signal = my_signal
        self.record_signal = record_signal
        self.sum_fg = 0
        self.image = img
        self.model = model
        self. rubbish_index = 10

    def run(self):
        while True:
            # time.sleep(0.01)
            self.sum_fg += 1
            send_data.vari8 = 10
            timef = 15  # 隔15帧保存一张图片
            self.rubbish_index = 10
            if self.sum_fg % timef == 0:
                try:
                    results, or_img = self.model.inference(self.image)
                except Exception as e:
                    print(e)
                    continue
                results = filter_box(results, 0.5, 0.5)
                self.my_signal.emit(list(results))
                print("inference", results)
                if results.shape[0] == 1:
                    send_data.vari8 = int(results[0][5])
                    self.rubbish_index =  int(results[0][5])
                    if(int(results[0][5])==1) and (results[0][4]<0.65):
                        send_data.vari8 = 10
                    # result_info.put(str(self.rubbish_index))
                    print(send_data.vari8)
                    # print(result_info.empty())
                    send_data_bytes = send_data.toBytes()
                    send_bytes = SIMPRO1.SimPro_Packet(send_data_bytes)
                    print(",send_data.vari8",send_data.vari8)
                    ser.write(send_bytes)
                result_info.put(str(self.rubbish_index))

record_rubbish = []

class Open_Camera(QtWidgets.QMainWindow, Ui_MainWindow):
    my_signal = pyqtSignal(list)
    record_signal = pyqtSignal(int)
    def __init__(self):
        super(Open_Camera, self).__init__()
        self.setupUi(self)  # 创建窗体对象
        self.T = None
        self.init()
        self.cap = cv2.VideoCapture()  # 初始化摄像头
        self.photo_flag = 0
        self.label.setScaledContents(False)  # 图片自适应
        self.label_2.setScaledContents(False)  # 图片自适应
        self.model_isLoad = False
        self.result = None
        self.tensor = None
        self.model_resnet = None
        self.sum_fg = 0

        self.record_status = [10, 10]



        iled_arr = np.tile(np.arange(640), 640).reshape((640,-1))
        # cap = cv2.VideoCapture(0)
        mask = np.zeros((*iled_arr.shape,3))
        center = (320,320)
        radius = 320
        mask = np.zeros((*iled_arr.shape,3))
        for i in range(iled_arr.shape[0]):
            mask_bool = ((iled_arr[i] - center[1])**2 + (i-center[0])**2) ** 0.5 <= radius
            mask_bool = np.tile(mask_bool,3).reshape((3,640))
            mask_bool = mask_bool.transpose([1,0])
            # m = np.where(mask_bool, [255,255,255], [0,0,0])
            m = np.where(mask_bool, [1,1,1], [0,0,0])
            mask[i] = m
        self.mask = mask.astype(np.uint8)
        # self.my_signal.connect(self.main_run_yolov5)
        self.my_signal.connect(self.get_detect_obj)
        self.detect_res = []
    def  get_detect_obj(self,result):
        print("get_detect_obj  ",np.array(result))
        self.detect_res =  result

    def init(self):
        # 定时器让其定时读取显示图片
        self.camera_timer = QTimer()
        self.read_timer = QTimer()
        self.camera_timer2 = QTimer()
        self.camera_timer.timeout.connect(self.show_image)
        self.camera_timer2.timeout.connect(self.open_vedio)
        # 打开摄像头
        self.pushButton_1.clicked.connect(self.open_camera)
        # 拍照
        self.pushButton_4.clicked.connect(self.taking_pictures)
        # 导入神经网络
        self.pushButton_2.clicked.connect(self.open_yolov5)
        # 关闭摄像头
        self.pushButton_3.clicked.connect(self.close_camera)
        # 打开视频
        self.pushButton_5.clicked.connect(self.open_vedio_connet)

    '''开启摄像头'''

    def open_camera(self):
        # self.cap = cv2.VideoCapture("5njf7-yoqfb.avi")  # 摄像头
        self.cap = cv2.VideoCapture(0)
        self.camera_timer.start(40)  # 每40毫秒读取一次，即刷新率为25帧

        self.show_image()

    '''显示图片'''

    def show_image(self):
        flag, self.image = self.cap.read()  # 从视频流中读取图片
      

            
        if not flag:
            return
        self.image =  self.image[:,80:560,:]
        self.image = cv2.resize(self.image, (640, 640))  # 把读到的帧的大小重新设置为 600*360
        self.image = (self.image * self.mask).astype(np.uint8)
        image_show =  self.image
        if not self.T is None:
            self.T.image = self.image

        if self.detect_res != []:
            draw(image_show,np.array(self.detect_res))
        # image_show = self.image
        width, height = image_show.shape[:2]  # 行:宽，列:高
        image_show = cv2.cvtColor(image_show, cv2.COLOR_BGR2RGB)  # opencv读的通道是BGR,要转成RGB
        image_show = cv2.flip(image_show, -1)  # 水平翻转，因为摄像头拍的是镜像的。
        # 把读取到的视频数据变成QImage形式(图片数据、高、宽、RGB颜色空间，三个通道各有2**8=256种颜色)
        self.showImage = QtGui.QImage(image_show.data, height, width, QImage.Format_RGB888)
        self.label.setPixmap(QPixmap.fromImage(self.showImage))  # 往显示视频的Label里显示QImage
        if not result_info.empty():
            res = int(result_info.get())
            self.record_status.append(res)
            print(self.record_status[-2])
            if self.record_status[-1] == 10 and self.record_status[-2] != 10:
                record_rubbish.append("已检测到垃圾信息:" + rubish[self.record_status[-2]] + "<br>")
                self.label_2.setText(''.join(record_rubbish))
                print("result is :", res)
            if len(self.record_status) >= 10:
                self.record_status = [10, 10]

        # if not record_info.empty():
        #     if int(record_info.get())!=10:
        #         record_rubbish.append("已检测到垃圾信息:<br>"+rubish[int(record_info.get())])
        #         print("from stm32", record_info.get())

        # print(rubish_index)
        # self.label.setScaledContents(True) #图片自适应

    '''拍照'''

    def taking_pictures(self):
        if self.cap.isOpened():
            FName = fr"images/cap{time.strftime('%Y%m%d%H%M%S', time.localtime())}"
            print(FName)
            self.label_2.setPixmap(QtGui.QPixmap.fromImage(self.showImage))
            # self.showImage.save(FName + ".jpg", "JPG", 100)
            self.showImage.save('./1.jpg')
        else:
            QMessageBox.critical(self, '错误', '摄像头未打开！')
            return None

    '''关闭摄像头'''

    def close_camera(self):
        self.camera_timer.stop()  # 停止读取
        self.cap.release()  # 释放摄像头
        self.label.clear()  # 清除label组件上的图片
        self.label_2.clear()  # 清除label组件上的图片
        self.label.setText("摄像头")
        # self.cap = cv2.VideoCapture(0, cv2.CAP_DSHOW)  # 摄像头

    # 导入图片
    def loadphoto(self):
        fname, _ = QFileDialog.getOpenFileName(self, '选择图片', '../', 'Image files(*.jpg *.gif *.png*.bmp)')
        self.showImage = fname
        self.label_2.setPixmap(QPixmap(self.showImage))
    # 加载神经网络
    def open_yolov5(self):
        self.camera_timer2.stop()  # 停止读取
        self.label_2.setText("正在加载中........")
        print("正在加载中........")
        # self.model = torch.hub.load(r'E:\download\resent', 'custom',
        #                             path=r'E:\download\resent\rub100.pt', source='local')
        self.model = YOLOV5('best.onnx')
        print(self.model)
        # self.model = torch.load("rub100.pt")
        self.label_2.setText("已加载神经网络")
        self.model_isLoad = True
        self.read_timer.start(40)
        self.main_run_yolov5()
        
        # print(" int(results[0][5])",rubish_index)
        self.T = loadmoudle(self.my_signal, self.image, self.model, self.record_signal)
        self.T.start()


    def main_run_yolov5(self):
        if self.model_isLoad:
            # bytes_data = ser.read_all()
            # SIMPRO1.SimPro_EnChar(bytes_data)  # 将收到的数据存如协议缓存章
            # send_bytes = SIMPRO1.SimPro_UnPacking()  # 使用协议打包字节串,得到要发送的字节串
            self.sum_fg += 1
            send_data.vari8 = 10
            timef = 15  # 隔15帧保存一张图片
            if self.sum_fg % timef == 0:
                results, or_img = self.model.inference(self.image)
                results = filter_box(results, 0.5, 0.5)
                print(results)
                print(results.shape[0])
                if results.shape[0] == 1:
                    send_data.vari8 = int(results[0][5])
            send_data_bytes = send_data.toBytes()
            send_bytes = SIMPRO1.SimPro_Packet(send_data_bytes)
            ser.write(send_bytes)
        
            # if len(send_bytes) > 0:   # 接收到数据
            #     print("成功解析出数据包 长度: %d  包序号: %d" % (len(send_bytes), SIMPRO1.recv_id))
            #     if len(send_bytes) == len(user_data):  # 和结构体长度匹配了，要检查否则用户发送别的长度的包导致长度不一致
            #         user_data.fromBytes(send_bytes)
            #         print("收到数据 ")
            #         print(f"varf32 {user_data.varf32s_1}")
            #         #if user_data.varf32s_1[0] <= 100:  # 检测到有物体靠近
            #         results, or_img = self.model.inference(self.image)
            #         results = filter_box(results, 0.2, 0.5)
            #         print(results)
    def open_vedio_connet(self):
        self.vedio=cv2.VideoCapture("ry31a-kvnuq.avi")
        self.camera_timer2.start(40)
        self.open_vedio()


    def open_vedio(self):
        flag, image = self.vedio.read()
        width, height = image.shape[:2]  # 行:宽，列:高
        showImage = QtGui.QImage(image.data, height, width, QImage.Format_RGB888)
        if flag:
            self.label_2.setPixmap(QPixmap.fromImage(showImage))  # 往显示视频的Label里显示QImage


if __name__ == '__main__':
    from PyQt5 import QtCore

    QtCore.QCoreApplication.setAttribute(QtCore.Qt.AA_EnableHighDpiScaling)  # 自适应分辨率

    app = QtWidgets.QApplication(sys.argv)
    ui = Open_Camera()

    ui.show()
    sys.exit(app.exec_())
    # model_resnet = torch.load("model_rubbish_1502023-08-14-18-02-20.pth")
    # print(model_resnet)
