@echo off
echo 测试与PN532的通信及读卡，测试前请正确连接好电源和串口，TXD和RXD必须交叉连接，要开始测试请按任意键继续
pause
:again
echo 正在读卡
nfc-list.exe --device PN532_UART:COM7:115200
pause
goto again
