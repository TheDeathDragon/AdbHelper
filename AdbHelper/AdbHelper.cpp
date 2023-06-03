#include <iostream>
#include <windows.h>
#include <filesystem>
#include <vector>
#include <string>
#include <ctime>
#include <memory>
#include <stdexcept>
#include <array>
using namespace std;
namespace fs = std::filesystem;

bool dirExists(const std::string& path) {

	DWORD ftyp = GetFileAttributesA(path.c_str());

	if (ftyp == INVALID_FILE_ATTRIBUTES)
		return false;

	if (ftyp & FILE_ATTRIBUTE_DIRECTORY)
		return true;

	return false;
}

std::string exec(const char* cmd) {
	std::array<char, 128> buffer;
	std::string result;
	std::shared_ptr<FILE> pipe(_popen(cmd, "r"), _pclose);
	if (!pipe) {
		throw std::runtime_error("_popen() failed!");
	}
	while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
		result += buffer.data();
	}
	return result;
}

bool checkAdbConnect() {

	string tempString("");
	string cmdOutput("");
	tempString = "adb get-state";
	cmdOutput = exec(tempString.c_str());
	if (!cmdOutput.empty()) {
		cmdOutput.erase(cmdOutput.size() - 1);
		if (cmdOutput == "device") {
			return true;
		}
	}
	else {
		cout << "\n没找到连接的ADB设备，或者有多台ADB设备，请确保当前只有一台设备再操作！！\n" << endl;
		cout << "请打开设置，连续点击[关于手机]-[版本号]5次打开开发者模式，\n" << endl;
		cout << "然后[返回]-[系统]-[开发者选项]，进入开发者选项设置界面，\n" << endl;
		cout << "找到[调试]-[USB调试]，打开开关即可！\n" << endl;
		system("pause");
		system("cls");
	}
	return false;
}

