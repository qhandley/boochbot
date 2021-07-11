#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"

#include "owb.h"
#include "owb_rmt.h"
#include "ds18b20.h"

#define GPIO_DS18B20_0       (CONFIG_ONE_WIRE_GPIO)
#define MAX_DEVICES          (8)
#define DS18B20_RESOLUTION   (DS18B20_RESOLUTION_12_BIT)
#define SAMPLE_PERIOD        (1000)   // milliseconds

OneWireBus *g_owb;
DS18B20_Info *g_device;
static bool init_complete = false;

esp_err_t init_ds18b20(void)
{
    OneWireBus_ROMCode device_rom_codes[MAX_DEVICES] = {0};
    OneWireBus_SearchState search_state = {0};
    int num_devices = 0;
    bool found = false;

    // Stable readings require a brief period before communication
    vTaskDelay(2000.0 / portTICK_PERIOD_MS);

    // Create a 1-Wire bus, using the RMT timeslot driver
    owb_rmt_driver_info rmt_driver_info;
    g_owb = owb_rmt_initialize(&rmt_driver_info, GPIO_DS18B20_0, RMT_CHANNEL_1, RMT_CHANNEL_0);
    owb_use_crc(g_owb, true);  // enable CRC check for ROM code

    // Find all connected devices
    printf("Finding devices:\n");
    owb_search_first(g_owb, &search_state, &found);
    while (found)
    {
        char rom_code_s[17];
        owb_string_from_rom_code(search_state.rom_code, rom_code_s, sizeof(rom_code_s));
        printf("  %d : %s\n", num_devices, rom_code_s);
        device_rom_codes[num_devices] = search_state.rom_code;
        ++num_devices;
        owb_search_next(g_owb, &search_state, &found);
    }

    if (num_devices != 1) {
        printf("Error: found %d device%s\n", num_devices, num_devices == 1 ? "" : "s");
        goto init_cleanup;
    }

    // In this example, if a single device is present, then the ROM code is probably
    // not very interesting, so just print it out.
    OneWireBus_ROMCode rom_code;
    owb_status status = owb_read_rom(g_owb, &rom_code);
    if (status == OWB_STATUS_OK)
    {
        char rom_code_s[OWB_ROM_CODE_STRING_LENGTH];
        owb_string_from_rom_code(rom_code, rom_code_s, sizeof(rom_code_s));
        printf("Single device %s present\n", rom_code_s);
    }
    else
    {
        printf("An error occurred reading ROM code: %d\n", status);
        goto init_cleanup;
    }

    // Create DS18B20 device on the 1-Wire bus
    DS18B20_Info *ds18b20_info = ds18b20_malloc(); // heap allocation
    if (ds18b20_info == NULL)
    {
        printf("Unable to allocate memory for ds18b20");
        goto init_cleanup;
    }

    g_device = ds18b20_info;
    ds18b20_init_solo(ds18b20_info, g_owb); // only one device on bus
    ds18b20_use_crc(ds18b20_info, true); // enable CRC check on all reads
    ds18b20_set_resolution(ds18b20_info, DS18B20_RESOLUTION);

#ifdef CONFIG_ENABLE_STRONG_PULLUP_GPIO
    // An external pull-up circuit is used to supply extra current to OneWireBus devices
    // during temperature conversions.
    owb_use_strong_pullup_gpio(g_owb, CONFIG_STRONG_PULLUP_GPIO);
#endif

    init_complete = true;
    return ESP_OK;

init_cleanup:
    owb_uninitialize(g_owb);
    return ESP_FAIL;
}

esp_err_t read_ds18b20_temp(float *reading)
{
    DS18B20_ERROR error = {0};
    static int error_count = 0;

    if (init_complete != true)
    {
        return ESP_FAIL;
    }

    ds18b20_convert_all(g_owb);
    ds18b20_wait_for_conversion(g_device);

    // Read the results immediately after conversion otherwise it may fail
    // (using printf before reading may take too long)
    error = ds18b20_read_temp(g_device, reading);

    if (error != DS18B20_OK)
    {
        ++error_count;
    }

    return ESP_OK;
}
