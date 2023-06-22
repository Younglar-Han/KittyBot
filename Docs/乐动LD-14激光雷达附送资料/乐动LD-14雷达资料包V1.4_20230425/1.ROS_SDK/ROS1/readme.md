①编译方法：使用catkin编译，执行如下操作
将功能包解压后复制到工作空间的路径下，然后执行指令“catkin_make”进行编译。

②设备别名：端口设备重命名
launch中启动的LD14雷达默认设备名为：/dev/LD14，别名文件是“ldlidar14_udev.sh”，
如果您使用的ttl电平转换芯片为CP2102，设备号需要改为0001；
如果您使用的ttl电平转换芯片为CH9102设备号则需要改为0003。
具体修改方法请分别查看对应的驱动资料。

③运行方法
source devel/setup.bash
roslaunch ldlidar ld14.launch

④rviz可视化查看点云：
开启rviz，rviz的基坐标选择laser，选择“laserscan”的可视化类型，话题选择“scan”。

## 测试：代码在ubuntun18.04melodic版本下测试，使用rviz可视化。
