#include <stdio.h>
#include <stdlib.h>
#include <linux/string.h>
#include <unistd.h>
#include <cups/cups.h>
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
        if (strlen(line) > 0 && strstr(command,"push") == NULL && strstr(command,"am broadcast") == NULL) {
            printf("\n *** Error please note down the message below *** 4 %s\n",command);
        }
        printf("%s", line);

    }
    pclose(fpipe);

    return true;
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

int main(int argc, char **argv) {
    printf(" >> starting ..\n");

    char *command;
    FILE * pFile;

    system("adb shell 'echo \"halt\" > /sys/power/wake_lock'");

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
    if (asprintf(&command, "%s%s%s", "echo \"ro.serialno=", argv[1] , "\" >> ./case2ramdisk/default.prop") < 0) {
        printf("Problems allocating adb command @ %d\n", __LINE__);
        return -1;
    }
    if (!issue_adb_command(command)) {
        return -1;
    }
    free(command);


    if (asprintf(&command, "%s", "mkbootfs ./case2ramdisk | minigzip > ./ramdisk-recovery.img") < 0) {
        printf("Problems allocating adb command @ %d\n", __LINE__);
        return -1;
    }
    if (!issue_adb_command(command)) {
        return -1;
    }
    free(command);


    if (asprintf(&command, "%s", "mkbootimg  --kernel ./case2kernel --ramdisk ./ramdisk-recovery.img --cmdline \"console=ttyS0,115200 rw init=/init loglevel=5\" --base 0x40000000 --output ./modfrecovery.img") < 0) {
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

    printf(" << exiting ..\n");
    return 0;
}
