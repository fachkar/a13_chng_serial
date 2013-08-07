#include <stdio.h>
#include <stdlib.h>
#include <linux/string.h>
#include <unistd.h>
#include <cups/cups.h>
#include <curl/curl.h>
using namespace std;

char * mac_add = NULL;

bool issue_adb_command(char * command)
{
    FILE *fpipe;
    char line[256];

    memset(line,0, sizeof line);
    //printf("now executing command %s\n", command);
    if ( !(fpipe = (FILE*)popen(command,"r")) )
    {  // If fpipe is NULL
        printf("Problems with adb shell command 4 %s\n", command);
        return false;
    }

    while ( fgets( line, sizeof line, fpipe))
    {
        if (strlen(line) > 0 && strstr(command,"push") == NULL && strstr(command,"am broadcast") == NULL && strstr(command,"rm /databk/data_backup.tar") == NULL) {
            printf("\n *** Error please note down the message below *** 4 %s\n",command);
            return false;
        }
        printf("%s", line);

    }
    pclose(fpipe);

    return true;
}

bool issue_split_command(char * command)
{
    FILE *fpipe;
    char line[256];
    bool krnl_extract_problm = true;

    memset(line,0, sizeof line);
    //printf("now executing command %s\n", command);
    if ( !(fpipe = (FILE*)popen(command,"r")) )
    {  // If fpipe is NULL
        printf("Problems with adb shell command 4 %s\n", command);
        return false;
    }

    while ( fgets( line, sizeof line, fpipe))
    {
        if (strlen(line) > 0 && strstr(line,"Writing nandc.img-kernel ... complete.") != NULL) {
            pclose(fpipe);
            return true;
        } else {
            krnl_extract_problm = false;
        }
    }
    pclose(fpipe);
    return krnl_extract_problm;
}


bool issue_mac_command(char * command)
{
    FILE *fpipe;
    char line[256];

    memset(line,0, sizeof line);

    if ( !(fpipe = (FILE*)popen(command,"r")) )
    {  // If fpipe is NULL
        printf("Problems with adb shell command 4 %s\n", command);
        return false;
    }

    char * pch;
    fgets( line, sizeof line, fpipe);
    pch = strstr(line, "00:");
    if (pch) {
        asprintf(&mac_add, "%s", pch);
    } else {
        printf("\n\n *** please enable Wifi on the tablet and try again ***\n\n");
        return false;
    }
    pclose(fpipe);

    return true;
}

struct MemoryStruct {
    char *memory;
    size_t size;
};

