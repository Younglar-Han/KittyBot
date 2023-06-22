①编译方法：使用catkin编译，执行如下操作
将功能包解压后复制到工作空间的路径下，然后执行指令“colcon build ”进行编译。

②设备别名：端口设备重命名
launch中启动的LD14雷达默认设备名为：/dev/LD14，别名文件是“ldlidar14_udev.sh”，
如果您使用的ttl电平转换芯片为CP2102，设备号需要改为0001；
如果您使用的ttl电平转换芯片为CH9102设备号则需要改为0003。
具体修改方法请分别查看对应的驱动资料。

③运行方法
source install/setup.bash
ros2 launch ld14 ld14.launch.py

④rviz可视化查看点云：
rviz的配置在功能包路径下的rviz文件中。

## 测试：代码在ubuntun20.04foxy版本下测试，使用rviz可视化。
