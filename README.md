# OneNet HTTP Demo on PlatformIO

> 正在开发中

该示例用于演示如何使用 OneNet 进行各种功能测试。

项目基于 OneNet 开发板，使用 PlatformIO 进行构建。搭载芯片为 STM32F103C8T6，使用 ESP8266 作为 WiFi（802.11）模块、M6312 作为 2G 通信芯片。

> [OneNet 官网](https://open.iot.10086.cn/) | [OneNet API](https://open.iot.10086.cn/doc) | [OneNet 控制台](https://open.iot.10086.cn/console.html)

## 1. 硬件准备

需要 OneNet 标准版开发板。

请在拿到板子后，检查 M6312/ESP8266 模块的跳线板，确保连接的是 M6312/ESP8266 一侧，否则无法正常使用 2G 蜂窝数据通信 / WiFi（802.11）功能。如果使用 M6312，请额外自备一张 USIM 实体卡（建议购买专用的物联网卡）。

有条件可以额外连一个 1602LCD，用于显示设备的状态（记得额外加电源），以及找一根 MicroUSB 数据线连接电脑、通过虚拟串口查看设备状态。

## 2. 软件准备

- [PlatformIO](https://platformio.org/)（[PlatformIO离线安装终极攻略](https://blog.csdn.net/kimiyang123/article/details/123726847)）
- [VSCode](https://code.visualstudio.com/)
- [Git](https://git-scm.com/)
- [STLink 驱动](https://www.st.com/en/development-tools/stsw-link009.html)（或者也可以使用其它烧写器，需自行更改构建参数）

## 3. 烧写

1. 克隆本仓库到本地
2. 使用 VSCode，以 PlatformIO 项目的形式打开本仓库，将烧写器连接开发板与电脑
3. 修改 `platformio.ini` 中的 `upload_protocol` 为自己的烧写设备，并修改 `.vscode/c_cpp_properties.json` 的有关 PlatformIO Core SDK的路径（可以通过插件配置或环境变量`PLATFORMIO_CORE_DIR`（如果人工设置）确认路径）
4. 修改 `src/main.cpp` 中的 `DEVICE_ID` 和 `API_KEY` 为自己的设备 ID 和 API Key，这两个数值可以从 OneNet 的**旧版**控制台获取
5. 修改 `src/main.cpp` 中的 `WIFI_SSID` 和 `WIFI_PWD` 为自己的 WiFi 的 SSID 和密码
6. 在左侧的 PlatformIO 窗口中，点击 `Build`，然后点击 `Upload`
