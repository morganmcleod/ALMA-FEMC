// Defines originally from globaldefinitions.h:
#define NO_ERROR                        0       //!< Global definition of NO_ERROR
#define ERROR                           (-1)    //!< Global definition of ERROR
#define ENABLE                          1
#define DISABLE                         0
#define TRUE                            1
#define FALSE                           0
#define PP_INPUT                        1
#define PP_OUTPUT                       0
#define CAN_MESSAGE_PAYLOAD_SIZE        0x08    // Max size of the CAN message payload
#define CAN_RX_HEADER_SIZE              0x05    // Size of the the message header (RCA + Payload size)
#define CAN_RX_MESSAGE_SIZE             0x0D    // Maximum size of the can message transmitted by the AMBSI1 on the parallel port
#define CAN_RX_MAX_PAYLOAD_SIZE         (CAN_MESSAGE_PAYLOAD_SIZE)  // Max payload size to receive from parallel port
#define CAN_TX_MAX_PAYLOAD_SIZE         (CAN_MESSAGE_PAYLOAD_SIZE)  // Maximum size of the can message to be transmitted to the AMBSI1 on the parallel port
#define CAN_MONITOR                     0x00    // Size of a monitor message
