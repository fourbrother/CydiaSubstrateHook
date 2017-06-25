#include <jni.h>
#include "substrate.h"
#include <android/log.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <string.h>
#include <sys/stat.h>
#include <stdlib.h>
#include "DexFile.h"

#define TAG "jw"
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, TAG, __VA_ARGS__)
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, TAG, __VA_ARGS__)

const char* ex0 = "re-initialized>";
const char* ex1 = "zygote";
const char* ex2 = "app_process";
const char* ex3 = "/system/bin/dexopt";
const char* ex4 = "com.google.android.gms";
const char* ex5 = "com.google.android.gms.persistent";
const char* ex6 = "com.google.process.gapps";
const char* ex7 = "com.google.android.gms.wearable";
const char* ex8 = "com.android.phone";
const char* ex9 = "com.android.systemui";
const char* ex10 = "com.google.android.gms.unstable";
const char* ex11 = "android.process.acore";
const char* ex12 = "android.process.media";
const char* ex13 = "dexopt";

#define BUF_SIZE 1024

MSConfig(MSFilterLibrary, "/system/lib/libdvm.so");

const char* workDir = "/sdcard/hookdex/";

void getNameByPid(pid_t pid, char *task_name) {
	char proc_pid_path[BUF_SIZE];
	char buf[BUF_SIZE];
	sprintf(proc_pid_path, "/proc/%d/status", pid);
	FILE* fp = fopen(proc_pid_path, "r");
	if(NULL != fp){
		if(fgets(buf, BUF_SIZE-1, fp) == NULL){
			fclose(fp);
		}
		fclose(fp);
		sscanf(buf, "%*s %s", task_name);
	}
}

int exclude(char* s){
	int i = !strcmp(s,ex0)||!strcmp(s,ex1)||!strcmp(s,ex2)||!strcmp(s,ex3)||
			!strcmp(s,ex4)||!strcmp(s,ex5)||!strcmp(s,ex6)||!strcmp(s,ex7)||
			!strcmp(s,ex8)||!strcmp(s,ex9)||!strcmp(s,ex10)||!strcmp(s,ex11)||
			!strcmp(s,ex12)||!strcmp(s,ex13);
	return i;
}

int checkDir()
{
	mode_t myMode = 777 ;
	if(0 == access(workDir,0)) {//目录存在
		return 0;
	} else{
		if(0 == mkdir(workDir,myMode)) {
			return 0;
		}
		else {
			return 1;
		}
	}
}

DexFile* (*oldDexFileParse)(const u1* data, size_t length, int flags);

DexFile* newDexFileParse(const u1* addr, size_t len, int dvmdex)
{

	char buf[200];
	char pname[50];
	pid_t pid = getpid();
	getNameByPid(pid, pname);

	if(exclude(pname)){
		LOGI("exclude process:%s", pname);
		return oldDexFileParse(addr, len, dvmdex);
	}

	LOGD("call dvm dex pid:%d,pname:%s", pid, pname);

	sprintf(buf,"/sdcard/hookdex/%s_%d.dex", pname, pid);
	FILE* file = fopen(buf, "wb");
	if(!file){
		LOGD("error open sdcard file to write");
	}else{
		fwrite(addr, 1, len, file);
		fclose(file);
		LOGD("write dex:%s len=%d succ!", buf, len);
	}

	//进行原来的调用，不影响程序运行
	return oldDexFileParse(addr,len,dvmdex);

}

int (*_oldDexFileOpen)(const void* addr, int len, void** dvmdex);

MSInitialize{
	LOGD("Substrate initialized.");
	//创建目录
	if(checkDir()){
		LOGD("create dir err...");
		return;
	}else{
		LOGD("create dir succ...");
	}
	MSImageRef image = MSGetImageByName("/system/lib/libdvm.so");
	if (image != NULL){
		void * dexload = MSFindSymbol(image, "_Z12dexFileParsePKhji");
		if(dexload == NULL){
			LOGD("error find _Z12dexFileParsePKhji");
		}else{
			MSHookFunction(dexload, (void*)&newDexFileParse, (void **)&oldDexFileParse);
		}
	}else{
		LOGD("ERROR FIND LIBDVM");
	}
}
