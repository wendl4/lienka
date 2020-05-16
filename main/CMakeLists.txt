idf_component_register(SRCS "other_functionality.c" "setup_sta.c" "setup_ap.c" "beebot_main.c" "setup_server.c" "perform.c" "translator.c" "stepper.c" "qr_recognition.c" "file_system.c" "file_server.c"
                       INCLUDE_DIRS "")

set(WEB_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../html")
spiffs_create_partition_image(www ${WEB_SRC_DIR} FLASH_IN_PROJECT)