static size_t
WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    size_t realsize = size * nmemb;
    struct MemoryStruct *mem = (struct MemoryStruct *)userp;

    mem->memory = (char*)realloc(mem->memory, mem->size + realsize + 1);
    if (mem->memory == NULL) {
        /* out of memory! */
        printf("not enough memory (realloc returned NULL)\n");
        exit(EXIT_FAILURE);
    }

    memcpy(&(mem->memory[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->memory[mem->size] = 0;

    return realsize;
}

static int query_ttyUSB(char * command)
{
    int ret = -1;
    FILE *fpipe = NULL;
    char line[256];

    if(command == NULL)
        return ret;
    memset(line,0, sizeof line);
    if ( !(fpipe = (FILE*)popen(command,"r")) )
    {  // If fpipe is NULL
        ret = -1;
        return ret;
    }

    ret =0;
    while ( fgets( line, sizeof line, fpipe))
    {
        ret++;
    }
    pclose(fpipe);

    return ret;
}


int main(int argc, char **argv) {
    puts(" >> starting ..");

    char *serialno;
    char *command;
    FILE * pFile;

    char* response;
    char *postrequest;
    CURL *curl;
    CURLcode res;
    struct MemoryStruct chunk;

    chunk.memory = (char*)malloc(1);
    chunk.size = 0;
    if (asprintf(&command, "ls %s*", "/dev/ttyUSB") < 0) {
        printf("Problems allocating adb command @ %d\n", __LINE__);
        return -1;
    }
    int nttyUSB = query_ttyUSB(command);
    printf("nttyUSB:%d\n", nttyUSB);
    if(command)
        free(command);
    return 0;

    system("adb shell 'echo \"halt\" > /sys/power/wake_lock'");

    if (asprintf(&command, "%s", "adb shell 'cat /dev/block/nandc > /mnt/sdcard/nandc.img'") < 0) {
        printf("Problems allocating adb command @ %d\n", __LINE__);
        return -1;
    }
    if (!issue_adb_command(command)) {
        printf("issue_adb_command issue command @ %d\n", __LINE__);
        return -1;
    }
    free(command);


    if (asprintf(&command, "%s", "adb pull /mnt/sdcard/nandc.img") < 0) {
        printf("Problems allocating adb command @ %d\n", __LINE__);
        return -1;
    }
    if (!issue_adb_command(command)) {
        printf("issue_adb_command issue command @ %d\n", __LINE__);
        return -1;
    }
    free(command);


    if (asprintf(&command, "%s", "./split_bootimg.pl nandc.img") < 0) {
        printf("Problems allocating adb command @ %d\n", __LINE__);
        return -1;
    }
    if (!issue_split_command(command)) {
        printf("Programming has to stop .. @ %d\n", __LINE__);
        return -1;
    }
    free(command);


    printf("now checking wifi MAC\n");
    if (asprintf(&command, "%s", "adb shell netcfg | grep wlan0") < 0) {
        printf("Problems allocating adb command @ %d\n", __LINE__);
        return -1;
    }
    if (!issue_mac_command(command)) {
        printf("MAC Address issue command @ %d\n", __LINE__);
        return -1;
    }
    free(command);

    asprintf(&postrequest, "%s%s", "clientid=ubislate&clientkey=d@t@w1nd&action=getserialid&macid=", mac_add);
    curl_global_init(CURL_GLOBAL_ALL);
    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, "https://support.datawind-s.com/progserver/touchrequest.jsp");
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postrequest);

        curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, WriteMemoryCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEHEADER, (void *)&chunk);
        curl_easy_setopt(curl, CURLOPT_USERAGENT, "libcurl-agent/1.0");

        res = curl_easy_perform(curl);

        if (res != CURLE_OK) {
            printf("failed to connect to server %s\n", curl_easy_strerror(res));
            curl_easy_cleanup(curl);
            curl_global_cleanup();

            if (chunk.memory)
                free(chunk.memory);
            return -1;
        }
        printf("%lu bytes retrieved\n", (long)chunk.size);
