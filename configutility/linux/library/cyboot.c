#include "CyUSBCommon.h"
#include "CyUSBBootloader.h"
/*
   CyReadFlash will read the content of flash from the 
   specified address
 */

UINT32 fmVersion;
UINT32 fmChecksum;
UINT32 firmwareSize = 0;
UINT32 firmwareEntry;
UINT32 firmwareStart;
typedef enum CY_BOOT_VENDOR_CMDS
{
    CY_CMD_GET_VERSION = 0xB0,      /* Get the version of the boot-loader.
                                           value = 0, index = 0, length = 4;
                                           data_in = 32 bit version. */
    CY_BOOT_CMD_GET_SILICON_ID= 0xB1,         /* Get the silicon ID for the device.
                                           value = 0, index = 0, length = 4;
                                           data_in = 32 bit silicon id. */
    CY_BOOT_CMD_READ_FLASH,             /* Read flash content: 
                                           value = MS word of address,
                                           index = LS word of address(4 byte aligned),
                                           length = length of read (4 byte multiple);
                                           data_in = requested data from flash. */
    CY_BOOT_CMD_PROG_FLASH,             /* Program data into flash:
                                           value = MS word of address,
                                           index = LS word of address(128 byte aligned),
                                           length = length of write (128 byte multiple);
                                           data_out = data to be written to flash. */
    CY_BOOT_CMD_RESERVED_0,               /* Reserved*/
    CY_BOOT_CMD_READ_CONFIG = 0xB5,     /* Read the device configuration table:
                                           value = 0, index = 0, length = 512;
                                           data_in = device configuration table. */
    CY_BOOT_CMD_PROG_CONFIG = 0xB6,            /* Program the device configuration table:
                                           value = 0, index = 0, length = 512;
                                           data_out = device configuration table. */
    CY_BOOT_CMD_READ_BOOT_CONFIG,       /* Read the boot-loader configuration table;
                                           value = 0, index = 0, length = 256;
                                           data_in = boot-loader configuration table. */
    CY_BOOT_CMD_PROG_BOOT_CONFIG,       /* Program the boot-loader configuration table;
                                           value = 0, index = 0, length = 256;
                                           data_out = boot-loader configuration table. */
    CY_BOOT_CMD_RESERVED_1,             /* RESERVED */
    CY_BOOT_CMD_VALIDATE_CHECKSUM = 0xBA,/* Calculate checksum and compare against the
                                           content of the device configuration table. */
    CY_BOOT_CMD_READ_MEM,               /* Read from the SRAM memory:
                                           index = LS word of address(4 byte aligned),
                                           length = length of read (4 byte multiple);
                                           data_in = requested data from SRAM. */
    CY_BOOT_CMD_WRITE_MEM,              /* Program data into SRAM memory:
                                           value = MS word of address,
                                           index = LS word of address(4 byte aligned),
                                           length = length of write (4 byte multiple);
                                           data_out = data to be written to SRAM. */
    CY_VENDOR_GET_SIGNATURE,            /*Get the signature of the firmware
                                            It is suppose to be 'CYUS' for normal firmware
                                            and 'CYBL' for Bootloader.*/
    CY_BOOT_CMD_MFG_JUMP,               /*Jump to bootloader mode*/
    
    CY_VENDOR_ENTER_MFG_MODE = 0xE2     /* Enter the configuration mode. This needs to be invoked to enter
                                           the manufacturing mode. If this is not set, then the device shall
                                           not allow any of the configuration requests (B0 to BF) to go through.
                                           Value = ~"CY" = 0xA6BC, index = ~"OF" = 0xB9B0: for disable,
                                           Value = ~"CY" = 0xA6BC, ~"ON" = 0xB1B0: for enable,
                                           Length = 0. */

} CY_BOOT_VENDOR_CMDS;

#define CY_DEVICE_CONFIG_SIZE 512
#define CY_GET_SILICON_ID_LEN 4
#define CY_USB_SERIAL_TIMEOUT 0

