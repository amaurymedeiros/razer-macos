/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA
 *
 * Should you need to contact me, the author, you can do so by
 * e-mail - mail your message to Terry Cain <terry@terrys-home.co.uk>
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "razermouse_driver.h"
#include "razercommon.h"
#include "razerchromacommon.h"

/**
 * Send report to the mouse
 */
static int razer_get_report(IOUSBDeviceInterface **usb_dev, struct razer_report *request_report, struct razer_report *response_report)
{
    UInt16 product = -1;
    (*usb_dev)->GetDeviceProduct(usb_dev, &product);

    switch (product) {
    // These devices require longer waits to read their firmware, serial, and other setting values
    case USB_DEVICE_ID_RAZER_LANCEHEAD_WIRELESS_RECEIVER:
    case USB_DEVICE_ID_RAZER_LANCEHEAD_WIRELESS_WIRED:
    case USB_DEVICE_ID_RAZER_MAMBA_WIRELESS_RECEIVER:
    case USB_DEVICE_ID_RAZER_MAMBA_WIRELESS_WIRED:
        return razer_get_usb_response(usb_dev, 0x00, request_report, 0x00, response_report, RAZER_NEW_MOUSE_RECEIVER_WAIT_MIN_US);
        break;

    default:
        return razer_get_usb_response(usb_dev, 0x00, request_report, 0x00, response_report, RAZER_MOUSE_WAIT_MIN_US);
    }
}

/**
 * Function to send to device, get response, and actually check the response
 */
static struct razer_report razer_send_payload(IOUSBDeviceInterface **usb_dev, struct razer_report *request_report)
{
    IOReturn retval = -1;

    struct razer_report response_report = {0};

    request_report->crc = razer_calculate_crc(request_report);

    retval = razer_get_report(usb_dev, request_report, &response_report);

    if(retval == 0) {
        // Check the packet number, class and command are the same
        if(response_report.remaining_packets != request_report->remaining_packets ||
           response_report.command_class != request_report->command_class ||
           response_report.command_id.id != request_report->command_id.id) {
            printf("Response doesn't match request");
//        } else if (response_report.status == RAZER_CMD_BUSY) {
//            print_erroneous_report(&response_report, "razermouse", "Device is busy");
        } else if (response_report.status == RAZER_CMD_FAILURE) {
            printf("Command failed");
        } else if (response_report.status == RAZER_CMD_NOT_SUPPORTED) {
            printf("Command not supported");
        } else if (response_report.status == RAZER_CMD_TIMEOUT) {
            printf("Command timed out");
        }
    } else {
        printf("Invalid Report Length");
    }

    return response_report;
}


/**
 * Read device file "device_type"
 *
 * Returns friendly string of device type
 */
