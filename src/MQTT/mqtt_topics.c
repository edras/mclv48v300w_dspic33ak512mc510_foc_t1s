/* ************************************************************************** */
/** mqtt_topics

  @Company
    Microchip Technology Inc.

  @File Name
    mqtt_topics.c

  @Summary
    MQTT topics management layer for lwIP MQTT client.

  @Description
    Platform-independent MQTT topic publish/subscribe engine using a hash table
    to store registered topics. Connects to a broker over lwIP raw TCP API.
 */
/* ************************************************************************** */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "mqtt_topics.h"
#include "hashtable.h"
#include "lwip/apps/mqtt.h"
#include "MQTT/mqtt_topics_hal.h"
#include "T1S/t1s_lwip.h"

#ifdef MQTT_ENABLED

// MQTT Server for T1S: 192.168.0.5
#define LWIP_MQTT_EXAMPLE_IPADDR_INIT IPADDR4_INIT(PP_HTONL(0xC0A80005))

static ip_addr_t mqtt_ip = LWIP_MQTT_EXAMPLE_IPADDR_INIT;
static mqtt_client_t* mqtt_client;
static const struct mqtt_connect_client_info_t mqtt_client_info =
{
    BOARD_LABEL,  /* user id */
    NULL,         /* user */
    NULL,         /* pass */
    100,          /* keep alive */
    NULL,         /* will_topic */
    NULL,         /* will_msg */
    0,            /* will_qos */
    0             /* will_retain */
};

// MQTT MAX TOPIC_NAME size and VALUE string size
#define TOPIC_NAME_LENGTH 100
#define TOPIC_VALUE_LENGTH 50

static char topic_name[TOPIC_NAME_LENGTH];
static char topic_value[TOPIC_VALUE_LENGTH];
static HASHTABLE* topics;
static TopicItem* topic_received;
static TopicItem* topic_published;
static TopicItem* topic_subscribed;

static bool mqtt_online = false;
static bool mqtt_refresh_topics = false;
static bool mqtt_waiting_cb = false;

/* Private function prototypes */
static void mqtt_build_topic_name(TopicItem* item);
static bool mqtt_split_string(const char* topic, char* key1, char* key2);
static bool mqtt_key_compare(const void *data1, const void *data2);
static void mqtt_topic_free(void *data);
static unsigned int mqtt_calc_hash(const void *data);
static bool mqtt_should_publish(TopicItem* topic);
static TopicItem* mqtt_topic_copy(TopicItem item);


/* functions implementation */

static TopicItem* mqtt_topic_copy(TopicItem item)
{
    TopicItem* new_item = (TopicItem*)malloc(sizeof(TopicItem));
    if (new_item == NULL)
    {
        return NULL;
    }

    new_item->nodename = strdup(item.nodename);
    new_item->topic_name = strdup(item.topic_name);
    new_item->get_str_value = item.get_str_value;
    new_item->sub_callback = item.sub_callback;
    new_item->autoPublish = item.autoPublish;

    return new_item;
}

static void mqtt_topic_free(void *data)
{
    TopicItem *item = (TopicItem *)data;
    if (item)
    {
        free(item->nodename);
        free(item->topic_name);
        free(item);
    }
}

static bool mqtt_key_compare(const void *data1, const void *data2)
{
    TopicItem *item1 = (TopicItem *)data1;
    TopicItem *item2 = (TopicItem *)data2;
    if(strcmp(item1->nodename, item2->nodename) != 0)
    {
        return false;
    }
    return (strcmp(item1->topic_name, item2->topic_name) == 0);
}

static unsigned int mqtt_calc_hash(const void *data)
{
    // djb2 algorithm
    unsigned int hash = 5381;
    TopicItem *item = (TopicItem *)data;

    const char *str = item->nodename;
    while (*str) {
        hash = ((hash << 5) + hash) + *str++; // hash * 33 + c
    }
    str = item->topic_name;
    while (*str) {
        hash = ((hash << 5) + hash) + *str++; // hash * 33 + c
    }
    return hash;
}

static void mqtt_build_topic_name(TopicItem* item)
{
    if (BOARD_NODE_NAME[0] == '\0')
        snprintf(topic_name, TOPIC_NAME_LENGTH, "%s/%s", item->nodename, item->topic_name);
    else
        snprintf(topic_name, TOPIC_NAME_LENGTH, "%s/%s/%s", BOARD_NODE_NAME, item->nodename, item->topic_name);
}

