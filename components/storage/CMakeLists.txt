set(srcs esp32/storage.c) 

idf_component_register(
    SRCS ${srcs} 
    INCLUDE_DIRS "./include" 
    PRIV_INCLUDE_DIRS "./include/storage" 
    PRIV_REQUIRES app_config dbg txtio
                  spiffs #PRIV_ESP_IDF
 )