ssize_t razer_mouse_attr_read_device_type(IOUSBDeviceInterface **usb_dev, char *buf)
{
    UInt16 product = -1;
    (*usb_dev)->GetDeviceProduct(usb_dev, &product);
    
    char *device_type = "";

    switch (product) {
    case USB_DEVICE_ID_RAZER_DEATHADDER_3_5G:
        device_type = "Razer DeathAdder 3.5G\n";
        break;

    case USB_DEVICE_ID_RAZER_MAMBA_2012_WIRED:
        device_type = "Razer Mamba 2012 (Wired)\n";
        break;

    case USB_DEVICE_ID_RAZER_MAMBA_2012_WIRELESS:
        device_type = "Razer Mamba 2012 (Wireless)\n";
        break;

    case USB_DEVICE_ID_RAZER_MAMBA_WIRED:
        device_type = "Razer Mamba (Wired)\n";
        break;

    case USB_DEVICE_ID_RAZER_MAMBA_WIRELESS:
        device_type = "Razer Mamba (Wireless)\n";
        break;

    case USB_DEVICE_ID_RAZER_MAMBA_TE_WIRED:
        device_type = "Razer Mamba Tournament Edition\n";
        break;

    case USB_DEVICE_ID_RAZER_ABYSSUS:
        device_type = "Razer Abyssus 2014\n";
        break;

    case USB_DEVICE_ID_RAZER_ABYSSUS_1800:
        device_type = "Razer Abyssus 1800\n";
        break;

    case USB_DEVICE_ID_RAZER_ABYSSUS_2000:
        device_type = "Razer Abyssus 2000\n";
        break;

    case USB_DEVICE_ID_RAZER_IMPERATOR:
        device_type = "Razer Imperator 2012\n";
        break;

    case USB_DEVICE_ID_RAZER_OUROBOROS:
        device_type = "Razer Ouroboros\n";
        break;

    case USB_DEVICE_ID_RAZER_OROCHI_2011:
        device_type = "Razer Orochi 2011\n";
        break;

    case USB_DEVICE_ID_RAZER_DEATHADDER_2013:
        device_type = "Razer DeathAdder 2013\n";
        break;

    case USB_DEVICE_ID_RAZER_OROCHI_2013:
        device_type = "Razer Orochi 2013\n";
        break;

    case USB_DEVICE_ID_RAZER_OROCHI_CHROMA:
        device_type = "Razer Orochi (Wired)\n";
        break;

    case USB_DEVICE_ID_RAZER_DEATHADDER_CHROMA:
        device_type = "Razer DeathAdder Chroma\n";
        break;

    case USB_DEVICE_ID_RAZER_NAGA_HEX_RED:
        device_type = "Razer Naga Hex (Red)\n";
        break;

    case USB_DEVICE_ID_RAZER_NAGA_HEX:
        device_type = "Razer Naga Hex\n";
        break;

    case USB_DEVICE_ID_RAZER_NAGA_2012:
        device_type = "Razer Naga 2012\n";
        break;

    case USB_DEVICE_ID_RAZER_NAGA_2014:
        device_type = "Razer Naga 2014\n";
        break;

    case USB_DEVICE_ID_RAZER_TAIPAN:
        device_type = "Razer Taipan\n";
        break;

    case USB_DEVICE_ID_RAZER_NAGA_HEX_V2:
        device_type = "Razer Naga Hex V2\n";
        break;

    case USB_DEVICE_ID_RAZER_NAGA_CHROMA:
        device_type = "Razer Naga Chroma\n";
        break;

    case USB_DEVICE_ID_RAZER_DEATHADDER_ELITE:
        device_type = "Razer DeathAdder Elite\n";
        break;

    case USB_DEVICE_ID_RAZER_ABYSSUS_V2:
        device_type = "Razer Abyssus V2\n";
        break;

    case USB_DEVICE_ID_RAZER_DIAMONDBACK_CHROMA:
        device_type = "Razer Diamondback Chroma\n";
        break;

    case USB_DEVICE_ID_RAZER_DEATHADDER_3500:
        device_type = "Razer DeathAdder 3500\n";
        break;

    case USB_DEVICE_ID_RAZER_LANCEHEAD_WIRED:
        device_type = "Razer Lancehead (Wired)\n";
        break;

    case USB_DEVICE_ID_RAZER_LANCEHEAD_WIRELESS:
        device_type = "Razer Lancehead (Wireless)\n";
        break;

    case USB_DEVICE_ID_RAZER_LANCEHEAD_TE_WIRED:
        device_type = "Razer Lancehead Tournament Edition\n";
        break;

    case USB_DEVICE_ID_RAZER_MAMBA_ELITE:
        device_type = "Razer Mamba Elite\n";
        break;

    case USB_DEVICE_ID_RAZER_DEATHADDER_ESSENTIAL:
        device_type = "Razer Deathadder Essential\n";
        break;

    case USB_DEVICE_ID_RAZER_NAGA_TRINITY:
        device_type = "Razer Naga Trinity\n";
        break;

    case USB_DEVICE_ID_RAZER_DEATHADDER_1800:
        device_type = "Razer Deathadder 1800\n";
        break;

    case USB_DEVICE_ID_RAZER_LANCEHEAD_WIRELESS_RECEIVER:
        device_type = "Razer Lancehead Wireless (Receiver)\n";
        break;

    case USB_DEVICE_ID_RAZER_LANCEHEAD_WIRELESS_WIRED:
        device_type = "Razer Lancehead Wireless (Wired)\n";
        break;

    case USB_DEVICE_ID_RAZER_MAMBA_WIRELESS_RECEIVER:
        device_type = "Razer Mamba Wireless (Receiver)\n";
        break;

    case USB_DEVICE_ID_RAZER_MAMBA_WIRELESS_WIRED:
        device_type = "Razer Mamba Wireless (Wired)\n";
        break;

    case USB_DEVICE_ID_RAZER_ABYSSUS_ELITE_DVA_EDITION:
        device_type = "Razer Abyssus Elite (D.Va Edition)\n";
        break;

    case USB_DEVICE_ID_RAZER_ABYSSUS_ESSENTIAL:
        device_type = "Razer Abyssus Essential\n";
        break;

    case USB_DEVICE_ID_RAZER_DEATHADDER_ESSENTIAL_WHITE_EDITION:
        device_type = "Razer DeathAdder Essential (White Edition)\n";
        break;

    case USB_DEVICE_ID_RAZER_VIPER:
        device_type = "Razer Viper\n";
        break;

    case USB_DEVICE_ID_RAZER_VIPER_ULTIMATE_WIRED:
        device_type = "Razer Viper Ultimate (Wired)\n";
        break;

    case USB_DEVICE_ID_RAZER_VIPER_ULTIMATE_WIRELESS:
        device_type = "Razer Viper Ultimate (Wireless)\n";
        break;

    case USB_DEVICE_ID_RAZER_BASILISK:
        device_type = "Razer Basilisk\n";
        break;

    case USB_DEVICE_ID_RAZER_DEATHADDER_V2:
        device_type = "Razer Deathadder V2\n";
        break;

    default:
        device_type = "Unknown Device\n";
    }

    return sprintf(buf, "%s", device_type);
}


/**
 * Write device file "logo_led_effect"
 */
ssize_t razer_attr_write_logo_led_effect(IOUSBDeviceInterface **usb_dev, const char *buf, size_t count)
{
    unsigned char effect = (unsigned char)strtoul(buf, NULL, 10);
    struct razer_report report = razer_chroma_standard_set_led_effect(VARSTORE, LOGO_LED, effect);
    report.transaction_id.id = 0x3F;

    razer_send_payload(usb_dev, &report);

    return count;
}

/**
 * Write device file "logo_led_rgb"
 */
ssize_t razer_attr_write_logo_led_rgb(IOUSBDeviceInterface **usb_dev, const char *buf, size_t count)
{
    struct razer_report report = {0};

    if(count == 3) {
        report = razer_chroma_standard_set_led_rgb(VARSTORE, LOGO_LED, (struct razer_rgb*)&buf[0]);
        report.transaction_id.id = 0x3F;
        razer_send_payload(usb_dev, &report);
    } else {
        printf("razermouse: Logo LED mode only accepts RGB (3byte)\n");
    }

    return count;
}