CY_RETURN_STATUS CyReadFlash (
        CY_HANDLE handle,
        PCY_BOOTLD_BUFFER readBuffer,
        UINT32 ioTimeout
        )
{
    UINT16 wValue, wIndex, wLength;
    UINT8 bmRequestType, bmRequest;
    UINT32 rStatus;
    CY_DEVICE *device;
    libusb_device_handle *devHandle;
    
    if (handle == NULL)
        return CY_ERROR_INVALID_HANDLE;
    if (readBuffer == NULL || readBuffer->buffer == NULL || readBuffer->length == 0)
        return CY_ERROR_INVALID_PARAMETER;
    device = (CY_DEVICE *)handle;
    devHandle = device->devHandle;
    bmRequestType = CY_VENDOR_REQUEST_DEVICE_TO_HOST;
    bmRequest = CY_BOOT_CMD_READ_FLASH; 
    wValue = ((readBuffer->address & 0xFFFF0000 ) >> 16);
    wIndex = (readBuffer->address & 0x0000FFFF );

    wLength = (readBuffer->length);
    rStatus = libusb_control_transfer (devHandle, bmRequestType, bmRequest,
            wValue, wIndex, readBuffer->buffer, wLength, ioTimeout);
    if (rStatus > 0){
        *(readBuffer->bytesReturned) = rStatus;
        CY_DEBUG_PRINT_ERROR ("CY:The Length is %d\n", rStatus);
        return CY_SUCCESS;
    }
    else if (rStatus == LIBUSB_ERROR_TIMEOUT){
        CY_DEBUG_PRINT_ERROR ("CY:Device Timed out.. \n");
        *(readBuffer->bytesReturned) = 0;
        return CY_ERROR_IO_TIMEOUT;        
    }
    else {
        CY_DEBUG_PRINT_ERROR ("CY:Error in process the request.. libusb error is %d \n", rStatus);
        *(readBuffer->bytesReturned) = 0;
        return CY_ERROR_REQUEST_FAILED;        
    }
}
/*
   CyProgFlash will write the content of flash from the 
   specified address
 */
CY_RETURN_STATUS CyProgFlash (
        CY_HANDLE handle,
        PCY_BOOTLD_BUFFER writeBuffer,
        UINT32 ioTimeout
        )
{
    UINT16 wValue, wIndex, wLength;
    UINT8 bmRequestType, bmRequest;
    UINT32 rStatus;
    CY_DEVICE *device;
    libusb_device_handle *devHandle;
    
    if (handle == NULL)
        return CY_ERROR_INVALID_HANDLE;
    if (writeBuffer == NULL || writeBuffer->buffer == NULL || writeBuffer->length == 0)
        return CY_ERROR_INVALID_PARAMETER;
    device = (CY_DEVICE *)handle;
    devHandle = device->devHandle;
    
    bmRequestType = CY_VENDOR_REQUEST_HOST_TO_DEVICE;
    bmRequest = CY_BOOT_CMD_PROG_FLASH; 
    wValue = ((writeBuffer->address & 0xFFFF0000 ) >> 16);
    wIndex = (writeBuffer->address & 0x0000FFFF );

    wLength = (writeBuffer->length);
    CY_DEBUG_PRINT_INFO ("CY:The Length is %d , Value is %d and index is %d\n", wLength, wValue, wIndex);
    rStatus = libusb_control_transfer (devHandle, bmRequestType, bmRequest,
            wValue, wIndex, writeBuffer->buffer, wLength, ioTimeout);
    if (rStatus > 0){
        *(writeBuffer->bytesReturned) = rStatus;
        return CY_SUCCESS;
    }
    else if (rStatus == LIBUSB_ERROR_TIMEOUT){
        CY_DEBUG_PRINT_ERROR ("CY:Device Timed out.. \n");
        *(writeBuffer->bytesReturned) = 0;
        return CY_ERROR_IO_TIMEOUT;        
    }
    else {
        CY_DEBUG_PRINT_ERROR ("CY:Error in process the request..libusb error is %d \n", rStatus);
        *(writeBuffer->bytesReturned) = 0;
        return CY_ERROR_REQUEST_FAILED;        
    }
}
/* CyReadMemory will read the content of SRAM from the 
   specified address
 */
