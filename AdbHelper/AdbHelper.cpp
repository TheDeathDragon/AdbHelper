#include <iostream>
#include <windows.h>
#include <filesystem>
#include<vector>
#include<string>
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

int main()
{
	string apkPath("Apks");
	string apkExt(".apk");
	string apkCurrent("");
	string tempString("");
	string powershell("C:\\Windows\\System32\\WindowsPowerShell\\v1.0\\powershell.exe");

	int choice = 0;
	int cameraShotCount = 0;

	cout << "请先确保ADB能正常工作！(当前版本：" << __DATE__ <<")" << endl;

	while (true) {
		cout << "\n便捷ADB小工具菜单：\n" << endl;
		cout << "0  > 退出程序" << endl;
		cout << "1  > 导出DebugLog(/data/debuglogger/)" << endl;
		cout << "2  > 导出DebugLog(/sdcard/debuglogger/)" << endl;
		cout << "3  > 导出相片、视频(/sdcard/DCIM/)" << endl;
		cout << "4  > 导出屏幕录像(/sdcard/Movies/)" << endl;
		cout << "5  > 导出屏幕截图(/sdcard/Pictures/)" << endl;
		cout << "6  > 查看当前Activity" << endl;
		cout << "7  > 一键Push文件到Sdcard目录，文件需放在Files中" << endl;
		cout << "8  > 一键批量安装APK，文件需放在Apks中" << endl;
		cout << "9  > 抓取ADBLog(保存为system.log)" << endl;
		cout << "10 > 抓取屏幕截图" << endl;
		cout << "11 > 循环拍照测试" << endl;
		cout << "12 > 导出电池曲线(/sdcard/BatteryLog.csv)" << endl;
		cout << "13 > 查看手机上安装的第三方软件" << endl;
		cout << "14 > 查看设备FingerPrint" << endl;
		cout << "15 > 查看设备屏幕分辨率和DPI" << endl;
		cout << "16 > 抓取Kernel Log(保存为dmesg.log)" << endl;

		cout << "\n请选择 > ";
		cin >> choice;
		cout << endl;
		switch (choice)
		{
		case 0:
			exit(0);
		case 1:
			system("adb pull /data/debuglogger/");
			break;
		case 2:
			system("adb pull /sdcard/debuglogger/");
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
			cout << "请把要Push的文件都放到Files文件夹里面，按回车开始Push！" << endl;
			getchar();
			getchar();
			system("cd .\\Files\\ & adb push . /sdcard/");
			break;
		case 8:
			if (!dirExists("Apks")) {
				system("mkdir Apks");
			}
			cout << "请把安装包文件都放到Apks文件夹里面，按回车开始安装！" << endl;
			getchar();
			getchar();
			for (auto& p : fs::recursive_directory_iterator(apkPath))
			{
				if (p.path().extension() == apkExt) {
					apkCurrent = p.path().stem().string() + apkExt;
					cout << "当前正在安装：" << apkCurrent << endl;
					tempString = "adb install -r .\\" + apkPath + "\\" + apkCurrent;
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
			cin >> cameraShotCount;
			if (cameraShotCount <= 0) {
				cout << "输入有误！" << endl;
				break;
			}
			system("adb shell am start com.mediatek.camera/com.mediatek.camera.CameraLauncher");
			Sleep(3 * 1000);
			for (int i = 0; i < cameraShotCount; i++)
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
		default:
			break;
		}
		cout << "\n按回车继续..." << endl;
		getchar();
		getchar();
	}
}
