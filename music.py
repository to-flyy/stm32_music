from machine import UART,Timer
from fpioa_manager import fm
import sensor,image,lcd,time
import KPU as kpu

#映射串口引脚
fm.register(6, fm.fpioa.UART1_RX, force=True)
fm.register(7, fm.fpioa.UART1_TX, force=True)

#串口初始化
uart = UART(UART.UART1, 9600, timeout=1000 , read_buf_len=4096)

#LCD初始化
lcd.init(freq=15000000)

#摄像头初始化
sensor.reset()
sensor.set_pixformat(sensor.RGB565)
sensor.set_framesize(sensor.QVGA)    #设置帧大小
sensor.set_vflip(1)                  #摄像头后置方式
sensor.run(1)                        #图像捕抓控制
sensor.set_auto_whitebal(False)
sensor.set_windowing((224, 224))     #设置LCD显示屏大小

classes = ['JW', 'FZU', 'ZJL']

task = kpu.load("/sd/model-165530.kmodel") #模型SD卡上

#网络参数
anchor = (6.72, 6.66, 6.84, 4.19, 6.78, 6.81, 6.91, 4.28, 6.94, 6.87)

a = kpu.init_yolo2(task, 0.75, 0.20, 5, anchor)

while(True):

    img = sensor.snapshot()
    code = kpu.run_yolo2(task, img) #运行yolo2网络

    if code:
        for i in code:
            a=img.draw_rectangle(i.rect())
            a = lcd.display(img)

            for i in code:
                lcd.draw_string(i.x(), i.y(), classes[i.classid()], lcd.BLUE, lcd.WHITE)
                lcd.draw_string(i.x(), i.y()+12, '{:.2f}'.format(i.value()), lcd.BLUE, lcd.WHITE)
                uart.write('@'+classes[i.classid()]+'\r\n')
                print('@'+classes[i.classid()]+'\r\n')

    else:
        a = lcd.display(img)

uart_A.deinit()             #注销串口
del uart_A
a = kpu.deinit(task)