CY_RETURN_STATUS CyReadMemory (
        CY_HANDLE handle,
        PCY_BOOTLD_BUFFER readBuffer,
        UINT32 ioTimeout
        )
{
    UINT16 wValue, wIndex, wLength;
    UINT8 bmRequestType, bmRequest;
    UINT32 rStatus;
    CY_DEVICE *device;
    libusb_device_handle *devHandle;
    
    if (handle == NULL)
        return CY_ERROR_INVALID_HANDLE;
    if (readBuffer == NULL || readBuffer->buffer == NULL || readBuffer->length == 0)
        return CY_ERROR_INVALID_PARAMETER;
    device = (CY_DEVICE *)handle;
    devHandle = device->devHandle;

    bmRequestType = CY_VENDOR_REQUEST_DEVICE_TO_HOST;
    bmRequest = CY_BOOT_CMD_READ_MEM;
    wValue = ((readBuffer->address & 0xFFFF0000 ) >> 16);
    wIndex = (readBuffer->address & 0x0000FFFF );
    wLength = (readBuffer->length);

    rStatus = libusb_control_transfer (devHandle, bmRequestType, bmRequest,
            wValue, wIndex, readBuffer->buffer, wLength, ioTimeout);
    if (rStatus > 0){
        *(readBuffer->bytesReturned) = rStatus;
        return CY_SUCCESS;
    }
    else if (rStatus == LIBUSB_ERROR_TIMEOUT){
        CY_DEBUG_PRINT_ERROR ("CY:Device Timed out.. \n");
        *(readBuffer->bytesReturned) = 0;
        return CY_ERROR_IO_TIMEOUT;        
    }
    else {
        CY_DEBUG_PRINT_ERROR ("CY:Error in processing the request.. libusb error is %d \n", rStatus);
        *(readBuffer->bytesReturned) = 0;
        return CY_ERROR_REQUEST_FAILED;        
    }
}
/*
   CyWriteMemory will write the content to specified address 
   in SRAM
 */
CY_RETURN_STATUS CyWriteMemory (
        CY_HANDLE handle,
        PCY_BOOTLD_BUFFER writeBuffer,
        UINT32 ioTimeout
        )
{
    UINT16 wValue, wIndex, wLength;
    UINT8 bmRequestType, bmRequest;
    UINT32 rStatus;
    CY_DEVICE *device;
    libusb_device_handle *devHandle;

    if (handle == NULL)
        return CY_ERROR_INVALID_HANDLE;
    if (writeBuffer == NULL || writeBuffer->buffer == NULL || writeBuffer->length == 0)
        return CY_ERROR_INVALID_PARAMETER;
    device = (CY_DEVICE *)handle;
    devHandle = device->devHandle;

    bmRequestType = CY_VENDOR_REQUEST_HOST_TO_DEVICE;
    bmRequest = CY_BOOT_CMD_WRITE_MEM;
    wValue = ((writeBuffer->address & 0xFFFF0000 ) >> 16);
    wIndex = (writeBuffer->address & 0x0000FFFF );
    wLength = (writeBuffer->length);

    rStatus = libusb_control_transfer (devHandle, bmRequestType, bmRequest,
            wValue, wIndex, writeBuffer->buffer, wLength, ioTimeout);
    if (rStatus > 0){
        *(writeBuffer->bytesReturned) = rStatus;
        return CY_SUCCESS;
    }
    else if (rStatus == LIBUSB_ERROR_TIMEOUT){
        CY_DEBUG_PRINT_ERROR ("CY:Device Timed out.. \n");
        *(writeBuffer->bytesReturned) = 0;
        return CY_ERROR_IO_TIMEOUT;        
    }
    else {
        CY_DEBUG_PRINT_ERROR ("CY:Error in process the request..libusb error is %d \n", rStatus);
        *(writeBuffer->bytesReturned) = 0;
        return CY_ERROR_REQUEST_FAILED;        
    }
}
/*
   This function will update the checksum value in device configuration table.
 */
CY_RETURN_STATUS CyValidateChecksum (
        CY_HANDLE handle
        )
{
    UINT16 wValue, wIndex, wLength;
    UINT8 bmRequestType, bmRequest;
    UINT32 rStatus, ioTimeout = CY_USB_SERIAL_TIMEOUT;
    CY_DEVICE *device;
    libusb_device_handle *devHandle;
    
    if (handle == NULL)
        return CY_ERROR_INVALID_HANDLE;
    device = (CY_DEVICE *)handle;
    devHandle = device->devHandle;
    
    bmRequestType = CY_VENDOR_REQUEST_HOST_TO_DEVICE;
    bmRequest = CY_BOOT_CMD_VALIDATE_CHECKSUM;
    wValue = 0x00;
    wIndex = 0x00;
    wLength = 0;

    rStatus = libusb_control_transfer (devHandle, bmRequestType, bmRequest,
            wValue, wIndex, NULL, wLength, ioTimeout);
    if (rStatus >= 0){
        return CY_SUCCESS;
    }
    else {
        CY_DEBUG_PRINT_ERROR ("CY:Validate Checksum Failed..libusb error is %d \n", rStatus);
        return CY_ERROR_REQUEST_FAILED;
    }
}
/*
   CyReadBootConfig will read the entire boot configuration table
 */
