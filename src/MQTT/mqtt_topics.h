/* ************************************************************************** */
/** Descriptive File Name

  @Company
    Company Name

  @File Name
    filename.h

  @Summary
    Brief description of the file.

  @Description
    Describe the purpose of this file.
 */
/* ************************************************************************** */

#ifndef _MQTT_TOPICS_H    /* Guard against multiple inclusion */
#define _MQTT_TOPICS_H

#include "MQTT/mqtt_topics_hal.h"

/* The TopicItem structure represents a topic in the MQTT system.
 * It contains information about the topic, including its name,
 * the node it belongs to, and callback functions for handling data.
 */
typedef struct
{
    char*   nodename;                      // the node which the topic belongs to
    char*   topic_name;                    // the topic name
    void    (*get_str_value)(char *data);  // a function to return a str representation of the value and topic sequence
    void    (*sub_callback) (char *data);  // a callback when receiving data from broker
    bool    autoPublish;                   // Sets a flag to auto-publish topic
}
TopicItem;


/************************************************
 *  topic publish/subscribe functions
 ***********************************************/

/**
 * @brief Initialize the MQTT topics stack.
 *
 * This function initializes the LWIP MQTT stack, it T1S is connected to the system.
 * The MQTT stack handles T1S connections.
 *
 */
void MQTT_init(void);

/**
 * @brief Check if we are connected to the MQTT broker.
 *
 * If we are not connected to the MQTT broker there is no need to send/check messages.
 * Call this function before communication with the broker to avoid memory stalls.
 *
 * @return true if the broker is connected and online.
 */
bool MQTT_available(void);

/**
 * @brief Publish all topics present at the topic's table.
 *
 * This function scans all topics registered at the internal MQTT topics table
 * and publish the contents of the topic with the respective publish callback.
 */
void MQTT_publish_topics(void);

/**
 * @brief Publish a specific topic.
 *
 * This function publishes the content of a specific topic.
 *
 * @param topic The topic to be published.
 */
void MQTT_publish_topic(TopicItem topic);

/**
 * @brief Insert a new topic into the internal MQTT topics table.
 *
 * This function adds a new topic to the internal MQTT topics table.
 *
 * @param topic The topic to be inserted.
 * @return true if the topic was successfully inserted.
 */
bool MQTT_insert_topic(TopicItem topic);

/**
 * @brief Get the number of topics in the internal MQTT topics table.
 *
 * This function returns the number of topics currently registered in the internal MQTT topics table.
 *
 * @return The number of topics.
 */
uint8_t MQTT_get_number_of_topics(void);

/**
 * @brief Remove a topic from the internal MQTT topics table.
 *
 * This function removes a specific topic from the internal MQTT topics table.
 *
 * @param topic The topic to be removed.
 */
void MQTT_remove_topic(TopicItem topic);

/**
 * @brief Get the full topic name of a specific topic.
 *
 * This function retrieves the name of a specific topic.
 *
 * @param topic The topic whose name is to be retrieved.
 * @param topic_name A buffer to store the topic name.
 */
void MQTT_get_topic_name(TopicItem topic, char* topic_name);

/**
 * @brief Get a specific topic from the internal MQTT topics table.
 *
 * This function retrieves a specific topic from the internal MQTT topics table.
 * Variable 'topic' should have the key values of the topic to be searched under
 * the topic's table.
 *
 * @param topic The topic to be retrieved.
 * @return A pointer to the topic if found, otherwise NULL.
 */
TopicItem* MQTT_get_topic(TopicItem topic);

/**
 * @brief Get the next topic in the internal MQTT topics table.
 *
 * This function retrieves the next topic in the internal MQTT topics table.
 *
 * @param topic A pointer to the current topic, NULL to get the first topic.
 * @return A pointer to the next topic if found, otherwise NULL.
 */
TopicItem* MQTT_get_next_topic(TopicItem* topic);


#endif /* _MQTT_TOPICS_H */

/* *****************************************************************************
 End of File
 */