//         printf("recieved:%s\n", chunk.memory);
        asprintf(&response,"%s",chunk.memory);
        char * pdw_messageStart = strstr(response, "dw-message:");
        if (pdw_messageStart != NULL) {
            char * pdw_messageEnd = strstr(pdw_messageStart + 0x0c, "\r\n");
            if (pdw_messageEnd != NULL) {
                char *dw_message = (char*) malloc(strlen(pdw_messageStart) - strlen(pdw_messageEnd));

                if (dw_message != NULL) {
                    memset(dw_message,0, strlen(pdw_messageStart) - strlen(pdw_messageEnd));
                    strncpy(dw_message, pdw_messageStart + 0x0c, strlen(pdw_messageStart) - strlen(pdw_messageEnd) - 0x0c );
                    if (strcmp(dw_message, "Success.")== 0) {
                        char * pdw_serialNoStart = strstr(pdw_messageEnd+ 0x2, "dw-serialid:");
                        if (pdw_serialNoStart != NULL) {
                            char * pdw_serialNoEnd = strstr(pdw_serialNoStart + 0x0d, "\r\n");
                            if (pdw_serialNoEnd != NULL) {
                                char* dw_serialno = (char*) malloc(strlen(pdw_serialNoStart) - strlen(pdw_serialNoEnd));
                                if (dw_serialno != NULL) {
                                    memset(dw_serialno,0, strlen(pdw_serialNoStart) - strlen(pdw_serialNoEnd));
                                    strncpy(dw_serialno, pdw_serialNoStart + 0x0d, strlen(pdw_serialNoStart) - strlen(pdw_serialNoEnd) - 0x0d );
                                    asprintf(&serialno,"%s",dw_serialno);
                                    puts(serialno);
                                    free(dw_serialno);
                                } else {
                                    printf("wrong server response\n %s\n",response);
                                    curl_easy_cleanup(curl);
                                    curl_global_cleanup();

                                    if (chunk.memory)
                                        free(chunk.memory);
                                    return -1;
                                }
                            } else {
                                printf("wrong server response\n %s\n",response);
                                curl_easy_cleanup(curl);
                                curl_global_cleanup();

                                if (chunk.memory)
                                    free(chunk.memory);
                                return -1;
                            }

                        } else {
                            printf("wrong server response\n %s\n", response);
                            curl_easy_cleanup(curl);
                            curl_global_cleanup();

                            if (chunk.memory)
                                free(chunk.memory);
                            return -1;
                        }

                    } else {
                        /* we got error from the server */
                        printf(" *** %s *** \n\n", dw_message);
                        free(dw_message);
                        curl_easy_cleanup(curl);
                        curl_global_cleanup();

                        if (chunk.memory)
                            free(chunk.memory);
                        return -1;
                    }

                    free(dw_message);
                } else {
                    printf("wrong server response\n %s\n", response);
                    curl_easy_cleanup(curl);
                    curl_global_cleanup();

                    if (chunk.memory)
                        free(chunk.memory);
                    return -1;
                }
            } else {
                printf("wrong server response\n %s\n", response);
                curl_easy_cleanup(curl);
                curl_global_cleanup();

                if (chunk.memory)
                    free(chunk.memory);
                return -1;
            }

        } else {
            printf("wrong server response\n %s\n", response);
            curl_easy_cleanup(curl);
            curl_global_cleanup();

            if (chunk.memory)
                free(chunk.memory);
            return -1;
        }

        curl_easy_cleanup(curl);

    } else {
        printf("failed to connect to server\n %s\n", response);
        curl_global_cleanup();
        if (chunk.memory)
            free(chunk.memory);
        return -1;
    }
    curl_global_cleanup();

    if (chunk.memory)
        free(chunk.memory);

    if (serialno == NULL) {
        printf(" *** failed to get serial number from server *** \n");
        return -1;
    }
//     printf("serialno:%s\n", serialno);
//     /// code stops here
//     return 0;