CY_RETURN_STATUS CyReadBootConfig (
        CY_HANDLE handle,
        UINT8 *bootConfig
        )
{
    UINT16 wValue, wIndex, wLength;
    UINT8 bmRequestType, bmRequest;
    UINT32 rStatus, ioTimeout = CY_USB_SERIAL_TIMEOUT;
    CY_DEVICE *device;
    libusb_device_handle *devHandle;

    if (handle == NULL)
        return CY_ERROR_INVALID_HANDLE;
    if (bootConfig == NULL)
        return CY_ERROR_INVALID_PARAMETER;
    device = (CY_DEVICE *)handle;
    devHandle = device->devHandle;
    
    bmRequestType = CY_VENDOR_REQUEST_DEVICE_TO_HOST;
    bmRequest = CY_BOOT_CMD_READ_BOOT_CONFIG;
    wValue = 0x00;
    wIndex = 0x00;
    wLength = CY_BOOT_CONFIG_SIZE;

    rStatus = libusb_control_transfer (devHandle, bmRequestType, bmRequest,
            wValue, wIndex, bootConfig, wLength, ioTimeout);
    if (rStatus == CY_BOOT_CONFIG_SIZE){
        CY_DEBUG_PRINT_INFO ("CY:Successfully read Boot loader configuration ... \n");
        return CY_SUCCESS;
    }
    else if (rStatus == LIBUSB_ERROR_TIMEOUT){
        CY_DEBUG_PRINT_ERROR ("CY: There was a Time out error in Reading Boot config .. \n");
        return CY_ERROR_IO_TIMEOUT;
    }
    else{
        CY_DEBUG_PRINT_ERROR ("CY: There was an error in Reading Boot config... libusb error is %d \n", rStatus);
        return CY_ERROR_REQUEST_FAILED;
    }
}
/*
   CyWriteBootConfig will write the entire boot configuration table
 */
CY_RETURN_STATUS CyWriteBootConfig (
        CY_HANDLE handle,
        UINT8 *bootConfig
        )
{
    UINT16 wValue, wIndex, wLength;
    UINT8 bmRequestType, bmRequest;
    UINT32 rStatus, ioTimeout = CY_USB_SERIAL_TIMEOUT;
    CY_DEVICE *device;
    libusb_device_handle *devHandle;
    
    if (handle == NULL)
        return CY_ERROR_INVALID_HANDLE;
    if (bootConfig == NULL)
        return CY_ERROR_INVALID_PARAMETER;
    device = (CY_DEVICE *)handle;
    devHandle = device->devHandle;
    
    bmRequestType = CY_VENDOR_REQUEST_HOST_TO_DEVICE;
    bmRequest = CY_BOOT_CMD_PROG_BOOT_CONFIG;
    wValue = 0x00;
    wIndex = 0x00;
    wLength = CY_BOOT_CONFIG_SIZE;

    rStatus = libusb_control_transfer (devHandle, bmRequestType, bmRequest,
            wValue, wIndex, bootConfig, wLength, ioTimeout);

    if (rStatus == CY_BOOT_CONFIG_SIZE){
        CY_DEBUG_PRINT_INFO ("CY:Successfully wrote boot configuration \n");
        return CY_SUCCESS;
    }
    else if (rStatus == LIBUSB_ERROR_TIMEOUT){
        CY_DEBUG_PRINT_ERROR ("CY: There was a Time out error in Writing Boot config .. \n");
        return CY_ERROR_IO_TIMEOUT;
    }
    else{
        CY_DEBUG_PRINT_ERROR ("CY: There was an error in Writing Boot config ..libusb error is %d \n", rStatus);
        return CY_ERROR_REQUEST_FAILED;
    }
}
/*
   This Api will download the firmware on to Cy USB serial device
 */