int main()
{
	string apkPath("Apks");
	string apkExt(".apk");
	string apkCurrent("");
	string tempString("");
	string cmdOutput("");
	string powershell("C:\\Windows\\System32\\WindowsPowerShell\\v1.0\\powershell.exe");

	int choice = 0;
	int testCount = 0;

	// 获取当前时间点
	std::chrono::system_clock::time_point now = std::chrono::system_clock::now();

	// 将时间点转换为time_t，以便用于strftime()
	std::time_t now_time_t = std::chrono::system_clock::to_time_t(now);

	// 将time_t转换为本地时间
	std::tm local_tm;
	localtime_s(&local_tm, &now_time_t);

	// 格式化时间字符串
	char formatted_time[20];
	std::strftime(formatted_time, sizeof(formatted_time), "%Y%m%d_%H_%M", &local_tm);

	std::cout << "当前系统时间: " << formatted_time << std::endl;

	cout << "\n请先确保ADB能正常工作！(当前版本：" << __DATE__ << ")\n" << endl;

	while (true) {
	checkAdb:
		if (!checkAdbConnect()) goto checkAdb;
		cout << "\n便捷ADB小工具菜单：\n" << endl;
		cout << "0  > 退出程序" << endl;
		cout << "1  > 导出MTK DebugLog(/data/debuglogger/)" << endl;
		cout << "2  > 导出展讯 YLog(/data/ylog/)" << endl;
		cout << "3  > 导出相片、视频(/sdcard/DCIM/)" << endl;
		cout << "4  > 导出屏幕录像(/sdcard/Movies/)" << endl;
		cout << "5  > 导出屏幕截图(/sdcard/Pictures/)" << endl;
		cout << "6  > 查看当前Activity" << endl;
		cout << "7  > 一键Push文件到Sdcard目录，文件需放在Files中(当前目录自动创建)" << endl;
		cout << "8  > 一键批量安装APK，文件需放在Apks中(当前目录自动创建)" << endl;
		cout << "9  > 抓取ADB Logcat log(保存为system.log)" << endl;
		cout << "10 > 抓取屏幕截图" << endl;
		cout << "11 > 循环拍照测试" << endl;
		cout << "12 > 导出电池曲线(/sdcard/BatteryLog.csv)" << endl;
		cout << "13 > 查看手机上安装的第三方软件" << endl;
		cout << "14 > 查看设备FingerPrint" << endl;
		cout << "15 > 查看设备屏幕分辨率和DPI" << endl;
		cout << "16 > 抓取Kernel Log(保存为dmesg.log)" << endl;
		cout << "17 > 结束Monkey测试" << endl;
		cout << "18 > 亮灭屏测试" << endl;
		cout << "19 > 触发展讯SystemDump" << endl;
		cout << "20 > 导出SD卡中的YLog(/storage/SD卡ID/ylog)" << endl;
		cout << "21 > 导出内部存储中的MtkLog(adb pull /sdcard/debuglogger)" << endl;

		cout << "\n请选择 > ";
		cin >> choice;
		cout << endl;
		switch (choice)
		{
		case 0:
			exit(0);
		case 1:
			tempString = "adb pull /data/debuglogger/ debuglogger_";
			tempString += formatted_time;
			system(tempString.c_str());
			tempString = "";
			break;
		case 2:
			tempString = "adb pull /data/ylog/ ylog_";
			tempString += formatted_time;
			system(tempString.c_str());
			tempString = "";
			break;
		case 3:
			system("adb pull /sdcard/DCIM/");
			break;
		case 4:
			system("adb pull /sdcard/Movies/");
			break;
		case 5:
			system("adb pull /sdcard/Pictures/");
			break;
		case 6:
			system("adb shell \"dumpsys window | grep mCurrentFocus\"");
			break;
		case 7:
			if (!dirExists("Files")) {
				system("mkdir Files");
			}
			cout << "请把要Push的文件都放到Files文件夹里面，按任意键开始Push！\n" << endl;
			system("pause");
			system("cd .\\Files\\ & adb push . /sdcard/");
			break;
		case 8:
			if (!dirExists("Apks")) {
				system("mkdir Apks");
			}
			cout << "请把安装包文件都放到Apks文件夹里面！\n" << endl;
			system("pause");
			for (auto& p : fs::recursive_directory_iterator(apkPath))
			{
				if (p.path().extension() == apkExt) {
					apkCurrent = p.path().stem().string() + apkExt;
					cout << "当前正在安装：" << apkCurrent << endl;
					tempString = "adb install -r -g .\\" + apkPath + "\\" + apkCurrent;
					system(tempString.c_str());
					tempString = "";
				}
			}
			break;
		case 9:
			cout << "日志文件保存为system.log，按Ctrl+C结束" << endl;
			tempString = powershell + " \"adb logcat -b all 2>&1 | tee system.log\"";
			system(tempString.c_str());
			tempString = "";
			break;
		case 10:
			cout << "抓取的截图保存在当前文件夹" << endl;
			system("adb shell /system/bin/screencap -p /sdcard/Pictures/screenshot.png");
			Sleep(1 * 1000);
			system("adb pull /sdcard/Pictures/screenshot.png");
			break;
		case 11:
			cout << "请输入要测试拍照的次数：";
			cin >> testCount;
			if (testCount <= 0) {
				cout << "输入有误！" << endl;
				break;
			}
			system("adb shell am start com.mediatek.camera/com.mediatek.camera.CameraLauncher");
			Sleep(3 * 1000);
			for (int i = 0; i < testCount; i++)
			{
				cout << "拍照次数：" << i + 1 << endl;
				system("adb shell input keyevent 27");
				Sleep(1 * 1000);
			}
			break;
		case 12:
			system("adb pull /sdcard/BatteryLog.csv");
			break;
		case 13:
			system("adb shell pm list packages -3");
			break;
		case 14:
			system("adb shell \"getprop | grep ro.vendor.build.fingerprint\"");
			system("adb shell \"getprop | grep product.product\"");
			break;
		case 15:
			system("adb shell wm size");
			system("adb shell wm density");
			break;
		case 16:
			cout << "日志文件保存为dmesg.log，按Ctrl+C结束" << endl;
			tempString = powershell + " \"adb shell 'dmesg -w' 2>&1 | tee dmesg.log\"";
			system(tempString.c_str());
			tempString = "";
			break;
		case 17:
			tempString = "adb shell \"ps - A | grep monkey | awk '{print $2}' | xargs kill - 9\"";
			system(tempString.c_str());
			tempString = "";
			break;
		case 18:
			cout << "请输入要测试亮灭屏的次数：";
			cin >> testCount;
			if (testCount <= 0) {
				cout << "输入有误！" << endl;
				break;
			}
			Sleep(3 * 1000);
			for (int i = 0; i < testCount; i++)
			{
				cout << "亮灭屏次数：" << i + 1 << endl;
				system("adb shell input keyevent KEYCODE_POWER");
				Sleep(1 * 1000);
			}
			break;
		case 19:
			cout << "请确保在进行SystemDump之前，插入了SD卡并且可用空间大于3G！\n" << endl;
			system("adb root");
			system("pause");
			system("adb shell \"echo 'c' > /proc/sysrq-trigger\"");
			cout << "\n已经下发指令给设备，设备将会重启进入SystemDump模式！" << endl;
		case 20:
			tempString = "adb shell \"sm list-volumes public | cut -d' ' -f3\"";
			cmdOutput = exec(tempString.c_str());
			if (!cmdOutput.empty()) {
				cmdOutput.erase(cmdOutput.size() - 1);
				cout << "SD卡的ID：\n" << cmdOutput << endl;
				tempString = "adb pull /storage/" + cmdOutput + "/ylog";
				cout << "[" << tempString << "]" << endl;
				system(tempString.c_str());
			}
			else {
				cout << "没找到SD卡，请查看SD卡是否正确插入？" << endl;
				continue;
			}
			break;
		case 21:
			tempString = "adb pull /sdcard/debuglogger/ debuglogger_";
			tempString += formatted_time;
			system(tempString.c_str());
			tempString = "";
			break;
		default:
			break;
		}
		cout << endl;
		system("pause");
		system("cls");
	}
}
