#include "task_wifi.h"
#include "nvs_flash.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "string.h"

WifiHandle hwifi;

static void wifiEventCallback(void *arg, esp_event_base_t base, int32_t id, void *data) {
    const char *TAG = "wifiEventCallback";
    wifi_event_ap_staconnected_t *event = NULL;
    if (base == WIFI_EVENT) {
        switch (id) {
            case WIFI_EVENT_AP_START:
                event = (wifi_event_ap_staconnected_t*)data;
                ESP_LOGI(TAG, "AP mode enabled.");
                break;
            case WIFI_EVENT_AP_STACONNECTED:
                event = (wifi_event_ap_staconnected_t*)data;
                ESP_LOGI(TAG, "AP: station %02X:%02X:%02X:%02X:%02X:%02X joined, AID=0x%02X", 
                    event->mac[0], event->mac[1], event->mac[2], event->mac[3], event->mac[4], event->mac[5], 
                    event->aid);
                break;
            case WIFI_EVENT_AP_STADISCONNECTED:
                event = (wifi_event_ap_staconnected_t*)data;
                ESP_LOGI(TAG, "AP: station %02X:%02X:%02X:%02X:%02X:%02X joined, AID=0x%02X", 
                    event->mac[0], event->mac[1], event->mac[2], event->mac[3], event->mac[4], event->mac[5], 
                    event->aid);
                break;
            case WIFI_EVENT_AP_STOP:
                event = (wifi_event_ap_staconnected_t*)data;
                ESP_LOGI(TAG, "AP mode disenabled.");
                break;
            case WIFI_EVENT_STA_START:
                event = (wifi_event_ap_staconnected_t*)data;
                ESP_LOGI(TAG, "STA mode enabled.");
                break;
            case WIFI_EVENT_STA_CONNECTED:
                event = (wifi_event_ap_staconnected_t*)data;
                ESP_LOGI(TAG, "STA connected.");
                break;
            case WIFI_EVENT_STA_DISCONNECTED:
                event = (wifi_event_ap_staconnected_t*)data;
                ESP_LOGI(TAG, "STA disconnected retrying....");
                break;
            case WIFI_EVENT_STA_STOP:
                event = (wifi_event_ap_staconnected_t*)data;
                ESP_LOGI(TAG, "STA mode disenabled.");
                break;
            default:
                break;
        }
    } 
    else if (base == IP_EVENT) {
        if (id == IP_EVENT_STA_GOT_IP) {
            ip_event_got_ip_t* event = (ip_event_got_ip_t*) data;
            ESP_LOGI(TAG, "STA got IP: %d.%d.%d.%d", 
                (int)((event->ip_info.ip.addr >> 24) & 0xFF),
                (int)((event->ip_info.ip.addr >> 16) & 0xFF),
                (int)((event->ip_info.ip.addr >> 8) & 0xFF),
                (int)((event->ip_info.ip.addr) & 0xFF)
            );
        }
    }
}

/// @brief wifi 外设初始化
/// @param  
void initWifiPeripheral(void) {
    const char *TAG = "wifiInit";

    // 初始化 nvs
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    ESP_LOGI(TAG, "NVS initialization completed.");

    // 初始化网络接口
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_ap();
    esp_netif_create_default_wifi_sta();
    ESP_LOGI(TAG, "Network port initialization completed.");

    // 初始化 wifi 驱动
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    // 注册事件处理器
    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifiEventCallback, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &wifiEventCallback, NULL));
    ESP_LOGI(TAG, "Event system registration completed.");

    // 配置 AP 模式
    wifi_config_t wifi_config = {
        .ap = {
            .ssid = WIFI_SSID,
            .ssid_len = strlen(WIFI_SSID),
            .password = WIFI_PASSWORD,
            .max_connection = WIFI_MAX_CONNECT,
            .authmode = WIFI_AUTH_WPA_WPA2_PSK
        },
    };
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());
    ESP_LOGI(TAG, "WiFi initialization completed.");
}