CY_RETURN_STATUS CyDownloadFirmware (
        CY_HANDLE handle,
        CHAR *filePath 
        )
{
    UINT32 fd, bytesRead, totalLength, bytesRemaining, ioTimeout = CY_USB_SERIAL_TIMEOUT, rStatus;
    UINT32 address, *tempAdd;
    UINT32 checkStart = 0;
    UCHAR buffer[CY_FIRMWARE_BREAKUP_SIZE];
    UINT16 wValue, wIndex, wLength;
    UINT8 bmRequestType, bmRequest;
    CY_DEVICE *device;
    libusb_device_handle *devHandle;

    if (handle == NULL)
        return CY_ERROR_INVALID_HANDLE;
    device = (CY_DEVICE *)handle;
    devHandle = device->devHandle;
    if (filePath == NULL)
        return CY_ERROR_INVALID_PARAMETER;
    fd = open (filePath, O_RDONLY);
    if (fd < 0){
        CY_DEBUG_PRINT_ERROR ("CY:Error in Opening the File ... \n");
        return CY_ERROR_DOWNLOAD_FAILED;
    }
    bytesRead = read (fd, buffer, 4);
    if (bytesRead != 4){
        CY_DEBUG_PRINT_ERROR ("CY: Error in Read Length ... \n");
        close (fd);
        return CY_ERROR_INVALID_FIRMWARE;
    }
    if (strncmp ((char*)buffer, "CYUS", 4)){
        CY_DEBUG_PRINT_ERROR ("CY:Not a valid image,no valid signature .... \n");
        close (fd);
        return CY_ERROR_FIRMWARE_INVALID_SIGNATURE;
    }
    // Read off the Version before reading the length of the section
    bytesRead = read (fd, buffer, 4);
    if (bytesRead != 4){
        CY_DEBUG_PRINT_ERROR ("CY: Error in Reading firmware version... \n");
        close (fd);
        return CY_ERROR_INVALID_FIRMWARE;
    }
    tempAdd = (UINT32 *)buffer;
    fmVersion = *tempAdd;
    CY_DEBUG_PRINT_ERROR ("CY:The firmware version is %d \n", fmVersion);
    while (1){
        bytesRead = read (fd, buffer, 4);
        if (bytesRead != 4){
            CY_DEBUG_PRINT_ERROR ("CY: Error in reading firmware Length ... \n");
            close (fd);
            return CY_ERROR_DOWNLOAD_FAILED;;

        }
        tempAdd = (UINT32 *)buffer;
        totalLength = (totalLength * 4);
        firmwareSize = firmwareSize + totalLength;
        bytesRead = read(fd, buffer, 4);
        if (bytesRead != 4){
            CY_DEBUG_PRINT_ERROR ("CY: Error in reading Address ... \n");
            close (fd);
            return CY_ERROR_DOWNLOAD_FAILED;
        }
        tempAdd = (UINT32 *)buffer;
        address = *tempAdd;
        bytesRemaining = totalLength;
        if (checkStart == 0){
            // Get the Firmware Start Address so that 
            // it is updated in the device configuration table
            firmwareStart = address;
            CY_DEBUG_PRINT_INFO ("CY: The firmware start address is %x \n", firmwareStart);
            checkStart++;
        }
        if (bytesRemaining == 0){
            break;
        }
        else {
            while (bytesRemaining >= CY_FIRMWARE_BREAKUP_SIZE) {
                bytesRead  = read(fd, buffer, CY_FIRMWARE_BREAKUP_SIZE);
                bytesRemaining -= bytesRead;
                bmRequestType = CY_VENDOR_REQUEST_HOST_TO_DEVICE;
                bmRequest = CY_BOOT_CMD_PROG_FLASH;
                wIndex = (address & 0x0000FFFF);
                wValue = ((address & 0xFFFF0000) >> 16);
                wLength = bytesRead;
                rStatus = libusb_control_transfer (devHandle, bmRequestType, bmRequest,
                        wValue, wIndex, buffer, wLength, ioTimeout);
                if (rStatus <= LIBUSB_SUCCESS){
                    CY_DEBUG_PRINT_ERROR ("CY: Error in programming device %d... \n", rStatus);
                    close (fd);
                    return CY_ERROR_DOWNLOAD_FAILED;
                }
                else {
                    CY_DEBUG_PRINT_INFO ("CY:The number of bytes is %d \n",rStatus); 
                }
                address += CY_FIRMWARE_BREAKUP_SIZE;
            }
            if (bytesRemaining != 0){
                bytesRead  = read(fd, buffer, bytesRemaining);
                bmRequestType = CY_VENDOR_REQUEST_HOST_TO_DEVICE;
                bmRequest = CY_BOOT_CMD_PROG_FLASH;
                wIndex = (address & 0x0000FFFF);
                wValue = ( (address & 0xFFFF0000) >> 16 );
                wLength = bytesRead;
                rStatus = libusb_control_transfer (devHandle, bmRequestType, bmRequest,
                        wValue, wIndex, buffer, wLength, ioTimeout);
                if (rStatus <= LIBUSB_SUCCESS){
                    CY_DEBUG_PRINT_ERROR ("CY: Error in programming device %d... \n", rStatus);
                    close (fd);
                    return CY_ERROR_DOWNLOAD_FAILED;
                }
            }
        }
    }
    CY_DEBUG_PRINT_INFO ("Firmware Size is %d \n", firmwareSize);
    // Reading firmwareEntry so that it is updated in device configuration
    // Table.
    firmwareSize = (firmwareSize / 4);
    firmwareEntry = address;
    CY_DEBUG_PRINT_INFO ("CY: Firmare entry %p ...\n", firmwareEntry);
    bytesRead = read (fd,buffer, 4);
    tempAdd = (UINT32*)buffer;
    // Get the firmware checksum.
    fmChecksum = *tempAdd;
    CY_DEBUG_PRINT_INFO ("CY:The Checksum value is %x \n",(*tempAdd));
    close (fd);
    return CY_SUCCESS;
}
/*
   This API is used to Read the Silicon ID
 */
