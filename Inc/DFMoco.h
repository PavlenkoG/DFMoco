#define DFMOCO_VERSION 1
#define DFMOCO_VERSION_STRING "1.2.7"
#define MOTOR_COUNT 1

#define USER_CMD_ARGS 40
#define TX_MSG_BUF_SIZE 16
#define CMD_BUFFER_SIZE 5
#define RX_BUFFER_SIZE 320

#define CMD_NONE					0
#define CMD_GETSTAT				76
#define CMD_GOTODIR				77
#define CMD_SOFTSTOP			78
#define CMD_GET_ACCEL			79
#define CMD_GET_DECCEL		80
#define CMD_GET_SPEED			81
#define CMD_GET_MARK 			82
#define CMD_GET_MAXSPEED	83
#define CMD_GET_MINSPEED	84
#define CMD_GET_POSITION	85
#define CMD_GO_HOME				86
#define CMD_GO_MARK				87
#define CMD_GOTO					88
#define CMD_HARDSTOP			89
#define CMD_MOVE					90
#define CMD_QUEUECMD			91
#define CMD_RUN						92
#define CMD_SENDQUEUE			93
#define CMD_SETACCEL			94
#define CMD_SETDECCEL			95
#define CMD_SETDIRECTION	96
#define CMD_SETHOME				97
#define CMD_SETMARK				98
#define CMD_SETMAXSPEED		99
#define	CMD_SETMINSPEED		100

#define MSG_GETSTAT				76
#define MSG_GOTODIR				77
#define MSG_SOFTSTOP			78
#define MSG_GET_ACCEL			79
#define MSG_GET_DECCEL		80
#define MSG_GET_SPEED			81
#define MSG_GET_MARK 			82
#define MSG_GET_MAXSPEED	83
#define MSG_GET_MINSPEED	84
#define MSG_GET_POSITION	85
#define MSG_GO_HOME				86
#define MSG_GO_MARK				87
#define MSG_GOTO					88
#define MSG_HARDSTOP			89
#define MSG_MOVE					90
#define MSG_QUEUECMD			91
#define MSG_RUN						92
#define MSG_SENDQUEUE			93
#define MSG_SETACCEL			94
#define MSG_SETDECCEL			95
#define MSG_SETDIRECTION	96
#define MSG_SETHOME				97
#define MSG_SETMARK				98
#define MSG_SETMAXSPEED		99
#define	MSG_SETMINSPEED		100

#define CMD_NONE       0
#define CMD_HI         10
#define CMD_MS         30
#define CMD_NP         31
#define CMD_MM         40 // move motor
#define CMD_PR         41 // pulse rate
#define CMD_SM         42 // stop motor
#define CMD_MP         43 // motor position
#define CMD_ZM         44 // zero motor
#define CMD_SA         50 // stop all (hard)
#define CMD_BF         60 // blur frame
#define CMD_GO         61 // go!

#define CMD_JM         70 // jog motor
#define CMD_IM         71 // inch motor


#define MSG_HI 01
#define MSG_MM 02
#define MSG_MP 03
#define MSG_MS 04
#define MSG_PR 05
#define MSG_SM 06
#define MSG_SA 07
#define MSG_BF 10
#define MSG_GO 11
#define MSG_JM 12
#define MSG_IM 13


#define MSG_STATE_START 0
#define MSG_STATE_CMD   1
#define MSG_STATE_DATA  2
#define MSG_STATE_ERR   3
#define MSG_STATE_DONE  100

#define P2P_MOVE_COUNT 7

struct UserCmd processUserMessage (uint8_t *RxBuffer, uint8_t len);
void processSerialCommand(struct UserCmd);
void sendMessage (char *data_to_send, uint16_t len);
void processMessage (uint8_t MSG);
void setupMotorMove(int motorIndex, int32_t destination);
uint8_t isValidMotor(int motorIndex);
void processGoPosition(int motorIndex, int32_t pos);
void calculatePointToPoint(int motorIndex, int32_t destination);