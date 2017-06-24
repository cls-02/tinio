/*
 * Command line utility for downloading FW
 * Copyright (C) 2013 Cypress Semiconductor
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <signal.h>

#include <stdbool.h>
#include <unistd.h>
#include <sys/time.h>
#include <pthread.h>
#include <ctype.h>

#include "include/CyUSBSerial.h"
#include "include/CyUSBBootloader.h"

#define CY_MAX_DEVICES 30
#define CY_MAX_INTERFACES 4

typedef struct _CY_DEVICE_STRUCT {
    int deviceNumber;
    int interfaceFunctionality[CY_MAX_INTERFACES];
    bool isI2c;
    bool isSpi;
    int numInterface;
}CY_DEVICE_STRUCT;

CY_DEVICE_STRUCT *glDevice;
int i2cDeviceIndex[CY_MAX_DEVICES][CY_MAX_INTERFACES];
unsigned char *deviceNumber = NULL;
int cyDevices, i2cDevices = 0, numDevices = 0;
int selectedDeviceNum = -1, selectedInterfaceNum = -1;
bool exitApp = false;
unsigned short pageAddress = -1;
short readWriteLength = -1;
bool deviceAddedRemoved = false;
unsigned char read_buffer[512];

void printListOfDevices (bool isPrint)
{
    int  index_i = 0, index_j = 0, i, j, countOfDevice = 0, devNum;
    int length, index = 0, numInterfaces, interfaceNum;
    bool set1 = false;

    unsigned char deviceID[CY_MAX_DEVICES];
    char functionality[64];
    CY_DEVICE_INFO deviceInfo;
    CY_DEVICE_CLASS deviceClass[CY_MAX_INTERFACES];
    CY_DEVICE_TYPE  deviceType[CY_MAX_INTERFACES];
    CY_RETURN_STATUS rStatus;

    deviceAddedRemoved = false;
    CyGetListofDevices ((UINT8 *)&numDevices);
    //printf ("The number of devices is %d \n", numDevices);
    for (i = 0; i < numDevices; i++){
        for (j = 0; j< CY_MAX_INTERFACES; j++)
            glDevice[i].interfaceFunctionality[j] = -1;
    }
    if (isPrint){
        printf ("\n\n---------------------------------------------------------------------------------\n");
        printf ("Device Number | VID | PID | INTERFACE NUMBER | FUNCTIONALITY \n");
        printf ("---------------------------------------------------------------------------------\n");
    }
    cyDevices = 0;
    for (devNum = 0; devNum < numDevices; devNum++){
        rStatus = CyGetDeviceInfo (devNum, &deviceInfo);
        interfaceNum = 0;
        if (!rStatus)
        {
            //if (isCypressDevice (devNum)){
              //  continue;
            //}
            strcpy (functionality, "NA");
            numInterfaces = deviceInfo.numInterfaces;
            glDevice[index].numInterface = numInterfaces;
            cyDevices++;

            while (numInterfaces){
                if (deviceInfo.deviceClass[interfaceNum] == CY_CLASS_VENDOR)
                {
                    glDevice[index].deviceNumber = devNum;
                    switch (deviceInfo.deviceType[interfaceNum]){
                        case CY_TYPE_I2C:
                            glDevice[index].interfaceFunctionality[interfaceNum] = CY_TYPE_I2C;
                            strcpy (functionality, "VENDOR_I2C");
                            glDevice[index].isI2c = true;
                            break;
                        case CY_TYPE_SPI:
                            glDevice[index].interfaceFunctionality[interfaceNum] = CY_TYPE_SPI;
                            strcpy (functionality, "VENDOR_SPI");
                            glDevice[index].isSpi = true;
                            break;
                        default:
                            strcpy (functionality, "NA");
                            break;
                    }
                }
                else if (deviceInfo.deviceClass[interfaceNum] == CY_CLASS_CDC){
                    strcpy (functionality, "NA");
                }
                if (isPrint) {
                    printf ("%d             |%x  |%x    | %d     | %s\n", \
                            index, \
                            deviceInfo.vidPid.vid, \
                            deviceInfo.vidPid.pid,  \
                            interfaceNum, \
                            functionality \
                           );
                }
                interfaceNum++;
                numInterfaces--;
            }
            index++;
        }
    }
    if (isPrint){
        printf ("---------------------------------------------------------------------------------\n\n");
    }
    printf("Cydevices %d",cyDevices);
}

void deviceHotPlug () {

  CY_RETURN_STATUS rStatus;
  deviceAddedRemoved = true;
  selectedDeviceNum = -1;
  selectedInterfaceNum = -1;
  printf ("Device of interest Removed/Added \n");
  rStatus = CyGetListofDevices ((UINT8 *)&numDevices);
  if (rStatus != CY_SUCCESS) {
    printf ("CY:Error in Getting List of Devices: Error NO:<%d> \n", rStatus);
    return;
  }
  printListOfDevices (false);
}

int getUserInput()
{
    char userInput[6], x;
    int output,i = 0;
    bool isDigit = true;
    x = getchar();
    while (x != '\n'){
        if (i < 5){
            userInput[i] = x;
            i++;
        }
        if (!isdigit(x))
            isDigit = false;

        x = getchar();
    }
    userInput[i] = '\0';
    if (isDigit == false)
        return -1;
    output = atoi(userInput);
    return output;
}


unsigned int GetCheckSum(unsigned int *buff, unsigned int length)
{
    unsigned int i,val;
    unsigned int checkSum =0;

    for (i = 0; i < length; i++) // start at 12th byte
    {
        checkSum += buff[i];
    }

    return checkSum;
}


int main (int argc, char **argv)
{
    int index = 0, i, j, userInput;
    int output;
    int count=0,h=0,cnt;
    int size_buffer,size_checksum;
    FILE *fp=NULL;
    CY_HANDLE handle;
    unsigned char buff[516];
    int silicon_id;
    int tempSelectedDeviceNum, tempSelectedInterfaceNum;
    CY_RETURN_STATUS rStatus;
    //signal (SIGUSR1, deviceHotPlug);
    char src_file[100];
    char id[4];

    memset(buff,0,sizeof(buff));


    glDevice = (CY_DEVICE_STRUCT *)malloc (CY_MAX_DEVICES *sizeof (CY_DEVICE_STRUCT));
    if (glDevice == NULL){
        printf ("Memory allocation failed ...!! \n");
        return -1;
    }
    rStatus = CyLibraryInit ();
    if (rStatus != CY_SUCCESS) {
        printf ("CY:Error in Doing library init Error NO:<%d> \n", rStatus);
        return rStatus;
    }
    rStatus = CyGetListofDevices ((UINT8 *)&numDevices);
    if (rStatus != CY_SUCCESS) {
        printf ("CY:Error in Getting List of Devices: Error NO:<%d> \n", rStatus);
        return rStatus;
    }
    printListOfDevices(true);
    do {

        printf ("-------------------------------------------------------------------\n");
        printf ("1: Print list of devices \n");
        if (selectedDeviceNum != -1 && selectedInterfaceNum != -1){
            printf ("2: Change device selection--selected device: [Device number %d] : [Interface No %d]",\
                    selectedDeviceNum, selectedInterfaceNum);
            if (glDevice[selectedDeviceNum].interfaceFunctionality[selectedInterfaceNum] == CY_TYPE_I2C)
                printf (" : I2C\n");
            else if (glDevice[selectedDeviceNum].interfaceFunctionality[selectedInterfaceNum] == CY_TYPE_SPI)
                printf (" : SPI\n");
            else if (glDevice[selectedDeviceNum].interfaceFunctionality[selectedInterfaceNum] == CY_TYPE_UART)
               printf ("UART : \n");
            else
                printf (" : NA\n");

        }
        else
            printf ("2: Select device...No device selected !!\n");


        userInput = getUserInput();
        if (userInput < 1 || userInput > 5){
            printf ("Wrong selection code ... Enter again \n");
            continue;
        }

        switch (userInput){

            case 1:
                printListOfDevices(true);
                break;
            case 2:
                if (cyDevices == 0) {
                    printf ("No device of interest connected ...!!\n");
                    continue;
                }
                printf ("Enter Device number to be selected.. \n");
                tempSelectedDeviceNum = getUserInput();
                //printf ("Selected device number is %d \n",tempSelectedDeviceNum);
                if (tempSelectedDeviceNum >= cyDevices || tempSelectedDeviceNum == -1){
                    printf ("Wrong device selection \n");
                    continue;
                }
                printf ("Enter interface number..\n");
                tempSelectedInterfaceNum = getUserInput();
                //printf ("Selected device number is %d %d\n",tempSelectedInterfaceNum, glDevice[tempSelectedDeviceNum].numInterface);

                if (tempSelectedInterfaceNum >= glDevice[tempSelectedDeviceNum].numInterface ||
                        tempSelectedInterfaceNum == -1) {
                    printf ("Wrong interface Selection selection \n");
                    continue;
                }

                if (deviceAddedRemoved == true) {
                    printf ("Device of interest was added/removed .... Print and select from new list\n");
                    continue;
                }
                selectedDeviceNum = tempSelectedDeviceNum;
                selectedInterfaceNum = tempSelectedInterfaceNum;


                rStatus = CyOpen (selectedDeviceNum,selectedInterfaceNum , &handle);
                //printf("return status = %d", rStatus);
                if (rStatus == CY_SUCCESS){
                    //printf("device opened \n");
                }
                else
                {
                    printf("fail \n");
                }



              // printf("Please enter file to be opened");

             fp = fopen(argv[1],"rb+");
            // printf("%s", src_file);
                rStatus = CyFlashConfigEnable(handle,1);

                if (rStatus == CY_SUCCESS){
                   // printf("Flash is configured");
                }
                else
                {
                    printf("Manufacturing mode of FLASH is not configured");
                }
              //  printf("status %d",rStatus);

                if(fp == NULL)
                {
                    printf("\n fopen() Error!!!\n");
                    return 1;
                }
                printf("\n File opened successfully\n");
                if(sizeof(buff) != fread(buff,1,516,fp))
                {
                    printf("\n fread() failed\n");
                    return 1;
                }
                printf("\n Bytes successfully read \n");
              //  printf("reached here");

               // silicon_id = *(unsigned int *)buff;

                rStatus=CyGetSiliconID(handle,(UINT32 *)&silicon_id);
                if (rStatus == CY_SUCCESS){
                   // printf(" Correct silicon id");

                }
                else
                {
                    printf("Not correct ID");
                }

            // printf("silicon id %04x,%d",silicon_id,sizeof(silicon_id));
                id[0]= (silicon_id);
               id[1]= ((silicon_id >> 8) & 0xFF);
               id[2]= ((silicon_id >> 16) & 0xFF);
              id[3]= ((silicon_id >> 24) & 0xFF);



                rStatus=CyReadDeviceConfig(handle, (UCHAR *)&read_buffer);
                if (rStatus == CY_SUCCESS){
                   // printf(" Got the data");

                }
                else
                {
                    printf("Not done");
                }

              /*printf (" 0 %02x, %02x \r \n", id[0],read_buffer[0]);
              printf (" 1 %02x, %02x \r \n", id[1],read_buffer[1]);
              printf (" 2 %02x, %02x \r \n", id[2],read_buffer[2]);
              printf (" 3 %02x, %02x\r \n", id[3],read_buffer[3]);*/

                size_buffer = sizeof(read_buffer);
                //printf("The size is %d, buff %d", size_buffer,sizeof(buff));

                /*for (i = 4; i < 516; i +=16)
                {
                    printf("\n%02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x",
                    buff[i], buff[i+1], buff[i+2], buff[i+3], buff[i+4], buff[i+5], buff[i+6], buff[i+7], buff[i+8],
                    buff[i+9], buff[i+10], buff[i+11], buff[i+12], buff[i+13], buff[i+14], buff[i+15]);
                }
                for (i = 0; i < 512; i +=16)
                {
                    printf("\n%02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x",
                    read_buffer[i], read_buffer[i+1], read_buffer[i+2], read_buffer[i+3], read_buffer[i+4], read_buffer[i+5], read_buffer[i+6],
                    read_buffer[i+7], read_buffer[i+8],
                    read_buffer[i+9], read_buffer[i+10], read_buffer[i+11], read_buffer[i+12], read_buffer[i+13], read_buffer[i+14], read_buffer[i+15]);
                }*/
                memcpy (&buff[4], read_buffer, 28);

                size_checksum= GetCheckSum((unsigned int *)(&buff[16]), 125);
              //  printf("The checksum size is %d",size_checksum);

                buff[12]= (size_checksum & 0xFF);
                buff[13]= ((size_checksum >> 8) & 0xFF);
                buff[14]= ((size_checksum >> 16) & 0xFF);
                buff[15]= ((size_checksum >> 24) & 0xFF);

               // printf("checksum 0x%08x\n", size_checksum);
              /*  for (i = 4; i < 516; i +=16)
                {
                    printf("\n%02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x",
                    buff[i], buff[i+1], buff[i+2], buff[i+3], buff[i+4], buff[i+5], buff[i+6], buff[i+7], buff[i+8],
                    buff[i+9], buff[i+10], buff[i+11], buff[i+12], buff[i+13], buff[i+14], buff[i+15]);
                }*/

               // if(((id[0] == ffffffA1) || (id[0] == ffffffA2) || (id[0] == ffffffA3)) && (id[1] == 08))
               if((silicon_id == 0x08A1) || (silicon_id == 0x08A2) || (silicon_id == 0x08A3))
                {

               rStatus= CyWriteDeviceConfig(handle,&buff[4]);
                if (rStatus == CY_SUCCESS){
                   printf(" Programming Flash is done");
                }
                else
                {
                    printf("Not done");
                }
               }
               else
               {
               printf("wrong silicon id");
                }

                fclose(fp);

                printf("\n File stream closed \n");



                rStatus= CyClose(handle);
                if (rStatus == CY_SUCCESS){
                    //printf("Device closed");
                }
                else
                {
                    printf("Not closed");
                }


                break;
            case 3:
                exitApp = true;
                CyLibraryExit ();
                break;
        }
    }while (exitApp == false);
    free (glDevice);
}
bool isCypressDevice (int deviceNum) {
    CY_HANDLE handle;
    unsigned char interfaceNum = 0;
    unsigned char sig[6];
int op;
    CY_RETURN_STATUS rStatus;
    rStatus = CyOpen (deviceNum, interfaceNum, &handle);
    //op= libusb_detach_kernel_driver(handle,0);
    if (rStatus == CY_SUCCESS){
        rStatus = CyGetSignature (handle, sig);
        if (rStatus == CY_SUCCESS){
            CyClose (handle);
            return true;
        }
        else {
            CyClose (handle);
            return false;
        }
    }
    else
        return false;
}