static bool mqtt_split_string(const char* topic, char* key1, char* key2)
{
    char *first_slash = strchr(topic, '/');
    if (first_slash)
    {
        char *second_slash = strchr(first_slash + 1, '/');
        if (second_slash)
        {
            size_t key1_len = second_slash - first_slash - 1;
            strncpy(key1, first_slash + 1, key1_len);
            key1[key1_len] = '\0';
            strcpy(key2, second_slash + 1);
            return true;
        }
        else
        {
            size_t key1_len = first_slash - topic;
            strncpy(key1, topic, key1_len);
            key1[key1_len] = '\0';
            strcpy(key2, first_slash + 1);
            return true;
        }
    }
    return false;
}

static void mqtt_incoming_data_cb(void *arg, const u8_t *data, u16_t len, u8_t flags)
{
    LWIP_UNUSED_ARG(arg);
    LWIP_UNUSED_ARG(flags);

    if(topic_received)
    {
        if(topic_received->sub_callback)
        {
            topic_received->sub_callback((char*)data);
        }
    }
}

static void mqtt_incoming_publish_cb(void *arg, const char *topic, u32_t tot_len)
{
    LWIP_UNUSED_ARG(arg);
    LWIP_UNUSED_ARG(tot_len);

    char module_name[20];
    char topic_name[20];

    topic_received = NULL;
    if(mqtt_split_string(topic, module_name, topic_name))
    {
        TopicItem key = {module_name, topic_name};
        topic_received = HASHTABLE_search(topics, &key);
    }
}

static void mqtt_burst_sub_request_cb(void *arg, err_t err)
{
    const struct mqtt_connect_client_info_t* client_info = (const struct mqtt_connect_client_info_t*)arg;
    LWIP_PLATFORM_DIAG(("MQTT client \"%s\" subscription request cb: err %d\n", client_info->client_id, (int)err));
    while((topic_subscribed = HASHTABLE_next(topics, topic_subscribed)) != NULL)
    {
        if(topic_subscribed->sub_callback == NULL) continue;
        mqtt_build_topic_name(topic_subscribed);
        mqtt_sub_unsub(mqtt_client, topic_name, 0, mqtt_burst_sub_request_cb, LWIP_CONST_CAST(void*, client_info), 1);
        break;
    }
}

static void mqtt_single_request_cb(void *arg, err_t err)
{
    LWIP_UNUSED_ARG(arg);
    LWIP_UNUSED_ARG(err);
}

static void mqtt_burst_request_cb(void *arg, err_t err)
{
    LWIP_UNUSED_ARG(arg);
    LWIP_UNUSED_ARG(err);
    if (!mqtt_online) return;
    while((topic_published = HASHTABLE_next(topics, topic_published)) != NULL)
    {
        if (!mqtt_should_publish(topic_published)) continue;
        topic_published->get_str_value(topic_value);
        if(topic_value[0] == 0) continue;
        size_t size = strlen(topic_value);
        mqtt_build_topic_name(topic_published);
        mqtt_publish(mqtt_client, topic_name, topic_value, size, 0, 0, mqtt_burst_request_cb, NULL);
        break;
    }

    if (topic_published == NULL)
    {
        mqtt_refresh_topics = false;
    }
}

static void mqtt_connection_cb(mqtt_client_t *client, void *arg, mqtt_connection_status_t status)
{
    const struct mqtt_connect_client_info_t* client_info = (const struct mqtt_connect_client_info_t*)arg;
    LWIP_UNUSED_ARG(client);
    LWIP_PLATFORM_DIAG(("MQTT client \"%s\" connection cb: status %d\n", client_info->client_id, (int)status));

    mqtt_waiting_cb = false;

    if (status == MQTT_CONNECT_ACCEPTED)
    {
        topic_subscribed = NULL;
        while ((topic_subscribed = HASHTABLE_next(topics, topic_subscribed)) != NULL)
        {
            if(topic_subscribed->sub_callback)
            {
                mqtt_build_topic_name(topic_subscribed);
                mqtt_sub_unsub(client, topic_name, 0, mqtt_burst_sub_request_cb,
                        LWIP_CONST_CAST(void*, client_info), 1);
                break;
            }
        }
        mqtt_online = true;
        mqtt_refresh_topics = true;
        printf("MQTT Connected \r\n");
    }
    else if (status == MQTT_CONNECT_TIMEOUT || status == MQTT_CONNECT_DISCONNECTED)
    {
        mqtt_online = false;
        mqtt_disconnect(client);
    }
}

