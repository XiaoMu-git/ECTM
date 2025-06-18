file(GLOB_RECURSE SRC_FILES "./src/*.c")

idf_component_register(
    SRCS ${SRC_FILES}
    INCLUDE_DIRS "./inc"
    REQUIRES	spiffs
				vfs
				json
				driver
				nvs_flash
				esp_wifi
				esp_event
				esp_netif
)