//     pFile = fopen ("./macs.html","w");
//     if (pFile!=NULL)
//     {
//         fputs ("<?xml version=\"1.0\"?><!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.1 Transitional//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\"><html xmlns=\"http://www.w3.org/1999/xhtml\"  xml:lang=\"en\" lang=\"en\"><head><style type=\"text/css\" title=\"Default-Style\">body {margin: 0em 0em;}</style><meta http-equiv=\"Default-Style\" content=\"Default-Style\" /><meta http-equiv=\"content-type\" content=\"text/html; charset=iso-8859-1\" /> </head> <body>" ,pFile);
//         fputs (argv[1],pFile);
//         fputs ("<br><br><br><br>",pFile);
//         fputs (mac_add,pFile);
//         fputs ("</body></html>",pFile);
//         fclose (pFile);
//     }
//
//
//
//     if (asprintf(&command, "%s %s %s", "html2ps", "./macs.html", "| lpr") < 0) {
//         printf("Problems allocating adb command @ %d\n", __LINE__);
//         return -1;
//     }
//     if (!issue_adb_command(command)) {
//         printf("MAC Address issue command @ %d\n", __LINE__);
//         return -1;
//     }
//     free(command);


    printf("now starting App backup quick phase 1\n");
    if (asprintf(&command, "%s", "adb pull /init ./case2ramdisk/init") < 0) {
        printf("Problems allocating adb command @ %d\n", __LINE__);
        return -1;
    }
    if (!issue_adb_command(command)) {
        printf("issue_adb_command issue command @ %d\n", __LINE__);
        return -1;
    }
    free(command);

    if (asprintf(&command, "%s", "adb pull /init.rc ./case2ramdisk/init.rc") < 0) {
        printf("Problems allocating adb command @ %d\n", __LINE__);
        return -1;
    }
    if (!issue_adb_command(command)) {
        printf("issue_adb_command issue command @ %d\n", __LINE__);
        return -1;
    }
    free(command);

    if (asprintf(&command, "%s", "adb pull /init.goldfish.rc ./case2ramdisk/init.goldfish.rc") < 0) {
        printf("Problems allocating adb command @ %d\n", __LINE__);
        return -1;
    }
    if (!issue_adb_command(command)) {
        printf("issue_adb_command issue command @ %d\n", __LINE__);
        return -1;
    }
    free(command);

    if (asprintf(&command, "%s", "adb pull /init.sun5i.rc ./case2ramdisk/init.sun5i.rc") < 0) {
        printf("Problems allocating adb command @ %d\n", __LINE__);
        return -1;
    }
    if (!issue_adb_command(command)) {
        printf("issue_adb_command issue command @ %d\n", __LINE__);
        return -1;
    }
    free(command);

    if (asprintf(&command, "%s", "adb pull /init.sun5i.usb.rc ./case2ramdisk/init.sun5i.usb.rc") < 0) {
        printf("Problems allocating adb command @ %d\n", __LINE__);
        return -1;
    }
    if (!issue_adb_command(command)) {
        printf("issue_adb_command issue command @ %d\n", __LINE__);
        return -1;
    }
    free(command);

    if (asprintf(&command, "%s", "adb pull /ueventd.goldfish.rc ./case2ramdisk/ueventd.goldfish.rc") < 0) {
        printf("Problems allocating adb command @ %d\n", __LINE__);
        return -1;
    }
    if (!issue_adb_command(command)) {
        printf("issue_adb_command issue command @ %d\n", __LINE__);
        return -1;
    }
    free(command);

    if (asprintf(&command, "%s", "adb pull /ueventd.rc ./case2ramdisk/ueventd.rc") < 0) {
        printf("Problems allocating adb command @ %d\n", __LINE__);
        return -1;
    }
    if (!issue_adb_command(command)) {
        printf("issue_adb_command issue command @ %d\n", __LINE__);
        return -1;
    }
    free(command);

    if (asprintf(&command, "%s", "adb pull /ueventd.sun5i.rc ./case2ramdisk/ueventd.sun5i.rc") < 0) {
        printf("Problems allocating adb command @ %d\n", __LINE__);
        return -1;
    }
    if (!issue_adb_command(command)) {
        printf("issue_adb_command issue command @ %d\n", __LINE__);
        return -1;
    }
    free(command);


    printf("now starting App backup *slow* phase 2\n");
    if (asprintf(&command, "%s", "adb push ./preinstall.sh /system/bin/") < 0) {
        printf("Problems allocating adb command @ %d\n", __LINE__);
        return -1;
    }
    if (!issue_adb_command(command)) {
        printf("issue_adb_command issue command @ %d\n", __LINE__);
        return -1;
    }
    free(command);

    if (asprintf(&command, "%s", "adb shell chmod 750 /system/bin/preinstall.sh") < 0) {
        printf("Problems allocating adb command @ %d\n", __LINE__);
        return -1;
    }
    if (!issue_adb_command(command)) {
        printf("issue_adb_command issue command @ %d\n", __LINE__);
        return -1;
    }
    free(command);

    if (asprintf(&command, "%s", "adb shell mkdir /databk") < 0) {
        printf("Problems allocating adb command @ %d\n", __LINE__);
        return -1;
    }
    if (!issue_adb_command(command)) {
        printf("issue_adb_command issue command @ %d\n", __LINE__);
        return -1;
    }
    free(command);


    if (asprintf(&command, "%s", "adb shell mount -t ext4 /dev/block/nandi /databk") < 0) {
        printf("Problems allocating adb command @ %d\n", __LINE__);
        return -1;
    }
    if (!issue_adb_command(command)) {
        printf("issue_adb_command issue command @ %d\n", __LINE__);
        return -1;
    }
    free(command);


    if (asprintf(&command, "%s", "adb shell rm /databk/data_backup.tar") < 0) {
        printf("Problems allocating adb command @ %d\n", __LINE__);
        return -1;
    }
    if (!issue_adb_command(command)) {
        printf("issue_adb_command issue command @ %d\n", __LINE__);
        return -1;
    }
    free(command);


    if (asprintf(&command, "%s", "adb shell sync") < 0) {
        printf("Problems allocating adb command @ %d\n", __LINE__);
        return -1;
    }
    if (!issue_adb_command(command)) {
        return -1;
    }
    free(command);


    if (asprintf(&command, "%s", "adb push ./case2_data_backup.tar /databk/data_backup.tar") < 0) {
        printf("Problems allocating adb command @ %d\n", __LINE__);
        return -1;
    }
    if (!issue_adb_command(command)) {
        return -1;
    }
    free(command);


    if (asprintf(&command, "%s", "adb shell sync") < 0) {
        printf("Problems allocating adb command @ %d\n", __LINE__);
        return -1;
    }
    if (!issue_adb_command(command)) {
        return -1;
    }
    free(command);

    if (asprintf(&command, "%s", "adb shell umount /databk") < 0) {
        printf("Problems allocating adb command @ %d\n", __LINE__);
        return -1;
    }
    if (!issue_adb_command(command)) {
        return -1;
    }
    free(command);

    if (asprintf(&command, "%s", "adb shell rmdir /databk") < 0) {
        printf("Problems allocating adb command @ %d\n", __LINE__);
        return -1;
    }
    if (!issue_adb_command(command)) {
        return -1;
    }
    free(command);

    if (asprintf(&command, "%s", "adb shell sync") < 0) {
        printf("Problems allocating adb command @ %d\n", __LINE__);
        return -1;
    }
    if (!issue_adb_command(command)) {
        return -1;
    }
    free(command);


    if (asprintf(&command, "%s", "adb shell mkdir /nanda") < 0) {
        printf("Problems allocating adb command @ %d\n", __LINE__);
        return -1;
    }
    if (!issue_adb_command(command)) {
        return -1;
    }
    free(command);


    if (asprintf(&command, "%s", "adb shell mount -t vfat /dev/block/nanda /nanda") < 0) {
        printf("Problems allocating adb command @ %d\n", __LINE__);
        return -1;
    }
    if (!issue_adb_command(command)) {
        return -1;
    }
    free(command);


    if (asprintf(&command, "%s", "adb push ./linux.bmp /nanda/linux/linux.bmp") < 0) {
        printf("Problems allocating adb command @ %d\n", __LINE__);
        return -1;
    }
    if (!issue_adb_command(command)) {
        return -1;
    }
    free(command);


    if (asprintf(&command, "%s", "adb shell sync") < 0) {
        printf("Problems allocating adb command @ %d\n", __LINE__);
        return -1;
    }
    if (!issue_adb_command(command)) {
        return -1;
    }
    free(command);


    if (asprintf(&command, "%s", "adb shell umount /nanda") < 0) {
        printf("Problems allocating adb command @ %d\n", __LINE__);
        return -1;
    }
    if (!issue_adb_command(command)) {
        return -1;
    }
    free(command);

    printf("now starting App slow backup phase 3\n");
    if (asprintf(&command, "%s%s%s", "echo \"ro.serialno=", serialno , "\" >> ./case2ramdisk/default.prop") < 0) {
        printf("Problems allocating adb command @ %d\n", __LINE__);
        return -1;
    }
    if (!issue_adb_command(command)) {
        return -1;
    }
    free(command);


    if (asprintf(&command, "%s", "../../../host/linux-x86/bin/mkbootfs ./case2ramdisk | minigzip > ./ramdisk-recovery.img") < 0) {
        printf("Problems allocating adb command @ %d\n", __LINE__);
        return -1;
    }
    if (!issue_adb_command(command)) {
        return -1;
    }
    free(command);


    if (asprintf(&command, "%s", "../../../host/linux-x86/bin/mkbootimg  --kernel ./nandc.img-kernel --ramdisk ./ramdisk-recovery.img --cmdline \"console=ttyS0,115200 rw init=/init loglevel=5\" --base 0x40000000 --output ./modfrecovery.img") < 0) {
        printf("Problems allocating adb command @ %d\n", __LINE__);
        return -1;
    }
    if (!issue_adb_command(command)) {
        return -1;
    }
    free(command);


    if (asprintf(&command, "%s", "adb push ./modfrecovery.img /mnt/sdcard/") < 0) {
        printf("Problems allocating adb command @ %d\n", __LINE__);
        return -1;
    }
    if (!issue_adb_command(command)) {
        return -1;
    }
    free(command);


    system("adb shell 'cat /dev/zero > /dev/block/nandc'");
    system("adb shell 'cat /mnt/sdcard/modfrecovery.img > /dev/block/nandc'");

    if (asprintf(&command, "%s", "adb shell rm /mnt/sdcard/modfrecovery.img") < 0) {
        printf("Problems allocating adb command @ %d\n", __LINE__);
        return -1;
    }
    if (!issue_adb_command(command)) {
        return -1;
    }
    free(command);



    if (asprintf(&command, "%s", "adb shell sync") < 0) {
        printf("Problems allocating adb command @ %d\n", __LINE__);
        return -1;
    }
    if (!issue_adb_command(command)) {
        return -1;
    }
    free(command);


    printf("now starting App final backup phase 4 ...\n");
    if (asprintf(&command, "%s", "adb push ./User_Manual_UbiSlate7Ci.pdf /mnt/sdcard/") < 0) {
        printf("Problems allocating adb command @ %d\n", __LINE__);
        return -1;
    }
    if (!issue_adb_command(command)) {
        return -1;
    }
    free(command);

    if (asprintf(&command, "%s", "adb shell am broadcast -a android.intent.action.MASTER_CLEAR") < 0) {
        printf("Problems allocating adb command @ %d\n", __LINE__);
        return -1;
    }
    if (!issue_adb_command(command)) {
        return -1;
    }
    free(command);

    system("rm ./nandc.img*");
    system("rm ./ramdisk-recovery.img");
    system("rm ./modfrecovery.img");


//     printf("now starting App final backup phase 3 ... this will take 2 minutes\n");
//     system("adb restore ./a13_case2.ab");
//
//
//     if (asprintf(&command, "%s", "adb reboot") < 0) {
//         printf("Problems allocating adb command @ %d\n", __LINE__);
//         return -1;
//     }
//     if (!issue_adb_command(command)) {
//         return -1;
//     }
//     free(command);

    if (asprintf(&command, "%s", "cp ./case2default.prop ./case2ramdisk/default.prop") < 0) {
        printf("Problems allocating adb command @ %d\n", __LINE__);
        return -1;
    }
    if (!issue_adb_command(command)) {
        return -1;
    }
    free(command);

    if (mac_add)
        free(mac_add);

    puts(" .. Success .. ");
    return 0;
}