static bool mqtt_should_publish(TopicItem* topic)
{
    if (topic->get_str_value == NULL) {
        return false;
    }

    if (mqtt_refresh_topics) {
        return true;
    }

    return topic->autoPublish;
}

/**********************************************
 *              Public methods
 * ********************************************
 */

bool MQTT_available(void)
{
    if(T1S_available())
    {
        if(mqtt_online)
        {
            return true;
        }
        else
        {
            MQTT_init();
        }
    }
    return false;
}

void MQTT_init(void)
{
    if(!T1S_available()) return;

    if(mqtt_waiting_cb) return;
    mqtt_waiting_cb = true;

    mqtt_client = mqtt_client_new();
    mqtt_client_connect(mqtt_client, &mqtt_ip, MQTT_PORT, mqtt_connection_cb,
            LWIP_CONST_CAST(void*, &mqtt_client_info), &mqtt_client_info);
    mqtt_set_inpub_callback(mqtt_client, mqtt_incoming_publish_cb,
          mqtt_incoming_data_cb, LWIP_CONST_CAST(void*, &mqtt_client_info));

}

void MQTT_publish_topics(void)
{
    TopicItem* topic = NULL;
    while((topic = HASHTABLE_next(topics, topic)) != NULL)
    {
        if(mqtt_should_publish(topic))
        {
            topic->get_str_value(topic_value);
            size_t size = strlen(topic_value);
            if(size && mqtt_online)
            {
                topic_published = topic;
                mqtt_build_topic_name(topic);
                mqtt_publish(mqtt_client, topic_name, topic_value, size, 0, 0, mqtt_burst_request_cb, NULL);
                break;
            }
        }
    }
}

void MQTT_publish_topic(TopicItem item)
{
    TopicItem *topic;
    if((topic = HASHTABLE_search(topics, &item)) != NULL)
    {
        if(topic->get_str_value)
        {
            topic->get_str_value(topic_value);
            size_t size = strlen(topic_value);
            if(size && mqtt_online)
            {
                mqtt_build_topic_name(topic);
                mqtt_publish(mqtt_client, topic_name, topic_value, size, 0, 0, mqtt_single_request_cb, NULL);
            }
        }
    }
}

uint8_t MQTT_get_number_of_topics(void)
{
    return (uint8_t)topics->count;
}

void MQTT_get_topic_name(TopicItem item, char* name)
{
    TopicItem *topic = HASHTABLE_search(topics, (const void*)&item);
    if(topic)
    {
        mqtt_build_topic_name(topic);
        strcpy(name, topic_name);
        return;
    }
    strcpy(name, "none");
}

bool MQTT_insert_topic(TopicItem item)
{
    if(topics == NULL)
    {
        if((topics = HASHTABLE_create(20, mqtt_calc_hash, mqtt_key_compare, mqtt_topic_free)) == NULL)
        {
            return false;
        }
    }

    if(item.nodename == NULL && item.topic_name == NULL) return false;
    if(HASHTABLE_search(topics, &item) != NULL) return false;

    TopicItem* new_item = mqtt_topic_copy(item);
    if (new_item == NULL)
    {
        return false;
    }

    if(HASHTABLE_insert(topics, new_item))
    {
        return true;
    }

    topics->freeFunction(new_item);
    return false;
}

void MQTT_remove_topic(TopicItem item)
{
    if(topics == NULL) return;
    TopicItem* toDelete = HASHTABLE_search(topics, &item);
    if(toDelete)
    {
        HASHTABLE_delete(topics, (const void*)&item);
    }
}

TopicItem* MQTT_get_topic(TopicItem topic)
{
    return (TopicItem*)HASHTABLE_search(topics, &topic);
}

TopicItem* MQTT_get_next_topic(TopicItem *topic)
{
    return (TopicItem*)HASHTABLE_next(topics, topic);
}

#endif

/* *****************************************************************************
 End of File
 */