CY_RETURN_STATUS CyGetSiliconID(
        CY_HANDLE handle,
        UINT32 *siliconID
        )
{
    UINT16 wValue, wIndex, wLength;
    UINT8 bmRequestType, bmRequest;
    UINT32 rStatus, ioTimeout = CY_USB_SERIAL_TIMEOUT;
    CY_DEVICE *device;
    libusb_device_handle *devHandle;

    if (handle == NULL)
        return CY_ERROR_INVALID_HANDLE;
    if (siliconID == NULL)
        return CY_ERROR_INVALID_PARAMETER;
    device = (CY_DEVICE *)handle;
    devHandle = device->devHandle;

    bmRequestType = CY_VENDOR_REQUEST_DEVICE_TO_HOST;
    bmRequest = CY_BOOT_CMD_GET_SILICON_ID; 
    wValue = 0x00;
    wIndex = 0x00;
    wLength = CY_GET_SILICON_ID_LEN;

    rStatus = libusb_control_transfer (devHandle, bmRequestType, bmRequest,
            wValue, wIndex, (UINT8 *)siliconID, wLength, ioTimeout);

    if (rStatus > 0){
        return CY_SUCCESS;
    }
    else if (rStatus == LIBUSB_ERROR_TIMEOUT){
        CY_DEBUG_PRINT_ERROR ("CY: There was a Time out error in Reading SiliconID .. \n");
        return CY_ERROR_IO_TIMEOUT;
    }
    else{
        CY_DEBUG_PRINT_ERROR ("CY: There was an error doing read of silicon ID..Libusb error is %d\n", rStatus);
        return CY_ERROR_REQUEST_FAILED;
    }
}
/*
   This API reads the SCB configurations and can be 
   used only at the time of manufacturing and programming
   the device
 */