/// @brief wifi_core 任务函数
/// @param param 
void wifiCoreTask(void* param) {
    static const char* TAG = "wifi_core";
    WifiHandle *hwifi = (WifiHandle*)param;
    char file_data[FILE_MAX_SIZE];
    WifiMsg msg_recv = {0};

    while (1) {
        if (xQueueReceive(hwifi->queue_cmd, &msg_recv, portMAX_DELAY) == pdTRUE) {
            size_t read_length = readFile(hwifi->hfile, file_data, FILE_MAX_SIZE);
            cJSON *root = cJSON_Parse(file_data);
            if (root == NULL) root = cJSON_CreateObject();
            cJSON *item = NULL;
            if (msg_recv.type == WIFI_CMD_RECONNECT) {
                // 检查模式
                cJSON *mode = cJSON_GetObjectItem(root, "mode");
                if (mode == NULL) {
                    ESP_LOGI(TAG, "mode == NULL");
                    mode = cJSON_CreateString("ap");
                    cJSON_AddItemToObject(root, "mode", mode);
                }
                if (strcmp(mode->valuestring, "ap") == 0) {
                    cJSON *ap_item = getJsonItem(root, "/ap");
                    cJSON *ssid_item = cJSON_GetObjectItem(ap_item, "ssid");
                    cJSON *pwd_item = cJSON_GetObjectItem(ap_item, "password");
                    if (ssid_item != NULL && pwd_item != NULL) {
                        wifi_config_t ap_config = { 0 };
                        strcpy((char*)ap_config.ap.ssid, ssid_item->valuestring);
                        strcpy((char*)ap_config.ap.password, pwd_item->valuestring);
                        ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &ap_config));
                    }
                    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
                }
                else if (strcmp(mode->valuestring, "sta") == 0) {
                    cJSON *sta_item = getJsonItem(root, "/sta");
                    cJSON *ssid_item = cJSON_GetObjectItem(sta_item, "ssid");
                    cJSON *pwd_item = cJSON_GetObjectItem(sta_item, "password");
                    if (ssid_item != NULL && pwd_item != NULL) {
                        wifi_config_t sta_config = { 0 };
                        strcpy((char*)sta_config.sta.ssid, ssid_item->valuestring);
                        strcpy((char*)sta_config.sta.password, pwd_item->valuestring);
                        ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &sta_config));
                    }
                    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
                }
                ESP_ERROR_CHECK(esp_wifi_start());
                ESP_ERROR_CHECK(esp_wifi_connect());

                // 修改配置文件
                cJSON_DeleteItemFromObject(root, "is_open");
                cJSON_AddBoolToObject(root, "is_open", 1);
                char *new_data = cJSON_Print(root);
                size_t write_length = writeFile(hwifi->hfile, new_data, strlen(new_data));
                ESP_LOGI(TAG, "\n%s", new_data);
                if (new_data != NULL) free(new_data);
                ESP_LOGI(TAG, "WiFi has been reconnected.");            
            }
            else if (msg_recv.type == WIFI_CMD_DISCONNECT) {
                
                ESP_LOGI(TAG, "msg_recv.type == WIFI_CMD_DISCONNECT");
                vTaskDelay(pdMS_TO_TICKS(1000));

                // 断开连接
                esp_wifi_disconnect();
                esp_wifi_stop();

                cJSON_DeleteItemFromObject(root, "is_open");
                cJSON_AddBoolToObject(root, "is_open", 0);
                char *new_data = cJSON_Print(root);
                size_t write_length = writeFile(hwifi->hfile, new_data, strlen(new_data));
                if (new_data != NULL) free(new_data);
                ESP_LOGI(TAG, "WiFi has been disconnected.");   
            }
            else if (msg_recv.type == WIFI_CMD_SWITCH_AP) {
                // 暂时关闭STA模式

                // 读取配置文件

                // 写入配置文件

                // 打开AP模式

            }
            else if (msg_recv.type == WIFI_CMD_SWITCH_STA) {
                // 暂时关闭AP模式

                // 读取配置文件

                // 写入配置文件

                // 打开STA模式

            }
            else if (msg_recv.type == WIFI_CMD_GET_INFO) {
                // 获取运行中的各类信息

                // 获取持久化保存的信息

                // 发送数据

            }
            else if (msg_recv.type == WIFI_CMD_UDP_ON) {
                // 暂缓开发

            }
            else if (msg_recv.type == WIFI_CMD_UDP_OFF) {
                // 暂缓开发

            }
            else if (msg_recv.type == WIFI_CMD_WEB_ON) {
                // 暂缓开发

            }
            else if (msg_recv.type == WIFI_CMD_WEB_OFF) {
                // 暂缓开发

            }
            else {
                // 打印日志，收到错误的命令

            }
            
            // 释放内存
            if (root != NULL) cJSON_Delete(root);
            if (msg_recv.data != NULL) free(msg_recv.data);
        }
    }
}

/// @brief wifi_udp 任务函数
/// @param param 
void wifiUdpTask(void* param) {
    static const char* TAG = "wifi_udp";
    WifiHandle *hwifi = (WifiHandle*)param;
    ESP_UNUSED(TAG);
    ESP_UNUSED(hwifi);

    while (1) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

/// @brief wifi_web 任务函数
/// @param param 
void wifiWebTask(void* param) {
    static const char* TAG = "wifi_web";
    WifiHandle *hwifi = (WifiHandle*)param;
    ESP_UNUSED(TAG);
    ESP_UNUSED(hwifi);

    while (1) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

/// @brief 
/// @param  
void createWifiTask(void) {
    hwifi.htask_core = NULL;
    hwifi.htask_udp = NULL;
    hwifi.htask_web = NULL;
    hwifi.htask_core = NULL;
    hwifi.queue_cmd = xQueueCreate(sizeof(WifiMsg), QUEUE_SIZE_SMALL);
    hwifi.hfile = &hfiles[3];
    xTaskCreatePinnedToCore(wifiCoreTask, "wifi_core", TASK_STACK_LARGE, &hwifi, TASK_PRIO_LOW, &hwifi.htask_core, APP_CPU_NUM);
}

/// @brief 
/// @param type 
/// @param data 
/// @param length 
uint8_t sendWifiMsg(uint8_t type, uint8_t *data, uint16_t length) {
    if (data == NULL && length != 0) return 0;
    WifiMsg msg_send = {0};
    if (length > 0) {
        msg_send.data = (uint8_t*)malloc(length);
        if (msg_send.data == NULL) return 0;
        memcpy(msg_send.data, data, length);
    }
    msg_send.type = type;
    return xQueueSend(hwifi.queue_cmd, &msg_send, TIME_WAIT_SHORT) == pdTRUE;
}