CY_RETURN_STATUS CyReadDeviceConfig (
        CY_HANDLE handle,
        UINT8 *deviceConfig
        )
{

    UINT8 bmRequestType, bmRequest;
    UINT16 wValue, wIndex, wLength;
    CY_DEVICE *device;
    libusb_device_handle *devHandle;
    UINT32 rStatus, timeout = CY_USB_SERIAL_TIMEOUT;

    if (handle == NULL)
        return CY_ERROR_INVALID_HANDLE;
    if (deviceConfig == NULL)
        return CY_ERROR_INVALID_PARAMETER;
    device = (CY_DEVICE *)handle;
    devHandle = device->devHandle;

    bmRequestType = CY_VENDOR_REQUEST_DEVICE_TO_HOST;
    bmRequest = CY_BOOT_CMD_READ_CONFIG;
    wValue = 0x00;
    wIndex = 0x00;
    wLength = CY_DEVICE_CONFIG_SIZE;

    rStatus = libusb_control_transfer (devHandle, bmRequestType, bmRequest,
            wValue, wIndex, deviceConfig, wLength, timeout);
    if (rStatus == CY_DEVICE_CONFIG_SIZE){
        CY_DEBUG_PRINT_INFO ("CY:Successfully Read the configuration ... \n"); 
        return CY_SUCCESS;
    }
    else{
        CY_DEBUG_PRINT_ERROR ("CY:Error in reading Device configuration... Libusb error is %d\n", rStatus);
        return CY_ERROR_REQUEST_FAILED;
    }
}
/*
   This Api writes the device configuration on to the device table of CY Usb Serial 
   It is used only when the device is in bootloader mode and at the time of configuring the
   device
 */
CY_RETURN_STATUS CyWriteDeviceConfig (
        CY_HANDLE handle,
        UINT8 *deviceConfig
        )
{
    UINT8 bmRequestType, bmRequest;
    UINT16 wValue, wIndex, wLength;
    UINT32 rStatus, timeout = CY_USB_SERIAL_TIMEOUT;
    CY_DEVICE *device;
    libusb_device_handle *devHandle;

    if (handle == NULL)
        return CY_ERROR_INVALID_HANDLE;
    if (deviceConfig == NULL)
        return CY_ERROR_INVALID_PARAMETER;
    device = (CY_DEVICE *)handle;
    devHandle = device->devHandle;
    bmRequestType = CY_VENDOR_REQUEST_HOST_TO_DEVICE;
    bmRequest = CY_BOOT_CMD_PROG_CONFIG;
    wValue = 0x00;
    wIndex = 0x00;

    wLength = CY_DEVICE_CONFIG_SIZE;
    rStatus = libusb_control_transfer (devHandle, bmRequestType, bmRequest,
            wValue, wIndex, deviceConfig, wLength, timeout);
    if (rStatus == CY_DEVICE_CONFIG_SIZE){
        CY_DEBUG_PRINT_INFO ("CY:Successfully wrote the configuration ... \n");
        return CY_SUCCESS; 
    }
    else{
        CY_DEBUG_PRINT_ERROR ("CY: Error in Writing Device config ... Libusb error is %d\n", rStatus);
        return CY_ERROR_REQUEST_FAILED;
    }
}
/* Flash config enable will enable reading/writing on flash*/
CY_RETURN_STATUS CyFlashConfigEnable (
        CY_HANDLE handle,
        BOOL isEnable
        )
{
    UINT16 wValue, wIndex, wLength;
    UINT8 bmRequestType, bmRequest;
    UINT32 rStatus, ioTimeout = CY_USB_SERIAL_TIMEOUT;
    CY_DEVICE *device;
    libusb_device_handle *devHandle;

    if (handle == NULL)
        return CY_ERROR_INVALID_HANDLE;

    device = (CY_DEVICE *)handle;
    devHandle = device->devHandle;
    if (isEnable){
        wValue = 0xA6BC;
        wIndex = 0xB1B0; //ON 
    }
    else{
        wValue = 0xA6B6;
        wIndex = 0xB9B0; //OFF
    }
    bmRequestType = CY_VENDOR_REQUEST_HOST_TO_DEVICE;
    bmRequest = CY_VENDOR_ENTER_MFG_MODE; 
    wLength = 0;
    rStatus = libusb_control_transfer (devHandle, bmRequestType, bmRequest,
            wValue, wIndex, NULL, wLength, 5000);
    if (rStatus >= 0){
        return CY_SUCCESS;
    }
    else if (rStatus == LIBUSB_ERROR_TIMEOUT){
        CY_DEBUG_PRINT_ERROR ("CY: There was a Time out error in Flash config enable .. \n");
        return CY_ERROR_IO_TIMEOUT;
    }
    else{
        CY_DEBUG_PRINT_ERROR ("CY: There was an error in Flash config enable..Libusb error is %d\n", rStatus);
        return CY_ERROR_REQUEST_FAILED;
    }
}

